#include "StateIdle.h"
#include"StateFall.h"
#include "StateJump.h"
#include"StateAttack.h"
#include "StateRun.h"
#include"StateEvade.h"
#include "Player.h"

void StateIdle::OnEnter(Player& player, const GameContext& context)
{
    // Idleアニメーションの再生設定など
    player.SetAnimation(Player::State::Idle, true);
    // StateIdle自身が、Idleアニメーション(Player::State::Idle)を再生するようAnimatorに指示する
    
}

void StateIdle::Update(Player& player, const GameContext& context)
{
    // --- 1. 落下判定 ---
    // 足場から落ちた場合など、接地しておらずY軸速度がマイナスになったら落下状態へ
    if (!player.GetIsGround() && player.GetVelocityY() < 0.0f)
    {
        player.ChangeState(std::make_unique<StateFall>(), context);
        return;
    }

    // --- 2. ジャンプ判定 ---
    if (context.input->IsKeyTrigger(KEY_INPUT_SPACE) && player.GetIsGround())
    {
        // Jump状態へ遷移
        player.ChangeState(std::make_unique<StateJump>(), context);
        return;
    }

    // --- 3. 回避判定 ---
    
    if (context.input->IsKeyTrigger(KEY_INPUT_E))
    {
        player.ChangeState(std::make_unique<StateEvade>(), context);
        return;
    }
    

    // --- 4. 移動判定 ---
    float lx = 0.0f;
    float ly = 0.0f;
    context.input->GetLeftStick(lx, ly, 0.2f);
    bool isMoveKeyPressed =
        context.input->IsKeyPressed(KEY_INPUT_W) || context.input->IsKeyPressed(KEY_INPUT_UP) ||
        context.input->IsKeyPressed(KEY_INPUT_S) || context.input->IsKeyPressed(KEY_INPUT_DOWN) ||
        context.input->IsKeyPressed(KEY_INPUT_A) || context.input->IsKeyPressed(KEY_INPUT_LEFT) ||
        context.input->IsKeyPressed(KEY_INPUT_D) || context.input->IsKeyPressed(KEY_INPUT_RIGHT);

    if (isMoveKeyPressed || (lx != 0.0f || ly != 0.0f))
    {
        if (context.input->IsKeyPressed(KEY_INPUT_LSHIFT))
        {
            //player.ChangeState(std::make_unique<StateWalk>(), context);
        }
        else
        {
            player.ChangeState(std::make_unique<StateRun>(), context);
        }
        return;
    }

    // --- 5. 攻撃判定 ---
    if (context.input->IsMouseTrigger(InputValidation::Mouse::Left) || context.input->IsButtonTrigger(XINPUT_BUTTON_X))
    {

        player.ChangeState(std::make_unique<StateAttack>(), context);
        return;
    }
}

void StateIdle::OnExit(Player& player, const GameContext& context)
{
    // Idle状態終了時の処理（もしあれば）
}