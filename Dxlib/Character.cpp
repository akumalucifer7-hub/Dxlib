#include "Character.h"
#include "ICharacterState.h"
#include "GameContext.h"
#include"Field.h"

void Character::Update(const GameContext& context)
{
     deltaTime = CalculateDeltaTime();

    // 1. 物理計算（接地・重力）
    UpdatePhysics(context);

    // 2. 現在のステート更新
    if (currentState)
    {
        currentState->Update(*this, context);
    }

    // 3. 行列更新
    UpdateTransform();

    // 4. アニメーション更新
    context.animator->updateAnimator(
        modelHandle,
        animstate,
        currentAnimID,
        isAnimLoop,
        deltaTime
    );
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
    if (context.field->CheckGround(pos, groundY))
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

void Character::Draw()
{
    if (modelHandle != -1)
    {
        MV1DrawModel(modelHandle);
    }
}

void Character::AttatchItems()
{
	// デフォルトでは何もしない
}
void Character::FrameLotate()
{
	// デフォルトでは何もしない
}