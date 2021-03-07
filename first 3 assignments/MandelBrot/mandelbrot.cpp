#include "mandelbrot.h"
#include <iostream>
#include "GL/glew.h"

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

Mandelbrot::Mandelbrot() : Scene()
{
	counter = 1;
	p = 2;
	z = 128;
	pW = 2;
	xOffset = 0;
	yOffset = 0;
}

//Game2::Game2(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}

void Mandelbrot::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0 };
	unsigned int slots[3] = { 0 , 1, 0 };

	AddShader("../res/shaders/pickingShader"); //0 - click on 3d obj always in 0 pos.. even in 2d
	AddShader("../res/shaders/mandelbrotShader"); // what we will write
	//AddShader("../res/shaders/basicShader"); // 1

	AddTexture("../res/textures/pal.png", 1); // try returning to 1 later when running or change back to 0
	//TextureDesine(840, 840); // replace shader

	AddMaterial(texIDs, slots, 1);

	AddShape(Plane, -1, TRIANGLES);
	SetShapeShader(0, 1); // according to shade index	
	//SetShapeMaterial(0, 0);
}

void Mandelbrot::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int shaderIndx)
{	
	if(counter)
		counter++;
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
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
	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if(shaderIndx!=1)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	//send param to shader
	//s->SetUniform1ui("counter", counter); // timer
	s->SetUniform1f("pixelWidth", pW); // px width

	s->SetUniform1f("p", p);
	s->SetUniform1f("z", z);
	s->SetUniform1f("x", x); // x param
	s->SetUniform1f("y", y); // y param - warning means we dont use it!
	s->SetUniform1f("xOffset", xOffset);
	s->SetUniform1f("yOffset", yOffset);
	s->Unbind();
}

void Mandelbrot::updateP(float change) {
	p += change;
	if (p < 2) {
		p = 2;
	}
}

void Mandelbrot::updateColorCounter(bool divide) {
	if (divide) {
		z = (z == 2) ? z : z / 2;
	}
	else
		z = (z == 128) ? z : z * 2;
	std::cout << "colors:" << z << std::endl;
}

void Mandelbrot::updatePixelWidth(float change) {
	if (change > 0) {
		pW /= (change + 1);
	}
	else {
		pW *= (-change + 1);
	}
	if (pW > 32) {
		pW = 32;
	}
	std::cout << "pixel width - " << pW/840 << std::endl;
}

void Mandelbrot::UpdatePosition(float xpos,  float ypos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); //??
	prevX = x;
	prevY = y;
	x = xpos / viewport[2]; // רוחב של המסך הספציפי// coord in px where window start in in case of split screen
	y = 1 - ypos / viewport[3]; //של המסך הספציפי אורך //// coord in px where window start in
}

void Mandelbrot::WhenRotate()
{
	xOffset += (x - prevX)*pW;
	yOffset += (y - prevY)*pW;
}

void Mandelbrot::WhenTranslate()
{
}



void Mandelbrot::Motion()
{
	if(isActive)
	{
	}
}

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
}

unsigned int Mandelbrot::TextureDesine(int width, int height)
{
	float p = 11;
	float iter = 50;
	unsigned char* data = new unsigned char[width * height * 4];
	for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			float cx = mapRange(i, width, -2, 1);
			float cy = mapRange(j, height, -1.5, 1.5);
			
			int counter = 0;
			float zx = 0;
			float zy = 0;
			while (counter < iter && zx*zx + zy*zy < 4.0) {
				float c = zx * zx + zy * zy;
				float t = pow(c, p/2)*cos(p*atan2(zy,zx)) + cx;
				zy = pow(c, p / 2) * sin(p * atan2(zy, zx)) + cy;
				zx = t;
				counter++;
			}

			float color = mapRange(counter, iter, 0, 255);
			if (counter == iter) {
				color = 0;
			}

			data[(i * height + j) * 4] = 0; //color;//(i + j) % 256;
			data[(i * height + j) * 4 + 1] = color;//color;//(i + j * 2) % 256;
			data[(i * height + j) * 4 + 2] = color;//color;
			data[(i * height + j) * 4 + 3] = color;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

Mandelbrot::~Mandelbrot(void)
{

}
