#pragma once
#include "Components/PrimitiveComponent.h"
#include "PangeaVoxelData.h"

#include "PhysicsEngine/ConvexElem.h"

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

	void SetVoxelData(TSharedPtr<FPangeaVoxelData> InVoxelData);

	TSharedPtr<FPangeaVoxelData> GetVoxelData() const { return PangeaVoxelData; }

	void SetVoxelGridSize(float InGridSize);

	float GetVoxelGridSize() const { return VoxelGridSize; }

	//~ Begin
	virtual void GetUsedMaterials(TArray<UMaterialInterface *>& OutMaterials, bool bGetDebugMaterials = false) const override;

	UMaterialInterface* GetVoxelMaterial() const { return VoxelMaterial; }

	void SetCachedMeshBuffers(const TSharedPtr<FPangeaVoxelMeshBuffers>& InCachedMeshBuffers);

	TSharedPtr<FPangeaVoxelMeshBuffers> GetCachedMeshBuffers() const { return CachedMeshBuffers; }

	void SetCollisionConvexMeshes(const TArray<TArray<FVector>>& ConvexMeshes);

	//~ Begin USceneComponent Interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	//~ Begin USceneComponent Interface.

protected:
	void UpdateLocalBounds(const FBox& InBounds);

	/** Ensure ProcMeshBodySetup is allocated and configured */
	void CreateMeshBodySetup();
	/** Mark collision data as dirty, and re-create on instance if necessary */
	void UpdateCollision();
	/** Once async physics cook is done, create needed state */
	void FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup);

	/** Helper to create new body setup objects */
	UBodySetup* CreateBodySetupHelper();

protected:
	/** Convex shapes used for simple collision */
	UPROPERTY()
	TArray<FKConvexElem> CollisionConvexElems;

	/** 
	 *	Controls whether the complex (Per poly) geometry should be treated as 'simple' collision. 
	 *	Should be set to false if this component is going to be given simple collision and simulated.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Procedural Mesh")
	bool bUseComplexAsSimpleCollision;

	/**
	*	Controls whether the physics cooking should be done off the game thread. This should be used when collision geometry doesn't have to be immediately up to date (For example streaming in far away objects)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Procedural Mesh")
	bool bUseAsyncCooking;

	/** Queue for async body setups that are being cooked */
	UPROPERTY(transient)
	TArray<TObjectPtr<UBodySetup>> AsyncBodySetupQueue;

	/** Collision data */
	UPROPERTY(Instanced)
	TObjectPtr<class UBodySetup> MeshBodySetup;

protected:
	/** The material to render with */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	TObjectPtr<UMaterialInterface> VoxelMaterial;

	/** The local length of two adjacent voxel vertices */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Voxel)
	float VoxelGridSize = 50.f;

	TSharedPtr<FPangeaVoxelData> PangeaVoxelData;

	TSharedPtr<FPangeaVoxelMeshBuffers> CachedMeshBuffers;

	FBoxSphereBounds LocalBounds;
};