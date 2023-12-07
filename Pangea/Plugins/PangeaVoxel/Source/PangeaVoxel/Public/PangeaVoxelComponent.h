#pragma once
#include "Components/PrimitiveComponent.h"

#include "PangeaVoxelData.h"

#include "PangeaVoxelComponent.generated.h"

class UMaterialInterface;
struct FPangeaVoxelMeshSection;
struct FPangeaVoxelMeshBuffers;

UCLASS(BlueprintType, ClassGroup = Pangea, meta = (BlueprintSpawnableComponent))
class PANGEAVOXEL_API UPangeaVoxelComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UPangeaVoxelComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual void GetUsedMaterials(TArray<UMaterialInterface *>& OutMaterials, bool bGetDebugMaterials = false) const override;

	UMaterialInterface* GetVoxelMaterial() const { return VoxelMaterial; }

	void SetCachedMeshBuffers(const TSharedPtr<FPangeaVoxelMeshBuffers>& InCachedMeshBuffers);

	TSharedPtr<FPangeaVoxelMeshBuffers> GetCachedMeshBuffers() const { return CachedMeshBuffers; }

	//~ Begin USceneComponent Interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ Begin USceneComponent Interface.

protected:
	void UpdateLocalBounds(const FBox& InBounds);

protected:
	/** The material to render with */
	UPROPERTY()
	TObjectPtr<UMaterialInterface> VoxelMaterial;

	UPROPERTY(config, EditAnywhere, Category = Packaging, AdvancedDisplay, meta = (DisplayName = "Directories to never cook", LongPackageName))
	TArray<FDirectoryPath> ExtraWorldcompositionScanPaths;

	TSharedPtr<FPangeaVoxelData> PangeaVoxelData;

	TSharedPtr<FPangeaVoxelMeshBuffers> CachedMeshBuffers;

	FBoxSphereBounds LocalBounds;
};