
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CustomGameState.generated.h"

UCLASS()
class UE5_AR_API ACustomGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACustomGameState();
	~ACustomGameState() = default;

	UFUNCTION(BlueprintCallable, Category = "Score")
		int32 GetScore();
	UFUNCTION(BlueprintCallable, Category = "Score")
		void SetScore(int32 NewScoreValue);

private:
	int32 Score;

};
