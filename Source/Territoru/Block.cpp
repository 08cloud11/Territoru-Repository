//--------------------------------------------------------------------
// ファイル名 ：Block.cpp
// 概要       ：ステージに配置するブロック
// 作成者     ：0231本間
// 更新内容   ：03/07　作成
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
	
	//ブロックのスタティックメッシュコンポーネントを取得
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	UActorComponent* smc = this->GetComponentByClass(findClass);

	_mymat = Cast<UStaticMeshComponent>(smc)->CreateDynamicMaterialInstance(0);

	//当たり判定をオーバーライド
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
	
	//対象の攻撃力を読み取ってダメージ
	_hp -= Cast<ACharacterBase>(OtherActor)->Get_attackdamage();

	_mymat->SetVectorParameterValue(TEXT("Reaction"), FVector(1.0, 0.2, 0.2));
	_reactionTimer = 0.0;
	_breaction = true;

	if (_hp > 0) return;

	//ブロックのスタティックメッシュコンポーネントを取得
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	UStaticMeshComponent* smc = Cast<UStaticMeshComponent>(this->GetComponentByClass(findClass));

	//見れないようにして、当たり判定を無効にする
	smc->SetVisibility(false);
	smc->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (Cast<ATerritoruCharacter>(OtherActor)) {
		//ブロックのマテリアルをプレイヤー用に変更
		smc->SetMaterial(0, _mats["Player"]);
		_breakChara = BreakChara::Player;
	}
	else if (Cast<AEnemy_Daring>(OtherActor)) {
		//
		if (Cast<AEnemy_Daring>(OtherActor)->ActorHasTag("enemy1")) {
			//ブロックのマテリアルをエネミー1用に変更
			smc->SetMaterial(0, _mats["Enemy_Daring"]);
			_breakChara = BreakChara::Enemy_Daring;
		}
		else if (Cast<AEnemy_Daring>(OtherActor)->ActorHasTag("enemy2")) {
			//ブロックのマテリアルをエネミー2用に変更
			smc->SetMaterial(0, _mats["dummy1"]);
			_breakChara = BreakChara::dummy1;
		}
		else if (Cast<AEnemy_Daring>(OtherActor)->ActorHasTag("enemy3")) {
			//ブロックのマテリアルをエネミー3用に変更
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