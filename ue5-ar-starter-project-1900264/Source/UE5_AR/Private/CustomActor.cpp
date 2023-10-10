// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomActor.h"


// Sets default values
ACustomActor::ACustomActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	
	t = 0;
}


// Called when the game starts or when spawned
void ACustomActor::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
}


// Called every frame
void ACustomActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	t += DeltaTime;
	float scale, x, y;
	scale = 2.0f / (3.0f - cos(2.0f * t));
	scale *= 10;
	x = scale * cos(t);
	y = scale * sin(2.0f * t) / 2.0f;

	SetActorLocation(FVector(StartLocation.X , StartLocation.Y + x, StartLocation.Z +y));


}

