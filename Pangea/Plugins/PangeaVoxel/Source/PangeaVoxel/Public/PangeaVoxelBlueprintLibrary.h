#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PangeaVoxelData.h"
#include "PangeaVoxelBlueprintLibrary.generated.h"

class UPangeaVoxelComponent;

/**
 * A blueprint struct to wrap actual voxel data
 */
USTRUCT(BlueprintType)
struct FTestPangeaVoxelData
{
	GENERATED_BODY()

	FPangeaVoxelData RealData;
};

UCLASS()
class UPangeaVoxelBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "PangeaVoxel")
	static void GenerateAndSetTestVoxelMeshBuffers(UPangeaVoxelComponent* PangeaVoxelComponent);

	UFUNCTION(BlueprintCallable, Category = "PangeaVoxel")
	static void GenerateSphereVoxelData(FTestPangeaVoxelData& OutVoxelData, float Radius);
};
