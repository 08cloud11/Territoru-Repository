//--------------------------------------------------------------------
// ファイル名 ：StageManager.h
// 概要       ：ステージに配置されるブロックの生成等の総括管理
// 作成者     ：0231本間
// 更新内容   ：03/07　作成
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.h"
#include "PlayerFocusCamera.h"
#include "Engine/DataTable.h"
#include "StageManager.generated.h"

USTRUCT(BlueprintType)
struct FResults : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameData", DisplayName = "PlayerNum", meta = (IgnoreForMemberInitializationTest))
	int32 playernum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameData", DisplayName = "Enemy1Num", meta = (IgnoreForMemberInitializationTest))
	int32 enemy1num;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameData", DisplayName = "Enemy2Num", meta = (IgnoreForMemberInitializationTest))
	int32 enemy2num;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameData", DisplayName = "Enemy3Num", meta = (IgnoreForMemberInitializationTest))
	int32 enemy3num;
};

UCLASS()
class TERRITORU_API AStageManager : public AActor
{
	GENERATED_BODY()

public:	
	AStageManager();

	virtual void Tick(float DeltaTime) override;

	//------------get functions----------------

	UFUNCTION(BlueprintCallable)
	FResults Get_results() const { return _results; }

	double Get_globalradius() const { return _globalradius; }

	UFUNCTION(BlueprintCallable)
	double Get_gametimer() const { return _gametimer; }

	UFUNCTION(BlueprintCallable)
	double Get_endtime() const { return _endtime; }

	//-------------------------------------

	UFUNCTION(BlueprintCallable)
	void Start();

protected:
	virtual void BeginPlay() override;
	
private:
	// 引　数：なし
	// 戻り値：void
	// 処理内容：ブロック初期生成処理
	void CreateBlocks();

	// 引　数：なし
	// 戻り値：void
	// 処理内容：カメラ生成、初期化
	void CreateCamera();

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：全てのブロックをチェックし、該当処理
	void CheckBlocks(float _deltaTime);

	// 引　数：なし
	// 戻り値：void
	// 処理内容：ゲーム終了タイミングの処理
	void GameTimeEnd();

private:
	//ブロックのアクターBP
	UPROPERTY(EditAnywhere, Category = "Block")
	TSubclassOf<AActor> _block;

	//プレイヤーに追従するカメラ
	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<APlayerFocusCamera> _camera;

	UPROPERTY(EditAnywhere, Category = "ResultsUI")
	TSubclassOf<UUserWidget> _ui_results;

	UPROPERTY()
	UUserWidget* _pui_results = nullptr;

	//全てのブロックを管理
	UPROPERTY()
	TArray<ABlock*> _blocks;

	FResults _results;

	UPROPERTY(EditAnywhere, Category = "Global")
	double _globalradius;

	double _gametimer; //ゲーム開始からの時間計測

	const double _endtime = 90.0;			//インゲーム終了までの時間 : 90.0
	const double _blockvisiblytime = 3.0;	//ブロックが破壊されて復活するまでの時間 : 3.0

	const int _blockmaxhp = 3;				//生成するブロックの最大体力 : 3

	UPROPERTY(EditAnywhere, Category = "IngameJudge")
	bool _bingame;

	bool _bstart = false;
};
