#pragma once
#include "scene.h"
#include "Bezier1D.h"
#include "Bezier2D.h"

class Game2 : public Scene
{
public:
	
	Game2();
	void Init();
	void Add3DBezier();
	void AddControlPoint(int indx);
	void MoveControlPoint(int indx, float x, float y);
	void MoveControlPoint(int segment, int indx, float x, float y);
	void RelocateControlPoint(int segment, int indx);
	void HideControlPoint(int indx);
	void Update(const glm::mat4& View, const glm::mat4& Projection, const glm::mat4 &Model,const int  shaderIndx);
	
	void WhenRotate(glm::mat4 View);
	void WhenTranslate(glm::mat4 View);
	void Motion();
	
	void onScroll(int yoffset, bool isPressed); // added a function to move the shape when scrolling

	unsigned int TextureDesine(int width, int height);
	~Game2(void);
	inline void ResetCounter() { tmp = counter; counter = 0; }
	inline void SetCounter() { counter = tmp; }

	void UpdatePosition( float xpos, float ypos);
	void Update3DBezier();
	void RemakeBezier(int segNum);
	void FixControlPoints();
	void ContinuityStateToggle();
	void MoveFixToView(glm::mat4 View, glm::vec4 move, bool isTransform);
	void movePlane(double x, double y);
private:
	unsigned int counter;
	unsigned int tmp;
	float x, y;
	float xprev, yprev;
	bool isContinuityState;
	int pointsStartIndx;
	int pps;
	int numOfBeziers;
	float pointsScale;
	float curveScale;
	Bezier1D* bez;
	Bezier2D* bez2;
	Bezier2D* bez22;
};

