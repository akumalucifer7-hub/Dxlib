#pragma once
#include "IController.h"
#include"Character.h"


class PlayerController : public IController
{
public:
    CommandState GetCommand(const Character& owner, const GameContext& context) override;
};