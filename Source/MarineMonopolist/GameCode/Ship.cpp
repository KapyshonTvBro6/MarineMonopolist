#include "Ship.h"
#include "Components/SceneComponent.h"

AShip::AShip()
{
	PrimaryActorTick.bCanEverTick = true;
    
	// Создаем корневой компонент
	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;
    
	// Создаем точки крепления в конструкторе
	CreateAttachmentPoints();
}

void AShip::BeginPlay()
{
	Super::BeginPlay();
}

void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AShip::CreateAttachmentPoints()
{
	// Точка для основного рыбака
	FishermanPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FishermanPoint"));
	FishermanPoint->SetupAttachment(RootComponent);
	FishermanPoint->SetRelativeLocation(FVector(200.0f, 0.0f, 100.0f));
    
	// Точка для второго рыбака (мультиплеер)
	SecondFishermanPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SecondFishermanPoint"));
	SecondFishermanPoint->SetupAttachment(RootComponent);
	SecondFishermanPoint->SetRelativeLocation(FVector(-200.0f, 0.0f, 100.0f));
    
	// Создаем начальные точки для сетей (только в конструкторе!)
	for (int32 i = 0; i < MaxNets; i++)
	{
		FString PointName = FString::Printf(TEXT("NetPoint_%d"), i);
		USceneComponent* NetPoint = CreateDefaultSubobject<USceneComponent>(*PointName);
		NetPoint->SetupAttachment(RootComponent);
        
		// Располагаем сети по бокам корабля
		float YOffset = -150.0f + (i * 100.0f);
		NetPoint->SetRelativeLocation(FVector(0.0f, YOffset, 50.0f));
        
		NetPoints.Add(NetPoint);
	}
}

USceneComponent* AShip::GetAvailableNetPoint()
{
	// Ищем первую точку, к которой еще не прикреплена сеть
	for (int32 i = 0; i < NetPoints.Num(); i++)
	{
		if (i < AttachedNets.Num())
		{
			// Если уже есть сеть на этой точке, пропускаем
			if (AttachedNets[i] != nullptr)
			{
				continue;
			}
		}
		// Если точка свободна или индекс больше количества сетей
		return NetPoints[i];
	}
    
	return nullptr;
}

void AShip::AttachNet(AActor* NetActor)
{
	if (!NetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to attach null net to ship!"));
		return;
	}
    
	// Получаем свободную точку
	USceneComponent* NetPoint = GetAvailableNetPoint();
	if (!NetPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("No available net points on ship!"));
		return;
	}
    
	// Прикрепляем сеть к точке
	NetActor->AttachToComponent(NetPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
	NetActor->SetActorRelativeLocation(FVector::ZeroVector);
	NetActor->SetActorRelativeRotation(FRotator::ZeroRotator);
    
	// Добавляем сеть в массив
	AttachedNets.Add(NetActor);
    
	UE_LOG(LogTemp, Log, TEXT("Net attached to ship. Total nets: %d"), AttachedNets.Num());
}

void AShip::Upgrade()
{
	ShipLevel++;
	MaxNets = BaseMaxNets + (ShipLevel - 1) * NetsPerLevel;
    
	UE_LOG(LogTemp, Log, TEXT("Ship upgraded to level %d. Max nets: %d"), ShipLevel, MaxNets);
    
	// Если нужно создать новые точки для сетей
	if (NetPoints.Num() < MaxNets)
	{
		int32 NetsToAdd = MaxNets - NetPoints.Num();
		for (int32 i = 0; i < NetsToAdd; i++)
		{
			AddNewNetPoint();
		}
	}
}

void AShip::AddNewNetPoint()
{
	// Этот метод вызывается во время выполнения, поэтому используем NewObject
	int32 NewIndex = NetPoints.Num();
	FString PointName = FString::Printf(TEXT("RuntimeNetPoint_%d"), NewIndex);
    
	// Создаем новую точку для сети во время выполнения
	USceneComponent* NewNetPoint = CreateNetPointComponent(NewIndex);
    
	if (NewNetPoint)
	{
		NetPoints.Add(NewNetPoint);
		UE_LOG(LogTemp, Log, TEXT("Added new net point. Total net points: %d"), NetPoints.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create new net point!"));
	}
}

USceneComponent* AShip::CreateNetPointComponent(int32 Index)
{
	// ВАЖНО: Этот метод работает во время выполнения, используем NewObject
	FString PointName = FString::Printf(TEXT("RuntimeNetPoint_%d"), Index);
    
	// Создаем компонент сцены с помощью NewObject
	USceneComponent* NewNetPoint = NewObject<USceneComponent>(this, *PointName);
	if (!NewNetPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create NewObject for net point!"));
		return nullptr;
	}
    
	// Настраиваем компонент
	NewNetPoint->SetupAttachment(RootComponent);
	NewNetPoint->RegisterComponent();
    
	// Располагаем новую точку
	float YOffset = -150.0f + (Index * 100.0f);
	NewNetPoint->SetRelativeLocation(FVector(0.0f, YOffset, 50.0f));
    
	return NewNetPoint;
}