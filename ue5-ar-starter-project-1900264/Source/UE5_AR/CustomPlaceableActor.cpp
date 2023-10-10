#include "CustomPlaceableActor.h"

// Sets default values
ACustomPlaceableActor::ACustomPlaceableActor()
{
	// Set this empty to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACustomPlaceableActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACustomPlaceableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACustomPlaceableActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
