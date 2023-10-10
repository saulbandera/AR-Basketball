// Copyright 2017 Google Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ARPlaneActor.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"


AARPlaneActor::AARPlaneActor()
{
	PlanePolygonMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("PlanePolygonMesh"));
	RootComponent = PlanePolygonMeshComponent;

	// Take material from editor
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("Material'/Game/Assets/Materials/ARPlane_Mat.ARPlane_Mat'"));
	Material_ = MaterialAsset.Object;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


//////////////       BEGIN PLAY       //////////////
void AARPlaneActor::BeginPlay()
{
	Super::BeginPlay();
	PlaneMaterial = UMaterialInstanceDynamic::Create(Material_, this);
	PlaneMaterial->SetScalarParameterValue("TextureRotationAngle", FMath::RandRange(0.0f, 1.0f));
	PlanePolygonMeshComponent->SetMaterial(0, PlaneMaterial);
}


//////////////       TICK       //////////////
void AARPlaneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlanePolygonMeshComponent->SetWorldTransform(ARCorePlaneObject->GetLocalToWorldTransform());

	switch (ARCorePlaneObject->GetTrackingState())
	{
	case EARTrackingState::Tracking:
		PlanePolygonMeshComponent->SetVisibility(false);
		break;
	case EARTrackingState::NotTracking:
		PlanePolygonMeshComponent->SetVisibility(false);
		break;
	case EARTrackingState::StoppedTracking:
		PlanePolygonMeshComponent->SetVisibility(false);
		break;
	}
}


//////////////       UPDATE PLANE POLYGON MESH       //////////////
void AARPlaneActor::UpdatePlanePolygonMesh()
{
	// Update polygon mesh vertex indices, using triangle fan due to its convex.
	TArray<FVector> BoundaryVertices;

	// Obtain the boundary vertices from ARCore's plane geometry
	BoundaryVertices = ARCorePlaneObject->GetBoundaryPolygonInLocalSpace();

	int BoundaryVerticesNum = BoundaryVertices.Num();

	if (BoundaryVerticesNum < 3)
	{
		PlanePolygonMeshComponent->ClearMeshSection(0);
		return;
	}

	int PolygonMeshVerticesNum = BoundaryVerticesNum * 2;

	// Triangle number is interior(n-2 for convex polygon) plus perimeter (EdgeNum * 2);
	int TriangleNum = BoundaryVerticesNum - 2 + BoundaryVerticesNum * 2;

	TArray<FVector> PolygonMeshVertices;
	TArray<FLinearColor> PolygonMeshVertexColors;
	TArray<int> PolygonMeshIndices;
	TArray<FVector> PolygonMeshNormals;
	TArray<FVector2D> PolygonMeshUVs;

	PolygonMeshVertices.Empty(PolygonMeshVerticesNum);
	PolygonMeshVertexColors.Empty(PolygonMeshVerticesNum);
	PolygonMeshIndices.Empty(TriangleNum * 3);
	PolygonMeshNormals.Empty(PolygonMeshVerticesNum);

	// Creating the triangle fan from the vertices obtained
	FVector PlaneNormal = ARCorePlaneObject->GetLocalToWorldTransform().GetRotation().GetUpVector();

	for (int i = 0; i < BoundaryVerticesNum; i++)
	{
		FVector BoundaryPoint = BoundaryVertices[i];
		float BoundaryToCenterDist = BoundaryPoint.Size();
		float FeatheringDist = FMath::Min(BoundaryToCenterDist, EdgeFeatheringDistance);
		FVector InteriorPoint = BoundaryPoint - BoundaryPoint.GetUnsafeNormal() * FeatheringDist;

		PolygonMeshVertices.Add(BoundaryPoint);
		PolygonMeshVertices.Add(InteriorPoint);

		PolygonMeshUVs.Add(FVector2D(BoundaryPoint.X, BoundaryPoint.Y));
		PolygonMeshUVs.Add(FVector2D(InteriorPoint.X, InteriorPoint.Y));

		PolygonMeshNormals.Add(PlaneNormal);
		PolygonMeshNormals.Add(PlaneNormal);

		PolygonMeshVertexColors.Add(FLinearColor(0.0f, 0.f, 0.f, 0.f));
		PolygonMeshVertexColors.Add(FLinearColor(0.0f, 0.f, 0.f, 1.f));
	}

	// Generate triangle indices
	// Perimeter triangles
	for (int i = 0; i < BoundaryVerticesNum - 1; i++)
	{
		PolygonMeshIndices.Add(i * 2);
		PolygonMeshIndices.Add(i * 2 + 2);
		PolygonMeshIndices.Add(i * 2 + 1);

		PolygonMeshIndices.Add(i * 2 + 1);
		PolygonMeshIndices.Add(i * 2 + 2);
		PolygonMeshIndices.Add(i * 2 + 3);
	}

	// Adding the last triangle to close the plane mesh completely
	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2);
	PolygonMeshIndices.Add(0);
	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2 + 1);

	PolygonMeshIndices.Add((BoundaryVerticesNum - 1) * 2 + 1);
	PolygonMeshIndices.Add(0);
	PolygonMeshIndices.Add(1);

	// Interior triangles
	for (int i = 3; i < PolygonMeshVerticesNum - 1; i += 2)
	{
		PolygonMeshIndices.Add(1);
		PolygonMeshIndices.Add(i);
		PolygonMeshIndices.Add(i + 2);
	}

	// No need to fill uv and tangent;
	PlanePolygonMeshComponent->CreateMeshSection_LinearColor(0, PolygonMeshVertices, PolygonMeshIndices, PolygonMeshNormals, PolygonMeshUVs, PolygonMeshVertexColors, TArray<FProcMeshTangent>(), false);
}


//////////////       SET COLOR       //////////////
void AARPlaneActor::SetColor(FColor InColor) {
	PlaneColor = InColor;
	PlaneMaterial->SetVectorParameterValue("PlaneTint", InColor);
}