//--------------------------------------------------------------------
// ファイル名 ：TerritoruCharacter.cpp
// 概要       ：プレイヤーキャラクター
// 作成者     ：0231本間
// 更新内容   ：03/05　作成
//--------------------------------------------------------------------

#include "TerritoruCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Trap.h"

ATerritoruCharacter::ATerritoruCharacter()	
{
	_meshnum = ShapeNumber::Pentagonal;

	_damagedComponent = CreateDefaultSubobject<UDamagedComponent>(TEXT("DamagedComponent"));
}

void ATerritoruCharacter::BeginPlay()
{
	Super::BeginPlay();	

	ShapeChange();

	/*_followCamera->Set_globalradius(_globalradius);*/
	_controller->bShowMouseCursor = true;

	if (IsValid(_controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(_controller->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(_defaultMappingContext, 0);
		}
	}
}

void ATerritoruCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (!_bstart) return;

	if (GetActorLocation().Z < -250.0) {
		FVector newpos = FVector(GetActorLocation().X, GetActorLocation().Y, 500.0);
		SetActorLocation(newpos);
	}

	if (!_bdead) {

		if (!_bstatusup) {
			DownLineTrace(deltaTime);
		}
		else {
			_statusuptimer += deltaTime;

			if (_statusuptimer > 3.0)
			{
				_bstatusup = false;

				switch (_meshnum)
				{
				case ShapeNumber::Triangular:
					_currentvalues = _valuesArray["Triangular"];

					break;
				case ShapeNumber::Quadrangular:
					_currentvalues = _valuesArray["Quadrangular"];

					break;
				case ShapeNumber::Pentagonal:
					_currentvalues = _valuesArray["Pentagonal"];

					break;
				}
			}
		}

		Move();

		if (_atkstate != AttackState::None) { AttackAction(); }

		HealHP(deltaTime);

		if (_binvincible) { Invincible(deltaTime); }

		if (_damageReaction) { MatColorReset(deltaTime); }
	}
	else
	{
		Respawn(deltaTime);
	}	
}

void ATerritoruCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// ジャンプ
		EnhancedInputComponent->BindAction(_InputActions["Jump"], ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(_InputActions["Jump"], ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// 移動
		EnhancedInputComponent->BindAction(_InputActions["Move"], ETriggerEvent::Triggered, this, &ATerritoruCharacter::InputMove);
		// 図形変更
		EnhancedInputComponent->BindAction(_InputActions["ShapeChange"], ETriggerEvent::Started, this, &ATerritoruCharacter::InputShapeChange);
		// 攻撃
		EnhancedInputComponent->BindAction(_InputActions["Attack"], ETriggerEvent::Started, this, &ATerritoruCharacter::InputAttack);
		// 罠配置
		EnhancedInputComponent->BindAction(_InputActions["Trap"], ETriggerEvent::Started, this, &ATerritoruCharacter::InputTrapArrange);
	}
}

void ATerritoruCharacter::InputMove()
{
	if (_status != CharaState::State_Attack)
	{
		_status = CharaState::State_Move;

		if (_controller->IsInputKeyDown(FKey(EKeys::Left)) ||
			_controller->IsInputKeyDown(FKey(EKeys::A))) {
			_moveDir = Move_Left;
			_bmove = true;
		}
		else if (_controller->IsInputKeyDown(FKey(EKeys::Right)) ||
			_controller->IsInputKeyDown(FKey(EKeys::D))) {
			_moveDir = Move_Right;
			_bmove = true;
		}
		else if (_controller->IsInputKeyDown(FKey(EKeys::Up)) ||
			_controller->IsInputKeyDown(FKey(EKeys::W))) {
			_moveDir = Move_Up;
		}
		else if (_controller->IsInputKeyDown(FKey(EKeys::Down)) ||
			_controller->IsInputKeyDown(FKey(EKeys::S))) {
			_moveDir = Move_Down;
		}
	}	
}

void ATerritoruCharacter::InputShapeChange()
{	
	if (_status != CharaState::State_Attack && !_bdead)
	{
		if (_controller->IsInputKeyDown(FKey(EKeys::Three))) {
			_meshnum = ShapeNumber::Triangular;
		}
		else if (_controller->IsInputKeyDown(FKey(EKeys::Four))) {
			_meshnum = ShapeNumber::Quadrangular;
		}
		else if (_controller->IsInputKeyDown(FKey(EKeys::Five))) {
			_meshnum = ShapeNumber::Pentagonal;
		}

		ShapeChange();
	}
}

void ATerritoruCharacter::InputTrapArrange()
{
	FTransform spawnTr;
	spawnTr.SetLocation(GetActorLocation());

	Cast<ATrap>(GetWorld()->SpawnActor<AActor>(_trap, spawnTr))->
		Set_arrangechara(ArrangeChara::Player);
}

void ATerritoruCharacter::MatColorReset(float DeltaTime)
{
	_reactiontimer += DeltaTime;

	if (_reactiontimer > 0.2)
	{	_reactiontimer = 0.0;
		_damageReaction = false;
		ShapeChange();
	}
}

void ATerritoruCharacter::MatChange(UStaticMeshComponent* smc)
{
	if (_damageReaction) {
		smc->SetMaterial(0, _playerMats["Damaged"]);
	}
	else {
		smc->SetMaterial(0, _playerMats["Normal"]);
	}	
}