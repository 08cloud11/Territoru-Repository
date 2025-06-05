//--------------------------------------------------------------------
// �t�@�C���� �FTerritoruCharacter.h
// �T�v       �F�v���C���[�L�����N�^�[
// �쐬��     �F0231�{��
// �X�V���e   �F03/05�@�쐬
//--------------------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.h"
#include "TerritoruCharacter.generated.h"


UCLASS(config=Game)
class ATerritoruCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	ATerritoruCharacter();
	
	virtual void Tick(float DeltaTime) override;
			
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay()override;

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�L�[�������ꂽ�Ƃ��Ɉړ�������L�����̏�Ԃ��w��
	virtual void InputMove()override;

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�L�[�������ꂽ�Ƃ��ɐ}�`�ԍ���ύX���A�}�`�Z�b�g
	virtual void InputShapeChange()override;

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�G�Ɏg�p����㩂�z�u����
	virtual void InputTrapArrange()override;

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F�_���[�W���󂯂���Ɍ��̐F�ɖ߂�
	virtual void MatColorReset(float DeltaTime) override;

	// ���@���FUStaticMeshComponent*
	// �߂�l�Fvoid
	// �������e�F�v���C���[���_���[�W���󂯂��ۂɃ}�e���A���ύX
	virtual void MatChange(UStaticMeshComponent* smc) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* _defaultMappingContext;//�C���v�b�g�̏ڍ׏��

	UPROPERTY(EditAnywhere, Category = "Mat")
	TMap < FName, TObjectPtr<UMaterialInterface>> _playerMats;

	UPROPERTY(EditAnywhere, Category = Input)
	TMap<FName, TObjectPtr<UInputAction>> _InputActions;//�C���v�b�g�A�N�V��������
};

