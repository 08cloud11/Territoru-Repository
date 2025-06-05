// Copyright Epic Games, Inc. All Rights Reserved.

#include "TerritoruGameMode.h"
#include "TerritoruCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATerritoruGameMode::ATerritoruGameMode()
{
	//プレイヤーキャラを指定
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MainFolder/BP/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
