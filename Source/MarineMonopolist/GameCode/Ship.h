#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ship.generated.h"

class AFishingNet;
class ANightBaitDevice;

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

    UFUNCTION(BlueprintCallable, Category = "Ship|Net")
    USceneComponent* GetNetPoint() const { return NetPoint; }

    UFUNCTION(BlueprintCallable, Category = "Ship|NightBait")
    USceneComponent* GetNightBaitPoint() const { return NightBaitPoint; }

    UFUNCTION(BlueprintCallable, Category = "Ship|Net")
    void AttachNet(AActor* NetActor);

    UFUNCTION(BlueprintCallable, Category = "Ship|Net")
    void RemoveNet();

    UFUNCTION(BlueprintCallable, Category = "Ship|NightBait")
    void AttachNightBait(AActor* DeviceActor);

    UFUNCTION(BlueprintCallable, Category = "Ship|Mesh")
    void SetShipMesh(int32 Level);

    UFUNCTION(BlueprintCallable, Category = "Ship|Mesh")
    UStaticMesh* GetNetMeshForLevel(int32 Level) const;

    UFUNCTION(BlueprintCallable, Category = "Ship")
    int32 GetShipLevel() const { return ShipLevel; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Meshes")
    TArray<UStaticMesh*> ShipMeshes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship|Meshes")
    TArray<UStaticMesh*> NetMeshes;

private:
    UPROPERTY(VisibleAnywhere, Category = "Ship")
    int32 ShipLevel = 1;

    UPROPERTY(VisibleAnywhere, Category = "Ship")
    class UStaticMeshComponent* ShipMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Ship")
    USceneComponent* FishermanPoint;

    UPROPERTY(VisibleAnywhere, Category = "Ship")
    USceneComponent* NetPoint;

    UPROPERTY(VisibleAnywhere, Category = "Ship")
    USceneComponent* NightBaitPoint;

    void CreateAttachmentPoints();

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
