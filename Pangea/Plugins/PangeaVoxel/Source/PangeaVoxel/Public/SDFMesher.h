#pragma once

#include "CoreMinimal.h"

class ISDFSample;
struct FPangeaVoxelMeshData;

enum class ESDFMesherType
{
	MarchingCubes,
	DualContouring
};

class ISDFMesher
{
public:
	struct FCacheVoxel
	{
		float Density;
		FVector3f Normal;
	};

	struct FVoxelVertex
	{
		float Density;
		FVector3f Position;
	};

	struct FMeshPoint
	{
		FVector3f Position;
		FVector3f Normal;
	};

public:
	static ISDFMesher* CreateMesher(ESDFMesherType MesherType, ISDFSample* InSDFSample, const FIntVector& InSampleBoxMin, const FIntVector& InSampleBoxMax);

public:
	ISDFMesher(ISDFSample* InSDFSample, const FIntVector& InSampleBoxMin, const FIntVector& InSampleBoxMax);

	virtual ~ISDFMesher();

	int32 ConvertSamplePositionToIndex(const FIntVector& InPosition) const;

	int32 ConvertVoxelPositionToIndex(const FIntVector& InPosition) const;

	FIntVector ConvertSamplePositionToVoxelPosition(const FIntVector& InSamplePosition) const;

	FIntVector ConvertVoxelPositionToSamplePosition(const FIntVector& InVoxelPosition) const;

	virtual void ComputeCachedVoxels();

	virtual void CreateMeshData(FPangeaVoxelMeshData& OutMesh) const = 0;

protected:
	static float Adapt(float D0, float D1);

protected:
	ISDFSample* SDFSample;

	FIntVector SampleBoxMin;
	FIntVector SampleBoxMax;

	FIntVector SampleBoxSize;
	FIntVector NumSampleGridCells;

	TArray<FCacheVoxel> CachedVoxels;
};
