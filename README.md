# UC02CH03P02


## 개요
- 언리얼 캠프 챕터 3의 과제용 프로젝트 소개

### 주요 구현 기능
- Pawn 클래스로 컨트롤 구현(필수)
  - CharacterMovement로 부터 제공받는 기본 이동 관련 함수를 사용하지 않고 Pawn 클래스를 상속받는 클래스로 직접 컨트롤 구현하기
  - 충돌 컴포넌트를 Root로 Camera, SpringArm 을 선택해 3인칭 시점을 구현한다. 다만 Drone Mesh를 넣고 싶어서 Skeletal 대신 Static 매쉬를 사용하였다.
- Enhanced Input 사용하기(필수)
  - 언리얼에서 제공하는 Enhanced Input을 통한 Input Mapping Context 와 Input Action 클래스를 활용한 Custom Controller 제작
  - 각 입력과 이동 기능 함수를 IMC와 IA를 통해 Controller에 바인딩한다.
  - Conroller와 Pawn 클래스 바인딩을 위해 GameMode를 상속받는 클래스를 만들어 이를 프로젝트 모드 및 월드 세팅에 설정한다. 
- 직접 이동 구현하기(필수)
  - WASD와 마우스 입력으로 Pawn이 직접 움직이도록 구현, 이때 메시와 루트 충돌 컴포넌트에 대해 물리효과를 받는 Physics 시뮬레이트 기능을 false로 설정한다.
  - WASD로 X,Y축 이동, 마우스로 Pawn의 회전을 사용 함
- 6 자유도 (DOF) 드론/ 비행체 구현(도전)
  - 축분할 이동 (X:축 전/후 -> W/S. Y:축 좌/우 -> A/D Z:축 상/하 -> Space/Left Shift)
  - 회전: Yaw/Pith : 마우스 X/Y, Roll: Q/E  
  - 이 때 각 이동은 현재 Pawn의 회전 상태에 따라 입력을 전환해서 입력해야함
- 중력 및 낙하 구현
  - Physics 대신 Tick 함수 로직을 통해 인공 중력 구현
  - 바닥 충돌 판정 구현
  - 착지 시 낙하 중단 또는 Z 속도를 0으로 초기화
  - 지상과 공중의 이동속도가 다름을 표현하는 에어 컨트롤 구현

---

### 주요 구현

#### GameMode 구현

- 프로젝트의 Custom 한 Pawn 클래스와 Controller를 넣기 위해 GameMode를 상속받는 C++ 클래스를 만든 뒤 이를 BP 클래스로 재 상속 받아 사용하였습니다 사용 방법은 다음과 같습니다.
![Image](https://github.com/user-attachments/assets/9baceccf-f6ec-49e0-bbf9-cbf0ac01f075)
![Image](https://github.com/user-attachments/assets/705d1195-7de1-48f3-bca8-06e1dd2b190a)

---

#### DroneController 세부 구현

- Drone Pawn의 움직임을 제공하며 동시에 바인딩 하기위한 PlayerConroller를 상속받는 클래스를 C++ 클래스로 구현 후 BP 클래스로 다시 랩핑하였습니다.
- 해당 클래스는 InputMappingContext 클래스와 Input Action 클래스를 UPROPERTY() 멤버 변수로 가지며 생성자에서 초기화합니다.
- BeginPlay() 시 현재 플레이어의 입력들 받는 방식으로 IMC 멤버 변수를 연결시켜줍니다.
```C++
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
```
---

##### 드론 움직임 입력을 위한 IMC 클래스와 IA 클래스 연동 방법
- 6방향 이동과 3개 축의 회전을 위해 3개의 IA 클래스를 준비했습니다.
	- P03_IA_Move : value_type(Axis3D/Vector)
	- P03_IA_Look : value_type(Axis2D/Vector)
	- P03_IA_Roll :value_type(Axis1D/float)
- IMC 맵핑
	- P03_IA_Move : 전/후/좌/우 와 상/하 이동을 구분할 수 있었지만 3d 벡터로 합친 이유는, 축변화 Modifier인 스위즐 입력 축 값을 사용해보고 싶었습니다. 상, 하 입력에 대한 Modifier는 아래 사진처럼 적용해 사용했습니다.
   - P03_IA_Look : 마우스 X/Y축 변화량에 대해 비행체가 Yaw/Pitch 축으로 회전하길 원했습니다.
  	- P03_IA_Roll : Q/E 키보드 입력으로 Roll 축 회전하도록 맵핑했습니다.
![Image](https://github.com/user-attachments/assets/74a2d207-dc42-4e71-ad26-5a2e5c02d600)

  ![Image](https://github.com/user-attachments/assets/5adbd866-c1a4-4737-aac4-1bf9ec288622)

#### Drone Pawn 클래스 세부 구현
- Drone 클래스의 주요 기능은 다음과 같습니다
	- 주요 컴포넌트(충돌 컴포넌트, 카메라,카메라 암, 스테틱 메쉬) 초기화 및 계층 설정
 	- 이동, 회전 함수와 PlayerInputComponent와 바인딩
  	- 바인딩 된 각 주요 함수의 기능
  	- Tick 함수 내부의 중력 및 낙하 처리
  	- 체공 상태, 착지 상태를 판별하는 CheckIfOnGround 함수 

##### 주요 컴포넌트 설정
- 주요 컴포넌트와 세부적인 드론의 설정 값(이동 속도, 회전 속도)은 전부 UPROPERTY() 속성을 통해 추후 상속받을 BP 클래스에서 세부설정이 UE Editor에서 가능하도록 했습니다.
- 이들은 전부 생성자에서 초기화 되며 계층구조를 형성합니다. 
```C++
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
```
```C++
AP03_Test_Drone::AP03_Test_Drone()
{
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
```

##### 주요 이동, 회전 함수 바인딩
- GameMode를 통해 전달해준 DroneController 클래스를 통해 전달받는 입력 값들이 알맞은 조건에 따라 각 기능이 수행될 수 있도록 주요 기능 함수들을 바인딩 해주는 과정을 SetupPlayerInputComponent 함수에서 수행합니다.
```C++
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
```

##### 각 주요 기능 함수 소개
###### 이동 함수
- Move 함수는 넘겨 받은 FVector3D 값을 DeltaSeconds 값에 맞춰 AddActorLocalOffset 함수를 통해 넘겨 줍니다.
- 이때 공중과 지상 속도를 다르게 하기 위해 멤버 변수 bool bIsOnGround에 상태에 따라 계산 값이 결정되도록 구현했습니다
- 충돌 감지를 위해 bool 인자를 추가로 전달합니다.
  ```C++
	const FVector3d MoveInput = value.Get<FVector3d>();
	
	// 땅과 하늘 중 맞는 속도의 값만 반영됨
	FVector NewVel = (MoveInput) * GroundSpeed * bIsOnGround + MoveInput * AirSpeed* !bIsOnGround;

	AddActorLocalOffset(NewVel * GetWorld()->GetDeltaSeconds(), true);
  ```
###### Look(Yaw/Pitch 회전) 함수
- Look 함수는 넘겨 받은 FVector2d 값을 DeltaSeconds 값에 맞춰 SetActorRelativeRotation 함수를 통해 넘겨 줍니다.
- 이때 오일러 각에 따른 회전 순서로 인한 짐볼 락(Gimbal Lock) 현상을 방지하기 위해 언리얼에서 제공하는 쿼터니언 값으로 Rotator 값을 수정했습니다. 
- 회전해야할 쿼터니안 값을 라디안을 통해 계산한 다음 이를 현재 회전 값에 결합(*)하여 이를 최종적으로 Actor 회전 값으로 설정합니다.
  ```C++
	const FVector3d MoveInput = value.Get<FVector3d>();
	
	// 땅과 하늘 중 맞는 속도의 값만 반영됨
	FVector NewVel = (MoveInput) * GroundSpeed * bIsOnGround + MoveInput * AirSpeed* !bIsOnGround;

	AddActorLocalOffset(NewVel * GetWorld()->GetDeltaSeconds(), true);
  ```
###### Roll(X 축 회전) 함수
- Roll 함수는 넘겨 받은 float 값을 DeltaSeconds 값에 맞춰 SetActorRelativeRotation 함수를 통해 넘겨 줍니다.
- 위 Look 함수와 동일한 방식으로 쿼터니안 값을 현재 회전 값에 결합해 액터의 Rotator를 설정해줍니다.
  ```C++
	const FVector3d MoveInput = value.Get<FVector3d>();
	
	// 땅과 하늘 중 맞는 속도의 값만 반영됨
	FVector NewVel = (MoveInput) * GroundSpeed * bIsOnGround + MoveInput * AirSpeed* !bIsOnGround;

	AddActorLocalOffset(NewVel * GetWorld()->GetDeltaSeconds(), true);
  ```
#### 착지 판단과 중력 적용 기능
##### 착지 판단
- 착지판단을 위해 일정 크기(Drone Pawn의 하부 크기를 가늠한 크기) 만큼 World의 Z축 음의 방향으로 RayCasting을 수행했습니다.
- 이때 RayCasting을 통해 특정 액터와 충돌 감지했을 경우 해당 액터의 **태그가 "Ground" 인 경우 땅이라고 판단**하도록 구현했습니다.
- 이렇게 설정된 멤버 변수 bIsOnGround bool 변수는 추후 이동 속도나 중력 가속도 적용 여부를 결정합니다.
```C++

// header
	// 바닥 체크하기
	bool bIsOnGround = false;  // 땅에 있는지 여부
	float GroundCheckDistance = 200.0f; // 레이캐스트 길이
// cpp
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
```
##### Tick 중력 적용
- Tick 함수 내부에서 CheckIfOnGround() 함수를 수행 후 이전 Z값과 현재 Z값의 변화량이 음의 방향인 경우(중력과 반대되는 벡터가 아닌 경우를 상승중이라고 임의로 설정한 기준에서 벗어남을 뜻함) 중력 가속도가 적용됩니다.

```
// header
	FVector FallVelocity = FVector::ZeroVector;      // 낙하 속도
	float Gravity = -98.0f; // 중력 가속도 (기본값: Unreal Engine 중력값)
	float LastZValue;
// cpp
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
```
### 시연 영상


---

### 회고 및 어려웠던 점


