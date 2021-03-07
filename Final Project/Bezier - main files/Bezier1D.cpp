#include "Bezier1D.h"
#include <iostream>
#include "GL/glew.h"
#include <renderer.h>

static void printMat(const glm::mat4 mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << mat[j][i] << " ";
		std::cout << std::endl;
	}
}

float max(float a, float b) {
	return a > b ? a : b;
}
float min(float a, float b) {
	return a < b ? a : b;
}

void Bezier1D::MoveControlPoint(int segment, int indx, float dx, float dy, bool preserveC1)
{
	glm::vec4 cp = segments[segment][indx];
	segments[segment][indx] = glm::vec4(cp.x + dx, max(cp.y + dy , 0), 0, 0);
	/*if (!preserveC1)
		return;
	if (indx == 1)
		CurveUpdate(segment * 3, dx, dy, false);
	else if (indx == 2) 
		CurveUpdate((segment + 1) * 3, dx, dy, false);*/
}

Bezier1D::Bezier1D(int segNum, int res, int mode, int viewport)
	:Shape(Scene::modes::LINE_STRIP, mode)
{
	M = glm::mat4(1, 0, 0, 0, -3, 3, 0, 0, 3, -6, 3, 0, -1, 3, -3, 1);
	DM = glm::mat4(-3, 3, 0, 0, 6, -12, 6, 0, -3, 9, -9, 3, 0, 0, 0, 0);
	resT = res;
	convecHullUnit = 0.1;

	segments = { glm::mat4(-1, 0, 0, 0, -1, 0.24, 0, 0, -0.84, 0.48, 0, 0, -0.5, 0.5, 0, 0) };
	if (segNum > 2) {
		segNum = min(segNum, 6);
		float itval = 1. / (segNum - 2);
		for (int i = 0; i < segNum - 2; i++)
			AddSegment(glm::vec4(-0.5 + (3 * i + 1) * itval / 3, 0.5, 0, 0),
				glm::vec4(-0.5 + (3 * i + 2) * itval / 3, 0.5, 0, 0),
				glm::vec4(-0.5 + (i + 1) * itval, 0.5, 0, 0));
	}
	AddSegment(glm::vec4(0.88, 0.53, 0, 0), glm::vec4(1, 0.18, 0, 0), glm::vec4(1, 0, 0, 0));

	mesh = new MeshConstructor(GetLine(), false);
}

IndexedModel Bezier1D::GetLine() {
	IndexedModel line;
	for (int seg = 0; seg < GetSegmentsNum(); seg++) {
		for (int i = 0; i < resT; i++) {
			glm::vec4 pos = GetPointOnCurve(seg, i);
			line.positions.push_back(glm::vec3(pos));
			line.colors.push_back(glm::vec3(0, 1, 1));
			line.normals.push_back(glm::vec3());
			line.texCoords.push_back(glm::vec2(pos.x, pos.y));
		}
	}
	line.indices.push_back(0);
	for (int i = 1; i < GetSegmentsNum() * resT - 1; i++) {
		line.indices.push_back(i);
		line.indices.push_back(i);
	}
	line.indices.push_back(GetSegmentsNum() * resT - 1);
	return line;
}

glm::vec4 Bezier1D::GetControlPoint(int segment, int indx) const
{
	return segments[segment][indx];
}

glm::vec4 Bezier1D::GetPointOnCurve(int segment, int t)
{
	float x = (float)t / resT;
	return segments[segment] * M * glm::vec4(1, x, x * x, x * x * x);
}

glm::vec3 Bezier1D::GetVelosity(int segment, int t)
{
	float x = (float)t / resT;
	return glm::vec3(segments[segment] * DM * glm::vec4(1, x, x * x, x * x * x));
}

void Bezier1D::SplitSegment(int segment, int t)
{
	if (GetSegmentsNum() == 6)
		return;
	glm::mat4 seg = segments[segment];
	glm::vec4 center = GetPointOnCurve(segment, resT / 2);
	glm::vec4 p12 = (seg[1] + seg[2]) * 0.5f;
	glm::vec4 l1 = (seg[0] + seg[1]) * 0.5f;
	glm::vec4 r2 = (seg[2] + seg[3]) * 0.5f;
	glm::mat4 segL = glm::mat4(seg[0], l1, 0.5f * (l1 + p12), center);
	glm::mat4 segR = glm::mat4(center, 0.5f * (r2 + p12), r2, seg[3]);
	segments[segment] = segR;
	segments.insert(segments.begin() + segment, segL);
}

void Bezier1D::AddSegment(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3)
{
	if (GetSegmentsNum() == 6)
		return;
	glm::vec4 p0 = segments[GetSegmentsNum()-1][3];
	segments.push_back(glm::mat4(p0, p1, p2, p3));
}

void Bezier1D::CurveUpdate(int pointIndx, float dx, float dy, bool preserveC1)
{
	if (pointIndx == GetSegmentsNum() * 3) {
		MoveControlPoint((pointIndx / 3) - 1, 3, dx, dy, preserveC1);
	}
	else if (pointIndx % 3 == 0 && pointIndx != 0) { // shared point
		int seg = pointIndx / 3;
		MoveControlPoint(seg - 1, 3, dx, dy, preserveC1);
		MoveControlPoint(seg, 0, dx, dy, preserveC1);
	}
	else {
		int seg = pointIndx / 3;
		MoveControlPoint(seg, pointIndx - seg * 3, dx, dy, preserveC1);
	}
	mesh = new MeshConstructor(GetLine(), false);
}

float pCross(glm::vec4 p1, glm::vec4 p2) { return p1.x * p2.y - p1.y * p2.x; }
float area(glm::vec4 p1, glm::vec4 p2, glm::vec4 p3)
{
	return abs(pCross(p2 - p1, p3 - p1));
}
bool isInConvexHull(glm::vec4 p, glm::vec4 a, glm::vec4 b, glm::vec4 c) {
	float s1 = area(a, b, c);
	float s2 = area(p, a, b) + area(p, b, c) + area(p, c, a);
	return abs(s1 - s2) < 1e-8;
}

int Bezier1D::GetSectionIsMouseInConvexHull(float x, float y) {
	int segment = GetSegmentsNum() - 1;
	for (int i = 0; i < GetSegmentsNum(); i++) {
		if (segments[i][0].x > x) {
			segment = i - 1;
			break;
		}
	}
	glm::vec4 p = glm::vec4(x, y, 0, 0);
	if (isInConvexHull(p, segments[segment][0], segments[segment][1], segments[segment][3]) ||
		isInConvexHull(p, segments[segment][0], segments[segment][2], segments[segment][3]) ||
		isInConvexHull(p, segments[segment][1], segments[segment][2], segments[segment][3]))
		return segment;
	//float t = (x - segments[segment][0].x) / (segments[segment][3].x - segments[segment][0].x);
	//glm::vec4 p = segments[segment] * M * glm::vec4(1, t, t * t, t * t * t);
	//if(p.y + convecHullUnit >= y && y >= p.y - convecHullUnit)
		//return segment;
	return -1;
}
Bezier1D::~Bezier1D(void)
{
}
