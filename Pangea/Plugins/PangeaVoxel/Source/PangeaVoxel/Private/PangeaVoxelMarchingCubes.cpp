#include "PangeaVoxelMarchingCubes.h"
#include "PangeaVoxelData.h"
#include "PangeaVoxelMeshChunk.h"
#include "Transvoxel.h"
#include "PangeaVoxelCommonsPrivate.h"

FPangeaVoxelMarchingCubes::FPangeaVoxelMarchingCubes(int32 InChunkSize)
	: ChunkSize(InChunkSize)
{
	ChunkCellSize = ChunkSize - 1;
	CurrentCache.SetNumUninitialized(ChunkCellSize * ChunkCellSize * 4);
	OldCache.SetNumUninitialized(ChunkCellSize * ChunkCellSize * 4);

	CurrentCacheData = CurrentCache.GetData();
	OldCacheData = OldCache.GetData();

	Step = 1;
}

FPangeaVoxelMarchingCubes::~FPangeaVoxelMarchingCubes()
{

}

void FPangeaVoxelMarchingCubes::GenerateMeshFromChunk(const FPangeaVoxelData& VoxelData, const FVector3f& BasePosition, float VoxelScale, FPangeaVoxelMeshData& OutMesh)
{
	using namespace Transvoxel;

	auto& Vertices = OutMesh.Positions;
	auto& Normals = OutMesh.Normals;
	auto& Tangents = OutMesh.Tangents;
	auto& TextureCoords = OutMesh.TextureCoordinates;
	auto& Colors = OutMesh.Colors;
	auto& Indices = OutMesh.Indices;

	auto VoxelValueToFloat = [](int32 VoxelVal) FORCENOINLINE
		{
			return VoxelVal / 127.f;
		};

	int32 CellCount = VoxelData.VertsSize - 1;

	int32 DataSize = VoxelData.VertsSize;
	for (int32 VZ = 0; VZ < CellCount; ++VZ)
	{
		for (int32 VY = 0; VY < CellCount; ++VY)
		{
			alignas(8) int8 distance[8];
			for (int32 VX = 0; VX < CellCount; ++VX)
			{
				CurrentCacheData[GetCacheIndex(0, VX, VY)] = -1; // Set EdgeIndex 0 to -1 if the cell isn't voxelized, eg all corners = 0
				CurrentCacheData[GetCacheIndex(1, VX, VY)] = -1; // Set EdgeIndex 0 to -1 if the cell isn't voxelized, eg all corners = 0
				CurrentCacheData[GetCacheIndex(2, VX, VY)] = -1; // Set EdgeIndex 0 to -1 if the cell isn't voxelized, eg all corners = 0
				CurrentCacheData[GetCacheIndex(3, VX, VY)] = -1; // Set EdgeIndex 0 to -1 if the cell isn't voxelized, eg all corners = 0

				int32 CubeIndices[8];
				CubeIndices[0] = GetVoxelIndex(VX,     VY,     VZ);						// (0, 0, 0)
				CubeIndices[1] = GetVoxelIndex(VX + 1, VY,     VZ);						// (1, 0, 0)
				CubeIndices[2] = GetVoxelIndex(VX,     VY + 1, VZ);						// (0, 1, 0) 
				CubeIndices[3] = GetVoxelIndex(VX + 1, VY + 1, VZ);						// (1, 1, 0)
				CubeIndices[4] = GetVoxelIndex(VX    , VY    , VZ + 1);					// (0, 0, 1)
				CubeIndices[5] = GetVoxelIndex(VX + 1, VY    , VZ + 1);					// (1, 0, 1)
				CubeIndices[6] = GetVoxelIndex(VX    , VY + 1, VZ + 1);					// (0, 1, 1)
				CubeIndices[7] = GetVoxelIndex(VX + 1, VY + 1, VZ + 1);					// (1, 1, 1)

				for (int32 tt = 0; tt < 8; ++tt)
				{
					distance[tt] = VoxelData.Data[CubeIndices[tt]];
				}

				uint32 CaseCode = 
					((distance[0] >> 7) & 0x01) | ((distance[2] >> 5) & 0x04) | ((distance[4] >> 3) & 0x10) | ((distance[6] >> 1) & 0x40) |
					((distance[1] >> 6) & 0x02) | ((distance[3] >> 4) & 0x08) | ((distance[5] >> 2) & 0x20) | (distance[7] & 0x80);
				//CaseCode ^= 0xFF; // Flip the case code

				const RegularCellData& CellData = regularCellData[regularCellClass[CaseCode]];

				if (CellData.geometryCounts != 0)
				{
					// first bit:  X is valid
					// second bit: Y is valid
					// third bit:  Z is valid
					const uint8 ValidityMask = uint8(VX != 0) | (uint8(VY != 0) << 1) | (uint8(VZ != 0) << 2);

					int32 TriangleCount = CellData.GetTriangleCount();

					int32 VertexCount = CellData.GetVertexCount();
					const uint16* VertexArray = regularVertexData[CaseCode];

					TStaticArray<int32, 16> VertexIndices;

					//for (int32 a = 0; a < VertexCount; ++a)
					for (int32 a = VertexCount - 1; a >= 0; --a)
					{
						int32 VertexIndex = -2;

						uint16 VertexData = VertexArray[a];
						uint16 V0 = (VertexData >> 4) & 0x0F;
						uint16 V1 = VertexData & 0x0F;

						int32 D0 = distance[V0];
						int32 D1 = distance[V1];

						uint8 EdgeIndex = (VertexData >> 8) & 0x0F; 
						// The lower nibble of higher byte tells the index of the vertex in the preceding cell that should be reused 
						// or the index of the vertex in the current cell that should be created

						// Direction to go to use an already created vertex: 
						// first bit:  X is different
						// second bit: Y is different
						// third bit:  Z is different
						// fourth bit: vertex isn't cached
						uint8 CacheDirection = VertexData >> 12;

						// Look at Voxel-Based Terrain for Real-Time Virtual Simulations, 3.3 High-Performance Implementation
						// In the event that the sample value at a corner is exactly zero ................
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

							VertexIndex = (ZDiff != 0 ? OldCacheData : CurrentCacheData)[GetCacheIndex(EdgeIndex, VX - XDiff, VY - YDiff)];

							//checkf(VertexIndex < 205 && VertexIndex >= 0, TEXT("VertexInde is out of range!!! VertexIndex: %d"), VertexIndex);
						}

						// VertexIndex = -1 is only from Cache, it is corner 7 (CacheEdge = 0) 
						if (!bIsVertexCached || VertexIndex == -1)
						{
							// Please refer to Figure 3.7, (V0 and V1 are the corners of a cell numbered as figure shown.)
							// Voxel vertex is carefully numbered, as lowest number is 0, ZYX each bit contain +1 in corresponding axis.
							const FIntVector PositionA((VX + (V0 & 0x01)) * Step,
								(VY + ((V0 & 0x02) >> 1)) * Step,
								(VZ + ((V0 & 0x04) >> 2)) * Step);
							const FIntVector PositionB((VX + (V1 & 0x01)) * Step, 
								(VY + ((V1 & 0x02) >> 1)) * Step, 
								(VZ + ((V1 & 0x04) >> 2)) * Step);

							FVector3f IntersectionPoint;

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
							else 
							{
								const float Alpha = VoxelValueToFloat(D0) / (VoxelValueToFloat(D0) - VoxelValueToFloat(D1));

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
									break;
								}
							}

							VertexIndex = Vertices.Num();

							Vertices.Add(FVector3f(IntersectionPoint) * VoxelScale + BasePosition);
							TextureCoords.Add(TArray<FVector2f>{{IntersectionPoint.X, IntersectionPoint.Y}});
							Colors.Add(FColor::White);

							// Central differences
							int32 Xn = FMath::Clamp(VX - 1, 0, DataSize - 1);
							int32 Xp = FMath::Clamp(VX + 1, 0, DataSize - 1);
							int32 Yn = FMath::Clamp(VY - 1, 0, DataSize - 1);
							int32 Yp = FMath::Clamp(VY + 1, 0, DataSize - 1);
							int32 Zn = FMath::Clamp(VZ - 1, 0, DataSize - 1);
							int32 Zp = FMath::Clamp(VZ + 1, 0, DataSize - 1);

							// calculate gradient based voxel distances using central differences (a kind of finite difference), \delta x = (x+1) - (x-1) = 2;
							// \delta y and \delta z are similar too. 
							// Gradient = ((D(x + 1, y , z) - D(x - 1, y, z)) / 2, (D(x, y + 1 , z) - D(x, y - 1, z)) / 2, (D(x, y , z + 1) - D(x, y, z - 1)) / 2)
							float DiffX = VoxelValueToFloat(VoxelData.Data[GetVoxelIndex(Xp, VY, VZ)]) - VoxelValueToFloat(VoxelData.Data[GetVoxelIndex(Xn, VY, VZ)]);
							float DiffY = VoxelValueToFloat(VoxelData.Data[GetVoxelIndex(VX, Yp, VZ)]) - VoxelValueToFloat(VoxelData.Data[GetVoxelIndex(VX, Yn, VZ)]);
							float DiffZ = VoxelValueToFloat(VoxelData.Data[GetVoxelIndex(VX, VY, Zp)]) - VoxelValueToFloat(VoxelData.Data[GetVoxelIndex(VX, VY, Zn)]);
							FVector3f IntersectionNormal = FVector3f(DiffX, DiffY, DiffZ).GetSafeNormal(); // 1.f / 2.f is not necessary here, since it will renormalize.
							Normals.Add(IntersectionNormal);

							// Adds a fake tangent (1, 0, 0)
							Tangents.Add(FPangeaMeshTangent(1.f, 0, 0));

							// Save vertex if not on edge, be careful here, CacheDirection is the higher nibble of higher byte of vertexdata
							if (CacheDirection & 0x08 || !CacheDirection) // ValueAtB.IsNull() && LocalIndexB == 7 => !CacheDirection
							{
								CurrentCacheData[GetCacheIndex(EdgeIndex, VX, VY)] = VertexIndex;
							}
						}

						VertexIndices[VertexCount - 1 - a] = VertexIndex; // actual index in vertex data stream, reverse the order
						//VertexIndices[a] = VertexIndex;

						// original codes from paper
						/*
						int32 t = (D1 << 8) / (D1 - D0);

						if ((t & 0x00FF) != 0) // it is false only when D0 is 0 but D1 is not 0
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
						*/
					}

					// Add triangles
					// 3 vertex per triangle
					for (int32 Index = 0; Index < 3 * CellData.GetTriangleCount(); Index += 3)
					{
						// CellData.vertexIndex is a sequence of integer, like 0 1 2 ....(it is not related to other stuff)
						// actual indices in vertex data stream are stored in VertexIndices array which CellData.vertexIndex value corresponding to its array index.
						Indices.Add(VertexIndices[CellData.vertexIndex[Index + 0]]);
						Indices.Add(VertexIndices[CellData.vertexIndex[Index + 1]]);
						Indices.Add(VertexIndices[CellData.vertexIndex[Index + 2]]);
					}
				}
			}
		}

		// ping pong cachedata
		Swap(CurrentCacheData, OldCacheData);
	}
}

int32 FPangeaVoxelMarchingCubes::GetCacheIndex(int32 EdgeIndex, int32 X, int32 Y) const
{
	return EdgeIndex + X * 4 + Y * 4 * ChunkCellSize;
}

int32 FPangeaVoxelMarchingCubes::GetVoxelIndex(int32 X, int32 Y, int32 Z) const
{
	return Z * ChunkSize * ChunkSize + Y * ChunkSize + X;
}
