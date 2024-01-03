#include "PangeaVoxelBlueprintLibrary.h"
#include "PangeaVoxelMeshBuffers.h"
#include "PangeaVoxelComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "PangeaVoxelMeshChunk.h"
#include "PangeaMeshTangent.h"
#include "PangeaVoxelMarchingCubes.h"

namespace
{
	void ConvertQuadToTriangles(TArray<uint32>& Triangles, uint32 Vert0, uint32 Vert1, uint32 Vert2, uint32 Vert3)
	{
		Triangles.Add(Vert0);
		Triangles.Add(Vert1);
		Triangles.Add(Vert3);

		Triangles.Add(Vert1);
		Triangles.Add(Vert2);
		Triangles.Add(Vert3);
	}

	void GenerateBoxMeshData(FPangeaVoxelMeshData& MeshData, const FVector3f& BoxRadius)
	{
		// Generate verts
		FVector3f BoxVerts[8];
		BoxVerts[0] = FVector3f(-BoxRadius.X, BoxRadius.Y, BoxRadius.Z);
		BoxVerts[1] = FVector3f(BoxRadius.X, BoxRadius.Y, BoxRadius.Z);
		BoxVerts[2] = FVector3f(BoxRadius.X, -BoxRadius.Y, BoxRadius.Z);
		BoxVerts[3] = FVector3f(-BoxRadius.X, -BoxRadius.Y, BoxRadius.Z);

		BoxVerts[4] = FVector3f(-BoxRadius.X, BoxRadius.Y, -BoxRadius.Z);
		BoxVerts[5] = FVector3f(BoxRadius.X, BoxRadius.Y, -BoxRadius.Z);
		BoxVerts[6] = FVector3f(BoxRadius.X, -BoxRadius.Y, -BoxRadius.Z);
		BoxVerts[7] = FVector3f(-BoxRadius.X, -BoxRadius.Y, -BoxRadius.Z);

		auto& Triangles = MeshData.Indices;
		auto& Vertices = MeshData.Positions;
		auto& Normals = MeshData.Normals;
		auto& Tangents = MeshData.Tangents;
		auto& Colors = MeshData.Colors;
		auto& UVs = MeshData.TextureCoordinates;

		// Generate triangles (from quads)
		Triangles.Reset();
		Triangles.Empty(6 * (3 * 2)); // 6 faces x 6 triangle indices per face

		const int32 NumVerts = 24; // 6 faces x 4 verts per face

		Vertices.Reset();
		Vertices.AddUninitialized(NumVerts);

		Normals.Reset();
		Normals.AddUninitialized(NumVerts);

		Tangents.Reset();
		Tangents.AddUninitialized(NumVerts);

		Colors.Init(FColor::White, NumVerts);

		Vertices[0] = BoxVerts[0];
		Vertices[1] = BoxVerts[1];
		Vertices[2] = BoxVerts[2];
		Vertices[3] = BoxVerts[3];
		ConvertQuadToTriangles(Triangles, 0, 1, 2, 3);
		Normals[0] = Normals[1] = Normals[2] = Normals[3] = FVector3f(0, 0, 1);
		Tangents[0] = Tangents[1] = Tangents[2] = Tangents[3] = FPangeaMeshTangent(0.f, -1.f, 0.f);

		Vertices[4] = BoxVerts[4];
		Vertices[5] = BoxVerts[0];
		Vertices[6] = BoxVerts[3];
		Vertices[7] = BoxVerts[7];
		ConvertQuadToTriangles(Triangles, 4, 5, 6, 7);
		Normals[4] = Normals[5] = Normals[6] = Normals[7] = FVector3f(-1, 0, 0);
		Tangents[4] = Tangents[5] = Tangents[6] = Tangents[7] = FPangeaMeshTangent(0.f, -1.f, 0.f);

		Vertices[8] = BoxVerts[5];
		Vertices[9] = BoxVerts[1];
		Vertices[10] = BoxVerts[0];
		Vertices[11] = BoxVerts[4];
		ConvertQuadToTriangles(Triangles, 8, 9, 10, 11);
		Normals[8] = Normals[9] = Normals[10] = Normals[11] = FVector3f(0, 1, 0);
		Tangents[8] = Tangents[9] = Tangents[10] = Tangents[11] = FPangeaMeshTangent(-1.f, 0.f, 0.f);

		Vertices[12] = BoxVerts[6];
		Vertices[13] = BoxVerts[2];
		Vertices[14] = BoxVerts[1];
		Vertices[15] = BoxVerts[5];
		ConvertQuadToTriangles(Triangles, 12, 13, 14, 15);
		Normals[12] = Normals[13] = Normals[14] = Normals[15] = FVector3f(1, 0, 0);
		Tangents[12] = Tangents[13] = Tangents[14] = Tangents[15] = FPangeaMeshTangent(0.f, 1.f, 0.f);

		Vertices[16] = BoxVerts[7];
		Vertices[17] = BoxVerts[3];
		Vertices[18] = BoxVerts[2];
		Vertices[19] = BoxVerts[6];
		ConvertQuadToTriangles(Triangles, 16, 17, 18, 19);
		Normals[16] = Normals[17] = Normals[18] = Normals[19] = FVector3f(0, -1, 0);
		Tangents[16] = Tangents[17] = Tangents[18] = Tangents[19] = FPangeaMeshTangent(1.f, 0.f, 0.f);

		Vertices[20] = BoxVerts[7];
		Vertices[21] = BoxVerts[6];
		Vertices[22] = BoxVerts[5];
		Vertices[23] = BoxVerts[4];
		ConvertQuadToTriangles(Triangles, 20, 21, 22, 23);
		Normals[20] = Normals[21] = Normals[22] = Normals[23] = FVector3f(0, 0, -1);
		Tangents[20] = Tangents[21] = Tangents[22] = Tangents[23] = FPangeaMeshTangent(0.f, 1.f, 0.f);

		// UVs
		UVs.Reset();
		UVs.Init(TArray<FVector2f>{{0, 0}}, NumVerts);

		UVs[0][0] = UVs[4][0] = UVs[8][0] = UVs[12][0] = UVs[16][0] = UVs[20][0] = FVector2f(0.f, 0.f);
		UVs[1][0] = UVs[5][0] = UVs[9][0] = UVs[13][0] = UVs[17][0] = UVs[21][0] = FVector2f(0.f, 1.f);
		UVs[2][0] = UVs[6][0] = UVs[10][0] = UVs[14][0] = UVs[18][0] = UVs[22][0] = FVector2f(1.f, 1.f);
		UVs[3][0] = UVs[7][0] = UVs[11][0] = UVs[15][0] = UVs[19][0] = UVs[23][0] = FVector2f(1.f, 0.f);
	}

	TSharedPtr<FPangeaVoxelMeshBuffers> ConvertMeshDataToMeshBuffers(const FPangeaVoxelMeshData& MeshData)
	{
		if (MeshData.Positions.Num() == 0)
			return nullptr;
		if (MeshData.TextureCoordinates.Num() == 0)
			return nullptr;

		TSharedRef<FPangeaVoxelMeshBuffers> MeshBuffersRef = MakeShared<FPangeaVoxelMeshBuffers>();
		FPangeaVoxelMeshBuffers& MeshBuffers = *MeshBuffersRef;

		auto& PositionVertexBuffer = MeshBuffers.VertexBuffers.PositionVertexBuffer;
		auto& StaticMeshVertexBuffer = MeshBuffers.VertexBuffers.StaticMeshVertexBuffer;
		auto& ColorVertexBuffer = MeshBuffers.VertexBuffers.ColorVertexBuffer;
		auto& IndexBuffer = MeshBuffers.IndexBuffer;

		int32 NumVerts = MeshData.Positions.Num();
		int32 NumTexCoords = MeshData.TextureCoordinates[0].Num();

		PositionVertexBuffer.Init(NumVerts);
		StaticMeshVertexBuffer.Init(NumVerts, MeshData.TextureCoordinates[0].Num());
		ColorVertexBuffer.Init(NumVerts);

		for (int32 i = 0; i < NumVerts; ++i)
		{
			const auto& Position = MeshData.Positions[i];
			const auto& Tangent = MeshData.Tangents[i];
			const auto& Normal = MeshData.Normals[i];
			const auto& Color = MeshData.Colors[i];

			PositionVertexBuffer.VertexPosition(i) = Position;
			StaticMeshVertexBuffer.SetVertexTangents(i, Tangent.TangentX, Tangent.GetY(Normal), Normal);
			for (int32 j = 0; j < NumTexCoords; ++j)
			{
				StaticMeshVertexBuffer.SetVertexUV(i, j, MeshData.TextureCoordinates[i][j]);
			}
			ColorVertexBuffer.VertexColor(i) = Color;

			MeshBuffers.LocalBounds += FVector(Position);
		}

		IndexBuffer.SetIndices(MeshData.Indices, EIndexBufferStride::Force32Bit);

		return MeshBuffersRef;
	}

	void GenerateConvexPoints(TArray<TArray<FVector>>& OutPoints, const FPangeaVoxelMeshData& MeshData)
	{
		OutPoints.SetNum(1);
		OutPoints[0].Empty(MeshData.Positions.Num());

		for (int32 i = 0; i < MeshData.Positions.Num(); ++i)
		{
			OutPoints[0].Add(FVector(MeshData.Positions[i]));
		}
	}
}

void UPangeaVoxelBlueprintLibrary::GenerateAndSetTestVoxelMeshBuffers(UPangeaVoxelComponent* PangeaVoxelComponent)
{
	FPangeaVoxelMeshData MeshData;
	GenerateBoxMeshData(MeshData, FVector3f(50));

	TSharedPtr<FPangeaVoxelMeshBuffers> VoxelMeshBuffers = ConvertMeshDataToMeshBuffers(MeshData);

	TArray<TArray<FVector>> ConvexPoints;
	GenerateConvexPoints(ConvexPoints, MeshData);

	PangeaVoxelComponent->SetCachedMeshBuffers(VoxelMeshBuffers);
	PangeaVoxelComponent->SetCollisionConvexMeshes(ConvexPoints);
}

namespace
{
	float SphereFunc(float X, float Y, float Z, float Radius)
	{
			return FMath::Sqrt(X * X + Y * Y + Z * Z) - (Radius * Radius);
	}

	int8 ConvertToInt8Norm(float Val)
	{
		float QuantVal = Val * 128;
		return (int8)FMath::Clamp((int32)QuantVal, -127, 127);
	}
}

static float ClampToNormalRange(float Value, float InRangeA, float InRangeB, float OutRangeA, float OutRangeB)
{
	return FMath::GetMappedRangeValueClamped(FVector2f(InRangeA, InRangeB), FVector2f(OutRangeA, OutRangeB), Value);
}

static void GenerateSphereVoxelData(FPangeaVoxelData& OutVoxelData, float Radius)
{
	int32 QuantUpRadius = FMath::CeilToInt(Radius);
	int32 NumCubeGrids = (QuantUpRadius + 2) * 2; // Adds one more grid at 3 axes both sides (-, + sides, <->)
	int32 NumVoxelPerDimension = NumCubeGrids + 1;

	int32 CenterVoxel = NumVoxelPerDimension / 2; // In flatten 0 to NumCubeGrids - 1 domain

	OutVoxelData.VertsSize = NumVoxelPerDimension;
	OutVoxelData.Data.SetNumUninitialized(NumVoxelPerDimension * NumVoxelPerDimension * NumVoxelPerDimension);

	auto ToFlattenArrayIndex =
		[CenterVoxel, NumVoxelPerDimension](int32 X, int32 Y, int32 Z) FORCENOINLINE
	{
			int32 ZFlatten = Z + CenterVoxel;
			int32 YFlatten = Y + CenterVoxel;
			int32 XFlatten = X + CenterVoxel;

			return ZFlatten * NumVoxelPerDimension * NumVoxelPerDimension +
					+YFlatten * NumVoxelPerDimension
					+ XFlatten;
	};

	float QuantExt = 2;

	auto RemapClampValue = [QuantExt](float Value) FORCENOINLINE
	{
		float QuantExtSq = QuantExt * QuantExt;
		return ClampToNormalRange(Value, -QuantExtSq, QuantExtSq, -1.f, 1.f);
	};

	int32 StartVoxelCoord = -NumVoxelPerDimension / 2;
	int32 EndVoxelCoord = NumVoxelPerDimension / 2;

	for (int32 Z = StartVoxelCoord; Z <= EndVoxelCoord; ++Z)
	{
		for (int32 Y = StartVoxelCoord; Y <= EndVoxelCoord; ++Y)
		{
			for (int32 X = StartVoxelCoord; X <= EndVoxelCoord; ++X)
			{
				float Val = SphereFunc(X, Y, Z, Radius);
				int8 QuantNorm8 = ConvertToInt8Norm(RemapClampValue(Val));
				int32 ArrayIndex = ToFlattenArrayIndex(X, Y, Z);
				OutVoxelData.Data[ArrayIndex] = QuantNorm8;
			}
		}
	}
}

void UPangeaVoxelBlueprintLibrary::GenerateSphereVoxelData(FTestPangeaVoxelData& OutVoxelData, float Radius)
{
	::GenerateSphereVoxelData(OutVoxelData.RealData, Radius);
}

void UPangeaVoxelBlueprintLibrary::GenerateSphereVoxelDataComponent(UPangeaVoxelComponent* PangeaVoxelComponent, float Radius)
{
	TSharedPtr<FPangeaVoxelData> VoxelDataPtr = MakeShared<FPangeaVoxelData>();
	::GenerateSphereVoxelData(*VoxelDataPtr.Get(), Radius);
	PangeaVoxelComponent->SetVoxelData(VoxelDataPtr);
}

void UPangeaVoxelBlueprintLibrary::DoMarchingCubesComponent(UPangeaVoxelComponent* PangeaVoxelComponent)
{
	TSharedPtr<FPangeaVoxelData> VoxelDataPtr = PangeaVoxelComponent->GetVoxelData();
	TSharedPtr<FPangeaVoxelMarchingCubes> MarchingCubesMesher = MakeShared<FPangeaVoxelMarchingCubes>(VoxelDataPtr->VertsSize);

	FPangeaVoxelMeshData MeshData;

	MarchingCubesMesher->GenerateMeshFromChunk(*VoxelDataPtr.Get(), FVector3f::Zero(), PangeaVoxelComponent->GetVoxelGridSize(), MeshData);

	TSharedPtr<FPangeaVoxelMeshBuffers> VoxelMeshBuffers = ConvertMeshDataToMeshBuffers(MeshData);

	TArray<TArray<FVector>> ConvexPoints;
	GenerateConvexPoints(ConvexPoints, MeshData);

	PangeaVoxelComponent->SetCachedMeshBuffers(VoxelMeshBuffers);
	PangeaVoxelComponent->SetCollisionConvexMeshes(ConvexPoints);
}
