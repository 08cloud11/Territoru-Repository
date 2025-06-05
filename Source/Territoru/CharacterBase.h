//--------------------------------------------------------------------
// ファイル名 ：CharacterBase.h
// 概要       ：プレイヤー、敵が継承するキャラクター基盤クラス
// 作成者     ：0231本間
// 更新内容   ：03/05　作成
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DamagedComponent.h"
#include "CharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

//キャラの状態
UENUM(BlueprintType)
enum class CharaState : uint8
{
	State_Idle UMETA(DisplayName = "None"),
	State_Move UMETA(DisplayName = "Move"),
	State_Jump UMETA(DisplayName = "Jump"),
	State_Attack UMETA(DisplayName = "Attack")
};

//図形情報
UENUM(BlueprintType)
enum class ShapeNumber : uint8
{
	Triangular UMETA(DisplayName = "Triangular"),
	Quadrangular UMETA(DisplayName = "Quadrangular"),
	Pentagonal UMETA(DisplayName = "Pentagonal")
};

UCLASS()
class TERRITORU_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	//移動方向
	enum MoveDirection
	{
		Move_Left,
		Move_Right,
		Move_Up,
		Move_Down
	};

	//攻撃方向
	enum AttackState
	{
		None,
		LeftDir,
		RightDir,
		UpDir,
		DownDir
	};

protected:
	//移動時計算用変数
	struct ShapeValues
	{
		double _max_moveLength;	//最大移動速度
		double _acceleration;	//加速量
		double _attenuation;	//減速量
		int _attackdamage;      //攻撃時の与えるダメージ量
	};

public:
	ACharacterBase();

	virtual void Tick(float DeltaTime) override {
		Super::Tick(DeltaTime);
	}

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override {
		Super::SetupPlayerInputComponent(PlayerInputComponent);
	}

	// 引　数：UPrimitiveComponent*, OverlappedComp, 
	// 　　　　UPrimitiveComponent*, int32, bool, FHitResult
	// 戻り値：void
	// 処理内容：当たり判定
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//------------get functions----------------

	UFUNCTION(BlueprintCallable, Category = "GetFunctions")
	CharaState Get_Status() const { return _status; }

	UFUNCTION(BlueprintCallable, Category = "GetFunctions")
	ShapeNumber Get_meshnum() const { return _meshnum; }

	MoveDirection Get_movedir() const { return _moveDir; }

	AttackState Get_atkstate() const { return _atkstate; }

	UFUNCTION(BlueprintCallable)
	double Get_respawntimer() const { return _respawntimer; }

	double Get_movelength() const { return _moveLength; }
	double Get_atkgeneralValue() const { return _atkgeneralValue; }

	UFUNCTION(BlueprintCallable)
	int Get_max_hp() const { return _max_hp; }

	UFUNCTION(BlueprintCallable)
	int Get_hp() const { return _hp; }

	int Get_attackdamage() const { return _currentvalues._attackdamage; }
	int Get_deadnum() const { return _deadnum; }
	int Get_killnum() const { return _killnum; }

	UFUNCTION(BlueprintCallable)
 	bool Get_bdead() const { return _bdead; }
	
	UFUNCTION(BlueprintCallable, Category = "GetFunctions")
	bool Get_damageReaction() const { return _damageReaction; }

	bool Get_bstart() const { return _bstart; }
	bool Get_binvincible() const { return _binvincible; } 

	//-------------------------------------

	void Start() { _bstart = true; }
	void Increment_killnum() { _killnum++; }
	void Offis_damageReaction() { _damageReaction = false; }
	void Stun() { 
		_bstun = true; 
		_status = CharaState::State_Idle;
	}

protected:
	virtual void BeginPlay() override;

	// 引　数：なし
	// 戻り値：void
	// 処理内容：移動処理
	virtual void Move();

	// 引　数：なし
	// 戻り値：void
	// 処理内容：攻撃処理
	virtual void AttackAction();

	// 引　数：なし
	// 戻り値：void
	// 処理内容：図形変更処理
	virtual void ShapeChange();

	// 引　数：なし
	// 戻り値：void
	// 処理内容：仮想関数定義
	virtual void InputMove() {};

	// 引　数：なし
	// 戻り値：void
	// 処理内容：移動方向から攻撃方向を算出
	virtual void InputAttack();

	// 引　数：なし
	// 戻り値：void
	// 処理内容：仮想関数定義
	virtual void InputShapeChange() {};

	// 引　数：なし
	// 戻り値：void
	// 処理内容：仮想関数定義
	virtual void InputTrapArrange() {};

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：体力回復
	void HealHP(float deltaTime);

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：死亡状態から復活
	void Respawn(float deltaTime);

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：無敵中の処理
	void Invincible(float deltaTime);

	// 引　数：フレーム秒
	// 戻り値：void
	// 処理内容：仮想関数定義
	virtual void MatColorReset(float DeltaTime) {};

	// 引　数：UStaticMeshComponent*
	// 戻り値：void
	// 処理内容：仮想関数定義
	virtual void MatChange(UStaticMeshComponent* smc) {};

	// 引　数：float
	// 戻り値：void
	// 処理内容：現在乗っているブロックが自身が破壊したものならステータス上昇
	void DownLineTrace(float deltaTime);

	// 引　数：なし
	// 戻り値：void
	// 処理内容：スタン中にブルブルと震える
	void StunAction();

private:
	// 引　数：なし
	// 戻り値：void
	// 処理内容：コンストラクタでの初期化処理
	void ConstInit();

	// 引　数：なし
	// 戻り値：void
	// 処理内容：BeginPlay()での初期化処理
	void BeginInit();	

protected:
	UPROPERTY()
	APlayerController* _controller = nullptr;

	UPROPERTY()
	UDamagedComponent* _damagedComponent;

	UPROPERTY()
	UMaterialInstanceDynamic* _mymat = nullptr;

	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<AActor> _trap;

	//移動ごとに進行方向を保管する。
	// アタックキーが押された時に判定
	MoveDirection _moveDir;
	
	//現在のキャラの行動
	CharaState _status;
	
	//現在の図形
	ShapeNumber _meshnum;
	
	//アタックキー押された時の攻撃方向判定用
	AttackState _atkstate;

	//図形ごとの速度や攻撃の値保管用
	TMap<FName, ShapeValues> _valuesArray;

	//移動時計算用変数
	ShapeValues _currentvalues;

	//死亡時のポジション
	UPROPERTY()
	FVector _deadpos;

	const double _atkgeneralValue = 0.015;	//攻撃時用汎用値

	double _attackmoveDistance;	//攻撃時の移動範囲測定用
	double _yawRotLength;		//回転用（画面横方向）
	double _moveLength;			//移動速度
	double _globalradius;		//ステージマネージャから取得したステージの半径

	const double _respawntime = 8.0;

	double _healingtimer;		//体力回復の時間計測
	double _respawntimer;		//倒されてから復活するまでの時間計測
	double _invincibletimer;	//無敵中の時間計測
	double _atkcooltimer;		//攻撃のクールタイム用
	double _reactiontimer;		//ダメージ時の計測
	double _statusuptimer;		//ステータス上昇中の計測
	double _stuntimer;

	const int _max_hp = 15;			//最大体力
	int _hp;					//体力
	int _deadnum;				//死亡回数
	int _killnum;				//倒した回数

	bool _bstart = false;		//行動可能か
	bool _bmove = false;		//移動できるか
	bool _bdead = false;		//死亡状態か
	bool _binvincible = false;	//無敵状態か
	bool _bafteratk = false;	//攻撃後か
	bool _damageReaction = false;	//ダメージ受けた後の無敵状態か
	bool _bstatusup = false;	//ステータス上昇中か
	bool _bstun = false;		//スタン中か
	bool _stunviberight = false;	//スタン中、trueなら右方向に微移動
};
