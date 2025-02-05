// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "P03DroneController.generated.h"

/**
 *  드론 움직임 제어 컨트롤러
 */

class UInputMappingContext;
class UInputAction;

UCLASS()
class UC02CH03P02_API AP03DroneController : public APlayerController
{
	GENERATED_BODY()
	

public:
	AP03DroneController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* P03_IA_Look;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* P03_IA_Roll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* P03_IA_Move;

protected:
	virtual void BeginPlay() override;

};
