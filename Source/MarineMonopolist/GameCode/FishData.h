#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FishData.generated.h"

USTRUCT(BlueprintType)
struct FFishData : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 FishID;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString FishName;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Probability; // 0.0 - 1.0
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UStaticMesh* FishModel;
};