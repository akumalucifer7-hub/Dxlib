// Villain.cpp
#include "Villain.h"
#include "StateIdle.h"
#include "AIController.h"
#include"Player.h"
Villain ::~Villain() = default;
void Villain::Init(const GameContext& context)
{
    // 旧モデルの削除
    if (modelHandle != -1)
    {
        MV1DeleteModel(modelHandle);
        modelHandle = -1;
    }

    //アニメーション状態のリセット
    animstate = Animstate();


    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
    modelHandle = MV1LoadModel("Assets/3Dmodel/koharu_rikka_natsufuku_mmd_vrm/KoharuRikka_Natsufuku.pmx");
    centerFrameIndex = MV1SearchFrame(modelHandle, "センター");

	pos = VGet(0.0f, 0.0f, 0.0f);
    weapon1.InitWeapon("Assets/3Dmodel/weapons/でっかいマチェット/でっかいマチェット.pmx", "センター");
    weapon2.InitWeapon("Assets/3Dmodel/weapons/でっかいマチェット/でっかいマチェット.pmx", "センター");
    Character::SetHP(maxHP); // 初期HPを設定

    
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
            { VGet(-5.0f, 0.0f, 0.0f), VGet(5.0f, 17.0f, -15.0f) }
        }
    );
	collision.RegisterAttackHitbox(
		static_cast<int>(CharacterStateID::Attack2),
		{
			0.3f, 0.7f,
			{ VGet(-5.0f, 0.0f, 0.0f), VGet(5.0f, 17.0f, -15.0f) }
		}
	);
    // AIコントローラー・ステート初期化
    controller = std::make_unique<AIController>();
    ChangeState(std::make_unique<StateIdle>(), context);

    if (context.animator)
    {
        context.animator->initAnimator(modelHandle, animstate, static_cast<int>(CharacterStateID::Idle));
    }
}

void Villain::Update(const GameContext& context)
{
	 
	MV1SetPosition(modelHandle, pos);

	Character::Update(context);
	weapon1.AttachWeapon(modelHandle, "右中指１", 0.0f, -0.3f, 0.0f, 0.0f, 90.0f, 0.0f);
	weapon2.AttachWeapon(modelHandle, "左中指１", 0.0f, -0.3f, 0.0f, 0.0f, 90.0f, 0.0f);

	Character::FrameRotate();
}

void Villain::OnTakeDamage(const GameContext& context)
{
    Character::OnTakeDamage(context);
	HP = HP - context.player->GetAttackPower();

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