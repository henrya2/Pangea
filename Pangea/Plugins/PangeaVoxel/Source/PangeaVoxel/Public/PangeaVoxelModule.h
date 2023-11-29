#pragma once

#include "Modules/ModuleInterface.h"

class PANGEAVOXEL_API FPangeaVoxelModule : public IModuleInterface
{
public:
	FPangeaVoxelModule()
	{

	}

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;
};
