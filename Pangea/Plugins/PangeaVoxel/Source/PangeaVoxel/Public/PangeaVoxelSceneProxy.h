#pragma once

#include "PrimitiveSceneProxy.h"
#include "StaticMeshResources.h"
#include "DynamicMeshBuilder.h"
#include "LocalVertexFactory.h"
#include "Materials/MaterialRelevance.h"

class UPangeaVoxelComponent;
class FPangeaVoxelMeshRenderData;
struct FPangeaVoxelMeshBuffers;

class PANGEAVOXEL_API FPangeaVoxelSceneProxy : public FPrimitiveSceneProxy
{
public:
	FPangeaVoxelSceneProxy(UPangeaVoxelComponent* Component);

	virtual ~FPangeaVoxelSceneProxy();

	bool IsCollisionView(const FEngineShowFlags& EngineShowFlags, bool& bDrawSimpleCollision, bool& bDrawComplexCollision) const;

	//~ Begin FPrimitiveSceneProxy Interface
	virtual void CreateRenderThreadResources() override;
	virtual void DestroyRenderThreadResources() override;
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	SIZE_T GetTypeHash() const;

	virtual bool CanBeOccluded() const override
	{
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }

	uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }
	//~ End FPrimitiveSceneProxy Interface

private:
	UMaterialInterface* Material;

	FMaterialRelevance MaterialRelevance;

	// The Collision Response of the component being proxied
	FCollisionResponseContainer CollisionResponse;
	ECollisionTraceFlag CollisionTraceFlag = ECollisionTraceFlag::CTF_UseDefault;

	TSharedPtr<FPangeaVoxelMeshBuffers> VoxelMeshBuffers;
	TSharedPtr<FPangeaVoxelMeshRenderData> VoxelMeshRenderData;
};