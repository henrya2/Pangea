#include "PangeaVoxelSceneProxy.h"
#include "PangeaVoxelComponent.h"
#include "MeshBatch.h"
#include "Materials/Material.h"
#include "MaterialDomain.h"
#include "Materials/MaterialRenderProxy.h"
#include "PangeaVoxelMeshRenderData.h"
#include "PangeaVoxelMeshBuffers.h"

FPangeaVoxelSceneProxy::FPangeaVoxelSceneProxy(UPangeaVoxelComponent* Component)
	: FPrimitiveSceneProxy(Component)
	, CollisionResponse(Component->GetCollisionResponseToChannels())
{
	const ERHIFeatureLevel::Type FeatureLevel = GetScene().GetFeatureLevel();

	Material = Component->GetVoxelMaterial();
	if (!Material)
	{
		Material = UMaterial::GetDefaultMaterial(MD_Surface);
	}

	MaterialRelevance = Material->GetRelevance_Concurrent(FeatureLevel);
}

FPangeaVoxelSceneProxy::~FPangeaVoxelSceneProxy()
{

}

bool FPangeaVoxelSceneProxy::IsCollisionView(const FEngineShowFlags& EngineShowFlags, bool& bDrawSimpleCollision, bool& bDrawComplexCollision) const
{
	bDrawSimpleCollision = bDrawComplexCollision = false;

	const bool bInCollisionView = EngineShowFlags.CollisionVisibility || EngineShowFlags.CollisionPawn;

#if ENABLE_DRAW_DEBUG
	// If in a 'collision view' and collision is enabled
	if (bInCollisionView && IsCollisionEnabled())
	{
		// See if we have a response to the interested channel
		bool bHasResponse = EngineShowFlags.CollisionPawn && CollisionResponse.GetResponse(ECC_Pawn) != ECR_Ignore;
		bHasResponse |= EngineShowFlags.CollisionVisibility && CollisionResponse.GetResponse(ECC_Visibility) != ECR_Ignore;

		if (bHasResponse)
		{
			// Visibility uses complex and pawn uses simple. However, if UseSimpleAsComplex or UseComplexAsSimple is used we need to adjust accordingly
			bDrawComplexCollision = (EngineShowFlags.CollisionVisibility && CollisionTraceFlag != ECollisionTraceFlag::CTF_UseSimpleAsComplex) || (EngineShowFlags.CollisionPawn && CollisionTraceFlag == ECollisionTraceFlag::CTF_UseComplexAsSimple);
			bDrawSimpleCollision = (EngineShowFlags.CollisionPawn && CollisionTraceFlag != ECollisionTraceFlag::CTF_UseComplexAsSimple) || (EngineShowFlags.CollisionVisibility && CollisionTraceFlag == ECollisionTraceFlag::CTF_UseSimpleAsComplex);
		}
	}
#endif
	return bInCollisionView;
}

void FPangeaVoxelSceneProxy::CreateRenderThreadResources()
{
	VoxelMeshRenderData = MakeShared<FPangeaVoxelMeshRenderData>(VoxelMeshBuffers, GetScene().GetFeatureLevel());
}

void FPangeaVoxelSceneProxy::DestroyRenderThreadResources()
{
	VoxelMeshRenderData.Reset();
}

void FPangeaVoxelSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FPangeaVoxelSceneProxy_GetDynamicMeshElements);

	const FEngineShowFlags& EngineShowFlags = ViewFamily.EngineShowFlags;

	const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

	bool bDrawSimpleCollision = false;
	bool bDrawComplexCollision = false;
	const bool bInCollisionView = IsCollisionView(EngineShowFlags, bDrawSimpleCollision, bDrawComplexCollision);

	// Sprites don't distinguish between simple and complex collision; when viewing visibility we should still render simple collision
	bDrawSimpleCollision |= bDrawComplexCollision;

	// Draw simple collision as wireframe if 'show collision', collision is enabled
	const bool bDrawWireframeCollision = EngineShowFlags.Collision && IsCollisionEnabled();

	auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
		GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL,
		FLinearColor(0, 0.5f, 1.f)
	);

	Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

	FMaterialRenderProxy* MaterialProxy = NULL;
	if (bWireframe)
	{
		MaterialProxy = WireframeMaterialInstance;
	}
	else
	{
		MaterialProxy = Material->GetRenderProxy();
	}

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			if ((bDrawSimpleCollision || bDrawWireframeCollision) && AllowDebugViewmodes())
			{
				const FSceneView* View = Views[ViewIndex];
				const bool bDrawSolid = !bDrawWireframeCollision;
				//DebugDrawCollision(View, ViewIndex, Collector, bDrawSolid);
			}
			// Draw bounds
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			//if (EngineShowFlags.Paper2DSprites)
			{
				RenderBounds(Collector.GetPDI(ViewIndex), EngineShowFlags, GetBounds(), IsSelected());
			}
#endif
		}
	}

	// Actual mesh rendering
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];

			auto& VertexBuffers = VoxelMeshBuffers->VertexBuffers;
			auto& IndexBuffer = VoxelMeshBuffers->IndexBuffer;
			auto& VertexFactory = VoxelMeshRenderData->VertexFactory;
			// Draw the mesh.
			FMeshBatch& Mesh = Collector.AllocateMesh();
			FMeshBatchElement& BatchElement = Mesh.Elements[0];
			BatchElement.IndexBuffer = &IndexBuffer;
			Mesh.bWireframe = bWireframe;
			Mesh.VertexFactory = &VertexFactory;
			Mesh.MaterialRenderProxy = MaterialProxy;

			bool bHasPrecomputedVolumetricLightmap;
			FMatrix PreviousLocalToWorld;
			int32 SingleCaptureIndex;
			bool bOutputVelocity;
			GetScene().GetPrimitiveUniformShaderParameters_RenderThread(GetPrimitiveSceneInfo(), bHasPrecomputedVolumetricLightmap, PreviousLocalToWorld, SingleCaptureIndex, bOutputVelocity);
			bOutputVelocity |= AlwaysHasVelocity();

			FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
			DynamicPrimitiveUniformBuffer.Set(GetLocalToWorld(), PreviousLocalToWorld, GetBounds(), GetLocalBounds(), true, bHasPrecomputedVolumetricLightmap, bOutputVelocity);
			BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;
			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = IndexBuffer.GetNumIndices() / 3;
			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.Type = PT_TriangleList;
			Mesh.DepthPriorityGroup = SDPG_World;
			Mesh.bCanApplyViewModeOverrides = false;
			Collector.AddMesh(ViewIndex, Mesh);
		}
	}
}

FPrimitiveViewRelevance FPangeaVoxelSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bDynamicRelevance = true;
	Result.bRenderInMainPass = ShouldRenderInMainPass();
	Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
	Result.bRenderCustomDepth = ShouldRenderCustomDepth();
	Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
	MaterialRelevance.SetPrimitiveViewRelevance(Result);
	Result.bVelocityRelevance = DrawsVelocity() && Result.bOpaque && Result.bRenderInMainPass;
	return Result;
}

SIZE_T FPangeaVoxelSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}
