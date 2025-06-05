//--------------------------------------------------------------------
// �t�@�C���� �FEnemy_Daring.h
// �T�v       �F�E�҉ʊ��ȓG
// �쐬��     �F0231�{��
// �X�V���e   �F03/13�@�쐬
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Enemy_Daring.generated.h"

UCLASS()
class TERRITORU_API AEnemy_Daring : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	AEnemy_Daring();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay()override;
	
	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�ړ�����
	virtual void Move()override;

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�L�[�������ꂽ�Ƃ��Ɉړ�������L�����̏�Ԃ��w��
	virtual void InputMove()override;

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�L�[�������ꂽ�Ƃ��ɐ}�`�ԍ���ύX���A�}�`�Z�b�g
	virtual void InputShapeChange()override;

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F�_���[�W���󂯂���Ɍ��̐F�ɖ߂�
	virtual void MatColorReset(float DeltaTime) override;

private:
	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F�s�������肷��֐�
	void ActionDecision(float deltaTime);

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F���E,�������Ƀ��C���΂��֐�
	void LineTrace(float deltaTime);

	// ���@���FACharacterBase*
	// �߂�l�Fvoid
	// �������e�F���C���΂�����ɔ������������
	// �@�@�@�@�@�ǐՂ�����������肷��
	void ChaseTargetDir(ACharacterBase* target);

private:
	UPROPERTY()
	ACharacterBase* _ptarget;

	double _targetingtimer;
};
