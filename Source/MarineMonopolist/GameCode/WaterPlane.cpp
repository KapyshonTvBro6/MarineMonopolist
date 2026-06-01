#include "WaterPlane.h"
#include "ProceduralMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"

AWaterPlane::AWaterPlane()
{
    PrimaryActorTick.bCanEverTick = true;

    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    RootComponent = ProceduralMesh;
    ProceduralMesh->SetCastShadow(false);

    Waves.Add(FWaterWaveParams{ 50.0f, 0.05f, 0.8f,  FVector2D(1.0f, 0.3f), 0.3f });
    Waves.Add(FWaterWaveParams{ 30.0f, 0.08f, 1.2f,  FVector2D(-0.7f, 1.0f), 0.2f });
    Waves.Add(FWaterWaveParams{ 20.0f, 0.12f, 1.5f,  FVector2D(0.5f, -0.8f), 0.25f });
    Waves.Add(FWaterWaveParams{ 15.0f, 0.18f, 2.0f,  FVector2D(-0.3f, -0.5f), 0.15f });
}

void AWaterPlane::BeginPlay()
{
    Super::BeginPlay();
    GenerateGrid();
    RebuildMesh();

    if (WaterMaterial)
    {
        MaterialInstance = UMaterialInstanceDynamic::Create(WaterMaterial, this);
        ProceduralMesh->SetMaterial(0, MaterialInstance);
    }
}

void AWaterPlane::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateWaves(DeltaTime);
}

void AWaterPlane::GenerateGrid()
{
    BaseVertices.Empty();
    UVs.Empty();
    Triangles.Empty();
    Normals.Empty();
    VertexColors.Empty();
    Tangents.Empty();

    int32 VertexCount = (SegmentsX + 1) * (SegmentsY + 1);
    BaseVertices.Reserve(VertexCount);
    UVs.Reserve(VertexCount);

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
            BaseVertices.Add(FVector(PosX, PosY, 0.0f));
            UVs.Add(FVector2D((float)X / SegmentsX, (float)Y / SegmentsY));
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

            Triangles.Add(A);
            Triangles.Add(C);
            Triangles.Add(B);

            Triangles.Add(B);
            Triangles.Add(C);
            Triangles.Add(D);
        }
    }
}

FVector AWaterPlane::GerstnerWave(const FWaterWaveParams& Params, const FVector& WorldPos, float Time)
{
    FVector2D Dir = Params.Direction.GetSafeNormal();
    float Wavelength = 2.0f * PI / Params.Frequency;
    float Speed = FMath::Sqrt(9.81f * 2.0f * PI / Wavelength);
    float Frequency = Params.Frequency;
    float Phase = Speed * Frequency * Time;

    float Theta = Frequency * FVector2D::DotProduct(Dir, FVector2D(WorldPos.X, WorldPos.Y)) + Phase;
    float S = FMath::Sin(Theta);
    float C = FMath::Cos(Theta);
    float Qi = 1.0f / (1.0f + Params.Steepness * Params.Steepness * Wavelength * Frequency);

    float DisplacementX = Qi * Params.Steepness * Wavelength * Dir.X * C;
    float DisplacementY = Qi * Params.Steepness * Wavelength * S;
    float DisplacementZ = Qi * Params.Steepness * Wavelength * Dir.Y * C;

    return FVector(DisplacementX, DisplacementY, DisplacementZ);
}

float AWaterPlane::GetWaveHeightAtLocation(FVector WorldLocation, FVector& OutNormal) const
{
    FVector Displacement = GetWaveDisplacement(WorldLocation, CurrentTime, OutNormal);
    return Displacement.Y;
}

FVector AWaterPlane::GetWaveDisplacement(const FVector& WorldPos, float Time, FVector& OutNormal) const
{
    FVector TotalDisplacement = FVector::ZeroVector;
    FVector AccumulatedNormal = FVector(0.0f, 0.0f, 1.0f);

    for (const FWaterWaveParams& Params : Waves)
    {
        FVector2D Dir = Params.Direction.GetSafeNormal();
        float Wavelength = 2.0f * PI / Params.Frequency;
        float Speed = FMath::Sqrt(9.81f * 2.0f * PI / Wavelength);
        float Frequency = Params.Frequency;
        float Phase = Speed * Frequency * Time;
        float Theta = Frequency * FVector2D::DotProduct(Dir, FVector2D(WorldPos.X, WorldPos.Y)) + Phase;

        float S = FMath::Sin(Theta);
        float C = FMath::Cos(Theta);

        float DisplacementY = Params.Amplitude * S;
        TotalDisplacement += FVector(0.0f, DisplacementY, 0.0f);

        float Nx = -(Dir.X * Frequency * Params.Amplitude * C);
        float Ny = 1.0f - (Frequency * Params.Amplitude * S);
        float Nz = -(Dir.Y * Frequency * Params.Amplitude * C);
        AccumulatedNormal += FVector(Nx, Ny, Nz);
    }

    OutNormal = AccumulatedNormal.GetSafeNormal();
    return TotalDisplacement;
}

void AWaterPlane::UpdateWaves(float DeltaTime)
{
    CurrentTime += DeltaTime;
    UpdateMeshSection();
}

void AWaterPlane::RebuildMesh()
{
    if (!ProceduralMesh) return;

    TArray<FVector> Vertices;
    TArray<FVector> NormalsArray;
    TArray<FProcMeshTangent> TangentsArray;
    TArray<FColor> ColorsArray;

    Vertices.SetNum(BaseVertices.Num());
    NormalsArray.SetNum(BaseVertices.Num());
    VertexColors.SetNum(BaseVertices.Num());
    TangentsArray.SetNum(BaseVertices.Num());

    for (int32 i = 0; i < BaseVertices.Num(); i++)
    {
        Vertices[i] = BaseVertices[i];
        NormalsArray[i] = FVector(0, 0, 1);
        TangentsArray[i] = FProcMeshTangent(1, 0, 0);
        VertexColors[i] = FColor::White;
    }

    ProceduralMesh->CreateMeshSection(
        0, Vertices, Triangles, NormalsArray, UVs, VertexColors, TangentsArray, false
    );
}

void AWaterPlane::UpdateMeshSection()
{
    if (!ProceduralMesh) return;

    TArray<FVector> Positions;
    Positions.SetNum(BaseVertices.Num());
    Normals.SetNum(BaseVertices.Num());

    FVector ActorWorld = GetActorLocation();

    for (int32 i = 0; i < BaseVertices.Num(); i++)
    {
        FVector WorldPos = BaseVertices[i] + ActorWorld;
        FVector Normal;
        FVector Displacement = GetWaveDisplacement(WorldPos, CurrentTime, Normal);
        Positions[i] = FVector(BaseVertices[i].X, BaseVertices[i].Y, BaseVertices[i].Z + Displacement.Y);
        Normals[i] = Normal;
    }

    ProceduralMesh->UpdateMeshSection(0, Positions, Normals, UVs, VertexColors, Tangents);
}
