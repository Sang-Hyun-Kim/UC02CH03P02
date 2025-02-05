// Fill out your copyright notice in the Description page of Project Settings.


#include "P03DroneController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
AP03DroneController::AP03DroneController()
	: InputMappingContext(nullptr),
	P03_IA_Look(nullptr),
	P03_IA_Roll(nullptr),
	P03_IA_Move(nullptr)
{



}

void AP03DroneController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem * Subsystem=
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
