#include "PangeaVoxelBlueprintLibrary.h"
#include "PangeaVoxelMeshBuffers.h"
#include "PangeaVoxelComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "ProceduralMeshComponent.h"
#include "PangeaVoxelMeshChunk.h"
#include "PangeaMeshTangent.h"

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
		auto& UVs = MeshData.TextureCoordinates;

		// Generate triangles (from quads)
		Triangles.Reset();

		const int32 NumVerts = 24; // 6 faces x 4 verts per face

		Vertices.Reset();
		Vertices.AddUninitialized(NumVerts);

		Normals.Reset();
		Normals.AddUninitialized(NumVerts);

		Tangents.Reset();
		Tangents.AddUninitialized(NumVerts);

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
}

void UPangeaVoxelBlueprintLibrary::GenerateAndSetTestVoxelMeshBuffers(UPangeaVoxelComponent* PangeaVoxelComponent)
{
	FPangeaVoxelMeshData MeshData;
	GenerateBoxMeshData(MeshData, FVector3f(50));

	TSharedPtr<FPangeaVoxelMeshBuffers> VoxelMeshBuffers = ConvertMeshDataToMeshBuffers(MeshData);

	PangeaVoxelComponent->SetCachedMeshBuffers(VoxelMeshBuffers);
}
