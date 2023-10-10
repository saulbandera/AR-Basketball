
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasketBall.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class USoundBase;

UCLASS()
class UE5_AR_API ABasketBall : public AActor
{
	GENERATED_BODY()
	
public:
	ABasketBall();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Called when projectile hits something
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Plays a specified sound at a specified location
	void ReproduceSound(USoundBase* Sound, FVector Location);

public:
	USphereComponent* GetCollisionComponent() const { return BallCollisionComponent; }
	UProjectileMovementComponent* GetBasketballMovement() const { return BasketballMovement; }
	void SetBasketballVelocity(FVector Velocity);

private:
	// Ball related variables:
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		USphereComponent* BallCollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* BasketballMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* BasketballMesh;

	// Sound related variables:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision", meta = (AllowPrivateAccess = "true"))
		FVector CollisionLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
		USoundBase* RimHitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
		USoundBase* BackboardHitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
		USoundBase* BasketSound;
};
