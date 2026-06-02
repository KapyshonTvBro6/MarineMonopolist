#include "Ship.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FishingNet.h"
#include "NightBaitDevice.h"

AShip::AShip()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = RootSceneComponent;

    ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMeshComponent"));
    ShipMeshComponent->SetupAttachment(RootComponent);

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

    NetPoint = CreateDefaultSubobject<USceneComponent>(TEXT("NetPoint"));
    NetPoint->SetupAttachment(RootComponent);
    NetPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

    NightBaitPoint = CreateDefaultSubobject<USceneComponent>(TEXT("NightBaitPoint"));
    NightBaitPoint->SetupAttachment(RootComponent);
    NightBaitPoint->SetRelativeLocation(FVector(-150.0f, 0.0f, 80.0f));
}

void AShip::AttachNet(AActor* NetActor)
{
    if (!NetActor || !NetPoint)
    {
        UE_LOG(LogTemp, Error, TEXT("Trying to attach null net to ship!"));
        return;
    }

    NetActor->AttachToComponent(NetPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
    NetActor->SetActorRelativeLocation(FVector::ZeroVector);
    NetActor->SetActorRelativeRotation(FRotator::ZeroRotator);

    UE_LOG(LogTemp, Log, TEXT("Net attached to ship"));
}

void AShip::RemoveNet()
{
    UE_LOG(LogTemp, Log, TEXT("Net point cleared"));

    UE_LOG(LogTemp, Log, TEXT("Net removed from ship"));
}

void AShip::AttachNightBait(AActor* DeviceActor)
{
    if (!DeviceActor || !NightBaitPoint)
    {
        UE_LOG(LogTemp, Error, TEXT("Trying to attach null night bait device to ship!"));
        return;
    }

    DeviceActor->AttachToComponent(NightBaitPoint, FAttachmentTransformRules::SnapToTargetIncludingScale);
    DeviceActor->SetActorRelativeLocation(FVector::ZeroVector);
    DeviceActor->SetActorRelativeRotation(FRotator::ZeroRotator);

    UE_LOG(LogTemp, Log, TEXT("Night bait device attached to ship"));
}

void AShip::SetShipMesh(int32 Level)
{
    ShipLevel = FMath::Clamp(Level, 1, ShipMeshes.Num());

    if (ShipMeshes.IsValidIndex(ShipLevel - 1) && ShipMeshes[ShipLevel - 1])
    {
        ShipMeshComponent->SetStaticMesh(ShipMeshes[ShipLevel - 1]);
        UE_LOG(LogTemp, Log, TEXT("Ship mesh set to level %d"), ShipLevel);
    }
}

UStaticMesh* AShip::GetNetMeshForLevel(int32 Level) const
{
    int32 Index = Level - 1;
    if (NetMeshes.IsValidIndex(Index))
    {
        return NetMeshes[Index];
    }
    return nullptr;
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
