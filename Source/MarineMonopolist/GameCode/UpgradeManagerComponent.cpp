#include "UpgradeManagerComponent.h"
#include "UpgradeData.h"

UUpgradeManagerComponent::UUpgradeManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UUpgradeManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

FUpgradeLevelInfo* UUpgradeManagerComponent::GetUpgradeRow(UDataTable* Table, int32 Level) const
{
    if (!Table) return nullptr;

    FName RowName = FName(FString::FromInt(Level));
    return Table->FindRow<FUpgradeLevelInfo>(RowName, TEXT(""));
}

FShipUpgradeInfo* UUpgradeManagerComponent::GetShipUpgradeRow(int32 Level) const
{
    if (!ShipUpgradeTable) return nullptr;

    FName RowName = FName(FString::FromInt(Level));
    return ShipUpgradeTable->FindRow<FShipUpgradeInfo>(RowName, TEXT(""));
}

int32 UUpgradeManagerComponent::GetRodMaxLevel() const
{
    if (ShipLevel >= 2)
        return RodMaxLevel_AfterShipUpgrade;
    return RodMaxLevel_BeforeShipUpgrade;
}

int32 UUpgradeManagerComponent::GetNightBaitMaxLevel() const
{
    if (ShipLevel >= 2)
        return NightBaitMaxLevel;
    return 0;
}

int32 UUpgradeManagerComponent::GetRodLuckBonus() const
{
    FUpgradeLevelInfo* Row = GetUpgradeRow(RodUpgradeTable, RodLevel);
    return Row ? FMath::RoundToInt(Row->EffectValue) : 1;
}

float UUpgradeManagerComponent::GetNetInterval() const
{
    if (NetLevel <= 0) return 9999.0f;
    FUpgradeLevelInfo* Row = GetUpgradeRow(NetUpgradeTable, NetLevel);
    return Row ? Row->EffectValue : 10.0f;
}

float UUpgradeManagerComponent::GetNetCatchChanceMultiplier() const
{
    if (NetLevel <= 0) return 0.0f;
    FUpgradeLevelInfo* Row = GetUpgradeRow(NetUpgradeTable, NetLevel);
    return Row ? Row->SecondaryValue : 1.0f;
}

float UUpgradeManagerComponent::GetNightBaitChanceBonus() const
{
    FUpgradeLevelInfo* Row = GetUpgradeRow(NightBaitUpgradeTable, NightBaitLevel);
    return Row ? Row->EffectValue : 0.0f;
}

bool UUpgradeManagerComponent::CanUpgradeRod() const
{
    if (RodLevel >= GetRodMaxLevel()) return false;
    FUpgradeLevelInfo* Row = GetUpgradeRow(RodUpgradeTable, RodLevel + 1);
    return Row != nullptr;
}

bool UUpgradeManagerComponent::CanUpgradeNet() const
{
    if (NetLevel >= MaxNetLevel) return false;
    FUpgradeLevelInfo* Row = GetUpgradeRow(NetUpgradeTable, NetLevel + 1);
    return Row != nullptr;
}

bool UUpgradeManagerComponent::CanUpgradeNightBait() const
{
    if (NightBaitLevel >= GetNightBaitMaxLevel()) return false;
    FUpgradeLevelInfo* Row = GetUpgradeRow(NightBaitUpgradeTable, NightBaitLevel + 1);
    return Row != nullptr;
}

bool UUpgradeManagerComponent::CanUpgradeShip() const
{
    if (ShipLevel >= 3) return false;
    FShipUpgradeInfo* Row = GetShipUpgradeRow(ShipLevel + 1);
    if (!Row) return false;

    return RodLevel >= Row->RequiredRodLevel
        && NetLevel >= Row->RequiredNetLevel
        && NightBaitLevel >= Row->RequiredNightBaitLevel;
}

int32 UUpgradeManagerComponent::GetRodUpgradeCost() const
{
    if (RodLevel >= GetRodMaxLevel()) return -1;
    FUpgradeLevelInfo* Row = GetUpgradeRow(RodUpgradeTable, RodLevel + 1);
    return Row ? Row->Cost : -1;
}

int32 UUpgradeManagerComponent::GetNetUpgradeCost() const
{
    if (NetLevel >= MaxNetLevel) return -1;
    FUpgradeLevelInfo* Row = GetUpgradeRow(NetUpgradeTable, NetLevel + 1);
    return Row ? Row->Cost : -1;
}

int32 UUpgradeManagerComponent::GetNightBaitUpgradeCost() const
{
    if (NightBaitLevel >= GetNightBaitMaxLevel()) return -1;
    FUpgradeLevelInfo* Row = GetUpgradeRow(NightBaitUpgradeTable, NightBaitLevel + 1);
    return Row ? Row->Cost : -1;
}

int32 UUpgradeManagerComponent::GetShipUpgradeCost() const
{
    if (ShipLevel >= 3) return -1;
    FShipUpgradeInfo* Row = GetShipUpgradeRow(ShipLevel + 1);
    return Row ? Row->Cost : -1;
}

void UUpgradeManagerComponent::UpgradeRod()
{
    if (!CanUpgradeRod()) return;
    RodLevel++;
    OnRodUpgraded.Broadcast(RodLevel);
}

void UUpgradeManagerComponent::UpgradeNet()
{
    if (!CanUpgradeNet()) return;
    NetLevel++;
    OnNetUpgraded.Broadcast(NetLevel);
}

void UUpgradeManagerComponent::UpgradeNightBait()
{
    if (!CanUpgradeNightBait()) return;
    NightBaitLevel++;
    OnNightBaitUpgraded.Broadcast(NightBaitLevel);
}

void UUpgradeManagerComponent::UpgradeShip()
{
    if (!CanUpgradeShip()) return;
    ShipLevel++;
    OnShipUpgraded.Broadcast(ShipLevel);
}

void UUpgradeManagerComponent::ResetNetLevel(int32 NewLevel)
{
    NetLevel = FMath::Max(0, NewLevel);
    OnNetUpgraded.Broadcast(NetLevel);
}

void UUpgradeManagerComponent::ResetNightBaitLevel(int32 NewLevel)
{
    NightBaitLevel = FMath::Max(0, NewLevel);
    OnNightBaitUpgraded.Broadcast(NightBaitLevel);
}
