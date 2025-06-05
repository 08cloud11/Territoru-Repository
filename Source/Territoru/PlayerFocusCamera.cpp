//--------------------------------------------------------------------
// ファイル名 ：PlayerFocusCamera.cpp
// 概要       ：プレイヤーを追従するカメラ
// 作成者     ：0231本間
// 更新内容   ：05/18　作成
//--------------------------------------------------------------------

#include "PlayerFocusCamera.h"

APlayerFocusCamera::APlayerFocusCamera()
	: _afterfocuslength(0.018)
	, _bfocus(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerFocusCamera::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerFocusCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//死亡状態かnull
	if (_player == nullptr || _player->Get_bdead()) return;

	//自身の角度を算出
	FVector pPos = GetActorLocation();
	double radian = atan2(pPos.Y, pPos.X); //単位円
	double degree = radian * 180.0 / PI;	//角度

	//ターゲットの角度を算出
	FVector targetPos = _player->GetActorLocation();
	double targetradian = atan2(targetPos.Y, targetPos.X); //単位円
	double targetdegree = targetradian * 180.0 / PI;	//角度

	//上下方向のカメラ位置調整
	if (abs(pPos.Z - targetPos.Z) > 10.0)
	{
		//カメラがプレイヤーより上
		if (pPos.Z > targetPos.Z)
		{
			SetActorLocation(FVector(pPos.X, pPos.Y, pPos.Z - 7.5));

			if (pPos.Z < targetPos.Z) SetActorLocation(FVector(pPos.X, pPos.Y, targetPos.Z));
		}
		//カメラがプレイヤーより下
		else if (pPos.Z < targetPos.Z)
		{
			SetActorLocation(FVector(pPos.X, pPos.Y, pPos.Z + 7.5));

			if (pPos.Z > targetPos.Z) SetActorLocation(FVector(pPos.X, pPos.Y, targetPos.Z));
		}
	}

	//スタートしていない
	if (!_player->Get_bstart()) return;

	//角度の差が300以上（境界位置）
	if (abs(degree - targetdegree) > 300) {
		if (degree > targetdegree) targetdegree += 360.0;
		else if (degree < targetdegree) degree += 360.0;
	}

	//攻撃中
	if (_player->Get_atkstate() != ACharacterBase::None)
	{
		//上下方向のカメラ位置調整
		if (abs(degree - targetdegree) > 5.0) 	_bfocus = true;

		switch (_player->Get_atkstate())
		{
		case ACharacterBase::LeftDir:
			SetCameraPosition(_player->Get_atkgeneralValue() / 2.0);
			break;

		case ACharacterBase::RightDir:
			SetCameraPosition(-_player->Get_atkgeneralValue() / 2.0);
			break;
		}
	}

	//移動中
	if (_player->Get_Status() == CharaState::State_Move)
	{
		switch (_player->Get_movedir())
		{
		case ACharacterBase::Move_Left:
			if (degree > targetdegree) return;
			if (abs(degree - targetdegree) > 5.0) 	_bfocus = true;

			break;

		case ACharacterBase::Move_Right:
			if (degree < targetdegree) return;
			if (abs(degree - targetdegree) > 5.0) 	_bfocus = true;

			break;
		}

		if (_bfocus) SetCameraPosition(_player->Get_movelength());
	}
	//攻撃、移動終了後
	else if (_player->Get_Status() == CharaState::State_Idle)
	{
		if (!_bfocus) return;

		if (degree < targetdegree){
			if (abs(degree - targetdegree) > 1.0) {
				SetCameraPosition(_afterfocuslength);
				return;
			}
		}
		else if (degree > targetdegree) {
			if (abs(degree - targetdegree) > 1.0) {
				SetCameraPosition(-_afterfocuslength);
				return;
			}
		}

		_bfocus = false;
	}
}

void APlayerFocusCamera::SetCameraPosition(double addValue)
{
	FVector pPos = GetActorLocation();
	double radian = atan2(pPos.Y, pPos.X); //単位円
	double degree = radian * 180.0 / PI;	//角度

	FVector newPos = FVector((_globalradius + _addradiusvalue) * cos(addValue + radian),
		(_globalradius + _addradiusvalue) * sin(addValue + radian), pPos.Z);

	SetActorLocation(newPos);
	SetActorRotation(FRotator(0.0, degree + 180.0, 0.0));
}