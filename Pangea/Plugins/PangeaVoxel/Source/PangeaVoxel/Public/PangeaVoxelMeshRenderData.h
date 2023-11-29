#pragma once

#include "CoreMinimal.h"
#include "StaticMeshResources.h"
#include "LocalVertexFactory.h"

struct FPangeaVoxelMeshBuffers;

// can only construct in render thread
class FPangeaVoxelMeshRenderData
{
public:
	FPangeaVoxelMeshRenderData(const TSharedPtr<FPangeaVoxelMeshBuffers>& InMeshBuffers, ERHIFeatureLevel::Type FeatureLevel);

	~FPangeaVoxelMeshRenderData();

public:
	TSharedPtr<FPangeaVoxelMeshBuffers> MeshBuffers;
	FLocalVertexFactory VertexFactory;
};
