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
    TimeUntilTransition = DayDuration;
    bIsNight = false;
}

void UDayNightManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TimeUntilTransition -= DeltaTime;

    if (TimeUntilTransition <= 0.0f)
    {
        if (bIsNight)
        {
            TransitionToDay();
        }
        else
        {
            TransitionToNight();
        }
    }

    if (DirectionalLight)
    {
        float Progress;
        FRotator StartRot, EndRot;

        if (bIsNight)
        {
            Progress = 1.0f - (TimeUntilTransition / NightDuration);
            StartRot = MoonriseRotation;
            EndRot = MoonsetRotation;
        }
        else
        {
            Progress = 1.0f - (TimeUntilTransition / DayDuration);
            StartRot = SunriseRotation;
            EndRot = SunsetRotation;
        }

        DirectionalLight->SetActorRotation(FMath::Lerp(StartRot, EndRot, Progress));
    }
}

void UDayNightManagerComponent::TransitionToDay()
{
    bIsNight = false;
    TimeUntilTransition = DayDuration;
    OnDayNightChanged.Broadcast(false);
}

void UDayNightManagerComponent::TransitionToNight()
{
    bIsNight = true;
    TimeUntilTransition = NightDuration;
    OnDayNightChanged.Broadcast(true);
}
