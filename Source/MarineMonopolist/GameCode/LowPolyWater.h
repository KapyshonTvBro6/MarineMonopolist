#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "LowPolyWater.generated.h"

UCLASS()
class MARINEMONOPOLIST_API ALowPolyWater : public AActor
{
    GENERATED_BODY()

public:
    ALowPolyWater();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    int32 SegmentsX = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    int32 SegmentsY = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    float PlaneSizeX = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    float PlaneSizeY = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    float MinZOffset = -50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    float MaxZOffset = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    bool bAnimate = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    float AnimationSpeed = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water")
    float AnimationAmplitude = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Water|Material")
    UMaterialInterface* WaterMaterial;

    UFUNCTION(BlueprintCallable, Category = "Low Poly Water")
    void RebuildMesh();

    UFUNCTION(BlueprintCallable, Category = "Low Poly Water")
    void RandomizeOffsets();

private:
    UPROPERTY()
    TObjectPtr<UProceduralMeshComponent> ProceduralMesh;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;

    TArray<FVector> GridVertices;
    TArray<FVector2D> GridUVs;
    TArray<float> VertexZs;
    TArray<float> AnimationPhases;
    TArray<int32> Triangles;

    int32 GridVertexCount;
    int32 CenterVertexCount;
    float CurrentTime = 0.0f;

    void GenerateMesh();
};
