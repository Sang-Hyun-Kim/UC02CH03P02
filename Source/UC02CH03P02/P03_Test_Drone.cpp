// Fill out your copyright notice in the Description page of Project Settings.


#include "P03_Test_Drone.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "P03DroneController.h"
// Sets default values
AP03_Test_Drone::AP03_Test_Drone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollisioComponent"));
	SetRootComponent(RootCollisionComponent);
	RootCollisionComponent->SetRelativeScale3D(FVector(3.0f,3.0f,3.0f));

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootCollisionComponent);
	CameraArm->TargetArmLength = 600.0f;

	CameraArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));
	CameraArm->bUsePawnControlRotation = true;


	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm);
	Camera->bUsePawnControlRotation = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootCollisionComponent);


}

// Called when the game starts or when spawned
void AP03_Test_Drone::BeginPlay()
{
	Super::BeginPlay();
	
	LastZValue = GetActorLocation().Z;
}

void AP03_Test_Drone::Move(const FInputActionValue& value)
{
	const FVector3d MoveInput = value.Get<FVector3d>();
	
	// 땅과 하늘 중 맞는 속도의 값만 반영됨
	FVector NewVel = (MoveInput) * GroundSpeed * bIsOnGround + MoveInput * AirSpeed* !bIsOnGround;

	AddActorLocalOffset(NewVel * GetWorld()->GetDeltaSeconds(), true);
}

void AP03_Test_Drone::Look(const FInputActionValue& value)
{
	const FVector2d MoveInput = value.Get<FVector2d>();
	FRotator CurrentRotation = GetActorRotation();
	FQuat CurrentQuat = FQuat(CurrentRotation);
	// Yaw와 Pitch에 대해 회전량 계산
	float YawDelta =  MoveInput.X * YawPitchRotationSpeed * GetWorld()->GetDeltaSeconds();
	float PitchDelta = MoveInput.Y * YawPitchRotationSpeed * GetWorld()->GetDeltaSeconds();

	FQuat YawQuat = FQuat(FVector(0, 0, 1), FMath::DegreesToRadians(YawDelta));  // Yaw (Z축 회전)
	FQuat PitchQuat = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(PitchDelta));  // Pitch (X축 회전)
	// 현재 회전값에 새 회전값을 합침
	CurrentQuat *= YawQuat;  // Yaw 회전 적용
	CurrentQuat *= PitchQuat;  // Pitch 회전 적용

	// 결과적으로 최종 회전값을 Actor에 적용
	SetActorRelativeRotation(CurrentQuat);
}

void AP03_Test_Drone::Roll(const FInputActionValue& value)
{
	const float MoveInput = value.Get<float>();
	FRotator CurrentRotation = GetActorRotation();
	FQuat CurrentQuat = FQuat(CurrentRotation);
	// Yaw와 Pitch에 대해 회전량 계산
	float RollDelta = MoveInput * RollRotationSpeed * GetWorld()->GetDeltaSeconds();
	FQuat RollQuat = FQuat(FVector(1, 0, 0), FMath::DegreesToRadians(RollDelta));

	CurrentQuat *= RollQuat;
	SetActorRelativeRotation(CurrentQuat);
}

void AP03_Test_Drone::CheckIfOnGround()
{
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, GroundCheckDistance); // 아래 방향

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 자기 자신 무시

	// 레이캐스트 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	// 바닥 감지 여부 업데이트


	if (bHit)
	{
		// 충돌한 액터가 있을 경우
		AActor* HitActor = HitResult.GetActor();

		// 충돌한 액터의 태그 비교
		if (HitActor)
		{
			// 태그가 "Enemy"인 경우 비교
			if (HitActor->ActorHasTag(TEXT("Ground")))
			{
				bIsOnGround = bHit;
			}
		}
	}
	else
	{
		bIsOnGround = bHit;
	}
}

// Called every frame
void AP03_Test_Drone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckIfOnGround();
	float CurrentZ = GetActorLocation().Z;
	if (CurrentZ - LastZValue > 0)
	{
		FallVelocity.Z = 0;
	}
	else
	{
		// 중력 적용
		if (!bIsOnGround) // 땅에 없으면 중력 적용
		{
			FallVelocity.Z += Gravity;
		}
		else
		{
			FallVelocity.Z = 0;
		}
	}
	LastZValue = CurrentZ;
	// 이동 적용
	AddActorWorldOffset(FallVelocity * DeltaTime, true);
}

// Called to bind functionality to input
void AP03_Test_Drone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AP03DroneController* DroneController = Cast<AP03DroneController>(GetController()))
		{
			if (DroneController->P03_IA_Move)
			{
				EnhancedInput->BindAction(
					DroneController->P03_IA_Move,
					ETriggerEvent::Triggered,
					this,
					&AP03_Test_Drone::Move
				);
			}

			if (DroneController->P03_IA_Look)
			{
				EnhancedInput->BindAction(
					DroneController->P03_IA_Look,
					ETriggerEvent::Triggered,
					this,
					&AP03_Test_Drone::Look
				);
			}

			if (DroneController->P03_IA_Roll)
			{
				EnhancedInput->BindAction(
					DroneController->P03_IA_Roll,
					ETriggerEvent::Triggered,
					this,
					&AP03_Test_Drone::Roll
				);
			}
		}
	}
	
}

