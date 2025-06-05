//--------------------------------------------------------------------
// ファイル名 ：DamagedComponent.cpp
// 概要       ：プレイヤー、敵がダメージを受けた際に使用するコンポーネント
// 作成者     ：0231本間
// 更新内容   ：03/24　作成
//--------------------------------------------------------------------

#include "DamagedComponent.h"
#include "Components/MeshComponent.h"
#include "CharacterBase.h"

UDamagedComponent::UDamagedComponent()
	:_damagedTimer(0.0)
	,_blinkingTimer(0.0)
	,_ownerMeshnum(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamagedComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//自身の"SMC"タグのついたスタティックメッシュコンポーネントを取得
	TSubclassOf<UStaticMeshComponent> findClass;
	findClass = UStaticMeshComponent::StaticClass();
	TArray<UActorComponent*> smcs = GetOwner()->GetComponentsByTag(findClass, "SMC");

	for (auto smc : smcs)
	{
		_smcs.Add(Cast<UStaticMeshComponent>(smc));
	}
}

void UDamagedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Cast<ACharacterBase>(GetOwner())->Get_bdead()) { return; }

	if (_isblinking) { BlinkingMat(DeltaTime); }
}

void UDamagedComponent::BlinkingMat(float deltaTime)
{
	_damagedTimer += deltaTime;

	_blinkingTimer += deltaTime;

	if (_blinkingTimer < 0.2) { return; }

	if (_isdisplay) {
		//自身の"SMC"タグのついたスタティックメッシュコンポーネントを取得
		TSubclassOf<UStaticMeshComponent> findClass;
		findClass = UStaticMeshComponent::StaticClass();
		TArray<UActorComponent*> smcs;

		//現在の図形に該当するモノのみ見れるようにする
		switch (Cast<ACharacterBase>(GetOwner())->Get_meshnum())
		{
		case ShapeNumber::Triangular:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Triangular");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		case ShapeNumber::Quadrangular:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Quadrangular");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		case ShapeNumber::Pentagonal:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Pentagonal");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		}

		_isdisplay = false;
	}
	else {
		for (auto smc : _smcs)
		{
			smc->SetVisibility(false);
		}

		_isdisplay = true;
	}

	_blinkingTimer = 0.0;

	if (_damagedTimer > 1.0)
	{
		_damagedTimer = 0.0;
		_isdisplay = false;
		_isblinking = false;

		//自身の"SMC"タグのついたスタティックメッシュコンポーネントを取得
		TSubclassOf<UStaticMeshComponent> findClass;
		findClass = UStaticMeshComponent::StaticClass();
		TArray<UActorComponent*> smcs;

		//現在の図形に該当するモノのみ見れるようにする
		switch (Cast<ACharacterBase>(GetOwner())->Get_meshnum())
		{
		case ShapeNumber::Triangular:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Triangular");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		case ShapeNumber::Quadrangular:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Quadrangular");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		case ShapeNumber::Pentagonal:
			smcs = Cast<ACharacterBase>(GetOwner())->GetComponentsByTag(findClass, "Pentagonal");

			for (auto smc : smcs)
			{
				Cast<UStaticMeshComponent>(smc)->SetVisibility(true);
			}
			break;
		}

		Cast<ACharacterBase>(GetOwner())->Offis_damageReaction();
	}
}