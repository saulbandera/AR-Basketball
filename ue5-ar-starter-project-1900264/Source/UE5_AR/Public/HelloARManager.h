
#pragma once

#include "GameFramework/Actor.h"
#include "HelloARManager.generated.h"

class UARSessionConfig;
class AARPlaneActor;
class UARPlaneGeometry;

UCLASS()
class UE5_AR_API AHelloARManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AHelloARManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	bool GetPlanesUpdateStatus();
	void SetPlanesUpdateStatus(bool ShouldUpdate);

	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

protected:
	// Updates the plane actors on every frame as long as the AR Session is running
	void UpdatePlaneActors();
	AARPlaneActor* SpawnPlaneActor();
	void ResetARCoreSession();

	// Configuration file for AR Session
	UARSessionConfig* Config;
	// Base plane actor for geometry detection
	AARPlaneActor* PlaneActor;
	// Map of geometry planes
	TMap<UARPlaneGeometry*, AARPlaneActor*> PlaneActors;

private:
	bool bPlanesToBeUpdated;

};
