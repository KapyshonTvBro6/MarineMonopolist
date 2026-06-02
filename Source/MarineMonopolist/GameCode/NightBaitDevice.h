#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NightBaitDevice.generated.h"

UCLASS()
class MARINEMONOPOLIST_API ANightBaitDevice : public AActor
{
    GENERATED_BODY()

public:
    ANightBaitDevice();

    UFUNCTION(BlueprintCallable, Category = "Night Bait")
    void SetLevel(int32 NewLevel);

    UFUNCTION(BlueprintCallable, Category = "Night Bait")
    int32 GetBaitLevel() const { return Level; }

    UFUNCTION(BlueprintCallable, Category = "Night Bait")
    bool IsActive() const { return Level > 0; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Night Bait")
    TArray<UStaticMesh*> LevelMeshes;

private:
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* MeshComponent;

    int32 Level = 0;
};
