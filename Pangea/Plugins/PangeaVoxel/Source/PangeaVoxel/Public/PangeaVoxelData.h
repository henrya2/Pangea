#pragma once

#include "CoreMinimal.h"

struct FPangeaVoxelData
{
	// Voxel chunk data stored in flat TArray
	TArray<int8> Data; 

	int32 VertsSize; // Number of vertices in one dimension

	FPangeaVoxelData()
		: VertsSize(0)
	{

	}
};
