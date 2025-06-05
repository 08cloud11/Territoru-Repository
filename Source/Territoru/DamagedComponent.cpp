//--------------------------------------------------------------------
// �t�@�C���� �FDamagedComponent.cpp
// �T�v       �F�v���C���[�A�G���_���[�W���󂯂��ۂɎg�p����R���|�[�l���g
// �쐬��     �F0231�{��
// �X�V���e   �F03/24�@�쐬
//--------------------------------------------------------------------

#include "DamagedComponent.h"
#include "Components/MeshComponent.h"
#include "CharacterBase.h"

UDamagedComponent::UDamagedComponent()
	:_damagedTimer(0.0)
	,_blinkingTimer(0.0)
	,_ownerMeshnum(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamagedComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//���g��"SMC"�^�O�̂����X�^�e�B�b�N���b�V���R���|�[�l���g���擾
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	TArray<UActorComponent*> smcs = GetOwner()->GetComponentsByTag(findClass, "SMC");

	for (auto smc : smcs)
	{
		_smcs.Add(Cast<UStaticMeshComponent>(smc));
	}
}

void UDamagedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Cast<ACharacterBase>(GetOwner())->Get_bdead()) { return; }

	if (_isblinking) { BlinkingMat(DeltaTime); }
}

void UDamagedComponent::BlinkingMat(float deltaTime)
{
	_damagedTimer += deltaTime;

	_blinkingTimer += deltaTime;

	if (_blinkingTimer < 0.2) { return; }

	if (_isdisplay) {
		//���g��"SMC"�^�O�̂����X�^�e�B�b�N���b�V���R���|�[�l���g���擾
		TSubclassOf<UStaticMeshComponent> findClass;
		findClass = UStaticMeshComponent::StaticClass();
		TArray<UActorComponent*> smcs;

		//���݂̐}�`�ɊY�����郂�m�̂݌����悤�ɂ���
		switch (Cast<ACharacterBase>(GetOwner())->Get_meshnum())
		{
		case ShapeNumber::Triangular:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Triangular");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		case ShapeNumber::Quadrangular:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Quadrangular");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		case ShapeNumber::Pentagonal:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Pentagonal");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		}

		_isdisplay = false;
	}
	else {
		for (auto smc : _smcs)
		{
			smc->SetVisibility(false);
		}

		_isdisplay = true;
	}

	_blinkingTimer = 0.0;

	if (_damagedTimer > 1.0)
	{
		_damagedTimer = 0.0;
		_isdisplay = false;
		_isblinking = false;

		//���g��"SMC"�^�O�̂����X�^�e�B�b�N���b�V���R���|�[�l���g���擾
		TSubclassOf<UStaticMeshComponent> findClass;
		findClass = UStaticMeshComponent::StaticClass();
		TArray<UActorComponent*> smcs;

		//���݂̐}�`�ɊY�����郂�m�̂݌����悤�ɂ���
		switch (Cast<ACharacterBase>(GetOwner())->Get_meshnum())
		{
		case ShapeNumber::Triangular:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Triangular");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		case ShapeNumber::Quadrangular:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Quadrangular");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		case ShapeNumber::Pentagonal:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Pentagonal");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		}

		Cast<ACharacterBase>(GetOwner())->Offis_damageReaction();
	}
}