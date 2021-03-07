#pragma once   //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "game2.h"
#include <iostream>

const float PI = 3.14;
/*
pick in mouse callback, so until the mouse is realesed we are picking the same shape
rotate/translate in position callback and move shape "forward"/"backward" in scroll callback
*/
void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game2* scn = (Game2*)rndr->GetScene();
	if (action == GLFW_PRESS)
	{
		// set/clear the passStencil around here, so we get to the else part
		// after the else part set the stencil2 flag
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		rndr->updatePress(xpos, ypos);
		if (!rndr->Picking((int)xpos, (int)ypos)) {
			rndr->ClearDrawFlag(4, rndr->inAction2); // clear the flag so DrawAll draws the blend
			//rndr->SetDrawFlag(4, rndr->stencil2);
			//rndr->SetDrawFlag(4, rndr->passStencil);
		}
		else {
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
			}
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
			}
		}
		scn->clearPicks();
	}
	if (action == GLFW_RELEASE) {
		rndr->pickMany();
		rndr->SetDrawFlag(4, rndr->inAction2);
		/*else {
			rndr->SetDrawFlag(4, rndr->scaleAbit);
			rndr->ClearDrawFlag(4, rndr->passStencil);
		}*/

		scn->SetCounter();
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game2* scn = (Game2*)rndr->GetScene();
	double x2, y2;
	glfwGetCursorPos(window, &x2, &y2);
	scn->onScroll(yoffset, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	//scn->MyTranslate(glm::vec3(0, 0, xoffset), 0);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game2* scn = (Game2*)rndr->GetScene();
	scn->UpdatePosition(xpos, ypos);
	rndr->UpdatePosition(xpos, ypos);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { 
		rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
	}
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);

	rndr->Resize(width, height);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	Game2* scn = (Game2*)rndr->GetScene();

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_SPACE:
			scn->Add3DBezier();
			/*if (scn->IsActive())
				scn->Deactivate();
			else
				scn->Activate();*/
			break;

		case GLFW_KEY_DOWN:
			rndr->MoveCamera(0, scn->zTranslate, -0.4f);
			break;
		case GLFW_KEY_UP:
			rndr->MoveCamera(0, scn->zTranslate, 0.4f);
			break;
		case GLFW_KEY_LEFT:
			rndr->MoveCamera(0, scn->xRotate, 0.4f);
			break;
		case GLFW_KEY_RIGHT:
			rndr->MoveCamera(0, scn->xRotate, -0.4f);
			break;
		case GLFW_KEY_L:
			rndr->MoveCamera(0, scn->zRotate, 0.4f);
			break;
		case GLFW_KEY_R:
			rndr->MoveCamera(0, scn->zRotate, -0.4f);
			break;
		case GLFW_KEY_U:
			rndr->MoveCamera(0, scn->xTranslate, 0.4f);
			break;
		case GLFW_KEY_D:
			rndr->MoveCamera(0, scn->xTranslate, -0.4f);
			break;
		case GLFW_KEY_B:
			rndr->MoveCamera(0, scn->yTranslate, 0.4f);
			break;
		case GLFW_KEY_F:
			rndr->MoveCamera(0, scn->yTranslate, -0.4f);
			break;
		case GLFW_KEY_C:
			scn->ContinuityStateToggle();
			break;
		case GLFW_KEY_2:
			scn->RemakeBezier(2);
			break;
		case GLFW_KEY_3:
			scn->RemakeBezier(3);
			break;
		case GLFW_KEY_4:
			scn->RemakeBezier(4);
			break;
		case GLFW_KEY_5:
			scn->RemakeBezier(5);
			break;
		case GLFW_KEY_6:
			scn->RemakeBezier(6);
			break;
		case GLFW_KEY_P:
			scn->ContinuityStateToggle();
			break;
		default:
			break;
		}
	}
}

void Init(Display& display)
{
	display.AddKeyCallBack(key_callback);
	display.AddMouseCallBacks(mouse_callback, scroll_callback, cursor_position_callback);
	display.AddResizeCallBack(window_size_callback);
}
