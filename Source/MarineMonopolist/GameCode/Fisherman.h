#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/DataTable.h"
#include "FishData.h"
#include "Fisherman.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFishCaught, int32, FishID, const FString&, FishName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishermanFishCaughtFull, FFishData, FishData);

UCLASS()
class MARINEMONOPOLIST_API AFisherman : public APawn
{
    GENERATED_BODY()

public:
    AFisherman();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Fishing")
	void StartFishing();

	UFUNCTION(BlueprintCallable, Category = "Fishing")
	void StopFishing();

	UFUNCTION(BlueprintCallable, Category = "Fishing")
	void SpeedUpFishing();

    void CatchFish();

    UPROPERTY(BlueprintAssignable, Category = "Fishing")
    FOnFishCaught OnFishCaught;

    UPROPERTY(BlueprintAssignable, Category = "Fishing")
    FOnFishermanFishCaughtFull OnFishermanFishCaughtFull;

    UFUNCTION(BlueprintCallable, Category = "Fishing")
    float GetCurrentFishingTime() const { return CurrentFishingTime; }

    UFUNCTION(BlueprintCallable, Category = "Fishing")
    float GetFishingTime() const { return FishingTime; }

    UFUNCTION(BlueprintCallable, Category = "Fishing")
    float GetSpeedUpValue() const { return SpeedUpValue; }

    UFUNCTION(BlueprintCallable, Category = "Fishing")
    UDataTable* GetFishDataTable() const { return FishDataTable; }

private:
    UPROPERTY(EditAnywhere, Category = "Fishing")
    float FishingTime = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Fishing")
    float SpeedUpValue = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Fishing")
    UDataTable* FishDataTable;

    float CurrentFishingTime = 0.0f;
    bool bIsFishing = false;

    FFishData* GetRandomFish();
};
