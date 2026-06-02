#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UpgradeData.generated.h"

USTRUCT(BlueprintType)
struct FUpgradeLevelInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Cost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Rod: luck value / Net: catch interval (sec) / NightBait: chance bonus (0.01=+1%)"))
    float EffectValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Net: catch chance multiplier (1.0=100%). Ignored for others."))
    float SecondaryValue;
};

USTRUCT(BlueprintType)
struct FShipUpgradeInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Cost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredRodLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredNetLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredNightBaitLevel;
};
