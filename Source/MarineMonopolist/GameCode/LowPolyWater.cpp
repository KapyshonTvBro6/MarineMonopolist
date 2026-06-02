#include "LowPolyWater.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"

ALowPolyWater::ALowPolyWater()
{
    PrimaryActorTick.bCanEverTick = true;

    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    RootComponent = ProceduralMesh;
    ProceduralMesh->SetCastShadow(true);
}

void ALowPolyWater::BeginPlay()
{
    Super::BeginPlay();
    GenerateMesh();
    RandomizeOffsets();
    RebuildMesh();

    if (WaterMaterial)
    {
        MaterialInstance = UMaterialInstanceDynamic::Create(WaterMaterial, this);
        ProceduralMesh->SetMaterial(0, MaterialInstance);
    }
}

void ALowPolyWater::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bAnimate)
    {
        CurrentTime += DeltaTime;
        RebuildMesh();
    }
}

void ALowPolyWater::GenerateMesh()
{
    GridVertices.Empty();
    GridUVs.Empty();
    Triangles.Empty();

    GridVertexCount = (SegmentsX + 1) * (SegmentsY + 1);
    CenterVertexCount = SegmentsX * SegmentsY;

    GridVertices.Reserve(GridVertexCount);
    GridUVs.Reserve(GridVertexCount);
    Triangles.Reserve(CenterVertexCount * 12);

    float HalfSizeX = PlaneSizeX * 0.5f;
    float HalfSizeY = PlaneSizeY * 0.5f;
    float StepX = PlaneSizeX / SegmentsX;
    float StepY = PlaneSizeY / SegmentsY;

    for (int32 Y = 0; Y <= SegmentsY; Y++)
    {
        for (int32 X = 0; X <= SegmentsX; X++)
        {
            float PosX = -HalfSizeX + X * StepX;
            float PosY = -HalfSizeY + Y * StepY;
            GridVertices.Add(FVector(PosX, PosY, 0.0f));
            GridUVs.Add(FVector2D((float)X / SegmentsX, (float)Y / SegmentsY));
        }
    }

    for (int32 Y = 0; Y < SegmentsY; Y++)
    {
        for (int32 X = 0; X < SegmentsX; X++)
        {
            int32 A = Y * (SegmentsX + 1) + X;
            int32 B = Y * (SegmentsX + 1) + X + 1;
            int32 C = (Y + 1) * (SegmentsX + 1) + X;
            int32 D = (Y + 1) * (SegmentsX + 1) + X + 1;
            int32 Center = GridVertexCount + Y * SegmentsX + X;

            Triangles.Add(A);
            Triangles.Add(B);
            Triangles.Add(Center);

            Triangles.Add(B);
            Triangles.Add(D);
            Triangles.Add(Center);

            Triangles.Add(D);
            Triangles.Add(C);
            Triangles.Add(Center);

            Triangles.Add(C);
            Triangles.Add(A);
            Triangles.Add(Center);
        }
    }

    int32 TotalVerts = GridVertexCount + CenterVertexCount;
    VertexZs.SetNum(TotalVerts);
    AnimationPhases.SetNum(TotalVerts);
}

void ALowPolyWater::RandomizeOffsets()
{
    for (int32 i = 0; i < VertexZs.Num(); i++)
    {
        VertexZs[i] = FMath::FRandRange(MinZOffset, MaxZOffset);
        AnimationPhases[i] = FMath::FRandRange(0.0f, TWO_PI);
    }
}

void ALowPolyWater::RebuildMesh()
{
    if (!ProceduralMesh || GridVertices.Num() == 0) return;

    int32 TotalVerts = GridVertexCount + CenterVertexCount;

    TArray<FVector> Positions;
    TArray<FVector2D> UVs;
    TArray<FVector> NormalsArray;
    TArray<FColor> Colors;
    TArray<FProcMeshTangent> TangentsArray;

    Positions.Reserve(TotalVerts);
    UVs.Reserve(TotalVerts);
    NormalsArray.Reserve(TotalVerts);
    Colors.Reserve(TotalVerts);
    TangentsArray.Reserve(TotalVerts);

    float HalfSizeX = PlaneSizeX * 0.5f;
    float HalfSizeY = PlaneSizeY * 0.5f;
    float StepX = PlaneSizeX / SegmentsX;
    float StepY = PlaneSizeY / SegmentsY;

    for (int32 i = 0; i < GridVertexCount; i++)
    {
        float Z = VertexZs[i];
        if (bAnimate)
        {
            Z += FMath::Sin(CurrentTime * AnimationSpeed + AnimationPhases[i]) * AnimationAmplitude;
        }
        Positions.Add(FVector(GridVertices[i].X, GridVertices[i].Y, Z));
        UVs.Add(GridUVs[i]);
    }

    for (int32 Y = 0; Y < SegmentsY; Y++)
    {
        for (int32 X = 0; X < SegmentsX; X++)
        {
            float X0 = -HalfSizeX + X * StepX;
            float Y0 = -HalfSizeY + Y * StepY;
            float CenterX = X0 + StepX * 0.5f;
            float CenterY = Y0 + StepY * 0.5f;

            int32 CenterIdx = GridVertexCount + Y * SegmentsX + X;
            float Z = VertexZs[CenterIdx];
            if (bAnimate)
            {
                Z += FMath::Sin(CurrentTime * AnimationSpeed + AnimationPhases[CenterIdx]) * AnimationAmplitude;
            }

            Positions.Add(FVector(CenterX, CenterY, Z));

            float U0 = (float)X / SegmentsX;
            float V0 = (float)Y / SegmentsY;
            float U1 = (float)(X + 1) / SegmentsX;
            float V1 = (float)(Y + 1) / SegmentsY;
            UVs.Add(FVector2D((U0 + U1) * 0.5f, (V0 + V1) * 0.5f));
        }
    }

    NormalsArray.Init(FVector::ZeroVector, TotalVerts);
    for (int32 Tri = 0; Tri < Triangles.Num(); Tri += 3)
    {
        int32 I0 = Triangles[Tri];
        int32 I1 = Triangles[Tri + 1];
        int32 I2 = Triangles[Tri + 2];

        FVector Edge1 = Positions[I1] - Positions[I0];
        FVector Edge2 = Positions[I2] - Positions[I0];
        FVector FaceNormal = FVector::CrossProduct(Edge1, Edge2).GetSafeNormal();

        NormalsArray[I0] += FaceNormal;
        NormalsArray[I1] += FaceNormal;
        NormalsArray[I2] += FaceNormal;
    }
    for (int32 i = 0; i < TotalVerts; i++)
    {
        NormalsArray[i].Normalize();
    }

    Colors.Init(FColor::White, TotalVerts);
    TangentsArray.Init(FProcMeshTangent(1.0f, 0.0f, 0.0f), TotalVerts);

    if (ProceduralMesh->GetNumSections() == 0)
    {
        ProceduralMesh->CreateMeshSection(
            0, Positions, Triangles, NormalsArray, UVs, Colors, TangentsArray, false
        );
    }
    else
    {
        ProceduralMesh->UpdateMeshSection(0, Positions, NormalsArray, UVs, Colors, TangentsArray);
    }
}
