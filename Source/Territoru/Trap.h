//--------------------------------------------------------------------
// �t�@�C���� �FTrap.h
// �T�v       �F�L�����N�^�[���z�u�����
// �쐬��     �F0231�{��
// �X�V���e   �F04/15�@�쐬
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"

enum class ArrangeChara : uint8
{
	Player,
	Enemy_Daring,
	dummy1,
	dummy2
};

UCLASS()
class TERRITORU_API ATrap : public AActor
{
	GENERATED_BODY()
	
public:	
	ATrap();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Set_arrangechara(ArrangeChara _num) { _arrangechara = _num; };

	ArrangeChara Get_arrangechara()const { return _arrangechara; }

protected:
	virtual void BeginPlay() override;

private:
	ArrangeChara _arrangechara;

	double _destroyTimer;
};
