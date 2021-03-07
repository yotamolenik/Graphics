#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

	
	Camera::Camera(float fov,float relationWH, float zNear, float zFar)
	{
		if(fov > 0) //prerspective
		{
			this->projection =  glm::perspective(fov,relationWH , zNear, zFar);
		}
		else  //ortho
		{
			this->projection = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,zNear,zFar);
		}
		this->near = zNear;
		this->far = zFar;
		this->fov = fov;
		this->relationWH = relationWH;
}

	void Camera::SetProjection(float fov, float relationWH)
	{
		if( this->fov>0)
		{
			this->projection = glm::perspective(fov,relationWH , near, far);	
			this->fov = fov;
		}
		this->relationWH = relationWH;
	}

