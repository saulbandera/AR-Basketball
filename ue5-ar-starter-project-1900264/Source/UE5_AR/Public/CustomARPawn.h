
#pragma once

#include "GameFramework/Pawn.h"
#include "CustomARPawn.generated.h"

class UCameraComponent;
class USphereComponent;
class ABasketBall;

UCLASS()
class UE5_AR_API ACustomARPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACustomARPawn();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Helper function to get viewport size 
	static FVector2D GetGameViewportSize()
	{
		FVector2D Result = FVector2D(1, 1);
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(Result);
		}
		return Result;
	}

protected:
	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);
	void OnTilt(FVector NewTilt);
	void OnAcceleration(FVector NewAcceleration);
	void OnRotation(FVector NewRotation);
	void ShootBasketball();
	void ResetBallLocation();
	void CalibrateMotionSensorData();
	void CheckBallMotionActivatedLastFrame();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene Component")
		USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera Component")
		UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		UStaticMeshComponent* BasketballMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
		USceneComponent* BasketballFireLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<ABasketBall> BasketballClass;
private:
	bool bShootBasketball;
	bool bDisplayBasketball;

	bool bIsTilting;
	bool bIsTiltedBack;
	bool bIsTiltedForward;
	bool bIsShootBallMotionActivated;
	bool bWasBasketballMotionActivatedLastFrame;

	bool bIsBasketballHoopPlaced;

	FVector BasketballStartLocation;
	FVector Tilt;
	FVector Acceleration;
	FVector Rotation;

};
