#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "DayNightManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayNightChanged, bool, bIsNight);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARINEMONOPOLIST_API UDayNightManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDayNightManagerComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Day/Night")
    bool IsNight() const { return bIsNight; }

    UFUNCTION(BlueprintCallable, Category = "Day/Night")
    float GetTimeUntilTransition() const { return TimeUntilTransition; }

    UFUNCTION(BlueprintCallable, Category = "Day/Night|Fish")
    UDataTable* GetDayFishTable() const { return DayFishTable; }

    UFUNCTION(BlueprintCallable, Category = "Day/Night|Fish")
    UDataTable* GetNightFishTable() const { return NightFishTable; }

    UFUNCTION(BlueprintCallable, Category = "Day/Night|Fish")
    float GetBaseNightFishChance() const { return BaseNightFishChance; }

    UPROPERTY(BlueprintAssignable, Category = "Day/Night")
    FOnDayNightChanged OnDayNightChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night|Fish")
    UDataTable* DayFishTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night|Fish")
    UDataTable* NightFishTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night")
    float DayDuration = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night")
    float NightDuration = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Day/Night|Fish")
    float BaseNightFishChance = 0.05f;

protected:
    virtual void BeginPlay() override;

private:
    bool bIsNight = false;
    float TimeUntilTransition = 60.0f;

    void TransitionToDay();
    void TransitionToNight();
};
