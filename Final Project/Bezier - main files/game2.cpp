#include "game2.h"
#include <iostream>
#include "GL/glew.h"

float planeScale = 1;
float planeSize = 2.5;

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

Game2::Game2() : Scene()
{
	counter = 1;
	pps = 20;
	x = 0;
	y = 0;
	xprev = 0;
	yprev = 0;
	curveScale = 0.9;
	pointsScale = 0.04;
	numOfBeziers = 0;
	isContinuityState = false;

	bez = new Bezier1D(3, pps, LINES);
	bez2 = new Bezier2D(bez, pps, pps, QUADS, 0);
	bez22 = new Bezier2D(bez, pps, pps, QUADS, 0);
}

void Game2::Init()
{		
	unsigned int texIDs[3] = { 0 , 1, 0};
	unsigned int slots[3] = { 0 , 1, 0 };
	
	AddShader("../res/shaders/pickingShader");
	AddShader("../res/shaders/basicShader");
	AddShader("../res/shaders/basicShader2");
	AddShader("../res/shaders/cubemapShader");
	AddShader("../res/shaders/pickingShader"); // for blending

	AddTexture("../res/textures/box0.bmp", 2);
	AddTexture("../res/textures/cubeMap/DayLight Box_", 3);

	AddMaterial(texIDs,slots, 1);
	AddMaterial(texIDs + 1, slots + 1, 1);

	//-------------------------SKY MAP------------------------
	AddShape(Cube, -1, TRIANGLES);
	SetShapeShader(0, 3);
	SetShapeMaterial(0, 1);   // 0 is box, 1 is cubemap

	pickedShape = 0;
	ShapeTransformation(xScale, 50);
	ShapeTransformation(yScale, 50);
	ShapeTransformation(zScale, 50);

	//-----------------------2D BEZIER------------------------
	pointsStartIndx = 1;
	AddShape(Axis, -1, LINES);
	AddShapeViewport(pointsStartIndx, 1);
	RemoveShapeViewport(pointsStartIndx, 0);
	AddShape(Cube, -1, TRIANGLES); // will be replaced by bez bez
	
	
	pointsStartIndx += 2;
	for (int i = 0; i < 6 * 3 + 1; i++) //max control points
		AddControlPoint(i);
	RemakeBezier(3);

	//---------------------BLENDING PLANE---------------------
	AddShape(Plane, -2, TRIANGLES); // parent is the camera
	pickedShape = pointsStartIndx + 19;
	SetShapeShader(pickedShape, 4);
	ShapeTransformation(xScale, 50);
	ShapeTransformation(yScale, 50);
	AddShapeViewport(pickedShape, 2);
	RemoveShapeViewport(pickedShape, 0);

	//-----------------------3D BEZIER------------------------
	Add3DBezier();

	pickedShape = -1;
}

#pragma region bez1D
void Game2::HideControlPoint(int indx) {
	pickedShape = indx + pointsStartIndx;
	ShapeTransformation(xScale, 1.1e-5);
	ShapeTransformation(yScale, 1.1e-5);
}

void Game2::RelocateControlPoint(int segment, int indx) {
	glm::vec4 cp = bez->GetControlPoint(segment, indx);
	pickedShape = segment * 3 + indx + pointsStartIndx;
	ShapeTransformation(ZeroTrans, 1);
	ShapeTransformation(xScale, pointsScale);
	ShapeTransformation(yScale, pointsScale);
	ShapeTransformation(xTranslate, (cp.x * curveScale) / pointsScale);
	ShapeTransformation(yTranslate, (cp.y * curveScale) / pointsScale);
}

void Game2::FixControlPoints() {
	for (int seg = 0; seg < bez->GetSegmentsNum(); seg++) {
		for (int i = 0; i < 3; i++)
			RelocateControlPoint(seg, i);
	}
	RelocateControlPoint(bez->GetSegmentsNum() - 1, 3);
	for (int i = bez->GetSegmentsNum() * 3 + 1; i < 6 * 3 + 1; i++) //max control points
		HideControlPoint(i);
	pickedShape = -1;
}

void Game2::AddControlPoint(int indx) {
	AddShape(Octahedron, 1, TRIANGLES);
	pickedShape = indx + pointsStartIndx;
	SetShapeShader(pickedShape, 2);
	AddShapeViewport(pickedShape, 1);
	RemoveShapeViewport(pickedShape, 0);
}

void Game2::RemakeBezier(int segNum) {
	bez = new Bezier1D(segNum, pps, LINES);
	pickedShape = pointsStartIndx - 1;
	ReplaceShape(pickedShape, bez);

	SetShapeShader(pickedShape, 1);
	AddShapeViewport(pickedShape, 1);
	RemoveShapeViewport(pickedShape, 0);
	ShapeTransformation(xScale, curveScale);
	ShapeTransformation(yScale, curveScale);

	FixControlPoints();
	Update3DBezier();
}

void Game2::MoveControlPoint(int indx, float x, float y) {
	pickedShape = indx + pointsStartIndx;
	ShapeTransformation(xTranslate, x / pointsScale);
	ShapeTransformation(yTranslate, y / pointsScale);
}

void Game2::MoveControlPoint(int segment, int indx, float x, float y) {
	pickedShape = segment * 3 + indx + pointsStartIndx;
	ShapeTransformation(xTranslate, x / pointsScale);
	ShapeTransformation(yTranslate, y / pointsScale);
}

#pragma endregion

#pragma region 1DhelpFuncs
glm::vec4 getLine(glm::vec4 p0, glm::vec4 p1) {
	float a = (p0.y - p1.y) / (p0.x - p1.x);
	float b = p0.y - a * p0.x;
	return glm::vec4(a, b, 0, 0);
}

float getAngle(glm::vec4 center, glm::vec4 point, float xOffset, float yOffset) {
	glm::vec4 vectorA = point - center;
	glm::vec4 vectorB = glm::vec4(point.x + xOffset, point.y + yOffset, 0, 0) - center;
	float dot = vectorA.x * vectorB.x + vectorA.y * vectorB.y;
	float det = vectorA.x * vectorB.y - vectorA.y * vectorB.x;
	return atan2(det, dot);
}

glm::vec4 rotatePoint(glm::vec4 center, glm::vec4 point, float angle) {
	point -= center;
	float s = sin(angle);
	float c = cos(angle);
	return center + glm::vec4(point.x * c - point.y * s, point.x * s + point.y * c, 0, 0);
}

#pragma endregion

#pragma region bez2D
void Game2::Add3DBezier() {
	Bezier2D* bezbez = new Bezier2D(bez, pps, pps, QUADS, 0);
	AddShape(bezbez, -1);
	
	pickedShape = pointsStartIndx + 20 + numOfBeziers;
	SetShapeShader(pickedShape, 2);

	/*AddShapeCopy(pickedShape, pickedShape, LINE_LOOP);
	pickedShape++;
	SetShapeShader(pickedShape, 4);
	ShapeTransformation(xScale, 1.1);
	ShapeTransformation(yScale, 1.1);
	ShapeTransformation(zScale, 1.1);
	ShapeTransformation(zTranslate, -0.2);*/
	//AddShapeViewport(pickedShape + 1, 2);
	//RemoveShapeViewport(pickedShape + 1, 0);
	numOfBeziers+= 1;

	pickedShape = -1;
}

void Game2::Update3DBezier() {
	/*for (int i = 0; i < numOfBeziers; i++) {
		pickedShape = pointsStartIndx + 20 + i;
		((Bezier2D*)shapes[pickedShape])->UpdateBezier(bez);
	}*/
	pickedShape = -1;
}

#pragma endregion 

void Game2::onScroll(int yoffset, bool isPressed)
{
	float scale = yoffset > 0 ? 0.9 : 1.1;
	if (isPressed) {
		if (pickedShapes.size() != 0) { //multi pick
			for (int i = 0; i < pickedShapes.size(); i++) {
				pickedShape = pickedShapes[i];
				ShapeTransformation(xScale, scale);
				ShapeTransformation(yScale, scale);
				ShapeTransformation(zScale, scale);
			}
		}
		else if (pickedShape > pointsStartIndx + 18) {
			ShapeTransformation(xScale, scale);
			ShapeTransformation(yScale, scale);
			ShapeTransformation(zScale, scale);
		}
	}
	else if(x > 1) {// 1D-BEZ, e - zoom graph
		curveScale *= scale;
		pickedShape = pointsStartIndx - 1;
		ShapeTransformation(xScale, scale);
		ShapeTransformation(yScale, scale);
		ShapeTransformation(zScale, scale);
		FixControlPoints();
	}
}

void Game2::ContinuityStateToggle() {
	isContinuityState = !isContinuityState;
	std::cout << "is continuityState ? " << (isContinuityState ? "true" : "false") << std::endl;
}

void Game2::MoveFixToView(glm::mat4 View, glm::vec4 move, bool isTransform) {
	move = glm::inverse(View) * move;
	if (isTransform) {
		ShapeTransformation(xTranslate, move.x);
		ShapeTransformation(yTranslate, move.y);
		ShapeTransformation(zTranslate, move.z);
	}
	else {
		ShapeTransformation(yRotate, move.x);
		ShapeTransformation(xRotate, move.y);
		ShapeTransformation(zRotate, move.z);
	}
}

void Game2::WhenRotate(glm::mat4 View)
{
	float rotScale3d = 1000;
	if (pickedShapes.size() != 0) { //2D-BEZ, multi pick
		for (int i = 0; i < pickedShapes.size(); i++) {
			pickedShape = pickedShapes[i];
			MoveFixToView(View, glm::vec4((x - xprev) * rotScale3d, (y - yprev) * rotScale3d, 0, 0), false);
		}
	}
	if (pickedShape < pointsStartIndx) {
		if (x > 1) {// 1D-BEZ, e - convex hull
			int segment = bez->GetSectionIsMouseInConvexHull(2 * (x - 1.5) / curveScale, (2 * y - 1) / curveScale);
			if (segment != -1) {
				bez->SplitSegment(segment, 0);
				FixControlPoints();
			}
		}
	}
	else if (pickedShape < pointsStartIndx + 19) { // max 18 control points, 1D-BEZ
		int currPoint = pickedShape - pointsStartIndx;
		int pointIndx = currPoint % 3;
		int pointSeg = currPoint / 3;
		if (pointIndx != 0) { //pressed p1 or p2
			glm::vec4 center = bez->GetControlPoint(pointSeg, pointIndx == 1 ? 0 : 3);
			glm::vec4 point = bez->GetControlPoint(pointSeg, pointIndx);
			float angle = getAngle(center, point, (x - xprev), (y - yprev));
			glm::vec4 nextLoc = rotatePoint(center, point, angle);
			float xOffset = nextLoc.x - point.x;
			float yOffset = nextLoc.y - point.y;
			bez->CurveUpdate(currPoint, xOffset, yOffset, false);
			RelocateControlPoint(pointSeg, pointIndx);
			if (isContinuityState) {
				if (currPoint % 3 == 1) {//p1 moves p-2
					if (pointSeg != 0) {
						point = bez->GetControlPoint(pointSeg - 1, 2);
						nextLoc = rotatePoint(center, point, angle);
						xOffset = nextLoc.x - point.x;
						yOffset = nextLoc.y - point.y;
						bez->CurveUpdate(currPoint - 2, xOffset, yOffset, false);
						RelocateControlPoint(pointSeg - 1, 2);
					}
				}
				else {//p2 moves p+1
					if (pointSeg != bez->GetSegmentsNum()) {
						point = bez->GetControlPoint(pointSeg + 1, 1);
						nextLoc = rotatePoint(center, point, angle);
						xOffset = nextLoc.x - point.x;
						yOffset = nextLoc.y - point.y;
						bez->CurveUpdate(currPoint + 2, xOffset, yOffset, false);
						RelocateControlPoint(pointSeg + 1, 1);
					}
				}
			}
		}
		else if (pointSeg != bez->GetSegmentsNum() && pointSeg != 0) { //option b
			glm::vec4 line = getLine(bez->GetControlPoint(pointSeg, 0), bez->GetControlPoint(pointSeg, 1));
			glm::vec4 p2 = bez->GetControlPoint(pointSeg - 1, 2);
			float yOffset = line[0] * p2.x + line[1] - p2.y;
			bez->CurveUpdate(currPoint - 1, 0, yOffset, false);
			RelocateControlPoint(pointSeg - 1, 2);
		}
		Update3DBezier();
		pickedShape = currPoint + pointsStartIndx;
	}
	else { // plane cannot be picked so there is not need to check??
		MoveFixToView(View, glm::vec4((x - xprev) * rotScale3d, (y - yprev) * rotScale3d, 0, 0), false);
	}
}

void Game2::WhenTranslate(glm::mat4 View)
{
	float transScale3d = 10;
	float xOffset = (x - xprev);
	float yOffset = (y - yprev);
	if (xOffset > 0.1 || yOffset > 0.1)
		return;
	if (pickedShapes.size() != 0) { //2D-BEZ, multi pick
		for (int i = 0; i < pickedShapes.size(); i++) {
			pickedShape = pickedShapes[i];
			MoveFixToView(View, glm::vec4(xOffset * transScale3d, yOffset * transScale3d, 0, 0), true);
		}
	}
	else if (pickedShape < pointsStartIndx) {
		if (x > 1) { // 1D-BEZ, d - convex hull
			int segment = bez->GetSectionIsMouseInConvexHull(2 * (x - 1.5) / curveScale, (2 * y - 1) / curveScale);
			if (segment == -1)
				return;
			for (int i = 0; i < 4; i++) {
				bez->CurveUpdate(segment * 3 + i, xOffset, yOffset, false);
				MoveControlPoint(segment * 3 + i, xOffset, yOffset);
			}
			if (isContinuityState) {
				if (segment != bez->GetSegmentsNum() - 1) { //lastSeg
					bez->CurveUpdate((segment + 1) * 3 + 1, xOffset, yOffset, false);
					MoveControlPoint((segment + 1) * 3 + 1, xOffset, yOffset);

				}
				if (segment != 0) { //firstSeg
					bez->CurveUpdate(segment * 3 - 1, xOffset, yOffset, false);
					MoveControlPoint(segment * 3 - 1, xOffset, yOffset);
				}
			}
			Update3DBezier();
		}
	}
	else if (pickedShape < pointsStartIndx + 19) { // max 18 control points, 1D-BEZ
		int currPoint = pickedShape - pointsStartIndx;
		int pointSeg = currPoint / 3;
		int pointIndx = currPoint % 3;
		if (pointIndx == 0) {
			bez->CurveUpdate(currPoint, xOffset, yOffset, false);
			if (pointSeg != bez->GetSegmentsNum()) { //lastPoint
				bez->CurveUpdate(currPoint + 1, xOffset, yOffset, false);
				RelocateControlPoint((currPoint + 1) / 3, (currPoint + 1) % 3);
			}
			else {
				pointSeg -= 1;
				pointIndx = 3;
			}
			if (currPoint != 0) { //firstPoint
				bez->CurveUpdate(currPoint - 1, xOffset, yOffset, false);
				RelocateControlPoint((currPoint - 1) / 3, (currPoint - 1) % 3);
			}
		}
		else if (isContinuityState) {
			glm::vec4 p = bez->GetControlPoint(pointSeg, currPoint % 3);
			glm::vec4 line;
			if (currPoint % 3 == 1) {//p1 moves tg with p0
				line = getLine(bez->GetControlPoint(pointSeg, 0), bez->GetControlPoint(pointSeg, 1));
			}
			else {//p2 moves tg with p3
				line = getLine(bez->GetControlPoint(pointSeg, 2), bez->GetControlPoint(pointSeg, 3));
			}
			//yOffset = p2.y - p1.y = lineY - p1.y
			yOffset = line[0] * (p.x + xOffset) + line[1] - p.y;
			bez->CurveUpdate(currPoint, xOffset, yOffset, false);
		}
		else {
			bez->CurveUpdate(currPoint, xOffset, yOffset, false);
		}
		RelocateControlPoint(pointSeg, pointIndx);
		Update3DBezier();
		pickedShape = currPoint + pointsStartIndx;
	}
	else { // plane cannot be picked so there is not need to check??
		MoveFixToView(View, glm::vec4(xOffset * transScale3d, yOffset * transScale3d, 0, 0), true);
	}
}

#pragma region garbage
void Game2::Update(const glm::mat4& View, const glm::mat4& Projection, const glm::mat4& Model, const int  shaderIndx)
{
	if (counter)
		counter++;
	Shader* s = shaders[shaderIndx];
	// r = pickedShape + 1 is great, but why is the bit shifting here? it looks like it does nothing
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	if (shapes[pickedShape]->GetMaterial() >= 0 && !materials.empty())
		BindMaterial(s, shapes[pickedShape]->GetMaterial());
	//textures[0]->Bind(0);
	s->Bind();

	s->SetUniformMat4f("View", View);
	s->SetUniformMat4f("Proj", Projection);
	s->SetUniformMat4f("Model", Model);

	s->SetUniform1i("sampler1", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(0));
	if (shaderIndx != 1)
		s->SetUniform1i("sampler2", materials[shapes[pickedShape]->GetMaterial()]->GetSlot(1));
	if (shaderIndx != 0)
		s->SetUniform4f("lightColor", 0.2f, 0.9f, 0.2f, 0.5f); // just a color and alpha = 0.5 for blending
	else
		s->SetUniform4f("lightColor", r / 255.0, g / 255.0, b / 255.0, 0);
	s->SetUniform4f("eyePosition", -3, 0, 4, 0); // for the light
	s->SetUniform1f("x", x);
	s->Unbind();
}

void Game2::UpdatePosition(float xpos, float ypos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	xprev = x;
	yprev = y;
	x = xpos / viewport[2];
	y = 1 - ypos / viewport[3];
}

void Game2::Motion()
{
	if(isActive)
	{
		/*pickedShape = pointsStartIndx + 6 * 3 + 1;
		ShapeTransformation(yRotate, 0.1);
		pickedShape = -1;*/
	}
}

unsigned int Game2::TextureDesine(int width, int height)
{
	unsigned char* data = new unsigned char[width * height * 4];
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			data[(i * height + j) * 4] = (i + j) % 256;
			data[(i * height + j) * 4 + 1] =  (i + j * 2) % 256;
			data[(i * height + j) * 4 + 2] = (i * 2 + j) % 256;
			data[(i * height + j) * 4 + 3] = (i * 3 + j) % 256;
		}
	}
	textures.push_back(new Texture(width, height));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note GL_RED internal format, to save 
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;
	return(textures.size() - 1);
}

Game2::~Game2(void)
{

}
#pragma endregion
