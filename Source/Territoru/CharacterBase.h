//--------------------------------------------------------------------
// �t�@�C���� �FCharacterBase.h
// �T�v       �F�v���C���[�A�G���p������L�����N�^�[��ՃN���X
// �쐬��     �F0231�{��
// �X�V���e   �F03/05�@�쐬
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

//�L�����̏��
UENUM(BlueprintType)
enum class CharaState : uint8
{
	State_Idle UMETA(DisplayName = "None"),
	State_Move UMETA(DisplayName = "Move"),
	State_Jump UMETA(DisplayName = "Jump"),
	State_Attack UMETA(DisplayName = "Attack")
};

//�}�`���
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
	//�ړ�����
	enum MoveDirection
	{
		Move_Left,
		Move_Right,
		Move_Up,
		Move_Down
	};

	//�U������
	enum AttackState
	{
		None,
		LeftDir,
		RightDir,
		UpDir,
		DownDir
	};

protected:
	//�ړ����v�Z�p�ϐ�
	struct ShapeValues
	{
		double _max_moveLength;	//�ő�ړ����x
		double _acceleration;	//������
		double _attenuation;	//������
		int _attackdamage;      //�U�����̗^����_���[�W��
	};

public:
	ACharacterBase();

	virtual void Tick(float DeltaTime) override {
		Super::Tick(DeltaTime);
	}

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override {
		Super::SetupPlayerInputComponent(PlayerInputComponent);
	}

	// ���@���FUPrimitiveComponent*, OverlappedComp, 
	// �@�@�@�@UPrimitiveComponent*, int32, bool, FHitResult
	// �߂�l�Fvoid
	// �������e�F�����蔻��
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

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�ړ�����
	virtual void Move();

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�U������
	virtual void AttackAction();

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�}�`�ύX����
	virtual void ShapeChange();

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F���z�֐���`
	virtual void InputMove() {};

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�ړ���������U���������Z�o
	virtual void InputAttack();

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F���z�֐���`
	virtual void InputShapeChange() {};

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F���z�֐���`
	virtual void InputTrapArrange() {};

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F�̗͉�
	void HealHP(float deltaTime);

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F���S��Ԃ��畜��
	void Respawn(float deltaTime);

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F���G���̏���
	void Invincible(float deltaTime);

	// ���@���F�t���[���b
	// �߂�l�Fvoid
	// �������e�F���z�֐���`
	virtual void MatColorReset(float DeltaTime) {};

	// ���@���FUStaticMeshComponent*
	// �߂�l�Fvoid
	// �������e�F���z�֐���`
	virtual void MatChange(UStaticMeshComponent* smc) {};

	// ���@���Ffloat
	// �߂�l�Fvoid
	// �������e�F���ݏ���Ă���u���b�N�����g���j�󂵂����̂Ȃ�X�e�[�^�X�㏸
	void DownLineTrace(float deltaTime);

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�X�^�����Ƀu���u���Ɛk����
	void StunAction();

private:
	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�F�R���X�g���N�^�ł̏���������
	void ConstInit();

	// ���@���F�Ȃ�
	// �߂�l�Fvoid
	// �������e�FBeginPlay()�ł̏���������
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

	//�ړ����Ƃɐi�s������ۊǂ���B
	// �A�^�b�N�L�[�������ꂽ���ɔ���
	MoveDirection _moveDir;
	
	//���݂̃L�����̍s��
	CharaState _status;
	
	//���݂̐}�`
	ShapeNumber _meshnum;
	
	//�A�^�b�N�L�[�����ꂽ���̍U����������p
	AttackState _atkstate;

	//�}�`���Ƃ̑��x��U���̒l�ۊǗp
	TMap<FName, ShapeValues> _valuesArray;

	//�ړ����v�Z�p�ϐ�
	ShapeValues _currentvalues;

	//���S���̃|�W�V����
	UPROPERTY()
	FVector _deadpos;

	const double _atkgeneralValue = 0.015;	//�U�����p�ėp�l

	double _attackmoveDistance;	//�U�����̈ړ��͈͑���p
	double _yawRotLength;		//��]�p�i��ʉ������j
	double _moveLength;			//�ړ����x
	double _globalradius;		//�X�e�[�W�}�l�[�W������擾�����X�e�[�W�̔��a

	const double _respawntime = 8.0;

	double _healingtimer;		//�̗͉񕜂̎��Ԍv��
	double _respawntimer;		//�|����Ă��畜������܂ł̎��Ԍv��
	double _invincibletimer;	//���G���̎��Ԍv��
	double _atkcooltimer;		//�U���̃N�[���^�C���p
	double _reactiontimer;		//�_���[�W���̌v��
	double _statusuptimer;		//�X�e�[�^�X�㏸���̌v��
	double _stuntimer;

	const int _max_hp = 15;			//�ő�̗�
	int _hp;					//�̗�
	int _deadnum;				//���S��
	int _killnum;				//�|������

	bool _bstart = false;		//�s���\��
	bool _bmove = false;		//�ړ��ł��邩
	bool _bdead = false;		//���S��Ԃ�
	bool _binvincible = false;	//���G��Ԃ�
	bool _bafteratk = false;	//�U���ォ
	bool _damageReaction = false;	//�_���[�W�󂯂���̖��G��Ԃ�
	bool _bstatusup = false;	//�X�e�[�^�X�㏸����
	bool _bstun = false;		//�X�^������
	bool _stunviberight = false;	//�X�^�����Atrue�Ȃ�E�����ɔ��ړ�
};
