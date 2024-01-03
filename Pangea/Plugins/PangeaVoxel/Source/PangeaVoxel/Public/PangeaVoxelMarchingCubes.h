#pragma once

#include "CoreMinimal.h"

struct FPangeaVoxelData;
struct FPangeaVoxelMeshData;

class PANGEAVOXEL_API FPangeaVoxelMarchingCubes
{
public:
	FPangeaVoxelMarchingCubes(int32 InChunkSize);

	~FPangeaVoxelMarchingCubes();

	void GenerateMeshFromChunk(const FPangeaVoxelData& VoxelData, const FVector3f& BasePosition, float VoxelScale, FPangeaVoxelMeshData& OutMesh);

protected:
	int32 GetCacheIndex(int32 EdgeIndex, int32 X, int32 Y) const;

	int32 GetVoxelIndex(int32 X, int32 Y, int32 Z) const;

	int32 ChunkSize;
	int32 ChunkCellSize;

	int32 Step;

	// Cache data stores indices of 4 vertices described in 3.3 section of Voxel-Based Terrain  for Real-Time Virtual Simulations
	TArray<int32> CurrentCache;
	TArray<int32> OldCache;

	int32* CurrentCacheData;
	int32* OldCacheData;
};
