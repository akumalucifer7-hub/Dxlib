#include "Player.h"
#include "StateIdle.h"
#include "PlayerController.h"

void Player::Init(const GameContext& context)
{
    modelHandle = MV1LoadModel("Assets/3Dmodel/natsuki_karin_mmd_vrm/夏色花梨.pmx");
    centerFrameIndex = MV1SearchFrame(modelHandle, "センター");

    SwordItems.InitWeapon("Assets/3Dmodel/weapons/匙式大直剣/匙式大直剣.pmx", "グリップ1");
    GunItemsRight.InitWeapon("Assets/3Dmodel/weapons/DesertEagle_MMD/DesertEagle.pmx", "全ての親");
    GunItemsLeft.InitWeapon("Assets/3Dmodel/weapons/DesertEagle_MMD/DesertEagle.pmx", "全ての親");

    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
    pos = VGet(0.0f, 26.0f, -320.0f);
    GunItemsLeft.SetShapes("ステンレス", 1.0f);

    // -------------------------------------------------------------
    // 当たり判定 (Collision) の初期化
    // -------------------------------------------------------------
    // 1. 被弾用 AABB (Hurtbox) のローカルサイズ設定
    collision.SetLocalHurtbox({
        VGet(-3.0f, 0.0f, -2.5f), // 最小座標 (Min)
        VGet(3.0f, 20.0f, 2.5f)   // 最大座標 (Max)
        });

    // 2. 攻撃用 AABB (Hitbox) テーブルの登録
    // Attack1 (アニメーションID = static_cast<int>(CharacterStateID::Attack1))
    collision.RegisterAttackHitbox(
        static_cast<int>(CharacterStateID::Attack1),
        {
            0.2f, 0.5f, // 発生時間 (開始 20% ? 終了 50%)
            { VGet(-20.0f, 0.0f, 0.0f), VGet(20.0f, 20.0f, 30.0f) } // ローカル AABB 範囲
        }
    );

    // Attack2 (コンボ2段目)
    collision.RegisterAttackHitbox(
        static_cast<int>(CharacterStateID::Attack2),
        {
            0.2f, 0.6f,
            { VGet(-25.0f, 0.0f, 0.0f), VGet(25.0f, 25.0f, 35.0f) }
        }
    );

    // -------------------------------------------------------------
    // コントローラー・ステート初期化
    // -------------------------------------------------------------
    controller = std::make_unique<PlayerController>();
    ChangeState(std::make_unique<StateIdle>(), context);

    if (context.animator)
    {
        context.animator->initAnimator(modelHandle, animstate, static_cast<int>(CharacterStateID::Idle));
    }
}

void Player::Update(const GameContext& context)
{

    deltaTime = CalculateDeltaTime();

    // 基底クラス（Character）の物理計算・ステート更新・アニメーション処理を実行
    Character::Update(context);

    // フレーム位置の補正と武器のアタッチ
    Character::FrameRotate();
    AttatchItems();

}

void Player::OnTakeDamage(const GameContext& context)
{
    
}

void Player::AttatchItems()
{
    if (isMeleeAttacking)
    {
        SwordItems.AttachWeapon(modelHandle, "右中指１", 0.2f, -0.4f, 0.0f, 0.0f, 0.0f, -90.0f);
    }
    else
    {
        SwordItems.AttachWeapon(modelHandle, "上半身", -4.0f, 6.5f, 1.5f, -135.0f, 90.0f, 0.0f);
        GunItemsRight.AttachWeapon(modelHandle, "右中指１", 0.3f, -0.3f, -0.15f, 0.0f, 90.0f, -90.0f);
        GunItemsLeft.AttachWeapon(modelHandle, "左中指１", -0.3f, -0.3f, -0.15f, 0.0f, -90.0f, 90.0f);
    }
}



void Player::Draw()
{
    // 基底クラスのモデル描画
    Character::Draw();

    // 武器の描画

    SwordItems.DrawWeapon();
    if (!isMeleeAttacking)
    {
        GunItemsRight.DrawWeapon();
        GunItemsLeft.DrawWeapon();
    }
}

void Player::DrawDebug() 
{
    // 当たり判定のデバッグ枠を描画
    collision.DrawDebug(*this);
    DrawFormatString(10, 500, GetColor(255, 255, 255), "AnimID: %d, AttachIndex: %d", currentAnimID, animstate.AttachIndex);
}