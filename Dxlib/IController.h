#pragma once
#include "DxLib.h"

struct CommandState
{
    float moveX = 0.0f;     // 移動ベクトルX (-1.0 ~ 1.0)
    float moveY = 0.0f;     // 移動ベクトルY (-1.0 ~ 1.0)
    bool isAttack = false;  // 攻撃要求
    bool isEvade = false;   // 回避要求
    bool isJump = false;    // ジャンプ要求
	bool isSpecial = false; // 特殊行動要求
	bool isDamaged = false;  // 被弾要求
};

class IController
{
public:
    virtual ~IController() = default;
    virtual CommandState GetCommand(const Character& owner, const GameContext& context) const = 0;
};