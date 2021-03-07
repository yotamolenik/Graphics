#ifndef CAMERA_INCLUDED_H
#define CAMERA_INCLUDED_H
//#include <glm/glm.hpp>
#include "MovableGLM.h"

class Camera : public MovableGLM
{
public:
	Camera( float fov,float relathionWH, float zNear, float zFar);

	inline glm::mat4 GetViewProjection() const	{
		return projection;}	

	inline float GetAngle(){
		return fov;}

	inline float GetNear(){
		return near;}

	inline float GetFar(){
		return far;}

	inline float GetRelationWH(){
		return relationWH;}
	
	void SetProjection(float fov, float relationWH);
protected:
private:
	glm::mat4 projection;
	float fov, relationWH;
	float far,near;
};

#endif
