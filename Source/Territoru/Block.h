//--------------------------------------------------------------------
// ファイル名 ：Block.h
// 概要       ：ステージに配置するブロック
// 作成者     ：0231本間
// 更新内容   ：03/07　作成
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Block.generated.h"

//自身を破壊したキャラ
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

	// 引　数：UPrimitiveComponent*, HitComponent, AActor*
	// 　　　　UPrimitiveComponent*, FVector, FHitResult
	// 戻り値：void
	// 処理内容：当たり判定
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//------------ゲッター-----------------

	BreakChara Get_breakChara() const { return _breakChara; }
	double Get_breakedTimer() const { return _breakedTimer; }
	int Get_hp() const { return _hp; }
	
	//-------------------------------------

	void Add_breakedTimer(float _deltaTime) { _breakedTimer += _deltaTime; }
	void Set_breakedTimer(double _timer) { _breakedTimer = _timer; }
	void Set_hp(int hp) { _hp = hp; }

	virtual void BeginPlay() override;

private:
	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：ダメージを受けた後に元の色に戻す
	void MatColorReset(float DeltaTime);

private:
	UPROPERTY(EditAnywhere, Category = Materials)
	TMap<FName, TObjectPtr<UMaterialInterface>> _mats;

	UPROPERTY()
	UMaterialInstanceDynamic* _mymat = nullptr;

	double _breakedTimer;		//破壊されてからの秒数
	double _reactionTimer;		//ダメージ時の計測

	const double _reactionendtime = 0.15;	//ダメージ時の色変化の終了時間 : 0.15

	int _hp;						//体力

	BreakChara _breakChara;		//現在のブロックの状態

	bool _breaction;			//ダメージ中か
};
