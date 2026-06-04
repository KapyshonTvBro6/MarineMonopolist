#include "DayNightManagerComponent.h"
#include "Engine/DirectionalLight.h"

UDayNightManagerComponent::UDayNightManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UDayNightManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    SunAngle = 0.0f;
    bIsNight = false;

    if (SunLight)
    {
        BaseSunRotation = SunLight->GetActorRotation();
    }
}

void UDayNightManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    SunAngle += (DeltaTime / FullCycleTime) * 360.0f;
    if (SunAngle >= 360.0f) SunAngle -= 360.0f;

    if (SunLight)
    {
        FRotator NewRot = BaseSunRotation;
        NewRot.Pitch += SunAngle;
        //NewRot.Yaw += SunAngle;
        SunLight->SetActorRotation(NewRot);
    }

    bool bNewIsNight = SunAngle <= 180.0f;
    if (bNewIsNight != bIsNight)
    {
        bIsNight = bNewIsNight;
        OnDayNightChanged.Broadcast(bIsNight);
    }
}
