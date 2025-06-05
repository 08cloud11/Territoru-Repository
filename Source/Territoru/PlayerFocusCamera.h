//--------------------------------------------------------------------
// �t�@�C���� �FPlayerFocusCamera.h
// �T�v       �F�v���C���[��Ǐ]����J����
// �쐬��     �F0231�{��
// �X�V���e   �F05/18�@�쐬
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "CharacterBase.h"
#include "PlayerFocusCamera.generated.h"

UCLASS()
class TERRITORU_API APlayerFocusCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:
	APlayerFocusCamera();

	virtual void Tick(float DeltaTime) override;

	// ���@���Fdouble
	// �߂�l�Fvoid
	// �������e�F���g�̃|�W�V�����Ɖ�]��ݒ�
	void SetCameraPosition(double);

	// ���@���Fdouble
	// �߂�l�Fvoid
	// �������e�F���g�̃|�W�V�����Ɖ�]��ݒ�
	void SetUpwardLocation();

	// ���@���Fdouble, ACharacterBase*
	// �߂�l�Fvoid
	// �������e�F���ʂ̔��a�ƒǐՂ���v���C���[��ݒ�
	void SetFocusPlayer(double val, ACharacterBase* focusplayer) {
		_globalradius = val; 
		_player = focusplayer;
	}

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> _player;

	UPROPERTY(EditAnywhere, Category = "AddRadiusValue")
	double _addradiusvalue;

	double _globalradius;

	double _afterfocuslength;

	bool _bfocus;
};
