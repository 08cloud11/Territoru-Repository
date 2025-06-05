//--------------------------------------------------------------------
// �t�@�C���� �FPlayerFocusCamera.cpp
// �T�v       �F�v���C���[��Ǐ]����J����
// �쐬��     �F0231�{��
// �X�V���e   �F05/18�@�쐬
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

	//���S��Ԃ�null
	if (_player == nullptr || _player->Get_bdead()) return;

	//���g�̊p�x���Z�o
	FVector pPos = GetActorLocation();
	double radian = atan2(pPos.Y, pPos.X); //�P�ʉ~
	double degree = radian * 180.0 / PI;	//�p�x

	//�^�[�Q�b�g�̊p�x���Z�o
	FVector targetPos = _player->GetActorLocation();
	double targetradian = atan2(targetPos.Y, targetPos.X); //�P�ʉ~
	double targetdegree = targetradian * 180.0 / PI;	//�p�x

	//�㉺�����̃J�����ʒu����
	if (abs(pPos.Z - targetPos.Z) > 10.0)
	{
		//�J�������v���C���[����
		if (pPos.Z > targetPos.Z)
		{
			SetActorLocation(FVector(pPos.X, pPos.Y, pPos.Z - 7.5));

			if (pPos.Z < targetPos.Z) SetActorLocation(FVector(pPos.X, pPos.Y, targetPos.Z));
		}
		//�J�������v���C���[��艺
		else if (pPos.Z < targetPos.Z)
		{
			SetActorLocation(FVector(pPos.X, pPos.Y, pPos.Z + 7.5));

			if (pPos.Z > targetPos.Z) SetActorLocation(FVector(pPos.X, pPos.Y, targetPos.Z));
		}
	}

	//�X�^�[�g���Ă��Ȃ�
	if (!_player->Get_bstart()) return;

	//�p�x�̍���300�ȏ�i���E�ʒu�j
	if (abs(degree - targetdegree) > 300) {
		if (degree > targetdegree) targetdegree += 360.0;
		else if (degree < targetdegree) degree += 360.0;
	}

	//�U����
	if (_player->Get_atkstate() != ACharacterBase::None)
	{
		//�㉺�����̃J�����ʒu����
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

	//�ړ���
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
	//�U���A�ړ��I����
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
	double radian = atan2(pPos.Y, pPos.X); //�P�ʉ~
	double degree = radian * 180.0 / PI;	//�p�x

	FVector newPos = FVector((_globalradius + _addradiusvalue) * cos(addValue + radian),
		(_globalradius + _addradiusvalue) * sin(addValue + radian), pPos.Z);

	SetActorLocation(newPos);
	SetActorRotation(FRotator(0.0, degree + 180.0, 0.0));
}