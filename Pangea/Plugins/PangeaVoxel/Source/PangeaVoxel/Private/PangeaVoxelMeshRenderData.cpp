#include "PangeaVoxelMeshRenderData.h"
#include "PangeaVoxelMeshBuffers.h"

FPangeaVoxelMeshRenderData::FPangeaVoxelMeshRenderData(const TSharedPtr<FPangeaVoxelMeshBuffers>& InMeshBuffers, ERHIFeatureLevel::Type FeatureLevel)
	: MeshBuffers(InMeshBuffers)
	, VertexFactory(FeatureLevel, "FPangeaVoxelMeshRenderData")
{
	check(IsInRenderingThread());

	BeginInitResource(&MeshBuffers->VertexBuffers.PositionVertexBuffer);
	BeginInitResource(&MeshBuffers->VertexBuffers.StaticMeshVertexBuffer);
	BeginInitResource(&MeshBuffers->VertexBuffers.ColorVertexBuffer);
	BeginInitResource(&MeshBuffers->IndexBuffer);

	auto& VertexBuffers = MeshBuffers->VertexBuffers;
	auto& IndexBuffer = MeshBuffers->IndexBuffer;

	FLocalVertexFactory::FDataType Data;
	VertexBuffers.PositionVertexBuffer.BindPositionVertexBuffer(&VertexFactory, Data);
	VertexBuffers.StaticMeshVertexBuffer.BindTangentVertexBuffer(&VertexFactory, Data);
	VertexBuffers.StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer(&VertexFactory, Data);
	VertexBuffers.ColorVertexBuffer.BindColorVertexBuffer(&VertexFactory, Data);
	VertexFactory.SetData(Data);
	
	BeginInitResource(&VertexFactory);
}

FPangeaVoxelMeshRenderData::~FPangeaVoxelMeshRenderData()
{
	auto& VertexBuffers = MeshBuffers->VertexBuffers;
	auto& IndexBuffer = MeshBuffers->IndexBuffer;

	VertexBuffers.PositionVertexBuffer.ReleaseResource();
	VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
	VertexBuffers.ColorVertexBuffer.ReleaseResource();
	IndexBuffer.ReleaseResource();
	VertexFactory.ReleaseResource();
}

