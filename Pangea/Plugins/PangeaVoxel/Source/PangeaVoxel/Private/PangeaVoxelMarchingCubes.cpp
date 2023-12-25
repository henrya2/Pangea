#include "PangeaVoxelMarchingCubes.h"
#include "PangeaVoxelData.h"
#include "PangeaVoxelMeshChunk.h"
#include "Transvoxel.h"
#include "PangeaVoxelCommonsPrivate.h"

FPangeaVoxelMarchingCubes::FPangeaVoxelMarchingCubes(int32 InChunkSize)
	: ChunkSize(InChunkSize)
{
	CurrentCache.SetNumUninitialized(ChunkSize * ChunkSize * 4);
	OldCache.SetNumUninitialized(ChunkSize * ChunkSize * 4);

	CurrentCacheData = CurrentCache.GetData();
	OldCacheData = OldCache.GetData();

	Step = 1;
}

FPangeaVoxelMarchingCubes::~FPangeaVoxelMarchingCubes()
{

}

void FPangeaVoxelMarchingCubes::GenerateMeshFromChunk(const FPangeaVoxelData& VoxelData, const FVector3f& BasePosition, float VoxelScale, FPangeaVoxelMeshData& OutMesh)
{
/*
	using namespace Transvoxel;

	auto& Vertices = OutMesh.Positions;
	auto& Indices = OutMesh.Indices;

	auto VoxelValueToFloat = [](int32 VoxelVal) FORCENOINLINE
		{
			return VoxelVal / 127.f;
		};

	int32 CellCount = VoxelData.VertsSize - 1;

	int32 DataSize = VoxelData.VertsSize;
	int32 VoxelIndex = 0;
	for (int32 VZ = 0; VZ < CellCount; ++VZ, VoxelIndex += DataSize * DataSize)
	{
		for (int32 VY = 0; VY < CellCount; ++VY, VoxelIndex += DataSize)
		{
			alignas(8) int8 distance[8];
			for (int32 VX = 0; VX < CellCount; ++VX, VoxelIndex += 1)
			{
				CurrentCache[GetCacheIndex(0, VX, VY)] = -1; // Set EdgeIndex 0 to -1 if the cell isn't voxelized, eg all corners = 0

				int32 CubeIndices[8];
				CubeIndices[0] = VoxelIndex;										// (0, 0, 0)
				CubeIndices[1] = VoxelIndex + 1;									// (1, 0, 0)
				CubeIndices[2] = VoxelIndex + DataSize;								// (0, 1, 0) 
				CubeIndices[3] = VoxelIndex + DataSize + 1;							// (1, 1, 0)
				CubeIndices[4] = VoxelIndex + DataSize * DataSize;					// (0, 0, 1)
				CubeIndices[5] = VoxelIndex + DataSize * DataSize + 1;				// (1, 0, 1)
				CubeIndices[6] = VoxelIndex + DataSize * DataSize + DataSize;		// (0, 1, 1)
				CubeIndices[7] = VoxelIndex + DataSize * DataSize + DataSize + 1;	// (1, 1, 1)

				for (int32 tt = 0; tt < 8; ++tt)
				{
					distance[tt] = VoxelData.Data[CubeIndices[tt]];
				}

				uint32 CaseCode = 
					((distance[0] >> 7) & 0x01) | ((distance[2] >> 5) & 0x04) | ((distance[4] >> 3) & 0x10) | ((distance[6] >> 1) & 0x40) |
					((distance[1] >> 6) & 0x02) | ((distance[3] >> 4) & 0x08) | ((distance[5] >> 2) & 0x20) | (distance[7] & 0x80);

				const RegularCellData& CellData = regularCellData[regularCellClass[CaseCode]];

				if (CellData.geometryCounts != 0)
				{
					// first bit:  X is valid
					// second bit: Y is valid
					// third bit:  Z is valid
					const uint8 ValidityMask = uint8(VX != 0) | (uint8(VY != 0) << 1) | (uint8(VZ != 0) << 2);
					uint32 CellVertexIndex[12];

					int32 TriangleCount = CellData.GetTriangleCount();

					int32 VertexCount = CellData.GetVertexCount();
					const uint16* VertexArray = regularVertexData[CaseCode];

					TStaticArray<int32, 16> VertexIndices;
					for (int32 a = 0; a < VertexCount; ++a)
					{
						uint32 Index;
						int32 VertexIndex = -2;

						uint16 VertexData = VertexArray[a];
						uint16 V0 = (VertexData >> 4) & 0x0F;
						uint16 V1 = VertexData & 0x0F;

						int32 D0 = distance[V0];
						int32 D1 = distance[V1];

						uint8 EdgeIndex = (VertexData >> 8) & 0x0F; 
						// the lower nibble tells the index of the vertex in the preceding cell that should be reused 
						// or the index of the vertex in the current cell that should be created

						// Direction to go to use an already created vertex: 
						// first bit:  X is different
						// second bit: Y is different
						// third bit:  Z is different
						// fourth bit: vertex isn't cached
						uint8 CacheDirection = VertexData >> 12;

						if (D0 == 0)
						{
							EdgeIndex = 0;
							CacheDirection = V0 ^ 7;
						}
						if (D1 == 0)
						{
							EdgeIndex = 0;
							CacheDirection = V1 ^ 7;
						}

						const bool bIsVertexCached = ((ValidityMask & CacheDirection) == CacheDirection) && (CacheDirection != 0); 
						// CacheDirection == 0 => LocalIndexB = 0 (as only B can be = 7) and ValueAtB = 0

						if (bIsVertexCached)
						{
							int32 XDiff = (CacheDirection & 0x01) != 0 ? 1 : 0;
							int32 YDiff = (CacheDirection & 0x02) != 0 ? 1 : 0;
							int32 ZDiff = (CacheDirection & 0x04) != 0 ? 1 : 0;

							VertexIndex = (ZDiff ? OldCache : CurrentCache)[GetCacheIndex(EdgeIndex, VX - XDiff, VY - YDiff)];
						}

						if (!bIsVertexCached || VertexIndex == -1)
						{
							const FIntVector PositionA((VX + (V0 & 0x01)) * Step,
								(VY + ((V0 & 0x02) >> 1)) * Step,
								(VZ + ((V0 & 0x04) >> 2)) * Step);
							const FIntVector PositionB((VX + (V1 & 0x01)) * Step, 
								(VY + ((V1 & 0x02) >> 1)) * Step, 
								(VZ + ((V1 & 0x04) >> 2)) * Step);

							FVector3f IntersectionPoint;
							FIntVector MatPosition;

							if (EdgeIndex == 0)
							{
								if (D0 == 0)
								{
									IntersectionPoint = FVector3f(PositionA);
								}
								else
								{
									IntersectionPoint = FVector3f(PositionB);
								}
							}
							else // From Voxel plugin's LOD = 0
							{
								const float Alpha = VoxelValueToFloat(D1) / (VoxelValueToFloat(D1) - VoxelValueToFloat(D0));

								switch (EdgeIndex)
								{
								case 2: // X
									IntersectionPoint = FVector3f(FMath::Lerp<float>(PositionA.X, PositionB.X, Alpha), PositionA.Y, PositionA.Z);
									break;
								case 1: // Y
									IntersectionPoint = FVector3f(PositionA.X, FMath::Lerp<float>(PositionA.Y, PositionB.Y, Alpha), PositionA.Z);
									break;
								case 3: // Z
									IntersectionPoint = FVector3f(PositionA.X, PositionA.Y, FMath::Lerp<float>(PositionA.Z, PositionB.Z, Alpha));
									break;
								default:
								}
							}

							VertexIndex = Vertices.Num();

							Vertices.Add(FVector3f(IntersectionPoint));

							// Save vertex if not on edge
							if (CacheDirection & 0x80 || !CacheDirection) // ValueAtB.IsNull() && LocalIndexB == 7 => !CacheDirection
							{
								CurrentCache[GetCacheIndex(EdgeIndex, VX, VY)] = VertexIndex;
							}
						}

						VertexIndices[a] = VertexIndex;

						int32 t = (D1 << 8) / (D1 - D0);

						if ((t & 0x00FF) != 0)
						{
							// Vertex lies in the interior of the edge
							float Alpha = VoxelValueToFloat(D1) / (VoxelValueToFloat(D1) - VoxelValueToFloat(D0));


						}
						else if (t == 0)
						{
							// Vertex lies at the higher-numbered endpoint.

							if (V1 == 7)
							{
								// This cell owns the vertex
							}
							else
							{
								// Try to reuse corner vertex from a preceding cell
							}
						}
						else
						{
							// Vertex lies at the lower-numbered endpoint
							// Always try to reuse corner vertex from a preceding cell.
						}
					}

					// Add triangles
					// 3 vertex per triangle
					for (int32 Index = 0; Index < 3 * CellData.GetTriangleCount(); Index += 3)
					{
						Indices.Add(VertexIndices[CellData.vertexIndex[Index + 0]]);
						Indices.Add(VertexIndices[CellData.vertexIndex[Index + 1]]);
						Indices.Add(VertexIndices[CellData.vertexIndex[Index + 2]]);
					}
				}
			}
		}

		Swap(CurrentCacheData, OldCacheData);
	}
*/
}

int32 FPangeaVoxelMarchingCubes::GetCacheIndex(int32 EdgeIndex, int32 X, int32 Y)
{
	return EdgeIndex + X * 4 + Y * 4 * ChunkSize;
}