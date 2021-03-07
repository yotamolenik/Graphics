#pragma once
#include "Bezier1D.h"

class Bezier2D : public Shape
{
	int circularSubdivision; //usualy 4 how many subdivision in circular direction
	std::vector <glm::vec4> controlPoints;
	Bezier1D* bez1;
	int resT; 
	int resS;
	int subNum;
	void AddToModel(IndexedModel& model, float s, float t, const std::vector<glm::vec4> subSurf ,int subIndx);  // add a new Nurb to the model
	glm::vec4 CalcNurbs(float s, float t, const std::vector<glm::vec4> subSurf); 
	glm::vec3 CalcNormal(float s, float t, const std::vector<glm::vec4> subSurf);
	void CalcControlPoints(Bezier1D* c);  // calculates control points cubic Bezier manifold.

public:
	Bezier2D(Bezier1D* c,int _resS,int _resT,int mode, int viewport = 0);
	void UpdateBezier(Bezier1D* c);
	IndexedModel GetSurface();	//generates a model for MeshConstructor Constructor
	glm::vec4 GetPointOnSurface(int segmentS, int segmentT, int s, int t);  //returns a point on the surface in the requested segment for value of t and s
	glm::vec3 GetNormal(int segmentS, int segmentT, int s, int t); //returns a normal of a point on the surface in the requested segment for value of t and s
	inline int GetSegmentsNum() const { return controlPoints.size() / 16; } // divide by 16 to get segmentsNum
	//void MoveControlPoint(int segmentS, int segmentT, int indx, bool preserveC1);
	~Bezier2D(void);
	//void MyTranslate(const glm::vec3 delta, int mode);
	//glm::mat4 MakeTrans();
	glm::vec4 pos; // position of the center of the object. starting in (0,0,0)
	//void rotateBezier(float xoffset, float yoffset);
	//void translateBezier(float xoffset, float yoffset);
	
};