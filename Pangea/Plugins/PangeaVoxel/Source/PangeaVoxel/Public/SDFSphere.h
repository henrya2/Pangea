#pragma once

#include "CoreMinimal.h"
#include "SDFSample.h"

class FSDFSphere : public ISDFSample
{
public:
	FSDFSphere(const FVector3f& InCenter, float InRadius);

	float GetValueAt(const FVector3f& Position) const override;

	FVector3f GetNormalAt(const FVector3f& Position) const override;

protected:
	FVector3f Center;
	float Radius;
};
