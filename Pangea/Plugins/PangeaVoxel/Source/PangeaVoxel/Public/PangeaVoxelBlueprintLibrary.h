#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PangeaVoxelBlueprintLibrary.generated.h"

class UPangeaVoxelComponent;

UCLASS()
class UPangeaVoxelBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "PangeaVoxel")
	static void GenerateAndSetTestVoxelMeshBuffers(UPangeaVoxelComponent* PangeaVoxelComponent);
};
