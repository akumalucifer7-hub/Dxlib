// Villain.cpp
#include "Villain.h"
#include "StateIdle.h"
#include "AIController.h"

void Villain::Init(const GameContext& context)
{
    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
    modelHandle = MV1LoadModel("Assets/3Dmodel/koharu_rikka_natsufuku_mmd_vrm/KoharuRikka_Natsufuku.pmx");
    centerFrameIndex = MV1SearchFrame(modelHandle, "センター");

    weapon1.InitWeapon("Assets/3Dmodel/weapons/でっかいマチェット/でっかいマチェット.pmx", "センター");
    weapon2.InitWeapon("Assets/3Dmodel/weapons/でっかいマチェット/でっかいマチェット.pmx", "センター");

    // -------------------------------------------------------------
    // 当たり判定 (Collision) の初期化
    // -------------------------------------------------------------
    // 1. 被弾用 AABB (Hurtbox) 設定
    collision.SetLocalHurtbox({
        VGet(-3.5f, 0.0f, -3.0f),
        VGet(3.5f, 18.0f, 3.0f)
        });

    // 2. 敵固有の攻撃 AABB (Hitbox) 登録
    collision.RegisterAttackHitbox(
        static_cast<int>(CharacterStateID::Attack1),
        {
            0.3f, 0.7f, // 発生タイミング
            { VGet(-15.0f, 0.0f, 0.0f), VGet(15.0f, 20.0f, 25.0f) }
        }
    );

    // -------------------------------------------------------------
    // AIコントローラー・ステート初期化
    // -------------------------------------------------------------
    controller = std::make_unique<AIController>(30.0f, 1.5f);
    ChangeState(std::make_unique<StateIdle>(), context);

    if (context.animator)
    {
        context.animator->initAnimator(modelHandle, animstate, static_cast<int>(CharacterStateID::Idle));
    }
}

void Villain::Update(const GameContext& context)
{
	 deltaTime = CalculateDeltaTime();

	MV1SetPosition(modelHandle, pos);

	Character::Update(context);
	weapon1.AttachWeapon(modelHandle, "右中指１", 0.0f, -0.3f, 0.0f, 0.0f, 90.0f, 0.0f);
	weapon2.AttachWeapon(modelHandle, "左中指１", 0.0f, -0.3f, 0.0f, 0.0f, 90.0f, 0.0f);

	Character::FrameRotate();
}

void Villain::OnTakeDamage(const GameContext& context)
{
	
}


void Villain::Draw()
{
	if (modelHandle != -1)
	{
		MV1DrawModel(modelHandle);
	}
	else
	{
		DrawString(260, 300, "Model Load Failed", GetColor(255, 0, 0));
	}
	weapon1.DrawWeapon();
	weapon2.DrawWeapon();
}

void Villain::DrawDebug()
{
	collision.DrawDebug(*this);
}