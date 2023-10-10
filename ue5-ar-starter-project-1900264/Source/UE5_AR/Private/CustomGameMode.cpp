
#include "CustomGameMode.h"
#include "CustomARPawn.h"
#include "CustomGameState.h"
#include "CustomActor.h"
#include "HelloARManager.h"
#include "ARPin.h"
#include "ARBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "PlaceableActor.h"


ACustomGameMode::ACustomGameMode():
	SpawnedActor(nullptr), ARManager(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set the default pawn and game state to be our custom pawn and game state programatically
	DefaultPawnClass = ACustomARPawn::StaticClass();
	GameStateClass = ACustomGameState::StaticClass();

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("Material'/Game/Assets/Materials/ARPlane_Mat.ARPlane_Mat'"));
	Material_ = MaterialAsset.Object;

	bCanHoopBePlaced = false;
	bIsHoopPlaced = false;
}


//////////////       START PLAY       //////////////
void ACustomGameMode::StartPlay() 
{
	// Spawn an instance of the AR manager class which deals with finding and updating AR planes
	SpawnARManager();

	// This function will transcend to call BeginPlay on all the actors 
	Super::StartPlay();

	// Create the dynamic material instance for the hoop "hologram" and set it to green
	DelimiterMaterial = UMaterialInstanceDynamic::Create(Material_, this);
	DelimiterMaterial->SetScalarParameterValue("TextureRotationAngle", FMath::RandRange(0.0f, 1.0f));
	DelimiterMaterial->SetVectorParameterValue("PlaneTint", FColor::Emerald);

}


//////////////       TICK       //////////////
void ACustomGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


//////////////       LINE TRACE SPAWN ACTOR       //////////////
void ACustomGameMode::LineTraceSpawnActor(FVector ScreenPos, bool SpawnHoop)
{

	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos; // Corresponding 3D position in world space from touch on 2D screen space
	FVector WorldDir; // World space direction vector away from camera at touch point

	// Gets the screen touch in world space and the tracked objects from a line trace from the touch position
	UGameplayStatics::DeprojectScreenToWorld(playerController, FVector2D(ScreenPos), WorldPos, WorldDir);

	// Perform line trace against tracked AR geometry 
	auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ScreenPos), false, false, false, true);


	// Check if the location is valid
	if (TraceResult.IsValidIndex(0))
	{
		// Get the first found plane object in the line trace
		auto TrackedTF = TraceResult[0].GetLocalToWorldTransform();

		// We only want horizontal planes that face upwards
		if (TrackedTF.GetRotation().GetUpVector().Z == 1)
		{
			if (FVector::DotProduct(TrackedTF.GetRotation().GetUpVector(), WorldDir) < 0)
			{
				// Spawn the actor pin and get the transform
				UARPin* ActorPin = UARBlueprintLibrary::PinComponent(nullptr, TraceResult[0].GetLocalToWorldTransform(), TraceResult[0].GetTrackedGeometry());

				// Check if ARPins are available on your current device. 
				if (ActorPin)
				{
					// If the pin is valid 
					auto PinTF = ActorPin->GetLocalToWorldTransform();

					// Spawn a "hologram" of the basketball hoop so user knows how much space is required  
					if (!SpawnedActor && SpawnHoop == false)
					{
						const FActorSpawnParameters SpawnInfo;
						const FRotator MyRot(0, 0, 0);
						const FVector MyLoc(0, 0, 0);
						SpawnedActor = GetWorld()->SpawnActor<APlaceableActor>(BasketballHoop, MyLoc, MyRot, SpawnInfo);

					}

					// Spawn the basketball hoop
					else if (SpawnedActor && SpawnHoop == true)
					{
						bIsHoopPlaced = true;
						SpawnedActor->Destroy();
						const FActorSpawnParameters SpawnInfo;
						const FRotator MyRot(0, 0, 0);
						const FVector MyLoc(0, 0, 0);
						SpawnedActor = GetWorld()->SpawnActor<APlaceableActor>(BasketballHoop, MyLoc, MyRot, SpawnInfo);

					}

					// Set the spawned actor location based on the Pin
					SpawnedActor->SetActorTransform(PinTF);
					SpawnedActor->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
					SpawnedActor->PinComponent = ActorPin;

					// If hoop not spawned set material to green to represent that the hoop can be spawned at current location
					if (SpawnedActor && SpawnHoop == false)
					{
						DelimiterMaterial->SetVectorParameterValue("PlaneTint", FColor::Emerald);
						SpawnedActor->StaticMeshComponent->SetMaterial(0, DelimiterMaterial);
						SpawnedActor->StaticMeshComponent->SetMaterial(1, DelimiterMaterial);
						SpawnedActor->StaticMeshComponent->SetMaterial(2, DelimiterMaterial);
						bCanHoopBePlaced = true;
					}
				}
			}
		}
	}
	else
	{
		// Set material to red to represent that the hoop cannot be spawned at current location
		DelimiterMaterial->SetVectorParameterValue("PlaneTint", FColor::Red);
		bCanHoopBePlaced = false;
	}
}


//////////////       SPAWN AR MANAGER       //////////////
void ACustomGameMode::SpawnARManager()
{
	// Spawn an instance of the HelloARManager class (DONE)
	const FActorSpawnParameters SpawnInfo;
	const FRotator MyRot(0, 0, 0);
	const FVector MyLoc(0, 0, 0);
	ARManager = GetWorld()->SpawnActor<AHelloARManager>(MyLoc, MyRot, SpawnInfo);
}


//////////////       GETTERS AND SETTERS       //////////////
AHelloARManager* ACustomGameMode::GetARManager()
{
	return ARManager;
}

APlaceableActor* ACustomGameMode::GetPlaceableActor()
{
	return SpawnedActor;
}

int32 ACustomGameMode::GetScore()
{
	return GetGameState<ACustomGameState>()->GetScore();
}

void ACustomGameMode::SetScore(const int32 NewScore)
{
	GetGameState<ACustomGameState>()->SetScore(NewScore);
}

bool ACustomGameMode::CanHoopBePlaced()
{
	return bCanHoopBePlaced;
}

bool ACustomGameMode::IsHoopPlaced()
{
	return bIsHoopPlaced;
}



