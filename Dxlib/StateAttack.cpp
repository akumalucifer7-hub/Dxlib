#include "StateAttack.h"
#include "StateIdle.h"
#include "StateRun.h"
#include "Player.h"
#include"Field.h"
#include"InputValidation.h"
#include"ICharacterState.h"

void StateAttack::OnEnter(Character& character, const GameContext& context)
{
    // 1. 近接攻撃フラグを立てて大剣を構えさせる
    character.SetIsMeleeAttacking(true);
        // 2. 指定された攻撃アニメーションを単発再生でセット
    character.SetAnimation(attackAnimState, false);
    // 3. 攻撃開始時の前進（踏み込み）速度を設定（コンボ段階に応じて調整可）
    stepForwardSpeed = 2.0f;
}

void StateAttack::Update(Character& character, const GameContext& context)
{
    // アニメーション再生率を取得（0.0f 〜 1.0f）
    float normTime = character.GetAnimState().GetNormalizedTime();

    if (stepForwardSpeed > 0.0f&& (normTime >= 0.3f && normTime <= 0.5f))
    {
        // 前方に踏み込む移動処理
        MATRIX rotMat = MGetRotY(character.GetAngle());
        VECTOR forwardVec = VTransform(VGet(0.0f, 0.0f, -1.0f), rotMat); // モデルの正面方向
        VECTOR moveOffset = VScale(forwardVec, stepForwardSpeed);

        VECTOR currentPos = character.GetPos();
        VECTOR finalPos = context.field->WallCollision(currentPos, moveOffset);
        currentPos.x = finalPos.x;
        currentPos.z = finalPos.z;
        character.SetPos(currentPos);

        // 徐々に減速
        //stepForwardSpeed -= 15.0f * deltaTime;
        if (stepForwardSpeed < 0.0f) stepForwardSpeed = 0.0f;
    }
    // アニメーションの30%〜80%の間で次の攻撃入力を受け付ける
    if (normTime >= 0.3f && normTime <= 0.8f)
    {
        if (context.input->IsMouseTrigger(InputValidation::Mouse::Left) ||
            context.input->IsButtonTrigger(XINPUT_BUTTON_X))
        {
            isNextComboRequested = true;
        }
    }

    //コンボ派生判定（アニメーション終盤で入力があれば次の段へ） 

    if (normTime > 0.9f && isNextComboRequested)
    {// 次のコンボ段階へ遷移
        int nextComboStep = comboStep + 1;
        Player::StateID nextAttackState = Player::StateID::Attack1;
        switch (nextComboStep)
        {
        case 2:
            nextAttackState = Player::StateID::Attack2;
            break;
        case 3:
            nextAttackState = Player::StateID::Attack3;
            break;
        case 4:
            nextAttackState = Player::StateID::Attack4;
            break;
        default:
            // コンボが最大段階に達した場合は Idle に戻す
            player.ChangeState(std::make_unique<StateIdle>(), context);
            return;
        }

        // 次の攻撃状態に遷移
        character.ChangeState(std::make_unique<StateAttack>(nextComboStep, nextAttackState), context);
    }
    else
    {
		// 攻撃アニメーションが終了したら Idle に戻す
		if (character.GetIsAnimationEnd())
		{
			character.ChangeState(std::make_unique<StateIdle>(), context);
		}
    }

}
void StateAttack::OnExit(Character & character, const GameContext & context)
{
        character.SetIsMeleeAttacking(false);
}