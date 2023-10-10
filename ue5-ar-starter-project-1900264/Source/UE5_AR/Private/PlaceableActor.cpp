
#include "PlaceableActor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ARPin.h"
#include "Components/BoxComponent.h"
#include "ARBlueprintLibrary.h"


APlaceableActor::APlaceableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	ScoreCheckpoint = CreateDefaultSubobject<UBoxComponent>(TEXT("BasketCheckpoint"));
	ScoreCheckpoint->SetupAttachment(StaticMeshComponent);

	BackboardBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BackboardBoxComponent"));
	BackboardBoxComp->SetupAttachment(StaticMeshComponent);

	SuggestedVelocityLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Suggested Velocity Location"));
	SuggestedVelocityLocation->SetupAttachment(StaticMeshComponent);
}


//////////////       BEGIN PLAY       //////////////
void APlaceableActor::BeginPlay()
{
	Super::BeginPlay();
	
}


//////////////       TICK       //////////////
void APlaceableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Making sure the actor remains on the ARPin that has been found.
	if(PinComponent)
	{
		auto TrackingState = PinComponent->GetTrackingState();
		switch (TrackingState)
		{

		case EARTrackingState::Tracking:
			SceneComponent->SetVisibility(true);
			SetActorTransform(PinComponent->GetLocalToWorldTransform());
			SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
			break;

		case EARTrackingState::NotTracking:
			PinComponent = nullptr;
			break;
		}
	}
}

