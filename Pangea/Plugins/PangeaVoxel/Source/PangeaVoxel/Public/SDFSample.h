#pragma once

#include "CoreMinimal.h"

class ISDFSample
{
public:
	virtual float GetValueAt(const FVector3f& Position) const = 0;
	virtual FVector3f GetNormalAt(const FVector3f& Position) const = 0;
};
