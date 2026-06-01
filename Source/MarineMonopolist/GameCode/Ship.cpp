#include "Ship.h"
#include "Components/SceneComponent.h"

AShip::AShip()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootSceneComponent;

	CreateAttachmentPoints();
}

void AShip::BeginPlay()
{
	Super::BeginPlay();
	BaseWaterZ = GetActorLocation().Z;
}

void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bFloatOnWaves)
	{
		UpdateFloatOnWaves(DeltaTime);
	}
}

void AShip::CreateAttachmentPoints()
{
	FishermanPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FishermanPoint"));
	FishermanPoint->SetupAttachment(RootComponent);
	FishermanPoint->SetRelativeLocation(FVector(200.0f, 0.0f, 100.0f));

	SecondFishermanPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SecondFishermanPoint"));
	SecondFishermanPoint->SetupAttachment(RootComponent);
	SecondFishermanPoint->SetRelativeLocation(FVector(-200.0f, 0.0f, 100.0f));

	for (int32 i = 0; i < MaxNets; i++)
	{
		FString PointName = FString::Printf(TEXT("NetPoint_%d"), i);
		USceneComponent* NetPoint = CreateDefaultSubobject<USceneComponent>(*PointName);
		NetPoint->SetupAttachment(RootComponent);

		float YOffset = -150.0f + (i * 100.0f);
		NetPoint->SetRelativeLocation(FVector(0.0f, YOffset, 50.0f));

		NetPoints.Add(NetPoint);
	}
}

USceneComponent* AShip::GetAvailableNetPoint()
{
	for (int32 i = 0; i < NetPoints.Num(); i++)
	{
		if (i < AttachedNets.Num())
		{
			if (AttachedNets[i] != nullptr)
			{
				continue;
			}
		}
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

	USceneComponent* NetPoint = GetAvailableNetPoint();
	if (!NetPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("No available net points on ship!"));
		return;
	}

	NetActor->AttachToComponent(NetPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
	NetActor->SetActorRelativeLocation(FVector::ZeroVector);
	NetActor->SetActorRelativeRotation(FRotator::ZeroRotator);

	AttachedNets.Add(NetActor);

	UE_LOG(LogTemp, Log, TEXT("Net attached to ship. Total nets: %d"), AttachedNets.Num());
}

void AShip::Upgrade()
{
	ShipLevel++;
	MaxNets = BaseMaxNets + (ShipLevel - 1) * NetsPerLevel;

	UE_LOG(LogTemp, Log, TEXT("Ship upgraded to level %d. Max nets: %d"), ShipLevel, MaxNets);

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
	int32 NewIndex = NetPoints.Num();
	FString PointName = FString::Printf(TEXT("RuntimeNetPoint_%d"), NewIndex);

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
	FString PointName = FString::Printf(TEXT("RuntimeNetPoint_%d"), Index);

	USceneComponent* NewNetPoint = NewObject<USceneComponent>(this, *PointName);
	if (!NewNetPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create NewObject for net point!"));
		return nullptr;
	}

	NewNetPoint->SetupAttachment(RootComponent);
	NewNetPoint->RegisterComponent();

	float YOffset = -150.0f + (Index * 100.0f);
	NewNetPoint->SetRelativeLocation(FVector(0.0f, YOffset, 50.0f));

	return NewNetPoint;
}

void AShip::UpdateFloatOnWaves(float DeltaTime)
{
	WaveTime += DeltaTime;

	float BobOffset = FMath::Sin(WaveTime * BobSpeed) * BobAmplitude;

	FVector NewLoc = GetActorLocation();
	NewLoc.Z = BaseWaterZ + BobOffset;
	SetActorLocation(NewLoc);

	float Pitch = FMath::Sin(WaveTime * PitchSpeed) * PitchAmplitude;
	float Roll = FMath::Sin(WaveTime * RollSpeed) * RollAmplitude;

	FRotator ShipRot = GetActorRotation();
	ShipRot.Pitch = Pitch;
	ShipRot.Roll = Roll;
	SetActorRotation(ShipRot);
}
