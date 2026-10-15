#pragma once

class Character;
struct GameContext;

class ICharacterState
{
public:
    virtual ~ICharacterState() = default;
    virtual void OnEnter(Character& owner, const GameContext& context) = 0;
    virtual void Update(Character& owner, const GameContext& context) = 0;
    virtual void OnExit(Character& owner, const GameContext& context) = 0;
};