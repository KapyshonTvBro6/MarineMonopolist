#include "GameManager.h"
#include "Fisherman.h"
#include "Ship.h"
#include "FishingNet.h"
#include "NightBaitDevice.h"
#include "DayNightManagerComponent.h"
#include "UpgradeManagerComponent.h"
#include "GameUIWidget.h"
#include "Engine/World.h"

AGameManager* AGameManager::Instance = nullptr;

AGameManager::AGameManager()
{
    PrimaryActorTick.bCanEverTick = false;
    Money = 0;
    Fisherman = nullptr;
    CurrentShip = nullptr;
    CurrentNet = nullptr;
    NightBaitDevice = nullptr;

    DayNightManager = CreateDefaultSubobject<UDayNightManagerComponent>(TEXT("DayNightManager"));
    UpgradeManager = CreateDefaultSubobject<UUpgradeManagerComponent>(TEXT("UpgradeManager"));
}

void AGameManager::BeginPlay()
{
    Super::BeginPlay();
    Instance = this;
    InitializeGame();
}

AGameManager* AGameManager::GetInstance()
{
    return Instance;
}

void AGameManager::InitializeGame()
{
    UE_LOG(LogTemp, Log, TEXT("Initializing game..."));

    CreateShip();
    CreateFisherman();
    PlaceFishermanOnShip();
    CreateGameUIWidget();

    if (Fisherman && GameUIWidget)
    {
        Fisherman->OnFishermanFishCaughtFull.AddDynamic(this, &AGameManager::OnFishermanCatchesFish);
    }

    BroadcastMoneyChanged();
    UE_LOG(LogTemp, Log, TEXT("Game initialized. Money: %d"), Money);
}

void AGameManager::CreateShip()
{
    if (!ShipClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShipClass not set in GameManager!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    CurrentShip = World->SpawnActor<AShip>(ShipClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    if (CurrentShip)
    {
        CurrentShip->SetShipMesh(1);
        UE_LOG(LogTemp, Log, TEXT("Ship created successfully"));
    }
}

void AGameManager::CreateFisherman()
{
    if (!FishermanClass)
    {
        UE_LOG(LogTemp, Error, TEXT("FishermanClass not set in GameManager!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    Fisherman = World->SpawnActor<AFisherman>(FishermanClass, FVector(100.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);

    if (Fisherman)
    {
        UE_LOG(LogTemp, Log, TEXT("Fisherman created successfully"));
    }
}

void AGameManager::PlaceFishermanOnShip()
{
    if (!CurrentShip || !Fisherman)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot place fisherman: ship or fisherman is null!"));
        return;
    }

    USceneComponent* FishermanPoint = CurrentShip->GetFishermanPoint();
    if (!FishermanPoint) return;

    Fisherman->AttachToComponent(FishermanPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
    Fisherman->SetActorRelativeLocation(FVector::ZeroVector);
    Fisherman->SetActorRelativeRotation(FRotator::ZeroRotator);
}

void AGameManager::SpawnNet()
{
    if (!FishingNetClass || !CurrentShip)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot spawn net: missing class or ship"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    CurrentNet = World->SpawnActor<AFishingNet>(FishingNetClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    if (CurrentNet)
    {
        CurrentShip->AttachNet(CurrentNet);

        UDataTable* NetTable = DayNightManager ? DayNightManager->GetDayFishTable() : nullptr;
        if (!NetTable && Fisherman)
        {
            NetTable = Fisherman->GetFishDataTable();
        }
        if (NetTable)
        {
            CurrentNet->SetFishDataTable(NetTable);
            UE_LOG(LogTemp, Log, TEXT("Net FishDataTable set: %s"), *NetTable->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Net FishDataTable is NULL (no DayFishTable or Fisherman table)!"));
        }

        CurrentNet->SetCatchInterval(UpgradeManager->GetNetInterval());
        CurrentNet->SetCatchChanceMultiplier(UpgradeManager->GetNetCatchChanceMultiplier());

        UStaticMesh* NetMesh = CurrentShip->GetNetMeshForLevel(UpgradeManager->GetShipLevel());
        if (NetMesh)
        {
            CurrentNet->SetNetMesh(NetMesh);
        }

        CurrentNet->OnNetFishCaught.AddDynamic(this, &AGameManager::OnNetCatchesFish);
        CurrentNet->StartFishing();

        UE_LOG(LogTemp, Log, TEXT("Net spawned and attached to ship"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn net!"));
    }
}

void AGameManager::SpawnNightBaitDevice()
{
    if (!NightBaitDeviceClass || !CurrentShip) return;

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    NightBaitDevice = World->SpawnActor<ANightBaitDevice>(NightBaitDeviceClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

    if (NightBaitDevice && CurrentShip)
    {
        CurrentShip->AttachNightBait(NightBaitDevice);
        NightBaitDevice->SetLevel(0);
        UE_LOG(LogTemp, Log, TEXT("Night bait device spawned on ship"));
    }
}

void AGameManager::AddMoney(int32 Amount)
{
    Money += Amount;
    BroadcastMoneyChanged();
}

bool AGameManager::SpendMoney(int32 Amount)
{
    if (Money < Amount)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough money! Need: %d, Have: %d"), Amount, Money);
        return false;
    }

    Money -= Amount;
    BroadcastMoneyChanged();
    return true;
}

int32 AGameManager::GetMoney() const
{
    return Money;
}

UDataTable* AGameManager::GetActiveFishTable() const
{
    if (!DayNightManager) return nullptr;

    if (DayNightManager->IsNight())
    {
        float NightChance = DayNightManager->GetBaseNightFishChance();

        if (UpgradeManager && UpgradeManager->GetNightBaitLevel() > 0)
        {
            NightChance += UpgradeManager->GetNightBaitChanceBonus();
        }

        NightChance = FMath::Clamp(NightChance, 0.0f, 1.0f);

        if (FMath::FRand() <= NightChance)
        {
            UDataTable* NightTable = DayNightManager->GetNightFishTable();
            if (IsValid(NightTable) && NightTable->GetRowStruct())
            {
                return NightTable;
            }
        }
    }

    UDataTable* DayTable = DayNightManager->GetDayFishTable();
    if (IsValid(DayTable) && DayTable->GetRowStruct())
    {
        return DayTable;
    }

    return nullptr;
}

bool AGameManager::UpgradeRod()
{
    if (!UpgradeManager || !UpgradeManager->CanUpgradeRod()) return false;

    int32 Cost = UpgradeManager->GetRodUpgradeCost();
    if (Cost < 0 || !SpendMoney(Cost)) return false;

    UpgradeManager->UpgradeRod();
    return true;
}

bool AGameManager::UpgradeNet()
{
    if (!UpgradeManager || !UpgradeManager->CanUpgradeNet()) return false;

    int32 Cost = UpgradeManager->GetNetUpgradeCost();
    if (Cost < 0 || !SpendMoney(Cost)) return false;

    int32 OldLevel = UpgradeManager->GetNetLevel();
    UpgradeManager->UpgradeNet();
    int32 NewLevel = UpgradeManager->GetNetLevel();

    if (OldLevel == 0 && NewLevel == 1)
    {
        SpawnNet();
    }
    else if (CurrentNet)
    {
        CurrentNet->SetCatchInterval(UpgradeManager->GetNetInterval());
        CurrentNet->SetCatchChanceMultiplier(UpgradeManager->GetNetCatchChanceMultiplier());
    }

    return true;
}

bool AGameManager::UpgradeNightBait()
{
    if (!UpgradeManager || !UpgradeManager->CanUpgradeNightBait()) return false;

    int32 Cost = UpgradeManager->GetNightBaitUpgradeCost();
    if (Cost < 0 || !SpendMoney(Cost)) return false;

    UpgradeManager->UpgradeNightBait();

    if (NightBaitDevice)
    {
        NightBaitDevice->SetLevel(UpgradeManager->GetNightBaitLevel());
    }

    return true;
}

bool AGameManager::UpgradeShip()
{
    if (!UpgradeManager || !UpgradeManager->CanUpgradeShip()) return false;

    int32 Cost = UpgradeManager->GetShipUpgradeCost();
    if (Cost < 0 || !SpendMoney(Cost)) return false;

    int32 OldLevel = UpgradeManager->GetShipLevel();
    UpgradeManager->UpgradeShip();
    int32 NewLevel = UpgradeManager->GetShipLevel();

    if (CurrentShip)
    {
        CurrentShip->SetShipMesh(NewLevel);
    }

    if (OldLevel == 1 && NewLevel == 2)
    {
        if (CurrentShip) CurrentShip->RemoveNet();
        if (CurrentNet)
        {
            CurrentNet->Destroy();
            CurrentNet = nullptr;
        }
        UpgradeManager->ResetNetLevel(0);

        SpawnNightBaitDevice();
    }
    else if (OldLevel == 2 && NewLevel == 3)
    {
        if (CurrentShip) CurrentShip->RemoveNet();
        if (CurrentNet)
        {
            CurrentNet->Destroy();
            CurrentNet = nullptr;
        }
        UpgradeManager->ResetNetLevel(0);
    }

    return true;
}

void AGameManager::BroadcastMoneyChanged()
{
    OnMoneyChanged.Broadcast(Money);
}

void AGameManager::CreateGameUIWidget()
{
    if (!GameUIWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameUIWidgetClass not set in GameManager!"));
        return;
    }

    GameUIWidget = CreateWidget<UGameUIWidget>(
        GetWorld(),
        GameUIWidgetClass
    );

    if (GameUIWidget)
    {
        GameUIWidget->AddToViewport(100);
    }
}

void AGameManager::OnFishermanCatchesFish(FFishData FishData)
{
    if (GameUIWidget)
    {
        GameUIWidget->AddFishNotification(FishData);
    }
}

void AGameManager::OnNetCatchesFish(FFishData FishData)
{
    if (GameUIWidget)
    {
        GameUIWidget->AddFishNotification(FishData);
    }
}
