#include "MovableGLM.h"
#include <stdio.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"


static void printMat(const glm::mat4 mat)
{
	printf(" matrix: \n");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			printf("%f ", mat[j][i]);
		printf("\n");
	}
}

MovableGLM::MovableGLM()
{
	ZeroTrans();
}

glm::mat4 MovableGLM::MakeTrans(const glm::mat4 &prevTransformations) const
{
	return prevTransformations * MakeTrans();
}

glm::mat4 MovableGLM::MakeTrans() const
{ // ps of 26/11 says do rot on the right in order to rotate around the center
	return scl * trans * rot;
}

void MovableGLM::MyTranslate(const glm::vec3 delta,int mode)
{
	//the first line is from the ps of 10/12. the 2nd one was there before 
	//make something move in the left/right of the scene instead of the left/right of itself
	// *SPECIFICALLY for rotation,  transpose(rot) = rot^-1
	//trans = glm::translate(trans,glm::vec3(glm::transpose(rot) * glm::vec4(delta,0)));
	trans = glm::translate(trans, delta);
}

void  MovableGLM::MyRotate(float angle,const glm::vec3 &vec,int mode)
{
	if (mode == 0) // multiplying by rot works thanks to trigo
		rot = glm::rotate(glm::mat4(1), angle, vec) * rot;
	else
		rot = glm::rotate(rot,angle,glm::vec3(glm::transpose(rot)*glm::vec4(vec,0)));
}
	
void  MovableGLM::MyScale(const glm::vec3 scale)
{
	scl = glm::scale(scl,scale);
}

void MovableGLM::ZeroTrans()
{
	trans = glm::mat4(1);
	rot = glm::mat4(1);
	scl = glm::mat4(1);
}
