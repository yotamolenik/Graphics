#include "renderer.h"
#include "GL/glew.h"
#include <array>
#include <iostream>


Renderer::Renderer()
{
	cameras.push_back(new Camera(60.0f, 1.0, 0.1f, 100.0f));

	xold = 0;
	yold = 0;
	
}

Renderer::Renderer(float angle, float relationWH, float near, float far)
{
	glLineWidth(5);
	cameras.push_back(new Camera(angle, relationWH, near, far));

	xold = 0;
	yold = 0;

}

void Renderer::Init(Scene* scene,  std::list<int>xViewport,  std::list<int>yViewport)
{
	scn = scene;
	MoveCamera(0, zTranslate, 10); //move the camera back so we see things
	glm::ivec4 viewport;
	glGetIntegerv(GL_VIEWPORT, &viewport[0]); // put in viewport the x, y, width and height of the viewport
	buffers.push_back(new DrawBuffer());

	if (xViewport.empty() && yViewport.empty())
	{
		//we never get here because xViewport is not empty
		//viewports.push_back(viewport);
		//drawInfo.push_back(new DrawInfo(0, 0, 0, 0, inAction | toClear | blackClear | depthTest | clearDepth | sceneTrans));
		//drawInfo.push_back(new DrawInfo(0, 0, 1, 0,  toClear | depthTest | clearDepth | sceneTrans));
	}
	else
	{
		xViewport.push_front(viewport.x);
		yViewport.push_front(viewport.y);
		xViewport.push_back(viewport.z);
		yViewport.push_back(viewport.w);
		std::list<int>::iterator xit = xViewport.begin();
		int index = 0;
		for (++xit; xit != xViewport.end(); ++xit)
		{
			std::list<int>::iterator yit = yViewport.begin();
			for (++yit; yit != yViewport.end(); ++yit)
			{
				viewports.push_back(glm::ivec4(*std::prev(xit), *std::prev(yit), *xit - *std::prev(xit), *yit - *std::prev(yit)));
				if (index < 1) // the difference between index = 0 and index = 1 is the sceneTrans flag. im not sure yet why 
				{
					// the inAction flag is used for picking. rndr->Picking() does ActionDraw()
					drawInfo.push_back(new DrawInfo(index, 0, 0, 0, index < 1 | inAction | depthTest | blackClear ));
					// drawing drawinfo
					drawInfo.push_back(new DrawInfo(index, 0, 1, 0, index < 1 | depthTest 
						| sceneTrans | stencilTest | passStencil | clearStencil));
					// blending drawinfo
					//drawInfo.push_back(new DrawInfo(index, 0, 2, 0, index < 1 | blend | stencilTest | passStencil));
					
				}
				else {
					drawInfo.push_back(new DrawInfo(index, 1, 0, 0, index < 1 | inAction | depthTest | stencilTest |
						passStencil | blackClear | clearStencil));
					drawInfo.push_back(new DrawInfo(index, 1, 1, 0, index < 1 | depthTest));
				}

				index++;
			 }
		}
	}

}

void Renderer::Draw(int infoIndx)
{
	DrawInfo info = *drawInfo[infoIndx];

	buffers[info.buffer]->Bind();
	glViewport(viewports[info.viewportIndx].x, viewports[info.viewportIndx].y, viewports[info.viewportIndx].z, viewports[info.viewportIndx].w);

	if (info.flags & scissorTest) {
		glEnable(GL_SCISSOR_TEST);
		int x = glm::min(xWhenPress, xold);
		int y = glm::min(viewports[info.viewportIndx].w - yWhenPress, viewports[info.viewportIndx].w - yold);
		glScissor(x, y, glm::abs(xWhenPress - xold), glm::abs(yWhenPress - yold));
	}
	else
		glDisable(GL_SCISSOR_TEST);

	if (info.flags & scaleAbit) {
		scn->scalePicked();
	}

	if (info.flags & stencilTest) {
		glEnable(GL_STENCIL_TEST);
		if (info.flags & passStencil) {
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		}
		else {
			if (info.flags & stencil2) {
				glStencilFunc(GL_EQUAL, 1, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
			}
			else {
				glStencilFunc(GL_EQUAL, 0, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
			}
		}
	}

	else
		glDisable(GL_STENCIL_TEST);

	if (info.flags & scaleAbit) {
		(*drawInfo[infoIndx]).flags &= ~scaleAbit;
		(*drawInfo[infoIndx]).flags &= ~stencil2;
		scn->unscalePicked();
	}

	if (info.flags & depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (info.flags & blend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	}
	else
		glDisable(GL_BLEND);

	glm::mat4 Projection = cameras[info.cameraIndx]->GetViewProjection();
	glm::mat4 View = glm::inverse(cameras[info.cameraIndx]->MakeTrans());

	if (info.flags & toClear)
	{
		if (info.flags & blackClear)
			Clear(0, 0, 0, 0);
		else
			Clear(1, 1, 1, 1);
	}
	scn->Draw(info.shaderIndx, View, Projection, info.viewportIndx, debugMode);

}

void Renderer::DrawAll()
{
	for (int i =   0; i < drawInfo.size(); i++)
	{
		// if all normal, draw normally
		if ( !(drawInfo[i]->flags & inAction) && !(drawInfo[i]->flags & inAction2)) {
			Draw(i);
		}
	}
}

bool Renderer::Picking(int x, int y)
{
	ActionDraw();
	GLint viewport[4];
	unsigned char data[4];
	glGetIntegerv(GL_VIEWPORT, viewport); //reading viewport parameters
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	
	return scn->Picking(data);
	//return depth;

}

void Renderer::MouseProccessing(int button)
{
	glm::mat4 View = glm::inverse(cameras[0]->MakeTrans());
	scn->MouseProccessing(View, button, xrel, yrel);
}

void Renderer::AddCamera(const glm::vec3& pos, float fov, float relationWH, float zNear, float zFar, int infoIndx)
{
	if (infoIndx >= 0 && infoIndx < drawInfo.size())
		drawInfo[infoIndx]->SetCamera(cameras.size());
	cameras.push_back(new Camera(fov, relationWH, zNear, zFar));
	cameras.back()->MyTranslate(pos, 0);
}

void Renderer::AddViewport(int left, int bottom, int width, int height)
{
	viewports.push_back(glm::ivec4(left, bottom, width, height));
	glViewport(left, bottom, width, height);

}

unsigned int Renderer::AddBuffer(int infoIndx, bool stencil)
{
	CopyDraw(infoIndx, buffers.size());

	DrawInfo* info = drawInfo.back();
	info->SetFlags(stencilTest | toClear | blackClear);
	//info->ClearFlags(depthTest);
	int width = viewports[info->viewportIndx].z, height = viewports[info->viewportIndx].w;

	unsigned int texId;

	texId = scn->AddTexture(width, height, 0, COLOR);
	if (stencil)
		scn->AddTexture(width, height, 0, STENCIL);
	else
		scn->AddTexture(width, height, 0, DEPTH);
	//scn->BindTexture(texId, info->buffer - 1);
	buffers.push_back(new DrawBuffer(width, height, stencil, texId + 1));

	return texId;
}

int Renderer::Create2Dmaterial(int texsNum)
{
	std::vector<unsigned int> texIds;
	std::vector<unsigned int> slots;
	for (size_t i = 0; i < texsNum; i++)
	{
		unsigned int texId = AddBuffer(1, true);
		texIds.push_back(texId);
		slots.push_back(i);
	}

	materialIndx2D = scn->AddMaterial((unsigned int*)& texIds[0], (unsigned int*)& slots[0], texsNum);
	return materialIndx2D;
}

void Renderer::AddDraw(int view, int camera, int shader, int buff, unsigned int flags)
{
	drawInfo.push_back(new DrawInfo(view, camera, shader, buff, flags));
}

void Renderer::CopyDraw(int infoIndx, int buff)
{
	DrawInfo* info = drawInfo[infoIndx];
	drawInfo.push_back(new DrawInfo(info->viewportIndx, info->cameraIndx, info->shaderIndx, buff, info->flags));
}

void Renderer::SetViewport(int left, int bottom, int width, int height, int indx)
{
	viewports[indx] = glm::ivec4(left, bottom, width, height);
	glViewport(left, bottom, width, height);
}

void Renderer::UpdatePosition(float xpos, float ypos)
{
	xrel = xold - xpos;
	yrel = yold - ypos;
	xold = xpos;
	yold = ypos;
}

void Renderer::updatePress(int xpos, int ypos)
{
	xWhenPress = xpos;
	yWhenPress = ypos;
	xold = xpos;
	yold = ypos;
}

void Renderer::Resize(int width, int height)
{
	//not working properly
	cameras[0]->SetProjection(cameras[0]->GetAngle(), (float)width / height);
	//glViewport(0, 0, width, height);
	viewports[0].z = width;
	viewports[0].w = height;
	//std::cout << cameras[0]->GetRelationWH() << std::endl;
}

void Renderer::MoveCamera(int cameraIndx, int type, float amt)
{
	switch (type)
	{
	case xTranslate:
		cameras[cameraIndx]->MyTranslate(glm::vec3(amt, 0, 0), 0);
		break;
	case yTranslate:
		cameras[cameraIndx]->MyTranslate(glm::vec3(0, amt, 0), 0);
		break;
	case zTranslate:
		cameras[cameraIndx]->MyTranslate(glm::vec3(0, 0, amt), 0);
		break;
	case xRotate:
		cameras[cameraIndx]->MyRotate(amt, glm::vec3(1, 0, 0), 0);
		break;
	case yRotate:
		cameras[cameraIndx]->MyRotate(amt, glm::vec3(0, 1, 0), 0);
		break;
	case zRotate:
		cameras[cameraIndx]->MyRotate(amt, glm::vec3(0, 0, 1), 0);
		break;
	default:
		break;
	}
}

bool Renderer::checkViewport(int x, int y, int viewportIndx)
{
	return (viewports[viewportIndx].x < x&& viewports[viewportIndx].y < y&& viewports[viewportIndx][2] + viewports[viewportIndx][0] > x&& viewports[viewportIndx][3] + viewports[viewportIndx][1] > y);
}

Renderer::~Renderer()
{
	for (Camera* cam : cameras)
	{
		delete cam;
	}

	for (DrawInfo* info : drawInfo)
	{
		delete info;
	}

	
}

bool Renderer::pickMany()
{
	int x = glm::min(xWhenPress, xold);
	int y = glm::min(viewports[2].w - yWhenPress, viewports[2].w - yold);
	glm::mat4 view = glm::inverse(cameras[0]->MakeTrans());
	return scn->pickMany(x, y, glm::abs(xWhenPress - xold), glm::abs(yWhenPress - yold), view);
}

void Renderer::Clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::ActionDraw()
{
	for (int i = 0; i < drawInfo.size(); i++)
	{
		if (drawInfo[i]->flags & inAction)
			Draw(i);
	}

}


void Renderer::SetScene(Scene* scene)
{
	scn = scene;
}




