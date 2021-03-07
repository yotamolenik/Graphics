#include "game.h"
#include <iostream>
#include "GL/glew.h"

int MAX_ANIMATIONS = 20;
int TOTAL_FRAMES = 90;
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
void printFace(int cubeSize, std::vector<int> Cube) {
	for (int i = 0; i < cubeSize; i++) {
		for (int j = 0; j < cubeSize; j++)
			std::cout << Cube[(cubeSize - i - 1) * cubeSize + j] << " ";
		std::cout << std::endl;
	}
}
void printCube(int cubeSize, std::vector<int> Cube) {
	std::cout << "cube" << std::endl;
	for (int i = 0; i < cubeSize; i++) {
		for (int k = 0; k < cubeSize; k++) {
			for (int j = 0; j < cubeSize; j++) {
				int val = Cube[k * cubeSize * cubeSize + (cubeSize - i - 1) * cubeSize + j];
				std::cout << ((val < 10) ? "0" + std::to_string(val) : std::to_string(val)) << " ";
			}
			std::cout << "|";
		}
		std::cout << std::endl;
	}
}
void printInstructions(int cubeSize) {
	std::cout << "up and down keys move the camera" << std::endl;
	std::cout << "R rotate right wall, L rotate left wall" << std::endl;
	std::cout << "U rotate top wall, D rotate bottom wall" << std::endl;
	std::cout << "F rotate front wall, B rotate back wall" << std::endl;
	if (cubeSize > 3) {
		std::cout << "G rotate 2nd front wall, V rotate 2nd back wall" << std::endl;
		std::cout << "Y rotate 2nd top wall, S rotate 2nd down wall" << std::endl;
		std::cout << "E rotate 2nd right wall, K rotate 2nd back wall" << std::endl;
	}
	std::cout << "Z speed up the rotation, A slow it down" << std::endl;
	std::cout << "M make 10 random rotation" << std::endl;
	std::cout << "SPACE changes rotation direction *and create outer space and move us to the forth dimension*" << std::endl;
}

Game::Game() : Scene()
{
	cubeSize = 4; //maximum of six because code support up to 255 cubes in total
	animSpeed = 1;
	currFrame = 0;
	isRotateClockWise = true;
}

//Game::Game(float angle ,float relationWH, float near, float far) : Scene(angle,relationWH,near,far)
//{ 	
//}
void Game::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader2");	
	AddShader("../res/shaders/basicShader2");
	
	TextureDesine(840, 840);
	float center = (cubeSize - 1.0) / 2.0;
	for (int i = 0; i < cubeSize; i++) {
		for (int j = 0; j < cubeSize; j++) {
			for (int k = 0; k < cubeSize; k++) {
				AddShape(Cube, -1, TRIANGLES);
				pickedShape = i*cubeSize*cubeSize + j*cubeSize + k;
				ShapeTransformation(xTranslate, 2 * (k - center));
				ShapeTransformation(yTranslate, 2 * (j - center));
				ShapeTransformation(zTranslate, 2 * (i - center));
				
				cubesIndexs.push_back(pickedShape);
			}
		}
	}
	printInstructions(cubeSize);
	UpdateAnimationSpeed(0);
	printCube(cubeSize, cubesIndexs);
	pickedShape = -1;
	SetShapeShader(0, 1);

	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int shaderIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	textures[0]->Bind(0);
	s->Bind();
	if (shaderIndx != 2)
	{
		s->SetUniformMat4f("View", MVP);
		s->SetUniformMat4f("Model", Model);
	}
	else
	{
		s->SetUniformMat4f("View", glm::mat4(1));
		s->SetUniformMat4f("Model", glm::mat4(1));
	}
	s->SetUniformMat4f("Proj", glm::mat4(1));
	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if(shaderIndx!=2)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	s->SetUniform4f("lightColor", r / 255.0, 0, 0, 0);
	s->Unbind();
}

void Game::WhenRotate()
{
}
void Game::WhenTranslate()
{
}
void Game::Motion()
{
	if(isActive)
	{
	}
}

void Game::toggleRotationDir() {
	isRotateClockWise = !isRotateClockWise;
}
void Game::UpdateAnimationSpeed(int change) {
	animSpeed += change;
	if (animSpeed < 1)
		animSpeed = 1;
	else if (animSpeed > MAX_ANIMATIONS)
		animSpeed = MAX_ANIMATIONS;
	std::cout << "animationSpeed " << animSpeed << std::endl;
}

std::vector<int> getFaceIndexes(int cubeSize, int direction, int faceIndex){
	std::vector<int> indexs;
	switch (direction) {
		case 0: //col
			for (int i = 0; i < cubeSize * cubeSize; i++) {
				indexs.push_back(cubeSize * i + faceIndex);
			}
			break;
		case 1: //row
			for (int j = cubeSize - 1; j >= 0; j--) {
				for (int i = cubeSize - 1; i >= 0; i--) {
					int index = i * cubeSize * cubeSize +
						faceIndex * cubeSize;
					indexs.push_back(index + j);
				}
			}
			break;
		case 2: // the 3rd options
			for (int j = cubeSize - 1; j >= 0; j--) {
				for (int i = cubeSize - 1; i >= 0; i--) {
					int index = faceIndex * cubeSize * cubeSize;
					indexs.push_back(index + i*cubeSize + j);
				}
			}
			break;
		default:
			break;
	}
	return indexs;
}
std::vector<int> rotatedFaceIndexes(int cubeSize, bool isClockWise) {
	std::vector<int> newPositions;
	if (isClockWise) {
		for (int i = 0; i < cubeSize; i++) { //col
			for (int j = cubeSize - 1; j >= 0; j--) { //row
				newPositions.push_back(j * cubeSize + i);
			}
		}
	}
	else {
		for (int j = cubeSize - 1; j >= 0; j--) { //row
			for (int i = 0; i < cubeSize; i++) { //col
				newPositions.push_back(i * cubeSize + j);
			}
		}
	}
	return newPositions;
}

void Game::addRotation(int faceDirection, int faceIndex) {
	std::vector<int> pointers = getFaceIndexes(cubeSize, faceDirection, faceIndex);
	std::vector<int> positions = rotatedFaceIndexes(cubeSize, faceDirection == 2 ? isRotateClockWise : !isRotateClockWise);
	std::vector<int> values;
	for (int i = 0; i < cubeSize * cubeSize; i++) {
		values.push_back(cubesIndexs[pointers[i]]);
	}
	for (int i = 0; i < cubeSize * cubeSize; i++) {
		cubesIndexs[pointers[i]] = values[positions[i]];
	}
	
	Operation operation;
	operation.type = faceDirection;
	operation.indexs = values;
	printCube(cubeSize, cubesIndexs);
	operations.push(operation);
}
void Game::AddOperation(int operation) {
	if (operation < 6) {
		addRotation(operation / 2, (operation % 2) * (cubeSize - 1));
	}//rotation
	else if (operation >= 10) {
		if (cubeSize > 3) {
			operation -= 10;
			addRotation(operation / 2, (operation % 2) ? (cubeSize - 2) : 1);
		}
		else
			std::cout << "operation is not premitted in cubes sized less then 3" << std::endl;
	}//rotationSecond
	else {
		Operation op;
		op.type = operation - 3;
		operations.push(op);
	} //others
}
void Game::ReadOperation() {
	if (operations.empty())
		return;
	Operation op = operations.front();
	if (op.type < 3)
		return rotateWall(op.type, op.indexs);
	operations.pop();
	if(op.type == 3)
		toggleRotationDir();
	else
		UpdateAnimationSpeed((op.type - 4.5)*4);
}
void Game::rotateWall(int type, std::vector<int> indexs) {
	currFrame += animSpeed;
	float amount = (isRotateClockWise ? -1 : 1);
	if (currFrame >= TOTAL_FRAMES) {
		amount *= (TOTAL_FRAMES + animSpeed - currFrame);
		operations.pop();
		currFrame = 0;
	}
	else
		amount *= animSpeed;

	for (int i = 0; i < cubeSize * cubeSize; i++) {
		pickedShape = indexs[i];
		switch (type) {
			case 0:
				ShapeTransformation(xRotate, amount);
				break;
			case 1:
				ShapeTransformation(yRotate, amount);
				break;
			case 2:
				ShapeTransformation(zRotate, amount);
				break;
		}
	}
}

void Game::WhenPicked() {
	std::vector<int>::iterator itr = std::find(cubesIndexs.begin(), cubesIndexs.end(), pickedShape);
	if (itr != cubesIndexs.cend()) {
		int cubeIndex = std::distance(cubesIndexs.begin(), itr);
		std::cout << pickedShape << " present at index " << cubeIndex << std::endl;
		int faceIndex = cubeIndex / (cubeSize * cubeSize);
		cubeIndex -= faceIndex * cubeSize * cubeSize;
		int rowIndex = cubeIndex / cubeSize;
		int colIndex = cubeIndex - rowIndex * cubeSize;

		switch (pickedShapeNormalMax) {
			case 0:
				addRotation(0, colIndex);
				break;
			case 1:
				addRotation(1, rowIndex);
				break;
			case 2:
				addRotation(2, faceIndex);
			default:
				break;
		}
	}
	else {
		std::cout << "Element not found";
	}
	pickedShape = -1;
}
unsigned int Game::TextureDesine(int width, int height)
{
	unsigned char* data = new unsigned char[width * height * 4];
	for (size_t i = 0; i < width; i++)
	{
		for (size_t j = 0; j < height; j++)
		{
			bool isBlack = !(i < 40 || j < 40 || i > 800 || j > 800);
			data[(i * height + j) * 4] = isBlack ? 255 : 0;
			data[(i * height + j) * 4 + 1] = isBlack ? 255 : 0;
			data[(i * height + j) * 4 + 2] = isBlack ? 255 : 0;
			data[(i * height + j) * 4 + 3] = 0;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

Game::~Game(void)
{

}
