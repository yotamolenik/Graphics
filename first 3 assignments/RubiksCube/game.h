#pragma once
#include "scene.h"
#include <iostream>
#include <queue>

class Game : public Scene
{
public:

	Game();
	void Init();
	void Update(const glm::mat4& MVP, const glm::mat4& Model, const int  shaderIndx);
	void AddOperation(int operation);
	void ReadOperation();

	void WhenRotate();
	void WhenTranslate();
	void Motion();

	void UpdateAnimationSpeed(int change);
	void toggleRotationDir();
	void addRotation(int faceDirection, int faceIndex);
	void rotateWall(int type, std::vector<int> indexs);
	void WhenPicked();

	unsigned int TextureDesine(int width, int height);
	~Game(void);

private:
	int cubeSize;
	int animSpeed;
	float currFrame;
	bool isRotateClockWise;
	std::vector<int> cubesIndexs;
};

