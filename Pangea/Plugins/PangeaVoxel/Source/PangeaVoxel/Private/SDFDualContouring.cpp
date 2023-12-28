#include "SDFDualContouring.h"

FSDFDualContouring::FSDFDualContouring(ISDFSample* InSDFSample, const FIntVector& InSampleBoxMin, const FIntVector& InSampleBoxMax)
	: ISDFMesher(InSDFSample, InSampleBoxMin, InSampleBoxMin)
{

}

void FSDFDualContouring::CreateMeshData(FPangeaVoxelMeshData& OutMesh) const
{
	// Iterate through all cells
	for (int32 Z = SampleBoxMin.Z; Z < SampleBoxMax.Z; ++Z)
	{
		for (int32 Y = SampleBoxMin.Y; Y < SampleBoxMax.Y; ++Y)
		{
			for (int32 X = SampleBoxMin.X; X < SampleBoxMax.X; ++X)
			{
				
			}
		}
	}
}
