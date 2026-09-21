#pragma once
#include "DxLib.h"

// 前方宣言
class Player;
struct GameContext;

class IPlayerState
{
public:
    virtual ~IPlayerState() = default;

    // 状態に入った瞬間の処理（アニメーションの初期化など）
    virtual void OnEnter(Player& player, const GameContext& context) = 0;

    // 毎フレームの更新処理（入力判定や移動など）
    virtual void Update(Player& player, const GameContext& context) = 0;

    // 状態から抜ける瞬間の処理（フラグのリセットなど）
    virtual void OnExit(Player& player, const GameContext& context) = 0;
};