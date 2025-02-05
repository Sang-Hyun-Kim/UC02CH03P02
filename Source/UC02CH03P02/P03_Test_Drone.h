// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "P03_Test_Drone.generated.h"

class USphereComponent;
struct FInputActionValue;
UCLASS()
class UC02CH03P02_API AP03_Test_Drone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AP03_Test_Drone();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CapsuleComponent, meta = (AllowPrivateAccess = "true"))
	USphereComponent* RootCollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkelatalMesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float GroundSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float AirSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float YawPitchRotationSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovementSpeed)
	float RollRotationSpeed = 80.0f;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Move(const FInputActionValue& value);

	UFUNCTION()
	void Look(const FInputActionValue& value);

	UFUNCTION()
	void Roll(const FInputActionValue& value);

	UFUNCTION()
	void CheckIfOnGround(); // 바닥 감지 함수
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	bool IsFalling = false;
	FVector FallVelocity = FVector::ZeroVector;      // 낙하 속도
	float Gravity = -98.0f; // 중력 가속도 (기본값: Unreal Engine 중력값)
	float LastZValue;
	// 바닥 체크하기
	bool bIsOnGround = false;  // 땅에 있는지 여부
	float GroundCheckDistance = 200.0f; // 레이캐스트 길이


};
