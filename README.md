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

#### GameMode

- 프로젝트의 Custom 한 Pawn 클래스와 Controller를 넣기 위해 GameMode를 상속받는 C++ 클래스를 만든 뒤 이를 BP 클래스로 재 상속 받아 사용하였습니다 사용 방법은 다음과 같습니다.
![Image](https://github.com/user-attachments/assets/9baceccf-f6ec-49e0-bbf9-cbf0ac01f075)
![Image](https://github.com/user-attachments/assets/705d1195-7de1-48f3-bca8-06e1dd2b190a)

---

#### DroneController

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

#### 드론을 위한 IMC 클래스와 IA 클래스 연동 방법
- 6방향 이동과 3개 축의 회전을 위해 3개의 IA 클래스를 준비했습니다.
- P03_IA_Move : value_type(Axis3D/Vector)
- P03_IA_Look : value_type(Axis2D/Vector)
- P03_IA_Roll :value_type(Vector3D)
### 시연 영상


---

### 회고 및 어려웠던 점


