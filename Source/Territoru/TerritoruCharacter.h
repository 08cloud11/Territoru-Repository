//--------------------------------------------------------------------
// ファイル名 ：TerritoruCharacter.h
// 概要       ：プレイヤーキャラクター
// 作成者     ：0231本間
// 更新内容   ：03/05　作成
//--------------------------------------------------------------------
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.h"
#include "TerritoruCharacter.generated.h"


UCLASS(config=Game)
class ATerritoruCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	ATerritoruCharacter();
	
	virtual void Tick(float DeltaTime) override;
			
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay()override;

	// 引　数：なし
	// 戻り値：void
	// 処理内容：キーが押されたときに移動方向やキャラの状態を指定
	virtual void InputMove()override;

	// 引　数：なし
	// 戻り値：void
	// 処理内容：キーが押されたときに図形番号を変更し、図形セット
	virtual void InputShapeChange()override;

	// 引　数：なし
	// 戻り値：void
	// 処理内容：敵に使用する罠を配置する
	virtual void InputTrapArrange()override;

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：ダメージを受けた後に元の色に戻す
	virtual void MatColorReset(float DeltaTime) override;

	// 引　数：UStaticMeshComponent*
	// 戻り値：void
	// 処理内容：プレイヤーがダメージを受けた際にマテリアル変更
	virtual void MatChange(UStaticMeshComponent* smc) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* _defaultMappingContext;//インプットの詳細情報

	UPROPERTY(EditAnywhere, Category = "Mat")
	TMap < FName, TObjectPtr<UMaterialInterface>> _playerMats;

	UPROPERTY(EditAnywhere, Category = Input)
	TMap<FName, TObjectPtr<UInputAction>> _InputActions;//インプットアクションたち
};

