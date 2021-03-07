#pragma once
#include "scene.h"
#include <Rays\sceneParser.h>

class Rays : public Scene
{
public:
	
	Rays();
	void Init();
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);

	void updateP(float change);

	void updateColorCounter(bool divide);

	void updatePixelWidth(float change);
	
	void WhenRotate();
	void trySelect(bool clicked);
	void WhenTranslate();
	void Motion();
	
	unsigned int TextureDesine(int width, int height, SceneData* scene);
	~Rays(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }

	void UpdatePosition( float xpos, float ypos);
private:
	int selected;
	unsigned int counter;
	unsigned int tmp;
	float x, y;
	float prevX, prevY;
	float xOffset, yOffset, zOffset;
	float p, z;
	glm::vec4 eye; 
	glm::vec4 ambient;
	glm::ivec4 sizes;
	std::vector<glm::vec4> objects;
	std::vector<glm::vec4> lights;
	std::vector<glm::vec4> directions;
	std::vector<glm::vec4> colors;
	std::vector<glm::vec4> intensities;
};

