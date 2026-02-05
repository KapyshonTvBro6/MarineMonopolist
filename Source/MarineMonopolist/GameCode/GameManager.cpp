#include "GameManager.h"
#include "Fisherman.h"
#include "Ship.h"
#include "Engine/World.h"

AGameManager* AGameManager::Instance = nullptr;

AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = false;
	Money = 0; // Начальные деньги для тестирования
	Fisherman = nullptr;
	CurrentShip = nullptr;
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	Instance = this;
    
	// Инициализируем игру
	InitializeGame();
}

AGameManager* AGameManager::GetInstance()
{
	return Instance;
}

void AGameManager::InitializeGame()
{
	UE_LOG(LogTemp, Log, TEXT("Initializing game..."));
    
	// Создаем корабль
	CreateShip();
    
	// Создаем рыбака
	CreateFisherman();
    
	// Размещаем рыбака на корабле
	PlaceFishermanOnShip();
    
	// Вызываем событие изменения денег (чтобы UI обновился с начальными деньгами)
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
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null!"));
		return;
	}
    
	// Спавним корабль
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
	FVector SpawnLocation = FVector(0.0f, 0.0f, 0.0f);
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
    
	CurrentShip = World->SpawnActor<AShip>(ShipClass, SpawnLocation, SpawnRotation, SpawnParams);
    
	if (CurrentShip)
	{
		UE_LOG(LogTemp, Log, TEXT("Ship created successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create ship!"));
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
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null!"));
		return;
	}
    
	// Спавним рыбака
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
	// Временная позиция, потом переместим на корабль
	FVector SpawnLocation = FVector(100.0f, 0.0f, 100.0f);
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
    
	Fisherman = World->SpawnActor<AFisherman>(FishermanClass, SpawnLocation, SpawnRotation, SpawnParams);
    
	if (Fisherman)
	{
		UE_LOG(LogTemp, Log, TEXT("Fisherman created successfully"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create fisherman!"));
	}
}

void AGameManager::PlaceFishermanOnShip()
{
	if (!CurrentShip || !Fisherman)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot place fisherman: ship or fisherman is null!"));
		return;
	}
    
	// Получаем точку для рыбака на корабле
	USceneComponent* FishermanPoint = CurrentShip->GetFishermanPoint();
	if (!FishermanPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("Ship has no fisherman point!"));
		return;
	}
    
	// Прикрепляем рыбака к точке на корабле
	Fisherman->AttachToComponent(FishermanPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
	Fisherman->SetActorRelativeLocation(FVector::ZeroVector);
	Fisherman->SetActorRelativeRotation(FRotator::ZeroRotator);
    
	UE_LOG(LogTemp, Log, TEXT("Fisherman placed on ship"));
}

void AGameManager::AddMoney(int32 Amount)
{
	Money += Amount;
	UE_LOG(LogTemp, Log, TEXT("Added %d money. Total: %d"), Amount, Money);
    
	// Вызываем событие изменения денег
	BroadcastMoneyChanged();
}

bool AGameManager::SpendMoney(int32 Amount)
{
	if (Money < Amount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough money to spend %d! Have: %d"), Amount, Money);
		return false;
	}
    
	Money -= Amount;
	UE_LOG(LogTemp, Log, TEXT("Spent %d money. Remaining: %d"), Amount, Money);
    
	// Вызываем событие изменения денег
	BroadcastMoneyChanged();
    
	return true;
}

int32 AGameManager::GetMoney() const
{
	return Money;
}

AFishingNet* AGameManager::BuyNet()
{
	// Проверяем, достаточно ли денег
	if (Money < NetCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough money to buy net! Need: %d, Have: %d"), NetCost, Money);
		return nullptr;
	}
    
	// Проверяем, есть ли корабль
	if (!CurrentShip)
	{
		UE_LOG(LogTemp, Error, TEXT("No ship to attach net to!"));
		return nullptr;
	}
    
	// Проверяем, есть ли свободные места для сетей
	if (CurrentShip->GetNetCount() >= CurrentShip->GetMaxNets())
	{
		UE_LOG(LogTemp, Warning, TEXT("Ship has no available net points!"));
		return nullptr;
	}
    
	// Проверяем класс сети
	if (!FishingNetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("FishingNetClass not set in GameManager!"));
		return nullptr;
	}
    
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("World is null!"));
		return nullptr;
	}
    
	// Спавним сеть
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
	// Временная позиция, потом переместим на корабль
	FVector SpawnLocation = FVector(0.0f, 0.0f, 50.0f);
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);
    
	AFishingNet* NewNet = World->SpawnActor<AFishingNet>(FishingNetClass, SpawnLocation, SpawnRotation, SpawnParams);
    
	if (NewNet)
	{
		// Снимаем деньги за покупку
		if (!SpendMoney(NetCost))
		{
			// Если не удалось потратить деньги, уничтожаем сеть и возвращаем nullptr
			NewNet->Destroy();
			return nullptr;
		}
        
		// Прикрепляем сеть к кораблю
		CurrentShip->AttachNet(NewNet);
        
		// Добавляем сеть в массив
		FishingNets.Add(NewNet);
        
		// Вызываем событие покупки сети
		BroadcastNetPurchased(NewNet);
        
		UE_LOG(LogTemp, Log, TEXT("Net created and attached to ship. Total nets: %d"), FishingNets.Num());
		return NewNet;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create net!"));
		return nullptr;
	}
}

bool AGameManager::UpgradeShip()
{
	// Используем SpendMoney для проверки и списания денег
	if (SpendMoney(ShipUpgradeCost) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough money to upgrade ship! Need: %d"), ShipUpgradeCost);
		return false;
	}
    
	// Проверяем, есть ли корабль
	if (!CurrentShip)
	{
		UE_LOG(LogTemp, Error, TEXT("No ship to upgrade!"));
		// Возвращаем деньги
		Money += ShipUpgradeCost;
		BroadcastMoneyChanged();
		return false;
	}
    
	// Вызываем метод улучшения корабля
	CurrentShip->Upgrade();
    
	// Получаем новый уровень корабля
	int32 NewLevel = CurrentShip->GetShipLevel();
    
	// Вызываем событие
	BroadcastShipUpgraded(NewLevel);
    
	UE_LOG(LogTemp, Log, TEXT("Ship upgraded to level %d. Max nets: %d"), 
		NewLevel, CurrentShip->GetMaxNets());
    
	return true;
}

void AGameManager::BroadcastMoneyChanged()
{
	// Вызываем событие Blueprint
	OnMoneyChanged.Broadcast(Money);
}

void AGameManager::BroadcastNetPurchased(AFishingNet* Net)
{
	// Вызываем событие Blueprint
	OnNetPurchased.Broadcast(Net);
}

void AGameManager::BroadcastShipUpgraded(int32 NewLevel)
{
	// Вызываем событие Blueprint
	OnShipUpgraded.Broadcast(NewLevel);
}