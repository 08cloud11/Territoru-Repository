//--------------------------------------------------------------------
// ファイル名 ：Enemy_Daring.cpp
// 概要       ：勇猛果敢な敵
// 作成者     ：0231本間
// 更新内容   ：03/13　作成
//--------------------------------------------------------------------

#include "Enemy_Daring.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Block.h"

AEnemy_Daring::AEnemy_Daring()
	: _ptarget(nullptr)
	, _targetingtimer(0.0)
{
	int randinitmoveDir = rand() % 2;

	_moveDir = MoveDirection(randinitmoveDir);

	_meshnum = ShapeNumber::Pentagonal;

	_damagedComponent = CreateDefaultSubobject<UDamagedComponent>(TEXT("DamagedComponent"));
}

void AEnemy_Daring::BeginPlay()
{
	Super::BeginPlay();

	ShapeChange();
}

void AEnemy_Daring::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	

	if (!_bstart) return;

	if (!_bdead) {

		if (_binvincible) { Invincible(DeltaTime); }

		if (_damageReaction) { MatColorReset(DeltaTime); }

		if (_bstun) {
			_stuntimer += DeltaTime;
			StunAction();

			if (_stuntimer > 3.0) {
				_bstun = false;
				_stuntimer = 0.0;
			}

			return;
		}

		if (!_bstatusup) {
			DownLineTrace(DeltaTime);
		}
		else {
			_statusuptimer += DeltaTime;

			if (_statusuptimer < 3.0) 
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

		ActionDecision(DeltaTime);

		HealHP(DeltaTime);
	}
	else
	{
		Respawn(DeltaTime);
	}	
}

void AEnemy_Daring::ActionDecision(float deltaTime)
{	
	LineTrace(deltaTime);

	if (_ptarget != nullptr) {
		if (!_ptarget->Get_bdead()) {
			ChaseTargetDir(_ptarget);
		}
		else{
			_ptarget = nullptr;
		}
		
		_targetingtimer += deltaTime;

		if (_targetingtimer >= 2.5) {
			_ptarget = nullptr;
		}
	}

	if (_status != CharaState::State_Attack) {
		_bmove = true;
	}

	Move();

	if (_bafteratk) {
		_atkcooltimer += deltaTime;

		if (_atkcooltimer > 0.35) {
			_bafteratk = false;
		}
	}

	if(_status == CharaState::State_Attack && _atkstate == AttackState::DownDir
		|| _status == CharaState::State_Attack && !_bafteratk){
		AttackAction();
	}
	else if (_status == CharaState::State_Jump)
	{
		_atkstate = AttackState::UpDir;
		AttackAction();
	}
}

void AEnemy_Daring::Move()
{
	if (_bmove)
	{
		//自身のポジションを取得し、それを基に逆正接（0,0 に対する現在の角度）を求める
		FVector pPos = GetActorLocation();
		double radian = atan2(pPos.Y, pPos.X); //単位円
		double degree = radian * 180.0 / PI;	//角度

		//移動方向に対する加速処理
		if (_moveDir == MoveDirection::Move_Left) {
			_moveLength += _currentvalues._acceleration;

			if (_moveLength > _currentvalues._max_moveLength)_moveLength = _currentvalues._max_moveLength;
		}
		else if (_moveDir == MoveDirection::Move_Right) {
			_moveLength -= _currentvalues._acceleration;

			if (_moveLength < -_currentvalues._max_moveLength)_moveLength = -_currentvalues._max_moveLength;
		}

		FVector newPos = FVector(_globalradius * cos(_moveLength + radian),
			_globalradius * sin(_moveLength + radian), pPos.Z);

		SetActorLocation(newPos);
		SetActorRotation(FRotator(0, degree, _yawRotLength += _moveLength * 400.0));

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

			_status = CharaState::State_Idle;
		}
	}
}

void AEnemy_Daring::InputMove()
{

}

void AEnemy_Daring::InputShapeChange()
{

}

void AEnemy_Daring::LineTrace(float deltaTime)
{
	FVector pPos = GetActorLocation();
	FVector endPos;
	double radian = atan2(pPos.Y, pPos.X); //単位円


	FName presetName = TEXT("");

	TArray<AActor*> actors_to_ignore;
	actors_to_ignore.Add(this);

	TArray<FHitResult> outHits;

	bool istargethit = false;

	//左右方向にレイを飛ばす
	for (int i = 1; i > -2; i -= 2)
	{
		endPos = FVector(_globalradius * cos((0.35 * i) + radian),
			_globalradius * sin((0.35 * i) + radian), pPos.Z);

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
			if (Cast<ACharacterBase>(outhit.GetActor()))
			{
				_ptarget = Cast<ACharacterBase>(outhit.GetActor());
				_targetingtimer = 0.0;

				istargethit = true;
			}
			else if (Cast<ABlock>(outhit.GetActor()))
			{
				_status = CharaState::State_Jump;
			}
		}
	}

	//自身以外のキャラが近くにいる、かつ一定の高さより上にいる
	if (istargethit != false && pPos.Z < -180.0) return;
	
	endPos = FVector(pPos.X, pPos.Y, pPos.Z - 400.0);

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
		FLinearColor(0.0f, 0.0f, 0.0f, 0.0f),
		FLinearColor(0.0f, 0.0f, 0.0f, 0.0f),
		deltaTime
	);

	for (FHitResult outhit : outHits) {
		if (Cast<ABlock>(outhit.GetActor())) {

			if (this->ActorHasTag("enemy1")) {
				if (Cast<ABlock>(outhit.GetActor())->Get_breakChara() != BreakChara::Enemy_Daring) {
					_status = CharaState::State_Attack;
					_atkstate = AttackState::DownDir;
				}
			}
			else if (this->ActorHasTag("enemy2")) {
				if (Cast<ABlock>(outhit.GetActor())->Get_breakChara() != BreakChara::dummy1) {
					_status = CharaState::State_Attack;
					_atkstate = AttackState::DownDir;
				}
			}
			else if (this->ActorHasTag("enemy3")) {
				if (Cast<ABlock>(outhit.GetActor())->Get_breakChara() != BreakChara::dummy2) {
					_status = CharaState::State_Attack;
					_atkstate = AttackState::DownDir;
				}
			}
		}
		else if (outhit.GetActor()->ActorHasTag("IsGround"))
		{
			_status = CharaState::State_Jump;
		}
	}	
}

void AEnemy_Daring::ChaseTargetDir(ACharacterBase* target)
{
	//自身の角度を算出
	FVector pPos = GetActorLocation();
	double radian = atan2(pPos.Y, pPos.X); //単位円
	double degree = radian * 180.0 / PI;	//角度

	//ターゲットの角度を算出
	FVector targetPos = target->GetActorLocation();
	double targetradian = atan2(targetPos.Y, targetPos.X); //単位円
	double targetdegree = targetradian * 180.0 / PI;	//角度

	//自身と相手が円の境目にいるなら
	if (abs(targetdegree - degree) > 180.0) {
		//通常とは逆方向の移動方向の算出
		if (degree > targetdegree) {
			_moveDir = MoveDirection::Move_Left;
		}
		else if (degree < targetdegree) {
			_moveDir = MoveDirection::Move_Right;
		}
	}
	else {
		//通常方向の移動方向の算出
		if (degree > targetdegree) {
			_moveDir = MoveDirection::Move_Right;
		}
		else if (degree < targetdegree) {
			_moveDir = MoveDirection::Move_Left;
		}
	}	

	//ターゲットの距離が指定内だったら
	if (abs(degree - targetdegree) < 10.0 && pPos.Z + 30.0 > targetPos.Z
		|| abs(degree - targetdegree) < 10.0 && pPos.Z - 30.0 < targetPos.Z){
		InputAttack();
	}
	//それ以外なら移動
	else{
		_status = CharaState::State_Move;
	}	
}

void AEnemy_Daring::MatColorReset(float DeltaTime)
{
	_reactiontimer += DeltaTime;

	if (_reactiontimer > 0.3)
	{
		if (this->ActorHasTag(TEXT("enemy1"))) {
			_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(1.0, 0.075, 0.0));
		}
		else if (this->ActorHasTag(TEXT("enemy2"))) {
			_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(1.0, 0.75, 0.3));
		}
		else if (this->ActorHasTag(TEXT("enemy3"))) {
			_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(0.03, 1.0, 0.1));
		}

		_reactiontimer = 0.0;
		_damageReaction = false;
	}
}