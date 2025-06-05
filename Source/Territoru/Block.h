//--------------------------------------------------------------------
// �t�@�C���� �FBlock.h
// �T�v       �F�X�e�[�W�ɔz�u����u���b�N
// �쐬��     �F0231�{��
// �X�V���e   �F03/07�@�쐬
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Block.generated.h"

//���g��j�󂵂��L����
enum class BreakChara: uint8
{
	None,
	Player,
	Enemy_Daring,
	dummy1,
	dummy2
};

UCLASS()
class TERRITORU_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlock();

	virtual void Tick(float DeltaTime) override;

	// ���@���FUPrimitiveComponent*, HitComponent, AActor*
	// �@�@�@�@UPrimitiveComponent*, FVector, FHitResult
	// �߂�l�Fvoid
	// �������e�F�����蔻��
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//------------�Q�b�^�[-----------------

	BreakChara Get_breakChara() const { return _breakChara; }
	double Get_breakedTimer() const { return _breakedTimer; }
	int Get_hp() const { return _hp; }
	
	//-------------------------------------

	void Add_breakedTimer(float _deltaTime) { _breakedTimer += _deltaTime; }
	void Set_breakedTimer(double _timer) { _breakedTimer = _timer; }
	void Set_hp(int hp) { _hp = hp; }

	virtual void BeginPlay() override;

private:
	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F�_���[�W���󂯂���Ɍ��̐F�ɖ߂�
	void MatColorReset(float DeltaTime);

private:
	UPROPERTY(EditAnywhere, Category = Materials)
	TMap<FName, TObjectPtr<UMaterialInterface>> _mats;

	UPROPERTY()
	UMaterialInstanceDynamic* _mymat = nullptr;

	double _breakedTimer;		//�j�󂳂�Ă���̕b��
	double _reactionTimer;		//�_���[�W���̌v��

	const double _reactionendtime = 0.15;	//�_���[�W���̐F�ω��̏I������ : 0.15

	int _hp;						//�̗�

	BreakChara _breakChara;		//���݂̃u���b�N�̏��

	bool _breaction;			//�_���[�W����
};
