#pragma once   //maybe should be static class
#include "display.h"
#include "renderer.h"
#include "game.h"
#include <iostream>
#include <random>


	void mouse_callback(GLFWwindow* window,int button, int action, int mods)
	{	
		
		if(action == GLFW_PRESS )
		{
			Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
			Game* scn = (Game*)rndr->GetScene();
			double x2,y2;
			glfwGetCursorPos(window,&x2,&y2);
			if (rndr->Picking((int)x2, (int)y2)) {
				rndr->UpdatePosition(x2, y2);
			}
		}
	}
	
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Game* scn = (Game*)rndr->GetScene();

		scn->MyTranslate(glm::vec3(0,0,xoffset),0);
		rndr->MoveCamera(0, scn->zTranslate, -0.4f*yoffset);
	}
	
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Game* scn = (Game*)rndr->GetScene();

		rndr->UpdatePosition((float)xpos,(float)ypos);

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
			}
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				rndr->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
			}

	}

	void window_size_callback(GLFWwindow* window, int width, int height)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		
		rndr->Resize(width,height);
		
	}
	
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
		Game* scn = (Game*)rndr->GetScene();

		if (action == GLFW_PRESS || action == GLFW_REPEAT)
		{
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			case GLFW_KEY_SPACE:
				scn->AddOperation(6);
				/*if (scn->IsActive())
					scn->Deactivate();
				else
					scn->Activate();*/
				break;

			case GLFW_KEY_UP:
				rndr->MoveCamera(0, scn->zTranslate, 0.4f);
				break;
			case GLFW_KEY_DOWN:
				rndr->MoveCamera(0, scn->zTranslate, -0.4f);
				break;
			case GLFW_KEY_R: // push right wall rot animation
				scn->AddOperation(1);
				break; 
			case GLFW_KEY_L: // push left wall rot animation
				scn->AddOperation(0);
				break; 
			case GLFW_KEY_U: // push up wall rot animation
				scn->AddOperation(3);
				break; 
			case GLFW_KEY_D: // push down wall rot animation
				scn->AddOperation(2);
				break; 
			case GLFW_KEY_B: // push back wall rot animation
				scn->AddOperation(4);
				break; 
			case GLFW_KEY_F: // push front wall rot animation
				scn->AddOperation(5);
				break;
			case GLFW_KEY_E: // push 2nd right wall rot animation
				scn->AddOperation(11);
				break;
			case GLFW_KEY_K: // push 2nd left wall rot animation
				scn->AddOperation(10);
				break;
			case GLFW_KEY_Y: // push 2nd up wall rot animation
				scn->AddOperation(13);
				break;
			case GLFW_KEY_S: // push 2nd down wall rot animation
				scn->AddOperation(12);
				break;
			case GLFW_KEY_V: // push 2nd back wall rot animation
				scn->AddOperation(14);
				break;
			case GLFW_KEY_G: // push 2nd front wall rot animation
				scn->AddOperation(15);
				break;
			case GLFW_KEY_Z:
				scn->AddOperation(8);
				break;
			case GLFW_KEY_A:
				scn->AddOperation(7);
				break;
			case GLFW_KEY_M: // mix random 10 rotations
				for(int i=0; i < 10; i++)
					scn->AddOperation(rand() % 6);
				break;
			default:
				break;
			}
		}
	}

	void Init(Display &display)
	{
		display.AddKeyCallBack(key_callback);
		display.AddMouseCallBacks(mouse_callback,scroll_callback,cursor_position_callback);
		display.AddResizeCallBack(window_size_callback);
	}
