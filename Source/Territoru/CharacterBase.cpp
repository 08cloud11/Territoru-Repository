//--------------------------------------------------------------------
// ファイル名 ：CharacterBase.cpp
// 概要       ：プレイヤー、敵が継承するキャラクター基盤クラス
// 作成者     ：0231本間
// 更新内容   ：03/05　作成
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

	//図形ごとのステータスを初期化
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

	//ジャンプ力や落下速度などを初期化
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
		
	//自身の"SMC"タグのついたスタティックメッシュコンポーネントを取得
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	TArray<UActorComponent*> smcs = this->GetComponentsByTag(findClass, "SMC");

	//全て見れなくする
	for (auto smc : smcs)
	{
		Cast<UStaticMeshComponent>(smc)->SetVisibility(false);
	}

	//当たり判定を無効にする
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
		//自身のポジションを取得し、それを基に逆正接（0,0 に対する現在の角度）を求める
		FVector pPos = GetActorLocation();
		double radian = atan2(pPos.Y, pPos.X); //単位円
		double degree = radian * 180.0 / PI;	//角度

		//移動方向に対する加速処理
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
			//自身のポジションを取得し、それを基に逆正接（0,0 に対する現在の角度）を求める
			FVector pPos = GetActorLocation();
			double radian = atan2(pPos.Y, pPos.X); //単位円
			double degree = radian * 180.0 / PI;	//角度

			//移動方向に対する減速処理
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
	double radian = atan2(pPos.Y, pPos.X); //単位円
	double degree = radian * 180.0 / PI;	//角度
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
	//自身の"SMC"タグのついたスタティックメッシュコンポーネントを取得
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	TArray<UActorComponent*> smcs = this->GetComponentsByTag(findClass, "SMC");

	//全て見れなくする
	for (auto smc : smcs)
	{
		Cast<UStaticMeshComponent>(smc)->SetVisibility(false);
	}

	//現在の図形に該当するモノのSMCを取得、ステータスを変更
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

	//現在の図形に該当するモノを表示する
	for (auto smc : smcs)
	{
		Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
		_mymat = Cast<UStaticMeshComponent>(smc)->CreateDynamicMaterialInstance(0);

		//このキャラがプレイヤーでかつダメージ後なら
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

	//死亡時のポジションに固定
	SetActorLocation(_deadpos);

	if (_respawntimer > 8.0)
	{
		//コリジョンプリセットをセット
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
	double radian = atan2(pPos.Y, pPos.X); //単位円

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
	//自身のポジションを取得し、それを基に逆正接（0,0 に対する現在の角度）を求める
	FVector pPos = GetActorLocation();
	double radian = atan2(pPos.Y, pPos.X); //単位円
	double degree = radian * 180.0 / PI;	//角度

	double movelength = _currentvalues._acceleration * 5.0;

	if (_stunviberight) movelength *= -1;

	FVector newPos = FVector(_globalradius * cos(movelength + radian),
		_globalradius * sin(movelength + radian), pPos.Z);

	SetActorLocation(newPos);
	_stunviberight = !_stunviberight;
}