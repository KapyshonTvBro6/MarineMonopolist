#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "FishData.h"
#include "FishingNet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetFishCaught, FFishData, FishData);

UCLASS()
class MARINEMONOPOLIST_API AFishingNet : public AActor
{
    GENERATED_BODY()

public:
    AFishingNet();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    void StartFishing();

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    void StopFishing();

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    void CollectAllFish();

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    void SetCatchInterval(float Seconds);

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    void SetCatchChanceMultiplier(float Multiplier);

    UFUNCTION(BlueprintCallable, Category = "Fishing Net|Mesh")
    void SetNetMesh(UStaticMesh* NewMesh);

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    float GetCatchProgress() const;

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    int32 GetFishCount() const { return CaughtFishIDs.Num(); }

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    int32 GetTotalValue() const;

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    bool WasLastCatchSuccessful() const { return bPredictedCatchResult; }

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    void SetFishDataTable(UDataTable* NewTable) { FishDataTable = NewTable; }

    UFUNCTION(BlueprintCallable, Category = "Fishing Net")
    float GetFishingInterval() const { return FishingInterval; }

    UPROPERTY(BlueprintAssignable, Category = "Fishing Net")
    FOnNetFishCaught OnNetFishCaught;

private:
    UPROPERTY(EditAnywhere, Category = "Fishing Net")
    UDataTable* FishDataTable;

    UPROPERTY(VisibleAnywhere, Category = "Fishing Net")
    class UStaticMeshComponent* NetMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Fishing Net")
    TArray<int32> CaughtFishIDs;

    float FishingInterval = 10.0f;
    float CatchChanceMultiplier = 1.0f;
    float CurrentCatchTime = 0.0f;
    bool bIsFishing = false;
    bool bPredictedCatchResult = false;

    void CatchSingleFish();
    FFishData* GetRandomFish();
};
