#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "UpgradeManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRodUpgraded, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetUpgraded, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNightBaitUpgraded, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShipUpgraded, int32, NewLevel);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARINEMONOPOLIST_API UUpgradeManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UUpgradeManagerComponent();

    UFUNCTION(BlueprintCallable, Category = "Upgrades")
    int32 GetRodLevel() const { return RodLevel; }

    UFUNCTION(BlueprintCallable, Category = "Upgrades")
    int32 GetNetLevel() const { return NetLevel; }

    UFUNCTION(BlueprintCallable, Category = "Upgrades")
    int32 GetNightBaitLevel() const { return NightBaitLevel; }

    UFUNCTION(BlueprintCallable, Category = "Upgrades")
    int32 GetShipLevel() const { return ShipLevel; }

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Caps")
    int32 GetRodMaxLevel() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Caps")
    int32 GetNetMaxLevel() const { return MaxNetLevel; }

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Caps")
    int32 GetNightBaitMaxLevel() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Effects")
    int32 GetRodLuckBonus() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Effects")
    float GetNetInterval() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Effects")
    float GetNetCatchChanceMultiplier() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Effects")
    float GetNightBaitChanceBonus() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Checks")
    bool CanUpgradeRod() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Checks")
    bool CanUpgradeNet() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Checks")
    bool CanUpgradeNightBait() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Checks")
    bool CanUpgradeShip() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Costs")
    int32 GetRodUpgradeCost() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Costs")
    int32 GetNetUpgradeCost() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Costs")
    int32 GetNightBaitUpgradeCost() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Costs")
    int32 GetShipUpgradeCost() const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Actions")
    void UpgradeRod();

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Actions")
    void UpgradeNet();

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Actions")
    void UpgradeNightBait();

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Actions")
    void UpgradeShip();

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Actions")
    void ResetNetLevel(int32 NewLevel);

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Actions")
    void ResetNightBaitLevel(int32 NewLevel);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Tables")
    UDataTable* RodUpgradeTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Tables")
    UDataTable* NetUpgradeTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Tables")
    UDataTable* NightBaitUpgradeTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Tables")
    UDataTable* ShipUpgradeTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Caps")
    int32 RodMaxLevel_BeforeShipUpgrade = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Caps")
    int32 RodMaxLevel_AfterShipUpgrade = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Caps")
    int32 MaxNetLevel = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Caps")
    int32 NightBaitMaxLevel = 3;

    UPROPERTY(BlueprintAssignable, Category = "Upgrades|Events")
    FOnRodUpgraded OnRodUpgraded;

    UPROPERTY(BlueprintAssignable, Category = "Upgrades|Events")
    FOnNetUpgraded OnNetUpgraded;

    UPROPERTY(BlueprintAssignable, Category = "Upgrades|Events")
    FOnNightBaitUpgraded OnNightBaitUpgraded;

    UPROPERTY(BlueprintAssignable, Category = "Upgrades|Events")
    FOnShipUpgraded OnShipUpgraded;

protected:
    virtual void BeginPlay() override;

private:
    int32 RodLevel = 1;
    int32 NetLevel = 0;
    int32 NightBaitLevel = 0;
    int32 ShipLevel = 1;

    struct FUpgradeLevelInfo* GetUpgradeRow(UDataTable* Table, int32 Level) const;
    struct FShipUpgradeInfo* GetShipUpgradeRow(int32 Level) const;
};
