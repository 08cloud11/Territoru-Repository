//--------------------------------------------------------------------
// �t�@�C���� �FCharacterBase.cpp
// �T�v       �F�v���C���[�A�G���p������L�����N�^�[��ՃN���X
// �쐬��     �F0231�{��
// �X�V���e   �F03/05�@�쐬
//--------------------------------------------------------------------

#include "CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TerritoruCharacter.h"
#include "Enemy_Daring.h"
#include "StageManager.h"
#include "Kismet/GameplayStatics.h"

ACharacterBase::ACharacterBase()
	: _moveDir(MoveDirection::Move_Left)
	, _status(CharaState::State_Idle)
	, _atkstate(AttackState::None)
	, _moveLength(0.0)
	, _healingtimer(0.0)
	, _respawntimer(0.0)
	, _statusuptimer(0.0)
	, _hp(15)
	, _deadnum(0)
	, _killnum(0)
{
	PrimaryActorTick.bCanEverTick = true;

	//�}�`���Ƃ̃X�e�[�^�X��������
	const ShapeValues triangular{ 0.006, 0.0003, 0.0009, 4 };
	const ShapeValues quadrangular{ 0.008, 0.0005, 0.001, 3 };
	const ShapeValues pentagonal{ 0.01, 0.0008, 0.0015, 2 };

	_valuesArray.Add("Triangular", triangular);
	_valuesArray.Add("Quadrangular", quadrangular);
	_valuesArray.Add("Pentagonal", pentagonal);

	ConstInit();
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (Cast<ATerritoruCharacter>(this)){
		_controller = Cast<APlayerController>(Controller);
	}	

	BeginInit();	
}

void ACharacterBase::ConstInit()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	//�W�����v�͂◎�����x�Ȃǂ�������
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
}

void ACharacterBase::BeginInit()
{
	FVector pPos = GetActorLocation();
	double radian = atan2(pPos.Y, pPos.X);
	double degree = radian * 180.0 / PI;

	if (Cast<ATerritoruCharacter>(this)) {
		_controller->SetControlRotation(FRotator(0.0, degree + 180.0, 0.0));
	}

	_damagedComponent = GetComponentByClass<UDamagedComponent>();

	Cast<UPrimitiveComponent>(GetRootComponent())->
		OnComponentBeginOverlap.AddDynamic(this, &ACharacterBase::OnOverlapBegin);

	TSubclassOf<AStageManager> findClass;
	findClass = AStageManager::StaticClass();

	_globalradius = Cast<AStageManager>(UGameplayStatics::
		GetActorOfClass(GetWorld(), findClass))->Get_globalradius();;
}

void ACharacterBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<ACharacterBase>(OtherActor)) { return; }
	if (Cast<ACharacterBase>(OtherActor)->Get_binvincible()) { return; }
	if (_damageReaction) { return; }
	if (Cast<ACharacterBase>(OtherActor)->Get_Status() != CharaState::State_Attack) return;
	
	_hp -= Cast<ACharacterBase>(OtherActor)->Get_attackdamage();

	_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(1.0, 0.2, 0.2));
	_damageReaction = true;
	if (Cast<ATerritoruCharacter>(this)) {
		this->ShapeChange();
	}

	if (_hp > 0) return;
		
	//���g��"SMC"�^�O�̂����X�^�e�B�b�N���b�V���R���|�[�l���g���擾
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	TArray<UActorComponent*> smcs = this->GetComponentsByTag(findClass, "SMC");

	//�S�Č���Ȃ�����
	for (auto smc : smcs)
	{
		Cast<UStaticMeshComponent>(smc)->SetVisibility(false);
	}

	//�����蔻��𖳌��ɂ���
	Cast<UPrimitiveComponent>(GetRootComponent())->
		SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	_deadpos = GetActorLocation();
	_bdead = true;
	_deadnum++;

	Cast<ACharacterBase>(OtherActor)->Increment_killnum();
}

void ACharacterBase::Move()
{
	if (_bmove)
	{
		//���g�̃|�W�V�������擾���A�������ɋt���ځi0,0 �ɑ΂��錻�݂̊p�x�j�����߂�
		FVector pPos = GetActorLocation();
		double radian = atan2(pPos.Y, pPos.X); //�P�ʉ~
		double degree = radian * 180.0 / PI;	//�p�x

		//�ړ������ɑ΂����������
		if (_controller->IsInputKeyDown(FKey(EKeys::A))
			|| _controller->IsInputKeyDown(FKey(EKeys::Left))) {
			_moveLength += _currentvalues._acceleration;

			if (_moveLength > _currentvalues._max_moveLength)_moveLength = _currentvalues._max_moveLength;
		}
		else if (_controller->IsInputKeyDown(FKey(EKeys::D))
			|| _controller->IsInputKeyDown(FKey(EKeys::Right))) {
			_moveLength -= _currentvalues._acceleration;

			if (_moveLength < -_currentvalues._max_moveLength)_moveLength = -_currentvalues._max_moveLength;
		}

		FVector newPos = FVector(_globalradius * cos(_moveLength + radian),
			_globalradius * sin(_moveLength + radian), pPos.Z);

		SetActorLocation(newPos);
		SetActorRotation(FRotator(0, degree, _yawRotLength += _moveLength * 400.0));
		_controller->SetControlRotation(FRotator(0.0, degree + 180.0, 0.0));

		_bmove = false;
	}
	else
	{
		if (_moveLength != 0.0)
		{
			//���g�̃|�W�V�������擾���A�������ɋt���ځi0,0 �ɑ΂��錻�݂̊p�x�j�����߂�
			FVector pPos = GetActorLocation();
			double radian = atan2(pPos.Y, pPos.X); //�P�ʉ~
			double degree = radian * 180.0 / PI;	//�p�x

			//�ړ������ɑ΂��錸������
			if (_moveLength > 0.0) {
				_moveLength -= _currentvalues._attenuation;

				if (_moveLength < 0.0)_moveLength = 0.0;
			}
			else {
				_moveLength += _currentvalues._attenuation;

				if (_moveLength > 0.0)_moveLength = 0.0;
			}

			FVector newPos = FVector(_globalradius * cos(_moveLength + radian),
				_globalradius * sin(_moveLength + radian), pPos.Z);

			SetActorLocation(newPos);
			SetActorRotation(FRotator(0, degree, _yawRotLength += _moveLength * 400.0));
			_controller->SetControlRotation(FRotator(0.0, degree + 180.0, 0.0));

			_status = CharaState::State_Idle;
		}
	}
}

void ACharacterBase::AttackAction()
{
	FVector pPos = GetActorLocation();
	double radian = atan2(pPos.Y, pPos.X); //�P�ʉ~
	double degree = radian * 180.0 / PI;	//�p�x
	FVector newPos;

	switch (_atkstate)
	{
	case ACharacterBase::LeftDir:
		if (_attackmoveDistance < _atkgeneralValue * 10) {
			newPos = FVector(_globalradius * cos((_atkgeneralValue * (_currentvalues._max_moveLength * 60.0)) + radian),
				_globalradius * sin((_atkgeneralValue * (_currentvalues._max_moveLength * 200.0)) + radian), pPos.Z);

			SetActorLocation(newPos);
			SetActorRotation(FRotator(0, degree, _yawRotLength += _atkgeneralValue * 700.0));
		}	
		else
		{
			newPos = FVector(_globalradius * cos((-(_atkgeneralValue * (_currentvalues._max_moveLength * 60.0)) / 2.0) + radian),
				_globalradius * sin((-(_atkgeneralValue * (_currentvalues._max_moveLength * 200.0)) / 2.0) + radian), pPos.Z);

			SetActorLocation(newPos);
			SetActorRotation(FRotator(0, degree, _yawRotLength += (_atkgeneralValue / 2.0) * 700.0));
		}
		break;
	case ACharacterBase::RightDir:
		if (_attackmoveDistance < _atkgeneralValue * 10) {
			newPos = FVector(_globalradius * cos(-(_atkgeneralValue * (_currentvalues._max_moveLength * 60.0)) + radian),
				_globalradius * sin(-(_atkgeneralValue * (_currentvalues._max_moveLength * 200.0)) + radian), pPos.Z);

			SetActorLocation(newPos);
			SetActorRotation(FRotator(0, degree, _yawRotLength += -_atkgeneralValue * 700.0));
		}
		else
		{
			newPos = FVector(_globalradius * cos(((_atkgeneralValue * (_currentvalues._max_moveLength * 60.0)) / 2.0) + radian),
				_globalradius * sin(((_atkgeneralValue * (_currentvalues._max_moveLength * 200.0)) / 2.0) + radian), pPos.Z);

			SetActorLocation(newPos);
			SetActorRotation(FRotator(0, degree, _yawRotLength += (-_atkgeneralValue / 2.0) * 700.0));
		}

		break;
	case ACharacterBase::UpDir:
		newPos = FVector(pPos.X, pPos.Y, pPos.Z + (50.0 -(_attackmoveDistance * 20.0)));

		SetActorLocation(newPos);
		SetActorRotation(FRotator(0, degree, _yawRotLength += -_atkgeneralValue * 700.0));
		break;
	case ACharacterBase::DownDir:
		if (_attackmoveDistance < _atkgeneralValue * 6) {
			newPos = FVector(pPos.X, pPos.Y, pPos.Z +(50.0 - (_attackmoveDistance * 150.0)));

			SetActorLocation(newPos);
		}
		else {
			newPos = FVector(pPos.X, pPos.Y, pPos.Z + (-35.0 + (-_attackmoveDistance * 5.0)));

			SetActorLocation(newPos);
		}

		SetActorRotation(FRotator(0, degree, _yawRotLength += -_atkgeneralValue * 700.0));
		break;
	}

	if (Cast<ATerritoruCharacter>(this)) {
		_controller->SetControlRotation(FRotator(0.0, degree + 180.0, 0.0));
	}

	_attackmoveDistance += _atkgeneralValue;

	if (_attackmoveDistance > _atkgeneralValue * 17) {
		_status = CharaState::State_Idle;
		_atkstate = None;
		_attackmoveDistance = 0.0;

		if (Cast<AEnemy_Daring>(this)) {
			_atkcooltimer = 0.0;
			_bafteratk = true;
		}		
	}
}

void ACharacterBase::ShapeChange()
{
	//���g��"SMC"�^�O�̂����X�^�e�B�b�N���b�V���R���|�[�l���g���擾
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	TArray<UActorComponent*> smcs = this->GetComponentsByTag(findClass, "SMC");

	//�S�Č���Ȃ�����
	for (auto smc : smcs)
	{
		Cast<UStaticMeshComponent>(smc)->SetVisibility(false);
	}

	//���݂̐}�`�ɊY�����郂�m��SMC���擾�A�X�e�[�^�X��ύX
	switch (_meshnum)
	{
	case ShapeNumber::Triangular:
		smcs = this->GetComponentsByTag(findClass, "Triangular");
		_currentvalues = _valuesArray["Triangular"];

		break;
	case ShapeNumber::Quadrangular:
		smcs = this->GetComponentsByTag(findClass, "Quadrangular");
		_currentvalues = _valuesArray["Quadrangular"];

		break;
	case ShapeNumber::Pentagonal:
		smcs = this->GetComponentsByTag(findClass, "Pentagonal");
		_currentvalues = _valuesArray["Pentagonal"];

		break;
	}

	if (_bstatusup)
	{
		_currentvalues._acceleration *= 1.5;
		_currentvalues._attenuation *= 1.5;
		_currentvalues._max_moveLength *= 1.5;
		_currentvalues._attackdamage += 2;
	}

	//���݂̐}�`�ɊY�����郂�m��\������
	for (auto smc : smcs)
	{
		Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
		_mymat = Cast<UStaticMeshComponent>(smc)->CreateDynamicMaterialInstance(0);

		//���̃L�������v���C���[�ł��_���[�W��Ȃ�
		if (Cast<ATerritoruCharacter>(this)) {
			this->MatChange(Cast<UStaticMeshComponent>(smc));
		}
	}
}

void ACharacterBase::InputAttack()
{	
	_status = CharaState::State_Attack;

	switch (_moveDir)
	{
	case ACharacterBase::Move_Left:
		_atkstate = LeftDir;
		break;
	case ACharacterBase::Move_Right:
		_atkstate = RightDir;
		break;
	case ACharacterBase::Move_Up:
		_atkstate = UpDir;
		break;
	case ACharacterBase::Move_Down:
		_atkstate = DownDir;
		break;
	}
}

void ACharacterBase::HealHP(float deltaTime)
{
	if (_hp < _max_hp)
	{
		_healingtimer += deltaTime;

		if (_healingtimer > 6.0)
		{
			_hp++;

			if (_hp >= _max_hp) { _hp = _max_hp; }

			_healingtimer = 0.0;
		}
	}
}

void ACharacterBase::Respawn(float deltaTime)
{
	_respawntimer += deltaTime;

	//���S���̃|�W�V�����ɌŒ�
	SetActorLocation(_deadpos);

	if (_respawntimer > 8.0)
	{
		//�R���W�����v���Z�b�g���Z�b�g
		Cast<UPrimitiveComponent>(GetRootComponent())->
			SetCollisionProfileName(TEXT("CharactorBase"));

		ShapeChange();

		_hp = _max_hp;
		_respawntimer = 0.0;
		_bdead = false;
		_binvincible = true;

		_damagedComponent->BlinkingTrigger();
	}
}

void ACharacterBase::Invincible(float deltaTime)
{
	_invincibletimer += deltaTime;

	if (_invincibletimer > 3.0)
	{
		_binvincible = false;
	}
}

void ACharacterBase::DownLineTrace(float deltaTime)
{
	FVector pPos = GetActorLocation();
	FVector endPos = FVector(pPos.X, pPos.Y, pPos.Z - 200.0);
	double radian = atan2(pPos.Y, pPos.X); //�P�ʉ~

	FName presetName = TEXT("");

	TArray<AActor*> actors_to_ignore;
	actors_to_ignore.Add(this);

	TArray<FHitResult> outHits;

	bool istargethit = false;

	UKismetSystemLibrary::LineTraceMulti(
		GetWorld(),
		pPos,
		endPos,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		actors_to_ignore,
		EDrawDebugTrace::ForOneFrame,
		outHits,
		true,
		FLinearColor::White,
		FLinearColor(0.0f, 0.0f, 0.0f, 0.0f),
		deltaTime
	);

	for (FHitResult outhit : outHits) {
		if (Cast<ABlock>(outhit.GetActor())) {

			if (this->ActorHasTag("Player")) {
				if (Cast<ABlock>(outhit.GetActor())->Get_breakChara() != BreakChara::Player) continue;
			}
			else if (this->ActorHasTag("enemy1")) {
				if (Cast<ABlock>(outhit.GetActor())->Get_breakChara() != BreakChara::Enemy_Daring) continue;
			}
			else if (this->ActorHasTag("enemy2")) {
				if (Cast<ABlock>(outhit.GetActor())->Get_breakChara() != BreakChara::dummy1) continue;
			}
			else if (this->ActorHasTag("enemy3")) {
				if (Cast<ABlock>(outhit.GetActor())->Get_breakChara() != BreakChara::dummy2) continue;
			}

			_bstatusup = true;
			_statusuptimer = 0.0;

			_currentvalues._acceleration *= 1.5;
			_currentvalues._attenuation *= 1.5;
			_currentvalues._max_moveLength *= 1.5;
			_currentvalues._attackdamage += 2;
		}
	}
}

void ACharacterBase::StunAction()
{
	//���g�̃|�W�V�������擾���A�������ɋt���ځi0,0 �ɑ΂��錻�݂̊p�x�j�����߂�
	FVector pPos = GetActorLocation();
	double radian = atan2(pPos.Y, pPos.X); //�P�ʉ~
	double degree = radian * 180.0 / PI;	//�p�x

	double movelength = _currentvalues._acceleration * 5.0;

	if (_stunviberight) movelength *= -1;

	FVector newPos = FVector(_globalradius * cos(movelength + radian),
		_globalradius * sin(movelength + radian), pPos.Z);

	SetActorLocation(newPos);
	_stunviberight = !_stunviberight;
}