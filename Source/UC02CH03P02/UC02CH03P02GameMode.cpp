// Copyright Epic Games, Inc. All Rights Reserved.

#include "UC02CH03P02GameMode.h"
#include "UC02CH03P02Character.h"
#include "UObject/ConstructorHelpers.h"

AUC02CH03P02GameMode::AUC02CH03P02GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
