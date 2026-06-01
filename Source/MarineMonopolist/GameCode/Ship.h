#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ship.generated.h"

UCLASS()
class MARINEMONOPOLIST_API AShip : public AActor
{
	GENERATED_BODY()
    
public:
	AShip();
    
protected:
	virtual void BeginPlay() override;
    
public:
	virtual void Tick(float DeltaTime) override;
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	USceneComponent* GetFishermanPoint() const { return FishermanPoint; }
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	USceneComponent* GetSecondFishermanPoint() const { return SecondFishermanPoint; }
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	TArray<USceneComponent*> GetNetPoints() const { return NetPoints; }
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	USceneComponent* GetAvailableNetPoint();
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	void AttachNet(AActor* NetActor);
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	int32 GetNetCount() const { return AttachedNets.Num(); }
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	int32 GetMaxNets() const { return MaxNets; }
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	void Upgrade();
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	void AddNewNetPoint();
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	int32 GetShipLevel() const { return ShipLevel; }
    
	UFUNCTION(BlueprintCallable, Category = "Ship")
	int32 GetUpgradeCost() const { return UpgradeCost; }

private:
	UPROPERTY(EditAnywhere, Category = "Ship")
	int32 ShipLevel = 1;
    
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	int32 MaxNets = 4;
    
	UPROPERTY(EditAnywhere, Category = "Ship")
	int32 BaseMaxNets = 4;
    
	UPROPERTY(EditAnywhere, Category = "Ship")
	int32 UpgradeCost = 500;
    
	UPROPERTY(EditAnywhere, Category = "Ship")
	int32 NetsPerLevel = 2;
    
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	USceneComponent* FishermanPoint;
    
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	USceneComponent* SecondFishermanPoint;
    
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	TArray<USceneComponent*> NetPoints;
    
	UPROPERTY(VisibleAnywhere, Category = "Ship")
	TArray<AActor*> AttachedNets;
    
	void CreateAttachmentPoints();
    
	USceneComponent* CreateNetPointComponent(int32 Index);

	UPROPERTY(EditAnywhere, Category = "Ship|Wave")
	bool bFloatOnWaves = true;

	UPROPERTY(EditAnywhere, Category = "Ship|Wave", meta = (ClampMin = "0.0"))
	float BobAmplitude = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Ship|Wave", meta = (ClampMin = "0.0"))
	float BobSpeed = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Ship|Wave", meta = (ClampMin = "0.0"))
	float PitchAmplitude = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Ship|Wave", meta = (ClampMin = "0.0"))
	float PitchSpeed = 1.2f;

	UPROPERTY(EditAnywhere, Category = "Ship|Wave", meta = (ClampMin = "0.0"))
	float RollAmplitude = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Ship|Wave", meta = (ClampMin = "0.0"))
	float RollSpeed = 0.8f;

	float BaseWaterZ = 0.0f;
	float WaveTime = 0.0f;

	void UpdateFloatOnWaves(float DeltaTime);
};
