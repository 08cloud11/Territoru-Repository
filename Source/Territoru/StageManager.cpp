//--------------------------------------------------------------------
// ファイル名 ：StageManager.cpp
// 概要       ：ステージに配置されるブロックの生成等の総括管理
// 作成者     ：0231本間
// 更新内容   ：03/07　作成
//--------------------------------------------------------------------

#include "StageManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TerritoruCharacter.h"
#include "CharacterBase.h"
#include "Blueprint/UserWidget.h"

AStageManager::AStageManager()
	: _gametimer(0.0)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStageManager::BeginPlay()
{
	Super::BeginPlay();
	
	CreateBlocks();

	CreateCamera();
}

void AStageManager::Tick(float DeltaTime)
{
	if (!_bstart) return;

	Super::Tick(DeltaTime);

	CheckBlocks(DeltaTime);

	//インゲームではない
	if (!_bingame) return;

	_gametimer += DeltaTime;
	if (_gametimer > _endtime)
	{
		GameTimeEnd();
	}
}

void AStageManager::Start()
{
	_bstart = true;

	TSubclassOf<ACharacterBase> findClass;
	findClass = ACharacterBase::StaticClass();
	TArray<AActor*> charas;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, charas);

	for (auto chara : charas)
	{
		Cast<ACharacterBase>(chara)->Start();
	}
}

void AStageManager::CreateBlocks()
{
	FTransform SpawnTr;

	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 90; i++)
		{
			FVector pos = FVector(FVector(_globalradius * cos(((i * 8.0) / 360.0) * PI),
				_globalradius * sin(((i * 8.0) / 360.0) * PI), -125.0 + (j * 120.0)));

			FRotator rot = FRotator(0.0, ((i * 8.0) / 360.0) * 180.0, 0.0);

			SpawnTr.SetLocation(pos);
			SpawnTr.SetRotation(UKismetMathLibrary::Conv_RotatorToQuaternion(rot));

			AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(_block, SpawnTr);
			Cast<ABlock>(SpawnActor)->Set_hp(_blockmaxhp);

			_blocks.Add(Cast<ABlock>(SpawnActor));
		}
	}
}

void AStageManager::CreateCamera()
{
	//カメラを生成
	AActor* SpawnCamera = GetWorld()->SpawnActor<AActor>(_camera);

	TSubclassOf<ATerritoruCharacter> findClass;
	findClass = ATerritoruCharacter::StaticClass();

	//プレイヤーを取得
	ACharacterBase* pplayer = Cast<ACharacterBase>(UGameplayStatics::GetActorOfClass(GetWorld(), findClass));

	//生成したカメラにステージの半径と追従するプレイヤーを渡す
	Cast<APlayerFocusCamera>(SpawnCamera)->SetFocusPlayer(_globalradius, pplayer);

	GetWorld()->GetFirstPlayerController()->AutoManageActiveCameraTarget(SpawnCamera);

	Cast<APlayerFocusCamera>(SpawnCamera)->SetCameraPosition(0.0);
}

void AStageManager::CheckBlocks(float _deltaTime)
{
	for (auto block : _blocks)
	{
		if (block->Get_hp() > 0) continue;
		
		block->Add_breakedTimer(_deltaTime);

		if (block->Get_breakedTimer() < _blockvisiblytime) continue;
		
		TSubclassOf<UStaticMeshComponent> findClass;
		findClass = UStaticMeshComponent::StaticClass();

		UActorComponent* smc = block->GetComponentByClass(findClass);
		Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
		Cast<UStaticMeshComponent>(smc)->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

		block->Set_breakedTimer(0.0);
		block->Set_hp(_blockmaxhp);
			
	}
}

void AStageManager::GameTimeEnd()
{
	for (auto block : _blocks)
	{
		switch (block->Get_breakChara())
		{
		case BreakChara::Player:
			_results.playernum++;
			break;

		case BreakChara::Enemy_Daring:
			_results.enemy1num++;
			break;

		case BreakChara::dummy1:
			_results.enemy2num++;
			break;

		case BreakChara::dummy2:
			_results.enemy3num++;
			break;
		}
	}

	//ブロックのスタティックメッシュコンポーネントを取得
	TSubclassOf<ACharacterBase> findClass;
	findClass = ACharacterBase::StaticClass();
	TArray<TObjectPtr<AActor>> charas{ nullptr };
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), findClass, charas);

	for (auto chara : charas)
	{
		if (Cast<ACharacterBase>(chara)->ActorHasTag("enemy1")) {
			_results.enemy1num = _results.enemy1num * (1 + Cast<ACharacterBase>(chara)->Get_killnum());
		}
		else if (Cast<ACharacterBase>(chara)->ActorHasTag("enemy2")) {
			_results.enemy2num = _results.enemy2num * (1 + Cast<ACharacterBase>(chara)->Get_killnum());
		}
		else if (Cast<ACharacterBase>(chara)->ActorHasTag("enemy3")) {
			_results.enemy3num = _results.enemy3num * (1 + Cast<ACharacterBase>(chara)->Get_killnum());
		}
		else {
			if (Cast<ACharacterBase>(chara)->Get_deadnum() != 0) {
				_results.playernum = _results.playernum / Cast<ACharacterBase>(chara)->Get_deadnum() * 5;
			}

			if (Cast<ACharacterBase>(chara)->Get_killnum() == 0) {
				_results.playernum -= 50;
			}
		}
	}

	_pui_results = CreateWidget<UUserWidget>(GetWorld(), _ui_results);

	if (_pui_results)
	{
		_pui_results->AddToViewport();
	}

	//ゲームを一時停止
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}