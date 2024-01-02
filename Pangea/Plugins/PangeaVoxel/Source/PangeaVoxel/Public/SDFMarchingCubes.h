#pragma once

#include "SDFMesher.h"

class FSDFMarchingCubes : public ISDFMesher
{
public:
	FSDFMarchingCubes(ISDFSample* InSDFSample, const FIntVector& InSampleBoxMin, const FIntVector& InSampleBoxMax);

	virtual void CreateMeshData(FPangeaVoxelMeshData& OutMesh) const override;

	void GenerateFaces(FVoxelVertex Corners[8], int32 EdgeIndex, FPangeaVoxelMeshData& OutMesh) const;

protected:
	static int _edgeTable[256];
	static int _triTable[256][16];
};
