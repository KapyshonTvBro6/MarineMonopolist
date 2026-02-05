#include "FishingNet.h"
#include "GameManager.h"
#include "TimerManager.h"

AFishingNet::AFishingNet()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsFishing = false;
}

void AFishingNet::BeginPlay()
{
    Super::BeginPlay();
    StartFishing();
}

void AFishingNet::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFishingNet::StartFishing()
{
    if (bIsFishing) return;
    
    bIsFishing = true;
    
    // Запускаем таймер для первой ловли
    GetWorld()->GetTimerManager().SetTimer(
        FishingTimerHandle,
        this,
        &AFishingNet::OnFishingTimerComplete,
        FishingInterval,
        false
    );
    
    UE_LOG(LogTemp, Log, TEXT("Fishing net started. First catch in %f seconds"), FishingInterval);
}

void AFishingNet::StopFishing()
{
    if (!bIsFishing) return;
    
    bIsFishing = false;
    GetWorld()->GetTimerManager().ClearTimer(FishingTimerHandle);
    UE_LOG(LogTemp, Log, TEXT("Fishing net stopped"));
}

void AFishingNet::OnFishingTimerComplete()
{
    // Ловим 1 рыбу
    CatchSingleFish();
    
    // Проверяем условие для продолжения ловли
    if (CaughtFishIDs.Num() < 2 + NetLevel)
    {
        // Если сеть не заполнена, запускаем таймер снова
        GetWorld()->GetTimerManager().SetTimer(
            FishingTimerHandle,
            this,
            &AFishingNet::OnFishingTimerComplete,
            FishingInterval,
            false
        );
        
        UE_LOG(LogTemp, Log, TEXT("Fishing net continues. Next catch in %f seconds"), FishingInterval);
    }
    else
    {
        // Сеть заполнена - останавливаем ловлю
        bIsFishing = false;
        UE_LOG(LogTemp, Log, TEXT("Fishing net is full (capacity: %d). Stopping."), 2 + NetLevel);
    }
}

void AFishingNet::CatchSingleFish()
{
    if (!FishDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("FishDataTable not set in fishing net!"));
        return;
    }
    
    // Рассчитываем максимальную вместимость сети
    int32 MaxCapacity = 2 + NetLevel;
    
    // Проверяем, есть ли место в сети
    if (CaughtFishIDs.Num() >= MaxCapacity)
    {
        UE_LOG(LogTemp, Warning, TEXT("Fishing net is full, cannot catch more fish"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Fishing net catching 1 fish"));
    
    // Ловим 1 рыбу
    FFishData* FishData = GetRandomFish();
    if (FishData)
    {
        CaughtFishIDs.Add(FishData->FishID);
        UE_LOG(LogTemp, Log, TEXT("Fishing net caught fish ID: %d, Name: %s"), 
            FishData->FishID, *FishData->FishName);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Fishing net now contains %d/%d fish"), 
        CaughtFishIDs.Num(), MaxCapacity);
}

void AFishingNet::CollectAllFish()
{
    if (CaughtFishIDs.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No fish to collect in fishing net!"));
        return;
    }
    
    if (!FishDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("FishDataTable not set in fishing net!"));
        return;
    }
    
    // Получаем все строки из таблицы для быстрого поиска
    TArray<FFishData*> AllFish;
    FishDataTable->GetAllRows<FFishData>(TEXT(""), AllFish);
    
    // Создаем карту для быстрого поиска рыбы по ID
    TMap<int32, FFishData*> FishMap;
    for (FFishData* Fish : AllFish)
    {
        if (Fish)
        {
            FishMap.Add(Fish->FishID, Fish);
        }
    }
    
    // Считаем общую стоимость
    int32 TotalValue = 0;
    
    for (int32 FishID : CaughtFishIDs)
    {
        FFishData** FoundFish = FishMap.Find(FishID);
        if (FoundFish && *FoundFish)
        {
            TotalValue += (*FoundFish)->Price;
            UE_LOG(LogTemp, Verbose, TEXT("Selling fish ID: %d for %d money"), FishID, (*FoundFish)->Price);
        }
    }
    
    // Добавляем деньги
    AGameManager* GameManager = AGameManager::GetInstance();
    if (GameManager)
    {
        GameManager->AddMoney(TotalValue);
        UE_LOG(LogTemp, Log, TEXT("Collected and sold %d fish for total of %d money"), 
            CaughtFishIDs.Num(), TotalValue);
    }
    
    // Очищаем сеть
    CaughtFishIDs.Empty();
    
    // После сбора начинаем ловлю снова, если она была остановлена
    if (!bIsFishing)
    {
        StartFishing();
    }
}

int32 AFishingNet::GetFishCount() const
{
    return CaughtFishIDs.Num();
}

int32 AFishingNet::GetTotalValue() const
{
    if (!FishDataTable || CaughtFishIDs.Num() == 0) return 0;
    
    // Получаем все строки из таблицы
    TArray<FFishData*> AllFish;
    FishDataTable->GetAllRows<FFishData>(TEXT(""), AllFish);
    
    // Создаем карту для быстрого поиска
    TMap<int32, FFishData*> FishMap;
    for (FFishData* Fish : AllFish)
    {
        if (Fish)
        {
            FishMap.Add(Fish->FishID, Fish);
        }
    }
    
    // Считаем общую стоимость
    int32 TotalValue = 0;
    
    for (int32 FishID : CaughtFishIDs)
    {
        FFishData** FoundFish = FishMap.Find(FishID);
        if (FoundFish && *FoundFish)
        {
            TotalValue += (*FoundFish)->Price;
        }
    }
    
    return TotalValue;
}

FFishData* AFishingNet::GetRandomFish()
{
    if (!FishDataTable) return nullptr;
    
    // Получаем все строки из таблицы
    TArray<FFishData*> AllFish;
    FishDataTable->GetAllRows<FFishData>(TEXT(""), AllFish);
    
    if (AllFish.Num() == 0) return nullptr;
    
    // Рассчитываем общую вероятность
    float TotalProbability = 0.0f;
    for (FFishData* Fish : AllFish)
    {
        TotalProbability += Fish->Probability;
    }
    
    // Выбираем случайную рыбу на основе вероятностей
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