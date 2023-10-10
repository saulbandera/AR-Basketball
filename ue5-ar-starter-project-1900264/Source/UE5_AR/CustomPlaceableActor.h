#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomPlaceableActor.generated.h"

UCLASS()
class UE5_AR_API ACustomPlaceableActor : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this empty's properties
	ACustomPlaceableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
