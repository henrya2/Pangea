#pragma once

#include "CoreMinimal.h"
#include "StaticMeshResources.h"

struct PANGEAVOXEL_API FPangeaVoxelMeshBuffers
{
	FStaticMeshVertexBuffers VertexBuffers;
	FRawStaticIndexBuffer IndexBuffer;

	FPangeaVoxelMeshBuffers();
	~FPangeaVoxelMeshBuffers();

	FBox LocalBounds = FBox(ForceInit);

	int32 GetNumVertices() const
	{
		return VertexBuffers.PositionVertexBuffer.GetNumVertices();
	}

	int32 GetNumIndices() const
	{
		return IndexBuffer.GetNumIndices();
	}
};
