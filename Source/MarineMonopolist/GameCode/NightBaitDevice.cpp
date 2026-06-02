#include "NightBaitDevice.h"
#include "Components/StaticMeshComponent.h"

ANightBaitDevice::ANightBaitDevice()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
    MeshComponent->SetVisibility(false);
}

void ANightBaitDevice::SetLevel(int32 NewLevel)
{
    Level = FMath::Clamp(NewLevel, 0, LevelMeshes.Num());

    if (Level > 0 && LevelMeshes.IsValidIndex(Level - 1))
    {
        MeshComponent->SetStaticMesh(LevelMeshes[Level - 1]);
        MeshComponent->SetVisibility(true);
    }
    else
    {
        MeshComponent->SetVisibility(false);
    }
}
