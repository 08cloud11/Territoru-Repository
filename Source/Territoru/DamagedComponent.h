//--------------------------------------------------------------------
// ファイル名 ：DamagedComponent.h
// 概要       ：プレイヤー、敵がダメージを受けた際に使用するコンポーネント
// 作成者     ：0231本間
// 更新内容   ：03/24　作成
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

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：無敵時間中の点滅処理
	UFUNCTION(BlueprintCallable)
	void BlinkingMat(float deltaTime);

private:
	UPROPERTY()
	TArray<UStaticMeshComponent*> _smcs;

	double _damagedTimer;	//ダメージを受けた時点からの計測
	double _blinkingTimer;	//無敵時間中の点滅用

	int _ownerMeshnum;	//オーナーの現在の図形番号
	
	bool _isblinking = false;
	bool _isdisplay = false;	//無敵時間中、キャラを表示するか
};
