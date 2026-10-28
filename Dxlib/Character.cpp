#include "Character.h"
#include "ICharacterState.h"
#include "StateDamage.h"
#include "GameContext.h"
#include "Field.h"
#include"Player.h"
#include"Villain.h"
Character::Character() = default;
Character::~Character() = default; // ここでデフォルトデストラクタを生成[cite: 17]

// --- 当たり判定関数の共通実装 ---

AABB Character::GetHurtboxAABB() const
{
    return collision.GetHurtboxAABB(*this);
}

bool Character::GetCurrentAttackAABB(AABB& outAABB) const
{
    return collision.GetCurrentAttackAABB(*this, outAABB);
}

void Character::DrawDebug()
{
    collision.DrawDebug(*this);
}

void Character::OnTakeDamage(const GameContext& context)
{
    if (!isInvincible )
    {
        ChangeState(std::make_unique<StateDamage>(), context);
    }
}
// --- 攻撃関連の共通実装 ---
Character::CharacterStateID Character::GetAttackAnimID(int comboStep) const
{
    switch (comboStep)
    {
    case 1: return CharacterStateID::Attack1;
    case 2: return CharacterStateID::Attack2;

    default: return CharacterStateID::Attack1;
    }
}
// --- その他の共通更新処理 ---

void Character::Update(const GameContext& context)
{


    if (currentState)
    {
        currentState->Update(*this, context);
    }

    UpdateTransform();

    if (modelHandle != -1 && context.animator)
    {
        context.animator->updateAnimator(
            modelHandle,
            animstate,
            currentAnimID,
            isAnimLoop,
            context.deltaTime
        );
    }
    UpdatePhysics(context);
    // プレイヤー vs 敵の衝突・パリィ・ダメージを一括解決
    if (context.player && context.villain)
    {
        CharacterCollision::ResolveCharacterCollisions(*context.player, *context.villain, context);
    }

}

void Character::ChangeState(std::unique_ptr<ICharacterState> newState, const GameContext& context)
{
    if (currentState)
    {
        currentState->OnExit(*this, context);
    }

    currentState = std::move(newState); 

    if (currentState)
    {
        currentState->OnEnter(*this, context);
    }
}

void Character::UpdatePhysics(const GameContext& context)
{
    float groundY = -9999.0f;
    if (context.field && context.field->CheckGround(pos, groundY))
    {
        isGround = true;
    }
    else
    {
        velocityY += gravity;
        isGround = false;
    }

    pos.y += velocityY;

    if (pos.y <= groundY || (isGround && pos.y <= groundY + 3.0f))
    {
        pos.y = groundY;
        velocityY = 0.0f;
        isGround = true;
    }
}

void Character::UpdateTransform()
{
    MATRIX rotMat = MGetRotY(angle);
    MATRIX transMat = MGetTranslate(pos);
    MV1SetMatrix(modelHandle, MMult(rotMat, transMat));
}

void Character::FrameRotate()
{
    MV1ResetFrameUserLocalMatrix(modelHandle, centerFrameIndex);
    MATRIX rootMatrix = MV1GetFrameLocalMatrix(modelHandle, centerFrameIndex);
    rootMatrix.m[3][0] = 0.0f;
    rootMatrix.m[3][2] = 0.0f;
    MV1SetFrameUserLocalMatrix(modelHandle, centerFrameIndex, rootMatrix);
}

void Character::AttatchItems()
{
    // アイテムの装備ロジック
}

void Character::Draw()
{
    if (modelHandle != -1)
    {
        MV1DrawModel(modelHandle);
    }
}