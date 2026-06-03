#include "Fisherman.h"
#include "GameManager.h"
#include "UpgradeManagerComponent.h"

AFisherman::AFisherman()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentFishingTime = 0.0f;
    bIsFishing = false;
}

void AFisherman::BeginPlay()
{
    Super::BeginPlay();
    StartFishing();
}

void AFisherman::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsFishing) return;

    CurrentFishingTime += DeltaTime;

    if (CurrentFishingTime >= FishingTime)
    {
        CatchFish();
    }
}

void AFisherman::StartFishing()
{
    bIsFishing = true;
    CurrentFishingTime = 0.0f;
}

void AFisherman::StopFishing()
{
	bIsFishing = false;
	CurrentFishingTime = 0.0f;
}

void AFisherman::SpeedUpFishing()
{
    if (!bIsFishing) return;

    CurrentFishingTime += SpeedUpValue;

    if (CurrentFishingTime >= FishingTime)
    {
        CatchFish();
    }
}

void AFisherman::CatchFish()
{
    FFishData* FishData = GetRandomFish();

    if (FishData)
    {
        UE_LOG(LogTemp, Log, TEXT("Caught fish! ID: %d, Name: %s"),
            FishData->FishID, *FishData->FishName);

        OnFishCaught.Broadcast(FishData->FishID, FishData->FishName);
        OnFishermanFishCaughtFull.Broadcast(*FishData);

        AGameManager* GameManager = AGameManager::GetInstance();
        if (GameManager)
        {
            GameManager->AddMoney(FishData->Price);
        }
    }

    StartFishing();
}

FFishData* AFisherman::GetRandomFish()
{
    UDataTable* ActiveTable = nullptr;

    AGameManager* GM = AGameManager::GetInstance();
    if (GM)
    {
        ActiveTable = GM->GetActiveFishTable();
    }

    if (!IsValid(ActiveTable))
    {
        ActiveTable = FishDataTable;
    }

    if (!IsValid(ActiveTable) || !ActiveTable->GetRowStruct())
    {
        UE_LOG(LogTemp, Error, TEXT("Fisherman: invalid fish table!"));
        return nullptr;
    }

    TArray<FFishData*> AllFish;
    ActiveTable->GetAllRows<FFishData>(TEXT(""), AllFish);

    if (AllFish.Num() == 0) return nullptr;

    int32 Luck = 1;
    if (GM && GM->GetUpgradeManager())
    {
        Luck = GM->GetUpgradeManager()->GetRodLuckBonus();
    }

    float TotalProbability = 0.0f;
    TArray<float> AdjustedProbabilities;

    for (FFishData* Fish : AllFish)
    {
        float AdjustedProb = Fish->Probability * (1.0f + (Luck * 0.1f));
        AdjustedProbabilities.Add(AdjustedProb);
        TotalProbability += AdjustedProb;
    }

    for (float& Prob : AdjustedProbabilities)
    {
        Prob /= TotalProbability;
    }

    float RandomValue = FMath::FRand();
    float CumulativeProbability = 0.0f;

    for (int32 i = 0; i < AllFish.Num(); i++)
    {
        CumulativeProbability += AdjustedProbabilities[i];
        if (RandomValue <= CumulativeProbability)
        {
            return AllFish[i];
        }
    }

    return AllFish.Last();
}
