#pragma once
#pragma once
#include <vector>
#include "GameObject.h"
#include "Player.h"
#include "Villain.h"
#include "Field.h"
#include "Camera.h"
#include"InputValidation.h"
#include"GameContext.h"
#include "Animator.h"
class GameScene
{
public:
	void InitGame(int selectMode, int SCREENWIDTH, int SCREENHEIGHT);

	void UpdateGame(InputValidation& input, int SCREENWIDTH, int SCREENHEIGHT);
	void DrawGame();

private:
	std::vector<GameObject*> gameObjects;
	GameContext CreateContext();
	GameContext context;
	int StartTime;
	bool SetHighMode = false;
	Camera camera;
	Player player;
	Field field;
	Villain villain;
	InputValidation inputValidation;
	Animator animator;
	int ShadowMapHandle;
	VECTOR lightDir;
	bool isDebugDrawMode;
};