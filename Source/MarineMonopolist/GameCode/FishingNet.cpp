#include "FishingNet.h"
#include "GameManager.h"
#include "Fisherman.h"
#include "Components/StaticMeshComponent.h"

AFishingNet::AFishingNet()
{
    PrimaryActorTick.bCanEverTick = true;

    NetMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NetMeshComponent"));
    RootComponent = NetMeshComponent;
}

void AFishingNet::BeginPlay()
{
    Super::BeginPlay();
    StartFishing();
}

void AFishingNet::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsFishing) return;

    CurrentCatchTime += DeltaTime;

    if (CurrentCatchTime >= FishingInterval)
    {
        CurrentCatchTime = 0.0f;

        if (bPredictedCatchResult)
        {
            CatchSingleFish();
        }

        bPredictedCatchResult = (FMath::FRand() <= CatchChanceMultiplier);
    }
}

void AFishingNet::StartFishing()
{
    if (bIsFishing) return;
    bIsFishing = true;
    CurrentCatchTime = 0.0f;
    bPredictedCatchResult = (FMath::FRand() <= CatchChanceMultiplier);
}

void AFishingNet::StopFishing()
{
    bIsFishing = false;
}

void AFishingNet::CollectAllFish()
{
    if (CaughtFishIDs.Num() == 0) return;

    int32 TotalValue = GetTotalValue();

    AGameManager* GameManager = AGameManager::GetInstance();
    if (GameManager)
    {
        GameManager->AddMoney(TotalValue);
    }

    TArray<FFishData*> AllFish;
    if (FishDataTable)
    {
        FishDataTable->GetAllRows<FFishData>(TEXT(""), AllFish);
    }

    TMap<int32, FFishData*> FishMap;
    for (FFishData* Fish : AllFish)
    {
        if (Fish) FishMap.Add(Fish->FishID, Fish);
    }

    for (int32 FishID : CaughtFishIDs)
    {
        FFishData** Found = FishMap.Find(FishID);
        if (Found && *Found)
        {
            OnNetFishCaught.Broadcast(**Found);
        }
    }

    CaughtFishIDs.Empty();
    CurrentCatchTime = 0.0f;
    bPredictedCatchResult = (FMath::FRand() <= CatchChanceMultiplier);
}

void AFishingNet::SetCatchInterval(float Seconds)
{
    FishingInterval = FMath::Max(0.5f, Seconds);
}

void AFishingNet::SetCatchChanceMultiplier(float Multiplier)
{
    CatchChanceMultiplier = FMath::Clamp(Multiplier, 0.0f, 1.0f);
}

void AFishingNet::SetNetMesh(UStaticMesh* NewMesh)
{
    if (NewMesh)
    {
        NetMeshComponent->SetStaticMesh(NewMesh);
    }
}

float AFishingNet::GetCatchProgress() const
{
    if (FishingInterval <= 0.0f) return 0.0f;
    return FMath::Clamp(CurrentCatchTime / FishingInterval, 0.0f, 1.0f);
}

int32 AFishingNet::GetTotalValue() const
{
    if (CaughtFishIDs.Num() == 0) return 0;

    TArray<FFishData*> AllFish;
    if (!FishDataTable) return 0;
    FishDataTable->GetAllRows<FFishData>(TEXT(""), AllFish);

    TMap<int32, int32> FishPriceMap;
    for (FFishData* Fish : AllFish)
    {
        if (Fish) FishPriceMap.Add(Fish->FishID, Fish->Price);
    }

    int32 Total = 0;
    for (int32 FishID : CaughtFishIDs)
    {
        int32* Price = FishPriceMap.Find(FishID);
        if (Price) Total += *Price;
    }
    return Total;
}

void AFishingNet::CatchSingleFish()
{
    FFishData* FishData = GetRandomFish();
    if (!FishData) return;

    CaughtFishIDs.Add(FishData->FishID);
}

FFishData* AFishingNet::GetRandomFish()
{
    UDataTable* ActiveTable = nullptr;

    AGameManager* GM = AGameManager::GetInstance();
    if (GM)
    {
        ActiveTable = GM->GetActiveFishTable();
    }

    if (!ActiveTable) ActiveTable = FishDataTable;

    if (!IsValid(ActiveTable) && GM && GM->GetFisherman())
    {
        ActiveTable = GM->GetFisherman()->GetFishDataTable();
    }

    if (!IsValid(ActiveTable) || !ActiveTable->GetRowStruct())
    {
        UE_LOG(LogTemp, Error, TEXT("FishingNet: invalid fish table!"));
        return nullptr;
    }

    TArray<FFishData*> AllFish;
    ActiveTable->GetAllRows<FFishData>(TEXT(""), AllFish);
    if (AllFish.Num() == 0) return nullptr;

    float TotalProbability = 0.0f;
    for (FFishData* Fish : AllFish)
    {
        TotalProbability += Fish->Probability;
    }

    float RandomValue = FMath::FRand() * TotalProbability;
    float CumulativeProbability = 0.0f;

    for (FFishData* Fish : AllFish)
    {
        CumulativeProbability += Fish->Probability;
        if (RandomValue <= CumulativeProbability)
        {
            return Fish;
        }
    }

    return AllFish.Last();
}
