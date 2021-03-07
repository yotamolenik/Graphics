#define GLM_SWIZZLE
#include "rays.h"
#include <iostream>
#include "GL/glew.h"
#include <glm/glm.hpp>
#define _USE_MATH_DEFINES
#include <math.h>


float delta = 0.0001;

static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Rays::Rays() : Scene()
{
	counter = 1;
	p = 2;
	z = 128;
	zOffset = 0;
	xOffset = 0;
	yOffset = 0;
	selected = -1;
}

void Rays::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0 };
	unsigned int slots[3] = { 0 , 1, 0 };
	std::string str = "../res/scenes/scene5.txt";
	SceneData myScene;
	SceneParser(str, &myScene);

	eye = myScene.eye;
	ambient = myScene.ambient;
	sizes = myScene.sizes;
	lights = myScene.lights;
	objects = myScene.objects;
	directions = myScene.directions;
	colors = myScene.colors;
	intensities = myScene.intensities;

	AddShader("../res/shaders/pickingShader"); //0 - click on 3d obj always in 0 pos.. even in 2d
	AddShader("../res/shaders/rays"); // what we will write
	//AddShader("../res/shaders/basicShader"); // 1
	//TextureDesine(840, 840, &myScene); // replace shader

	AddShape(Plane, -1, TRIANGLES);
	SetShapeShader(0, 1); 	

}

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
}
float max(float a, float b) {
	if (a > b)
		return a;
	return b;
}
int mod(int val, int m) {
	return val % m;
}

float dot(glm::vec3 p1, glm::vec3 p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}
glm::vec3 reflect(glm::vec3 D, glm::vec3 N) {
	return 2.0f * dot(N, D) * N - D;
}

glm::vec3 normalize(glm::vec3 v) {
	float nrm = sqrt(pow((v.x), 2) + pow((v.y), 2) + pow((v.z), 2));
	return glm::vec3(v.x / nrm, v.y / nrm, v.z / nrm);
}

glm::vec3 ConstructRayThroughPixel(glm::vec4 eye, float i, float j) {
	return normalize(glm::vec3(i, j, 0) - eye.xyz);
}

float intersectionPlane(glm::vec4 plane, glm::vec3 sourcePoint, glm::vec3 v) {
	glm::vec3 N = normalize(plane.xyz);
	glm::vec3 p0 = glm::vec3(0, 0, -plane.w / plane.z);
	if (dot(v, N) < delta)
		return 0;
	float t = dot(p0 - sourcePoint, N) / dot(v, N);
	return max(t, 0.0);
}
float intersectionSphere(glm::vec4 sphere, glm::vec3 sourcePoint, glm::vec3 v) {
	glm::vec3 src_obj = sourcePoint - sphere.xyz;
	float a = 1.0;
	float b = 2.0 * dot(v, src_obj);
	float c = dot(src_obj, src_obj) - sphere.w * sphere.w;
	float d = b * b - 4.0 * a * c;
	if (d < 0.0) {}// no intersection
	else // line intersects sphere			
	{
		if (-(b + sqrt(d)) / (2.0 * a) > 0)
			return -(b + sqrt(d)) / (2.0 * a);
	}
	return 0;
}
glm::vec2 intersection(glm::vec3 sourcePoint, glm::vec3 v, int skipped, int size, std::vector<glm::vec4> objects) {
	float tMin = 0;
	int index = -1;
	for (int k = 0; k < size; k++) {
		if (skipped == k)
			continue;
		float t = 0;
		if (objects[k].w < 0) { //plane	
		}
		else {
			t = intersectionSphere(objects[k], sourcePoint, v);
		}
		if (t > 0 && (tMin == 0.0 || tMin > t)) {
			tMin = t;
			index = k;
		}
	}
	return glm::vec2(tMin,index);
}


void Rays::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int shaderIndx)
{	
	if(counter)
		counter++;
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	
	s->Bind();
	if (shaderIndx != 1)
	{
		s->SetUniformMat4f("MVP", MVP); //projection + camera transform
		s->SetUniformMat4f("Normal", Model); //obj transform + scene
	}
	else
	{
		s->SetUniformMat4f("MVP", glm::mat4(1));
		s->SetUniformMat4f("Normal", glm::mat4(1));
	}
	s->SetUniform1f("zOffset", zOffset);
	s->SetUniform1f("xOffset", xOffset);
	s->SetUniform1f("yOffset", yOffset);
	s->SetUniform4f("eye", eye.x, eye.y, eye.z, eye.w);
	s->SetUniform4f("ambient", ambient.x, ambient.y, ambient.z, ambient.w);
	s->SetUniform4i("sizes", sizes.x, sizes.y, sizes.z, sizes.w);
	s->SetUniform4fv("objects", &objects[0], 20);
	s->SetUniform4fv("objColors", &colors[0], 20);
	s->SetUniform4fv("lightsDirection", &directions[0], 10);
	s->SetUniform4fv("lightsIntensity", &intensities[0], 10);
	s->SetUniform4fv("lightPosition", &lights[0], 10);
	//s->SetUniform1f("p", p);
	s->Unbind();
}

void Rays::updateP(float change) {
}

void Rays::updateColorCounter(bool divide) {

}

void Rays::updatePixelWidth(float change) {
	zOffset += change / 4;
}

void Rays::UpdatePosition(float xpos,  float ypos){
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); //??
	prevX = x;
	prevY = y;
	x = xpos / viewport[2]; // ���� �� ���� �������// coord in px where window start in in case of split screen
	y = 1 - ypos / viewport[3]; //�� ���� ������� ���� //// coord in px where window start in
}

void Rays::WhenRotate(){
	float pW = zOffset == 0 ? 1 : (zOffset > 0 ? (1 / zOffset) : zOffset);
	xOffset += (x - prevX) * pW;
	yOffset += (y - prevY) * pW;
}
void Rays::trySelect(bool clicked){
	if (clicked) {
		float mapedX = mapRange(prevX, 1, -1, 1);
		float mapedY = mapRange(prevY, 1, -1, 1);
		glm::vec3 V1 = ConstructRayThroughPixel(eye, mapedX, mapedY);
		//std::cout << "x " << x << "y " << y << "prevX " << prevX << "prevY " << prevY << std::endl;
		glm::vec3 P = eye.xyz - glm::vec3(xOffset, yOffset, zOffset);
		glm::vec2 sObj = intersection(P, V1, -1, sizes[0], objects);
		selected = sObj[1];
	}
	else
		selected = -1;
}

void Rays::WhenTranslate(){
	//shoot a ray through the mouse to the ball


	//if (sObj[1] > -1) {
	//	glm::vec3 V2 = ConstructRayThroughPixel(eye, x, y);
	//	glm::vec3 newpos = sObj[0] * (V2 - V1);
	//	objects[int(sObj[1])].x += newpos.x;// *pW;
	//	objects[int(sObj[1])].y += newpos.y;// *pW;
	//}
	float pW = zOffset == 0 ? 1 : (zOffset > 0 ? (1 / zOffset) : zOffset);
	if (selected > -1) {
		//move ball
		objects[selected].x += (x - prevX)* 2 * pW;
		objects[selected].y += (y - prevY)* 2 * pW;
	}
}

void Rays::Motion()
{
	if(isActive)
	{
	}
}



//int getIntersectingObj(std::vector<glm::vec4> objects, int size, glm::vec3 in_ray, glm::vec3 hit) {
//	for (int i = 0; i < size; i++) {
//		if (objects[i].w < 0) {
//			if (dot(normalize(objects[i].xyz), in_ray) > delta) {
//				return i;
//			}
//		}
//		else if (dot(hit - objects[i].xyz, hit - objects[i].xyz) - (objects[i].w * objects[i].w) < delta) {
//			return i;
//		}
//	}
//	return -1;
//	//i' j' (point) i' = i + V.x
//	//float a = acos(dot(normalize(scene->objects[k].xyz), in_ray));
//	//float newX = /*(cos(a) - sin(a)) * i + (in_ray.x * cos(a) - in_ray.y * sin(a)) * j;
//	//float newY = */
//	//float newX = i * cos(a) - j * sin(a) + in_ray.x;
//	//float newY = i * sin(a) + j * cos(a) + in_ray.y;
//	/*if (int(1.5 * mapRange(hit.x + 10, 2, 0, 5)) % 2 == int(1.5 * mapRange(hit.y+10, 2, 0, 5)) % 2) {
//		return calcAmbientColor(scene, k);
//	}*/
//}
//glm::vec3 calcAmbientColor(SceneData* scene, int objIndex) {
//	glm::vec3 color = glm::vec3(scene->ambient.x * scene->colors[objIndex].x,
//		scene->ambient.y * scene->colors[objIndex].y, 
//		scene->ambient.z * scene->colors[objIndex].z);
//	return color;
//}
//glm::vec3 getSphereNormal(glm::vec4 sphere, glm::vec3 hit) {
//	return glm::vec3(2 * (hit.x - sphere.x), 2 * (hit.y - sphere.y), 2 * (hit.z - sphere.z));
//}
//
//glm::vec3 calcdiffusecolor(glm::vec4 obj, glm::vec3 objColor, glm::vec3 lightColor, glm::vec3 D, glm::vec3 hit) {
//	glm::vec3 N = obj.w < 0.0 ? normalize(obj.xyz) : normalize(hit - obj.xyz);
//	return max(0.5*dot(N, normalize(D)), 0.0) * lightColor;
//}
//glm::vec3 calcspecularcolor(SceneData* scene, glm::vec3 N, glm::vec3 D, glm::vec3 viewDir, glm::vec4 lightColor, glm::vec3 objectColor){
//	glm::vec3 eye = scene->eye.xyz;
//	float specularStrength = 0.7;
//	glm::vec3 reflectDir = reflect(normalize(D), N); // -normalize(D) - orig
//	float spec = pow(max(dot(viewDir, reflectDir), 0.0), lightColor.w);
//	glm::vec3 specular = specularStrength * spec * lightColor.xyz;
//	return specular;
//}
//float calcSpotlight(glm::vec3 hit, glm::vec3 P, glm::vec3 D, float Q) { 
//	float theta = dot(normalize(P - hit), normalize(D)); //*/ size;
//	//float theta = dot(in_ray, hit);
//	if(acos(theta) > M_PI/2 || acos(theta) < 0){}
//	else if (theta > Q)
//		return 1;
//	return 0;
//	/*float Distance = sqrt(dot(P - hit, P - hit));
//	return 0.5 + 0.5 * Distance + 0.5 * Distance * Distance;*/
//}
//bool occluded(int index, glm::vec3 in_ray, glm::vec3 hit, glm::vec3 N, SceneData* scene, glm::vec3 D) {
//	glm::vec3 V = -normalize(D); //reflect(in_ray, N); // check 
//	float t = intersection(hit, V, index, scene->sizes[0], scene->objects);
//	return t > 0;
//}
//float occludedSpot(int index, glm::vec3 hit, glm::vec3 P, SceneData* scene) {
//	glm::vec3 V = (hit -P);
//	float t = intersection(hit, V, index, scene->sizes[0], scene->objects);
//	return t > 0 ? 0.0 : 1.0;
//}
//glm::vec3 GetColor(SceneData* scene, glm::vec3 in_ray, glm::vec3 hit)
//{
//	int index = getIntersectingObj(scene->objects, scene->sizes[0], in_ray, hit);
//	glm::vec3 N = scene->objects[index].w < 0.0 ? normalize(scene->objects[index].xyz) : normalize(hit - scene->objects[index].xyz);
//	glm::vec3 color = scene->ambient.xyz;
//	glm::vec3 ambient = scene->ambient.xyz;
//
//	for (int i = 0; i < scene->sizes[1]; i++) {
//		glm::vec3 diffuse = calcdiffusecolor(scene->objects[index], scene->colors[index].xyz, scene->intensities[i].xyz, scene->directions[i].xyz, hit);
//		glm::vec3 specular = calcspecularcolor(scene, N, scene->directions[i].xyz, in_ray, scene->intensities[i], scene->colors[index].xyz);
//		if (scene->directions[i].w == 1.0) {
//			float spotLight = 0.0;//calcSpotlight(hit, scene->lights[i].xyz, scene->directions[i].xyz, scene->lights[i].w);
//			float shadow = occludedSpot(index, hit, scene->lights[i].xyz, scene);
//			color += (diffuse + specular) * spotLight * shadow;
//		}
//		else if (!occluded(index, in_ray, hit, N, scene, scene->directions[i].xyz))
//			color += diffuse + specular;
//	}
//	// make diffuse and ambient + specular? smaller
//	int modX = mod(int(1.5 * hit.x + 1000), 2);
//	int modY = mod(int(1.5 * hit.y + 1000), 2);
//	if(scene->objects[index].w < 0 && modX == modY)
//		color /= 2;
//	return color * scene->colors[index].xyz;
//}
//
//glm::vec3 GetColor2(glm::vec3 in_ray, SceneData* scene){
//	glm::vec3 color = glm::vec3(0, 0, 0);  //scene->ambient.xyz;//
//	glm::vec3 sourcePoint = scene->eye.xyz;
//	glm::vec3 V = in_ray;
//	for (int i = 1; i < 6; i++) {
//		float t = intersection(sourcePoint, V, -1, scene->sizes[0], scene->objects);
//		if (!(t > 0)) {
//			return color;
//		}
//		glm::vec3 hit = sourcePoint + t * V;
//		color += GetColor(scene, V, hit); //pow(0.7f, i) *
//		sourcePoint = scene->eye.xyz + t * V;
//		int index = getIntersectingObj(scene->objects, scene->sizes[0], in_ray, hit);
//		if (index >= scene->sizes[2]) { break; }
//		glm::vec3 N = scene->objects[index].w < 0.0 ? normalize(scene->objects[index].xyz) : normalize(hit - scene->objects[index].xyz);
//		V = reflect(normalize(in_ray), normalize(N));
//	}
//	return color;
//}
//
////glm::vec3 colorCalc(glm::vec3 intersectionPoint, SceneData* scene, float i, float j)
////{
////	glm::vec3 color = glm::vec3(153, 0, 51);
////	glm::vec3 V = ConstructRayThroughPixel(scene->eye, mapRange(i, width, -1, 1), mapRange(j, height, -1, 1));
////	float t = intersection(scene->eye.xyz, V, scene->sizes[0], scene->objects);
////	if (t >= 0) {
////		color = GetColor(scene, V, scene->eye.xyz + t * V);
////		color.x = mapRange(color.x, 1, 0, 255);
////		color.y = mapRange(color.y, 1, 0, 255);
////		color.z = mapRange(color.z, 1, 0, 255);
////	}
////
////	return color;
////}

unsigned int Rays::TextureDesine(int width, int height, SceneData* scene)
{
	unsigned char* data = new unsigned char[width * height * 4];
	glm::vec3 color = glm::vec3(0, 0, 0);
	for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			//glm::vec3 V = ConstructRayThroughPixel(scene->eye, mapRange(j, width, -1, 1), mapRange(i, height, -1, 1));
			//glm::vec3 sourcePoint = scene->eye.xyz;
			//float t = intersection(sourcePoint, V, -1, scene->sizes[0], scene->objects);
			//glm::vec3 hit = sourcePoint + t * V;
			//glm::vec3 color = t > 0 ? GetColor(scene, V, hit) : scene->ambient.xyz; //GetColor2(V, scene);
			color.x = mapRange(color.x, 1, 0, 255);
			color.y = mapRange(color.y, 1, 0, 255);
			color.z = mapRange(color.z, 1, 0, 255);
			data[(i * height + j) * 4] = color.x;
			data[(i * height + j) * 4 + 1] = color.y;
			data[(i * height + j) * 4 + 2] = color.z;;
			data[(i * height + j) * 4 + 3] = 0;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

Rays::~Rays(void)
{

}
