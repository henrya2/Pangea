#include "SDFMesher.h"
#include "SDFSample.h"
#include "SDFMarchingCubes.h"
#include "SDFDualContouring.h"

ISDFMesher* ISDFMesher::CreateMesher(ESDFMesherType MesherType, ISDFSample* InSDFSample, const FIntVector& InSampleBoxMin, const FIntVector& InSampleBoxMax)
{
	ISDFMesher* Mesher = nullptr;
	switch (MesherType)
	{
	case ESDFMesherType::MarchingCubes:
		Mesher = new FSDFMarchingCubes(InSDFSample, InSampleBoxMin, InSampleBoxMax);
		break;
	case ESDFMesherType::DualContouring:
		Mesher = new FSDFDualContouring(InSDFSample, InSampleBoxMin, InSampleBoxMax);
		break;
	default:
		break;
	}

	if (Mesher)
	{
		Mesher->ComputeCachedVoxels();
	}

	return Mesher;
}

ISDFMesher::ISDFMesher(ISDFSample* InSDFSample, const FIntVector& InSampleBoxMin, const FIntVector& InSampleBoxMax)
	: SDFSample(InSDFSample)
	, SampleBoxMin(InSampleBoxMin)
	, SampleBoxMax(InSampleBoxMax)
{
	SampleBoxSize = (SampleBoxMax - SampleBoxMin) + FIntVector(1, 1, 1);

	NumSampleGridCells = (SampleBoxMax - SampleBoxMin);
}

ISDFMesher::~ISDFMesher()
{

}

int32 ISDFMesher::ConvertSamplePositionToIndex(const FIntVector& InPosition) const
{
	FIntVector Position = InPosition - SampleBoxMin;

	return (Position.Z * SampleBoxSize.Y * SampleBoxSize.X + Position.Y * SampleBoxSize.X + Position.X);
}

int32 ISDFMesher::ConvertVoxelPositionToIndex(const FIntVector& InPosition) const
{
	return (InPosition.Z * SampleBoxSize.Y * SampleBoxSize.X + InPosition.Y * SampleBoxSize.X + InPosition.X);
}

FIntVector ISDFMesher::ConvertSamplePositionToVoxelPosition(const FIntVector& InSamplePosition) const
{
	return InSamplePosition - SampleBoxMin;
}

FIntVector ISDFMesher::ConvertVoxelPositionToSamplePosition(const FIntVector& InVoxelPosition) const
{
	return InVoxelPosition + SampleBoxMin;	
}

void ISDFMesher::ComputeCachedVoxels()
{
	CachedVoxels.SetNumUninitialized(SampleBoxSize.Z * SampleBoxSize.Y * SampleBoxSize.Z);

	for (int32 Z = SampleBoxMin.Z; Z <= SampleBoxMax.Z; ++Z)
	{
		for (int32 Y = SampleBoxMin.Y; Y <= SampleBoxMax.Y; ++Y)
		{
			for (int32 X = SampleBoxMin.X; X <= SampleBoxMax.X; ++X)
			{
				int32 CacheIndex = ConvertSamplePositionToIndex(FIntVector(X, Y, Z));
				FCacheVoxel& CacheVoxel = CachedVoxels[CacheIndex];
				CacheVoxel.Density = SDFSample->GetValueAt(FVector3f(X, Y, Z));
				CacheVoxel.Normal = SDFSample->GetNormalAt(FVector3f(X, Y, Z));
			}
		}
	}
}

float ISDFMesher::Adapt(float D0, float D1)
{
	return D0 / (D0 - D1);
}
