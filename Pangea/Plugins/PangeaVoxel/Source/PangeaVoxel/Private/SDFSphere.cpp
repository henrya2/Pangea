#include "SDFSphere.h"

FSDFSphere::FSDFSphere(const FVector3f& InCenter, float InRadius)
	: Center(InCenter)
	, Radius(InRadius)
{

}

float FSDFSphere::GetValueAt(const FVector3f& Position) const
{
	FVector3f DeltaVec = (Position - Center);

	return (DeltaVec | DeltaVec - Radius * Radius);
}

FVector3f FSDFSphere::GetNormalAt(const FVector3f& Position) const
{
	FVector3f DeltaVec = (Position - Center);

	return DeltaVec.GetSafeNormal();
}
