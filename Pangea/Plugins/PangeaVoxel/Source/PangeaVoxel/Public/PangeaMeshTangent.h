#pragma once

#include "CoreMinimal.h"

struct FPangeaMeshTangent
{
public:

	/** Direction of X tangent for this vertex */
	FVector3f TangentX;

	/** Bool that indicates whether we should flip the Y tangent when we compute it using cross product */
	bool bFlipTangentY;

	FPangeaMeshTangent()
		: TangentX(1.f, 0.f, 0.f)
		, bFlipTangentY(false)
	{}

	FPangeaMeshTangent(float X, float Y, float Z)
		: TangentX(X, Y, Z)
		, bFlipTangentY(false)
	{}

	FPangeaMeshTangent(FVector3f InTangentX, bool bInFlipTangentY)
		: TangentX(InTangentX)
		, bFlipTangentY(bInFlipTangentY)
	{}

	FVector3f GetY(const FVector3f& Normal) const
	{
		return (Normal ^ TangentX) * (bFlipTangentY ? -1 : 1);
	}
};
