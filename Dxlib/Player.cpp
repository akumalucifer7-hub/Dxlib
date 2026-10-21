#include "Player.h"
#include "StateIdle.h"

#define _HAS_STD_BYTE 0

void Player::Init(const GameContext& context)
{
    // モデルおよびボーン枠の初期化
    modelHandle = MV1LoadModel("Assets/3Dmodel/natsuki_karin_mmd_vrm/夏色花梨.pmx");
    centerFrameIndex = MV1SearchFrame(modelHandle, "センター");

    // 武器の初期化
    SwordItems.InitWeapon("Assets/3Dmodel/weapons/匙式大直剣/匙式大直剣.pmx", "グリップ1");
    GunItemsRight.InitWeapon("Assets/3Dmodel/weapons/DesertEagle_MMD/DesertEagle.pmx", "全ての親");
    GunItemsLeft.InitWeapon("Assets/3Dmodel/weapons/DesertEagle_MMD/DesertEagle.pmx", "全ての親");

    MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
    pos = VGet(0.0f, 26.0f, -320.0f);
    GunItemsLeft.SetShapes("ステンレス", 1.0f);

    // 当たり判定クラスの初期化
    playercollision.Init();

    // 初期ステート（StateIdle）の設定
    ChangeState(std::make_unique<StateIdle>(), context);
    context.animator->initAnimator(modelHandle, animstate, static_cast<int>(StateID::Idle));
}

void Player::Update(const GameContext& context)
{

    deltaTime = CalculateDeltaTime();

    // 基底クラス（Character）の物理計算・ステート更新・アニメーション処理を実行
    Character::Update(context);

    // フレーム位置の補正と武器のアタッチ
    FrameLotate();
    AttatchItems();

    // 当たり判定・押し出し処理の更新
    playercollision.Update(*this, context, deltaTime);
}

void Player::OnTakeDamage()
{
    // 無敵時間中でなければ被弾ステート（StateDamage）へ遷移
    if (!IsInvincible && currentAnimID != static_cast<int>(StateID::Damage1))
    {
        // ChangeState(std::make_unique<StateDamage>(), context);
    }
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

void Player::FrameLotate()
{
    MV1ResetFrameUserLocalMatrix(modelHandle, centerFrameIndex);
    MATRIX rootMatrix = MV1GetFrameLocalMatrix(modelHandle, centerFrameIndex);
    rootMatrix.m[3][0] = 0.0f;
    rootMatrix.m[3][2] = 0.0f;
    MV1SetFrameUserLocalMatrix(modelHandle, centerFrameIndex, rootMatrix);
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

void Player::DrawDebug() const
{
    // 当たり判定のデバッグ枠を描画
    playercollision.DrawDebug(*this);
    DrawFormatString(10, 500, GetColor(255, 255, 255), "AnimID: %d, AttachIndex: %d", currentAnimID, animstate.AttachIndex);
}