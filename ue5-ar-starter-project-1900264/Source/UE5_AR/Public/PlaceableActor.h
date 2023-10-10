
#pragma once

#include "GameFramework/Actor.h"
#include "PlaceableActor.generated.h"

class UARPin;
class UBoxComponent;
UCLASS()
class UE5_AR_API APlaceableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APlaceableActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene Component")
		USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Basketball Checkpoint")
		UBoxComponent* ScoreCheckpoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Backboard Component")
		UBoxComponent* BackboardBoxComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Velocity")
		USceneComponent* SuggestedVelocityLocation;

	UARPin* PinComponent;
};
