//--------------------------------------------------------------------
// �t�@�C���� �FStageManager.h
// �T�v       �F�X�e�[�W�ɔz�u�����u���b�N�̐������̑����Ǘ�
// �쐬��     �F0231�{��
// �X�V���e   �F03/07�@�쐬
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
	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�u���b�N������������
	void CreateBlocks();

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�J���������A������
	void CreateCamera();

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F�S�Ẵu���b�N���`�F�b�N���A�Y������
	void CheckBlocks(float _deltaTime);

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�Q�[���I���^�C�~���O�̏���
	void GameTimeEnd();

private:
	//�u���b�N�̃A�N�^�[BP
	UPROPERTY(EditAnywhere, Category = "Block")
	TSubclassOf<AActor> _block;

	//�v���C���[�ɒǏ]����J����
	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<APlayerFocusCamera> _camera;

	UPROPERTY(EditAnywhere, Category = "ResultsUI")
	TSubclassOf<UUserWidget> _ui_results;

	UPROPERTY()
	UUserWidget* _pui_results = nullptr;

	//�S�Ẵu���b�N���Ǘ�
	UPROPERTY()
	TArray<ABlock*> _blocks;

	FResults _results;

	UPROPERTY(EditAnywhere, Category = "Global")
	double _globalradius;

	double _gametimer; //�Q�[���J�n����̎��Ԍv��

	const double _endtime = 90.0;			//�C���Q�[���I���܂ł̎��� : 90.0
	const double _blockvisiblytime = 3.0;	//�u���b�N���j�󂳂�ĕ�������܂ł̎��� : 3.0

	const int _blockmaxhp = 3;				//��������u���b�N�̍ő�̗� : 3

	UPROPERTY(EditAnywhere, Category = "IngameJudge")
	bool _bingame;

	bool _bstart = false;
};
