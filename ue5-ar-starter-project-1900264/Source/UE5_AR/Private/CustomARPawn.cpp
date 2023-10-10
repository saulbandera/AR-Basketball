
#include "CustomARPawn.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "ARBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CustomGameMode.h"
#include "HelloARManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Kismet/KismetInputLibrary.h" 
#include "GameFramework/PlayerController.h" 
#include "BasketBall.h"
#include "PlaceableActor.h"


ACustomARPawn::ACustomARPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SceneComponent);

	BasketballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BasketballMeshComponent"));
	BasketballMesh->SetVisibility(false);
	BasketballMesh->SetupAttachment(CameraComponent);

	BasketballFireLocation = CreateDefaultSubobject<USceneComponent>(TEXT("BasketballFireLocation"));
	BasketballFireLocation->SetupAttachment(BasketballMesh);

	bWasBasketballMotionActivatedLastFrame = false;
	bDisplayBasketball = false;
	bIsTilting = false;
	bIsTiltedBack = false;
	bIsTiltedForward = false;
	bIsBasketballHoopPlaced = false;

}


//////////////       BEGIN PLAY       //////////////
void ACustomARPawn::BeginPlay()
{
	Super::BeginPlay();
	BasketballStartLocation = BasketballMesh->GetRelativeLocation();
}


//////////////       TICK       //////////////
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Line trace from center of screen to spawn hoop required space delimiter ("hologram") where screen centre is pointing at in real life
	if (!bIsBasketballHoopPlaced) 
	{
		auto Temp = GetWorld()->GetAuthGameMode();
		auto GM = Cast<ACustomGameMode>(Temp);
		// Getting the center of the viewport
		FVector2D Result = GetGameViewportSize();
		Result.X = Result.X / 2;
		Result.Y = Result.Y / 2;
		FVector CenterOfViewport = FVector(Result, 1);
		GM->LineTraceSpawnActor(CenterOfViewport, false);
	}

	// COMMENT NEEDED
	CalibrateMotionSensorData();

	// Check if phone was tilted
	if (bIsShootBallMotionActivated && !bWasBasketballMotionActivatedLastFrame)
	{
		if (bDisplayBasketball)
		{
			bShootBasketball = true;
		}
	}

	// Shoot a basketball 
	if (bShootBasketball && bIsBasketballHoopPlaced)
	{
		ShootBasketball();
		ResetBallLocation();
	}

	
	BasketballMesh->SetVisibility(bDisplayBasketball);

	// COMMENT NEEDED
	CheckBallMotionActivatedLastFrame();
}


//////////////       SETUP PLAYER INPUT COMPONENT       //////////////
void ACustomARPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind various axes to functions
	PlayerInputComponent->BindVectorAxis(EKeys::RotationRate, this, &ACustomARPawn::OnRotation);
	PlayerInputComponent->BindVectorAxis(EKeys::Tilt, this, &ACustomARPawn::OnTilt);
	PlayerInputComponent->BindVectorAxis(EKeys::Acceleration, this, &ACustomARPawn::OnAcceleration);

	// Bind various player inputs to functions
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACustomARPawn::OnScreenTouch);

}


//////////////       ON SCREEN TOUCH       //////////////
void ACustomARPawn::OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos)
{
	// Spawn the basketball hoop
	if (!bIsBasketballHoopPlaced) 
	{
		auto Temp = GetWorld()->GetAuthGameMode();
		auto GM = Cast<ACustomGameMode>(Temp);

		if (GM)
		{
			if (GM->CanHoopBePlaced()) {
				// Getting the center of the viewport
				FVector2D Result = GetGameViewportSize();
				Result.X = Result.X / 2;
				Result.Y = Result.Y / 2;
				FVector CenterOfViewport = FVector(Result, 1);

				GM->LineTraceSpawnActor(CenterOfViewport, true);
				bIsBasketballHoopPlaced = true;
				// Stop updating planes
				GM->GetARManager()->SetPlanesUpdateStatus(false);
			}
		}
	}
	else
	{
		bDisplayBasketball = true;
	}
}


//////////////       ON TILT       //////////////
void ACustomARPawn::OnTilt(FVector NewTilt)
{
	Tilt = NewTilt;
}


//////////////       ON ACCELERATION       //////////////
void ACustomARPawn::OnAcceleration(FVector NewAcceleration)
{
	Acceleration = NewAcceleration;
}


//////////////       ON ROTATION       //////////////
void ACustomARPawn::OnRotation(FVector NewRotation)
{
	Rotation = NewRotation;
}


//////////////       SHOOT BASKETBALL       //////////////
void ACustomARPawn::ShootBasketball()
{
	if (BasketballClass)
	{
		if (UWorld* const World = GetWorld())
		{
			const FRotator SpawnRotation = CameraComponent->GetComponentRotation();

			// Get the spawn location of the ball
			FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(FVector(0));

			if (BasketballFireLocation)
			{
				SpawnLocation = BasketballFireLocation->GetComponentLocation();
			}
			

			// Set spawn collision handling override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the basketball 
			auto Basketball = World->SpawnActor<ABasketBall>(BasketballClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

			// Get suggested velocity for ball to go from spawn location to hoop
			auto Temp = GetWorld()->GetAuthGameMode();
			auto GM = Cast<ACustomGameMode>(Temp);
			FVector LaunchVelocity;
			FVector EndPos = GM->GetSuggestedVelocityEndPos();
			if (GM)
			{
				UGameplayStatics::SuggestProjectileVelocity_CustomArc(Basketball, LaunchVelocity, SpawnLocation, EndPos,0,0.5);
			}

			const FVector RotatedVector = SpawnRotation.RotateVector(LaunchVelocity);

			Basketball->SetBasketballVelocity(FVector(RotatedVector.X,0, RotatedVector.Z));

			bShootBasketball = false;
			bDisplayBasketball = false;
		}
	}
}


//////////////       RESET BALL LOCATION       //////////////
void ACustomARPawn::ResetBallLocation()
{
	BasketballMesh->SetRelativeLocation(BasketballStartLocation);
}


//////////////       CHECK BALL MOTION ACTIVATED LAST FRAME       //////////////
void ACustomARPawn::CheckBallMotionActivatedLastFrame()
{
	if(bIsShootBallMotionActivated)
	{
		bWasBasketballMotionActivatedLastFrame = true;
	}
	else
	{
		bWasBasketballMotionActivatedLastFrame = false;
	}
}


//////////////       CALIBRATE MOTION SENSOR DATA       //////////////
void ACustomARPawn::CalibrateMotionSensorData()
{
	// Phone motion sensor data includes Tilt, Acceleration and Rotation this
	// data is used for implementing the flick gesture to throw the ball.
	
	// Tilt: this value decrements when device is being tilted back.
	// The Y component of Tilt increments when tilting back and decrements
	// when tilting forward.
	// The Z component of Tilt decrements when tilting back and increments
	// when tilting forward.

	// Acceleration: it's absolute value is incremented when the device is tilted back.
	// The Z component of Acceleration increments when either tilting back or forward
	// and will be close to zero when device is kept still.

	// Rotation: this value varies on the X axis and is incremented when the device is tilted back.
	// The X component of Rotation increments when tilting back and decrements when tilting forward.

	const float TiltYComponent = FMath::Abs(Tilt.Y);
	const float TiltZComponent = FMath::Abs(Tilt.Z);
	const float RotationXComponent = FMath::Abs(Rotation.X);
	const float AccelerationZComponent = FMath::Abs(Acceleration.Z);

	// Check if the device is being moved
	if (AccelerationZComponent > 1)
	{
		bIsTilting = true;

		// Check if the device is being tilted back 
		if (TiltYComponent > 2.8 && RotationXComponent > 3)
		{
			bIsTiltedBack = true;
		}

		if (bIsTiltedBack)
		{
			// Check if the device is being tilted forward 
			if (TiltYComponent < 2 && TiltZComponent > 1.5 && RotationXComponent < 2)
			{
				bIsTiltedForward = true;
				bIsTiltedBack = false;
			}
		}
	}
	else
	{
		if (bIsTiltedForward)
		{
			// Shoot basketball
			bIsTiltedForward = false;
			bIsShootBallMotionActivated = true;
		}
		else
		{
			bIsTilting = false;
			bIsShootBallMotionActivated = false;
		}
	}

}




