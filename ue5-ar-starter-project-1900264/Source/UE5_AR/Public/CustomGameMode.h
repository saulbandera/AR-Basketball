
#pragma once

#include "PlaceableActor.h"
#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

class APlaceableActor;
class AHelloARManager;

UCLASS()
class UE5_AR_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	ACustomGameMode();
	virtual ~ACustomGameMode() = default;

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	/**
	 * @brief Perform a line trace at the screen position provided and place the basketball hoop
	 * @param ScreenPos Pass the Screen Position as an FVector, Spawn Hoop is a boolean that determines whether to spawn hoop
	 */
	virtual void LineTraceSpawnActor(FVector ScreenPos, bool SpawnHoop);
	/**
	 * @brief Spawns the AR manager
	 */
	virtual void SpawnARManager();


	AHelloARManager* GetARManager();
	APlaceableActor* GetPlaceableActor();
	FVector GetSuggestedVelocityEndPos() { return SpawnedActor->SuggestedVelocityLocation->GetComponentLocation(); };


	UFUNCTION(BlueprintCallable, Category = "Score")
		int32 GetScore();
	UFUNCTION(BlueprintCallable, Category = "Score")
		void SetScore(const int32 NewScore);
	UFUNCTION(BlueprintCallable, Category = "Placeable Actors")
		bool CanHoopBePlaced();
	UFUNCTION(BlueprintCallable, Category = "Placeable Actors")
		bool IsHoopPlaced();

	// Basketball hoop 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placeable Actors")
		TSubclassOf<APlaceableActor> BasketballHoop;
	// Basketball hoop required space delimiter material 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placeable Actor Materials")
		UMaterialInstanceDynamic* DelimiterMaterial;

	UMaterialInterface* Material_;

private:
	FTimerHandle Ticker;
	APlaceableActor* SpawnedActor;
	AHelloARManager* ARManager;

	bool bCanHoopBePlaced;
	bool bIsHoopPlaced;
};
