#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "WaterPlane.generated.h"

USTRUCT(BlueprintType)
struct FWaterWaveParams
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float Amplitude = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float Frequency = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float Speed = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    FVector2D Direction = FVector2D(1.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float Steepness = 0.3f;
};

UCLASS()
class MARINEMONOPOLIST_API AWaterPlane : public AActor
{
    GENERATED_BODY()

public:
    AWaterPlane();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Plane")
    int32 SegmentsX = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Plane")
    int32 SegmentsY = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Plane")
    float PlaneSizeX = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Plane")
    float PlaneSizeY = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Plane|Waves")
    TArray<FWaterWaveParams> Waves;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Plane|Material")
    UMaterialInterface* WaterMaterial;

    UFUNCTION(BlueprintCallable, Category = "Water Plane")
    void RebuildMesh();

    UFUNCTION(BlueprintCallable, Category = "Water Plane")
    void UpdateWaves(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Water Plane")
    float GetWaveHeightAtLocation(FVector WorldLocation, FVector& OutNormal) const;

    UFUNCTION(BlueprintCallable, Category = "Water Plane")
    float GetWaveTime() const { return CurrentTime; }

private:
    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProceduralMesh;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;

    TArray<FVector> BaseVertices;
    TArray<FVector2D> UVs;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;

    float CurrentTime = 0.0f;

    void GenerateGrid();
    FVector GerstnerWave(const FWaterWaveParams& Params, const FVector& WorldPos, float Time);
    FVector GetWaveDisplacement(const FVector& WorldPos, float Time, FVector& OutNormal) const;
    void UpdateMeshSection();
};
