
#include "CustomGameState.h"


ACustomGameState::ACustomGameState(){
	Score = 0;
}


int32 ACustomGameState::GetScore()
{
	return Score;
}


void ACustomGameState::SetScore(int32 NewScoreValue)
{
	Score = NewScoreValue;
}
