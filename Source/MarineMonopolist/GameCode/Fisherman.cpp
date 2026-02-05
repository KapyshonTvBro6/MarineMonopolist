#include "Fisherman.h"
#include "GameManager.h"

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
    
    // Увеличиваем текущее время ловли
    CurrentFishingTime += DeltaTime;
    
    // Проверяем, не пора ли поймать рыбу
    if (CurrentFishingTime >= FishingTime)
    {
        CatchFish();
    }
}

void AFisherman::StartFishing()
{
    bIsFishing = true;
    CurrentFishingTime = 0.0f;
    
    UE_LOG(LogTemp, Log, TEXT("Started fishing for %f seconds"), FishingTime);
}

void AFisherman::SpeedUpFishing()
{
    if (!bIsFishing) return;
    
    // Ускоряем ловлю
    CurrentFishingTime += SpeedUpValue;
    
    UE_LOG(LogTemp, Log, TEXT("Fishing sped up by %f seconds. Time remaining: %f"), 
        SpeedUpValue, FishingTime - CurrentFishingTime);
    
    // Проверяем, не пора ли поймать рыбу
    if (CurrentFishingTime >= FishingTime)
    {
        CatchFish();
    }
}

void AFisherman::CatchFish()
{
    if (!FishDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("FishDataTable not set!"));
        bIsFishing = false;
        return;
    }
    
    // Получаем случайную рыбу
    FFishData* FishData = GetRandomFish();
    
    if (FishData)
    {
        // Выводим информацию о пойманной рыбе
        UE_LOG(LogTemp, Log, TEXT("Caught fish! ID: %d, Name: %s"), 
            FishData->FishID, *FishData->FishName);
        
        // Вызываем событие поимки рыбы для обновления UI
        OnFishCaught.Broadcast(FishData->FishID, FishData->FishName);
        
        // Получаем гейм-менеджер и добавляем деньги
        AGameManager* GameManager = AGameManager::GetInstance();
        if (GameManager)
        {
            GameManager->AddMoney(FishData->Price);
            UE_LOG(LogTemp, Log, TEXT("Added %d money for fish: %s"), 
                FishData->Price, *FishData->FishName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No fish data found!"));
    }
    
    // Начинаем новую ловлю
    StartFishing();
}

FFishData* AFisherman::GetRandomFish()
{
    if (!FishDataTable) return nullptr;
    
    // Получаем все строки из таблицы
    TArray<FFishData*> AllFish;
    FishDataTable->GetAllRows<FFishData>(TEXT(""), AllFish);
    
    if (AllFish.Num() == 0) return nullptr;
    
    // Рассчитываем общую вероятность с учетом удачи
    float TotalProbability = 0.0f;
    TArray<float> AdjustedProbabilities;
    
    for (FFishData* Fish : AllFish)
    {
        // Модифицируем вероятность в зависимости от удачи
        float AdjustedProb = Fish->Probability * (1.0f + (Luck * 0.1f));
        AdjustedProbabilities.Add(AdjustedProb);
        TotalProbability += AdjustedProb;
    }
    
    // Нормализуем вероятности
    for (float& Prob : AdjustedProbabilities)
    {
        Prob /= TotalProbability;
    }
    
    // Выбираем случайную рыбу на основе вероятностей
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