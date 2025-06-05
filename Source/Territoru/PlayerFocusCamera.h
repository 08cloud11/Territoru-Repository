//--------------------------------------------------------------------
// ファイル名 ：PlayerFocusCamera.h
// 概要       ：プレイヤーを追従するカメラ
// 作成者     ：0231本間
// 更新内容   ：05/18　作成
//--------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "CharacterBase.h"
#include "PlayerFocusCamera.generated.h"

UCLASS()
class TERRITORU_API APlayerFocusCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:
	APlayerFocusCamera();

	virtual void Tick(float DeltaTime) override;

	// 引　数：double
	// 戻り値：void
	// 処理内容：自身のポジションと回転を設定
	void SetCameraPosition(double);

	// 引　数：double
	// 戻り値：void
	// 処理内容：自身のポジションと回転を設定
	void SetUpwardLocation();

	// 引　数：double, ACharacterBase*
	// 戻り値：void
	// 処理内容：共通の半径と追跡するプレイヤーを設定
	void SetFocusPlayer(double val, ACharacterBase* focusplayer) {
		_globalradius = val; 
		_player = focusplayer;
	}

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> _player;

	UPROPERTY(EditAnywhere, Category = "AddRadiusValue")
	double _addradiusvalue;

	double _globalradius;

	double _afterfocuslength;

	bool _bfocus;
};
