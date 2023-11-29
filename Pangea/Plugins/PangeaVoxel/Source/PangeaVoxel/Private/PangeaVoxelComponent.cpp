#include "PangeaVoxelComponent.h"
#include "PangeaVoxelSceneProxy.h"
#include "PangeaVoxelMeshBuffers.h"

UPangeaVoxelComponent::UPangeaVoxelComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{

}

FPrimitiveSceneProxy* UPangeaVoxelComponent::CreateSceneProxy()
{
	if (GetCachedMeshBuffers().IsValid())
	{
		return new FPangeaVoxelSceneProxy(this);
	}
	return nullptr;
}

void UPangeaVoxelComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials /*= false*/) const
{
	if (VoxelMaterial)
	{
		OutMaterials.Add(VoxelMaterial);
	}
}

void UPangeaVoxelComponent::SetCachedMeshBuffers(const TSharedPtr<FPangeaVoxelMeshBuffers>& InCachedMeshBuffers)
{
	CachedMeshBuffers = InCachedMeshBuffers;
	UpdateLocalBounds(CachedMeshBuffers->LocalBounds);
	MarkRenderStateDirty();
}

FBoxSphereBounds UPangeaVoxelComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds Ret(LocalBounds.TransformBy(LocalToWorld));

	Ret.BoxExtent *= BoundsScale;
	Ret.SphereRadius *= BoundsScale;

	return Ret;
}

void UPangeaVoxelComponent::UpdateLocalBounds(const FBox& InBounds)
{
	if (LocalBounds == InBounds)
		return;

	LocalBounds = InBounds;

	// Update global bounds
	UpdateBounds();
	// Need to send to render thread
	MarkRenderTransformDirty();
}

