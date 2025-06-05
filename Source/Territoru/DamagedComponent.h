//--------------------------------------------------------------------
// �t�@�C���� �FDamagedComponent.h
// �T�v       �F�v���C���[�A�G���_���[�W���󂯂��ۂɎg�p����R���|�[�l���g
// �쐬��     �F0231�{��
// �X�V���e   �F03/24�@�쐬
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DamagedComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERRITORU_API UDamagedComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class ACharacterBase;
	friend class AEnemy_Daring;
	friend class ATerritoruCharacter;

public:	
	UDamagedComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "GetFunctions")
	double Get_blinkingTimer()const { return _blinkingTimer; }

	void BlinkingTrigger() { _isblinking = true; }

protected:
	virtual void BeginPlay() override;

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F���G���Ԓ��̓_�ŏ���
	UFUNCTION(BlueprintCallable)
	void BlinkingMat(float deltaTime);

private:
	UPROPERTY()
	TArray<UStaticMeshComponent*> _smcs;

	double _damagedTimer;	//�_���[�W���󂯂����_����̌v��
	double _blinkingTimer;	//���G���Ԓ��̓_�ŗp

	int _ownerMeshnum;	//�I�[�i�[�̌��݂̐}�`�ԍ�
	
	bool _isblinking = false;
	bool _isdisplay = false;	//���G���Ԓ��A�L������\�����邩
};
