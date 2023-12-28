#pragma once

#include "SDFMesher.h"

class FSDFDualContouring : public ISDFMesher
{
public:
	FSDFDualContouring(ISDFSample* InSDFSample, const FIntVector& InSampleBoxMin, const FIntVector& InSampleBoxMax);

	virtual void CreateMeshData(FPangeaVoxelMeshData& OutMesh) const override;
};
