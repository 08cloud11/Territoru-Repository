//--------------------------------------------------------------------
// �t�@�C���� �FBlock.cpp
// �T�v       �F�X�e�[�W�ɔz�u����u���b�N
// �쐬��     �F0231�{��
// �X�V���e   �F03/07�@�쐬
//--------------------------------------------------------------------

#include "Block.h"
#include "CharacterBase.h"
#include "TerritoruCharacter.h"
#include "Enemy_Daring.h"

ABlock::ABlock()
	:_breakChara(BreakChara::None)
	,_breakedTimer(0.0)
	,_reactionTimer(0.0)
	,_hp(0)
	,_breaction(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABlock::BeginPlay()
{
	Super::BeginPlay();
	
	//�u���b�N�̃X�^�e�B�b�N���b�V���R���|�[�l���g���擾
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	UActorComponent* smc = this->GetComponentByClass(findClass);

	_mymat = Cast<UStaticMeshComponent>(smc)->CreateDynamicMaterialInstance(0);

	//�����蔻����I�[�o�[���C�h
	Cast<UStaticMeshComponent>(smc)->OnComponentHit.AddDynamic(this, &ABlock::OnHit);
}

void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (_breaction) { MatColorReset(DeltaTime); }
}

void ABlock::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!Cast<ACharacterBase>(OtherActor)) { return; }
	if (Cast<ACharacterBase>(OtherActor)->Get_Status() != CharaState::State_Attack) return;
	
	//�Ώۂ̍U���͂�ǂݎ���ă_���[�W
	_hp -= Cast<ACharacterBase>(OtherActor)->Get_attackdamage();

	_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(1.0, 0.2, 0.2));
	_reactionTimer = 0.0;
	_breaction = true;

	if (_hp > 0) return;

	//�u���b�N�̃X�^�e�B�b�N���b�V���R���|�[�l���g���擾
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(this->GetComponentByClass(findClass));

	//����Ȃ��悤�ɂ��āA�����蔻��𖳌��ɂ���
	smc->SetVisibility(false);
	smc->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (Cast<ATerritoruCharacter>(OtherActor)) {
		//�u���b�N�̃}�e���A�����v���C���[�p�ɕύX
		smc->SetMaterial(0, _mats["Player"]);
		_breakChara = BreakChara::Player;
	}
	else if (Cast<AEnemy_Daring>(OtherActor)) {
		//
		if (Cast<AEnemy_Daring>(OtherActor)->ActorHasTag("enemy1")) {
			//�u���b�N�̃}�e���A�����G�l�~�[1�p�ɕύX
			smc->SetMaterial(0, _mats["Enemy_Daring"]);
			_breakChara = BreakChara::Enemy_Daring;
		}
		else if (Cast<AEnemy_Daring>(OtherActor)->ActorHasTag("enemy2")) {
			//�u���b�N�̃}�e���A�����G�l�~�[2�p�ɕύX
			smc->SetMaterial(0, _mats["dummy1"]);
			_breakChara = BreakChara::dummy1;
		}
		else if (Cast<AEnemy_Daring>(OtherActor)->ActorHasTag("enemy3")) {
			//�u���b�N�̃}�e���A�����G�l�~�[3�p�ɕύX
			smc->SetMaterial(0, _mats["dummy2"]);
			_breakChara = BreakChara::dummy2;
		}				
	}	

	_mymat = Cast<UStaticMeshComponent>(smc)->CreateDynamicMaterialInstance(0);
}

void ABlock::MatColorReset(float DeltaTime)
{
	_reactionTimer += DeltaTime;

	if (_reactionTimer > _reactionendtime)
	{
		switch (_breakChara)
		{
		case BreakChara::None:
			_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(0.3, 0.3, 0.3));
			break;

		case BreakChara::Player:
			_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(0.0, 0.5, 1.0));
			break;
		
		case BreakChara::Enemy_Daring:
			_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(1.0, 0.075, 0.0));
			break;
		
		case BreakChara::dummy1:
			_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(1.0, 0.75, 0.3));
			break;
		
		case BreakChara::dummy2:
			_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(0.03, 1.0, 0.1));
			break;
		}

		_breaction = false;
	}
}