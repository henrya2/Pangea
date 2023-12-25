// ==============================================
//
// Transvoxel Algorithm Data Tables
// https://transvoxel.org/
//
// Copyright 2009 by Eric Lengyel
//
// ==============================================

#pragma once

#include "CoreMinimal.h"

namespace Transvoxel
{

	// The RegularCellData structure holds information about the triangulation
	// used for a single equivalence class in the modified Marching Cubes algorithm,
	// described in Section 3.2.

	struct RegularCellData
	{
		uint8	geometryCounts;		// High nibble is vertex count, low nibble is triangle count.
		uint8	vertexIndex[15];	// Groups of 3 indexes giving the triangulation.

		FORCEINLINE_DEBUGGABLE int32 GetVertexCount() const
		{
			return (geometryCounts >> 4);
		}

		FORCEINLINE_DEBUGGABLE int32 GetTriangleCount() const
		{
			return (geometryCounts & 0x0F);
		}
	};


	// The TransitionCellData structure holds information about the triangulation
	// used for a single equivalence class in the Transvoxel Algorithm transition cell,
	// described in Section 4.3.

	struct TransitionCellData
	{
		int32			geometryCounts;		// High nibble is vertex count, low nibble is triangle count.
		uint8	vertexIndex[36];	// Groups of 3 indexes giving the triangulation.

		FORCEINLINE_DEBUGGABLE int32 GetVertexCount() const
		{
			return (geometryCounts >> 4);
		}

		FORCEINLINE_DEBUGGABLE int32 GetTriangleCount() const
		{
			return (geometryCounts & 0x0F);
		}
	};

	extern PANGEAVOXEL_API const uint8 regularCellClass[256];
	extern PANGEAVOXEL_API const RegularCellData regularCellData[16];
	extern PANGEAVOXEL_API const uint16 regularVertexData[256][12];

	extern PANGEAVOXEL_API const uint8 transitionCellClass[512];
	extern PANGEAVOXEL_API const TransitionCellData transitionCellData[56];
	extern PANGEAVOXEL_API const uint8 transitionCornerData[13];
	extern PANGEAVOXEL_API const uint16 transitionVertexData[512][12];
}
