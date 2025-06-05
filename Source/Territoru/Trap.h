//--------------------------------------------------------------------
// ファイル名 ：Trap.h
// 概要       ：キャラクターが配置する罠
// 作成者     ：0231本間
// 更新内容   ：04/15　作成
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
