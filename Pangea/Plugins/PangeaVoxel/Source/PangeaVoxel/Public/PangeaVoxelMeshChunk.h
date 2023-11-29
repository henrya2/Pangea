#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "PangeaMeshTangent.h"

struct FPangeaVoxelMeshData
{
	TArray<uint32> Indices;
	TArray<FVector3f> Positions;

	// Will not be set if bRenderWorld is false
	TArray<FVector3f> Normals;
	TArray<FPangeaMeshTangent> Tangents;
	TArray<FColor> Colors;
	TArray<TArray<FVector2f>> TextureCoordinates;
};

struct FPangeaVoxelMeshChunk
{

};
