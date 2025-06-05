//--------------------------------------------------------------------
// ファイル名 ：Enemy_Daring.h
// 概要       ：勇猛果敢な敵
// 作成者     ：0231本間
// 更新内容   ：03/13　作成
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
	
	// 引　数：なし
	// 戻り値：void
	// 処理内容：移動処理
	virtual void Move()override;

	// 引　数：なし
	// 戻り値：void
	// 処理内容：キーが押されたときに移動方向やキャラの状態を指定
	virtual void InputMove()override;

	// 引　数：なし
	// 戻り値：void
	// 処理内容：キーが押されたときに図形番号を変更し、図形セット
	virtual void InputShapeChange()override;

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：ダメージを受けた後に元の色に戻す
	virtual void MatColorReset(float DeltaTime) override;

private:
	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：行動を決定する関数
	void ActionDecision(float deltaTime);

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：左右,下方向にレイを飛ばす関数
	void LineTrace(float deltaTime);

	// 引　数：ACharacterBase*
	// 戻り値：void
	// 処理内容：レイを飛ばした先に発見した相手を
	// 　　　　　追跡する方向を決定する
	void ChaseTargetDir(ACharacterBase* target);

private:
	UPROPERTY()
	ACharacterBase* _ptarget;

	double _targetingtimer;
};
