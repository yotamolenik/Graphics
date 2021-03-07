#define GLEW_STATIC
#include "GL/glew.h"
#include "scene.h"
#include <iostream>
#include <Game2\Bezier1D.h>
#include <Game2/Bezier2D.h>

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

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
}

Scene::Scene()
{
	glLineWidth(5);

	pickedShape = -1;
	depth = 0;

	isActive = true;
}

void Scene::AddShapeFromFile(const std::string& fileName, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(fileName, mode));
}

void Scene::AddShape(Shape* shp, int parent)
{
	chainParents.push_back(parent);
	shapes.push_back(shp);
}

void Scene::AddShape(int type, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(type, mode));
}

void Scene::AddShapeCopy(int indx, int parent, unsigned int mode)
{
	chainParents.push_back(parent);
	shapes.push_back(new Shape(*shapes[indx], mode));
}

int Scene::AddShader(const std::string& fileName)
{
	shaders.push_back(new Shader(fileName));
	return (shaders.size() - 1);
}

int  Scene::AddTexture(const std::string& textureFileName, int dim)
{
	textures.push_back(new Texture(textureFileName, dim));
	return(textures.size() - 1);
}

int Scene::AddTexture(int width, int height, unsigned char* data, int mode)
{
	textures.push_back(new Texture(width, height));

	switch (mode)
	{
	case COLOR:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save memory.
		break;
	case DEPTH:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
		break;
	case STENCIL:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, data);
		break;
	default:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save memory.
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return(textures.size() - 1);
}

int Scene::AddMaterial(unsigned int texIndices[], unsigned int slots[], unsigned int size)
{
	materials.push_back(new Material(texIndices, slots, size));
	return (materials.size() - 1);
}

void Scene::RemoveShape(int shpIndx)
{
	shapes.erase(shapes.begin()+shpIndx);
}

void Scene::ReplaceShape(int shpIndx, Shape* shp)
{
	shapes[shpIndx] = shp;
}

void Scene::Draw(int shaderIndx, const glm::mat4& View, const glm::mat4& Projection, int viewportIndx, unsigned int flags)
{
	glm::mat4 SceneTrans = MakeTrans();

	int p = pickedShape;

	for (pickedShape = 0; pickedShape < shapes.size(); pickedShape++)
	{
		if (shapes[pickedShape]->Is2Render(viewportIndx))
		{
			// as i understand the ps - the view, projection and normal should be like this
			glm::mat4 Model = shapes[pickedShape]->MakeTrans();
			//int p = chainParents[pickedShape];
			//if (p >= 0) // move the frames with the objects
				//Model = shapes[p]->MakeTrans() * Model;
			//if (p == -2) // the plane moves with the camera
			//	Model = glm::inverse(View) * Model;
			Model = SceneTrans * Model;
			if (shaderIndx > 0)
			{
				Update(View, Projection, Model, shapes[pickedShape]->GetShader());
				shapes[pickedShape]->Draw(shaders[shapes[pickedShape]->GetShader()], false);
			}
			else
			{ //picking - we draw the picking for Renderer::Picking()
					Update(View, Projection, Model, 0);
					shapes[pickedShape]->Draw(shaders[0], true);
			}
		}
	}
	pickedShape = p;
}

void Scene::ShapeTransformation(int type, float amt)
{
	if (glm::abs(amt) > 1e-5)
	{
		switch (type)
		{
		case xTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(amt, 0, 0), 0);
			break;
		case yTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(0, amt, 0), 0);
			break;
		case zTranslate:
			shapes[pickedShape]->MyTranslate(glm::vec3(0, 0, amt), 0);
			break;
		case xRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(1, 0, 0), 0);
			break;
		case yRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(0, 1, 0), 0);
			break;
		case zRotate:
			shapes[pickedShape]->MyRotate(amt, glm::vec3(0, 0, 1), 0);
			break;
		case xScale:
			shapes[pickedShape]->MyScale(glm::vec3(amt, 1, 1));
			break;
		case yScale:
			shapes[pickedShape]->MyScale(glm::vec3(1, amt, 1));
			break;
		case zScale:
			shapes[pickedShape]->MyScale(glm::vec3(1, 1, amt));
			break;
		case scaleAll:
			shapes[pickedShape]->MyScale(glm::vec3(1, 1, amt));
			shapes[pickedShape]->MyScale(glm::vec3(1, amt, 1));
			shapes[pickedShape]->MyScale(glm::vec3(amt, 1, 1));
			break;
		case ZeroTrans:
			shapes[pickedShape]->ZeroTrans();
			break;
		default:
			break;
		}
	}

}


bool Scene::Picking(unsigned char data[4])
{
		pickedShape = -1;
		std::cout << "now picked:" << data[0] - 1 << std::endl;
		if (data[0] > 0)
		{
			pickedShape = data[0]-1; //r 
			if (pickedShape != 0) { // if we picked the cubemap start drawing the blend
				return true;
			}
			return false; // false only if we picked the cubemap
		}
		return true;
		//WhenPicked();	
}

void Scene::MouseProccessing(glm::mat4 View, int button, int xrel, int yrel)
{

	if (button == 1)
	{
		WhenTranslate(View);
	}
	else
	{
		WhenRotate(View);
	}
}

void Scene::clearPicks()
{
	if(std::find(pickedShapes.begin(), pickedShapes.end(), pickedShape) == pickedShapes.end())
		pickedShapes.clear();
}

bool pointInRect(int x, int y, int x1, int y1, int width, int height)
{
	return (x > x1 && x < x1 + width &&
		y > y1 && y < y1 + height);
}

bool Scene::pickMany(int x, int y, float width, float height, glm::mat4 view)
{
	if (pickedShapes.size() > 0 &&
		std::find(pickedShapes.begin(), pickedShapes.end(), pickedShape) != pickedShapes.end())
		return true;
	pickedShapes.clear();
	for (int i = 23; i < shapes.size(); i++) {
		glm::vec4 pos = view * shapes[i]->MakeTrans() * glm::vec4(0.001, 0.001, 0.001, 1);
		
		if (pointInRect(mapRange(pos.x + 6, 12, 0, 840),
						mapRange(pos.y + 6, 12, 0, 840),
						x, y, width, height))
			pickedShapes.push_back(i);
	}
	//print
	if (pickedShapes.size() > 0) {
		std::cout << "picked shapes:";
		for (int i = 0; i < pickedShapes.size(); i++) {
			std::cout << pickedShapes[i] << ", ";
		}
		std::cout << std::endl;
		return true;
	}
	return false;
}

void Scene::scalePicked(){
	if (pickedShapes.size() > 0) {
		for (int i = 0; i < pickedShapes.size(); i++) {
			pickedShape = pickedShapes[i];
			ShapeTransformation(xScale, 1.1);
			ShapeTransformation(yScale, 1.1);
		}
	}
	else if (pickedShape != -1) {
		ShapeTransformation(xScale, 1.1);
		ShapeTransformation(yScale, 1.1);
	}
}
void Scene::unscalePicked() {
	if (pickedShapes.size() > 0) {
		for (int i = 0; i < pickedShapes.size(); i++) {
			pickedShape = pickedShapes[i];
			ShapeTransformation(xScale, 0.9);
			ShapeTransformation(yScale, 0.9);
		}
	}
	else if (pickedShape != -1) {
		ShapeTransformation(xScale, 0.9);
		ShapeTransformation(yScale, 0.9);
	}
}

void Scene::ZeroShapesTrans()
{
	for (unsigned int i = 0; i < shapes.size(); i++)
	{
		shapes[i]->ZeroTrans();
	}
}

void Scene::ReadPixel()
{
	glReadPixels(1, 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
}

void Scene::AddShapeViewport(int shpIndx,int viewPortIndx)
{
	shapes[shpIndx]->AddViewport(viewPortIndx);
}

void Scene::RemoveShapeViewport(int shpIndx, int viewPortIndx)
{
	shapes[shpIndx]->RemoveViewport(viewPortIndx);
}

void Scene::BindMaterial(Shader* s, unsigned int materialIndx)
{

	for (size_t i = 0; i < materials[materialIndx]->GetNumOfTexs(); i++)
	{
		materials[materialIndx]->Bind(textures, i);
		s->SetUniform1i("sampler" + std::to_string(i + 1), materials[materialIndx]->GetSlot(i));
	}
}

Scene::~Scene(void)
{
	for (Shape* shp : shapes)
	{
		delete shp;
	}

	for (Shader* sdr : shaders)
	{
		delete sdr;
	}
	for (Texture* tex : textures)
	{
		delete tex;
	}
	for (Material* mat : materials)
	{
		delete mat;
	}

}



