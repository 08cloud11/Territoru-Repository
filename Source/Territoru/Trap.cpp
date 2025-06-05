//--------------------------------------------------------------------
// �t�@�C���� �FTrap.cpp
// �T�v       �F�L�����N�^�[���z�u�����
// �쐬��     �F0231�{��
// �X�V���e   �F04/15�@�쐬
//--------------------------------------------------------------------

#include "Trap.h"
#include "TerritoruCharacter.h"
#include "Enemy_Daring.h"

ATrap::ATrap()
	:_destroyTimer(0.0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATrap::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UPrimitiveComponent>(GetRootComponent())->
		OnComponentBeginOverlap.AddDynamic(this, &ATrap::OnOverlapBegin);
}

void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	_destroyTimer += DeltaTime;

	if (_destroyTimer > 4.0) {
		this->Destroy();
	}
}

void ATrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<ACharacterBase>(OtherActor)) { return; }

	switch (_arrangechara)
	{
	case ArrangeChara::Player:
		if (OtherActor->ActorHasTag("Player")) { return; }
		break;
	case ArrangeChara::Enemy_Daring:
		if (OtherActor->ActorHasTag("enemy1")) { return; }
		break;
	case ArrangeChara::dummy1:
		if (OtherActor->ActorHasTag("enemy2")) { return; }
		break;
	case ArrangeChara::dummy2:
		if (OtherActor->ActorHasTag("enemy3")) { return; }
		break;
	}

	Cast<ACharacterBase>(OtherActor)->Stun();
}
