// Copyright Epic Games, Inc. All Rights Reserved.

#include "PangeaGameMode.h"
#include "PangeaCharacter.h"
#include "UObject/ConstructorHelpers.h"

APangeaGameMode::APangeaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
