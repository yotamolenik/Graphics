#include "Bezier2D.h"
#include <renderer.h>
#include <iostream>
#include "glm/gtc/matrix_transform.hpp"

const float PI = 2 * acos(0.0);
const float scaleMovement = 10;
void Bezier2D::AddToModel(IndexedModel& model, float s, float t, const std::vector<glm::vec4> subSurf, int subIndx)
{
}

glm::vec4 Bezier2D::CalcNurbs(float s, float t, const std::vector<glm::vec4> subSurf)
{
	return glm::vec4();
}

glm::vec3 Bezier2D::CalcNormal(float s, float t, const std::vector<glm::vec4> subSurf)
{
	return glm::vec3();
}

int fact(int n)
{
	int res = 1;
	for (int i = 2; i <= n; i++)
		res = res * i;
	return res;
}

int nCr(int n, int r)
{
	return fact(n) / (fact(r) *
		fact(n - r));
}

//int nCr(int n, int k) {
//    if (k == 0 || k == n)
//        return 1;
//    return nCr(n - 1, k - 1) + nCr(n - 1, k);
//}

// we have a small compilation error in the multiplication of subSurf

//glm::vec4 dSBezier(float s, float t, std::vector<glm::vec4> subSurf, int resT) {
//    glm::vec4 p0 = glm::vec4();
//    glm::vec4 p1 = glm::vec4();
//    glm::vec4 p2 = glm::vec4();
//    glm::vec4 p3 = glm::vec4();
//    glm::vec4 co = glm::vec4(-3 * (1 - t) * (1 - t), (3 * (1 - t) * (1 - t) - 6 * t * (1 - t)),
//        (6 * t * (1 - t) - 3 * t * t), 3 * t * t);
//
//    for (int i = 0; i < 4; ++i) {
//        p0 += nCr(resT, i) * pow((1 - s), i) * pow(s, resT - i) * subSurf[0 * resT + i];
//        p1 += nCr(resT, i) * pow((1 - s), i) * pow(s, resT - i) * subSurf[1 * resT + i];
//        p2 += nCr(resT, i) * pow((1 - s), i) * pow(s, resT - i) * subSurf[2 * resT + i];
//        p3 += nCr(resT, i) * pow((1 - s), i) * pow(s, resT - i) * subSurf[3 * resT + i];
//    }
//
//    return co[0]*p0 + co[1] * p1 + co[2] * p2 + co[3] * p3;
//}
//glm::vec4 dTBezier(float s, float t, std::vector<glm::vec4> subSurf, int resS) {
//    glm::vec4 p0 = glm::vec4();
//    glm::vec4 p1 = glm::vec4();
//    glm::vec4 p2 = glm::vec4();
//    glm::vec4 p3 = glm::vec4();
//    glm::vec4 co = glm::vec4(-3 * (1 - s) * (1 - s), (3 * (1 - s) * (1 - s) - 6 * s * (1 - s)),
//        (6 * s * (1 - s) - 3 * s * s), 3 * s * s);
//
//    for (int i = 0; i < 4; ++i) {
//        p0 += nCr(resS, i) * pow((1 - t), i) * pow(t, resS - i) * subSurf[i * resS + 0];
//        p1 += nCr(resS, i) * pow((1 - t), i) * pow(t, resS - i) * subSurf[i * resS + 1];
//        p2 += nCr(resS, i) * pow((1 - t), i) * pow(t, resS - i) * subSurf[i * resS + 2];
//        p3 += nCr(resS, i) * pow((1 - t), i) * pow(t, resS - i) * subSurf[i * resS + 3];
//    }
//
//    return co[0] * p0 + co[1] * p1 + co[2] * p2 + co[3] * p3;
//}
//
glm::vec3 crossProduct(glm::vec3 v1, glm::vec3 v2) {
    return glm::vec3(v1.y * v2.z - v1.z * v2.y, -(v1.x * v2.y - v1.z * v2.x), v1.x * v2.y - v1.y * v2.x);
}
//
//glm::vec3 Bezier2D::CalcNormal(float s, float t, const std::vector<glm::vec4> subSurf)
//{
//    glm::vec4 dS = dSBezier(s, t, subSurf, resT);
//    glm::vec4 dT = dTBezier(s, t, subSurf, resS);
//	return crossProduct(dS, dT);
//}

glm::vec3 rotatePoint(glm::vec3 center, glm::vec3 point, float angle= PI /4) {
    point -= center;
    float s = sin(angle);
    float c = cos(angle);
    return center + glm::vec3(0, point.x * c - point.y * s, point.x * s + point.y * c);
}

void Bezier2D::CalcControlPoints(Bezier1D* c)
{
	int segment = 0;
    std::vector<glm::vec4> XY;
	std::vector<glm::vec4> ZY;

	XY = {};
	ZY = {};
	for (int i = 0; i < 4; i++) {
		glm::vec4 cp = c->GetPointOnCurve(segment, i);
		XY.push_back(cp);
		ZY.push_back(glm::vec4(cp.x, 0, cp.y, 0));
	}
	/*for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			glm::vec4 cp = ZY[col] * XY[row];
			std::cout << "(" << cp.x << ", " << cp.y << ", " << cp.z << ")-";
			controlPoints.push_back(ZY[col] * XY[row]);
		}
		std::cout << std::endl;
	}*/
	for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
			std::cout << "(" << XY[i].x << ", " << XY[i].y << ", " << XY[i].z << ")-";
            bool isY = j % 2;
            int isPos =  1 - 2*(j / 2);
            controlPoints.push_back(glm::vec4(XY[i].x, isY ? isPos* XY[i].y : 0, !isY ? isPos * XY[i].y : 0, 0));
        }
		std::cout << std::endl;
    }
}

Bezier2D::Bezier2D(Bezier1D* c, int _resS, int _resT, int mode, int viewport)
	: Shape(Scene::modes::QUADS, mode)
{
	resT = _resT;
	resS = _resS;
	UpdateBezier(c);
}

void Bezier2D::UpdateBezier(Bezier1D* c) {
	bez1 = c;
	mesh = new MeshConstructor(GetSurface(), true);
}

glm::vec3 getNormal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2){
	return crossProduct(p1 - p0, p2 - p0);
}

IndexedModel Bezier2D::GetSurface()
{
	IndexedModel surface;

	for(int segment = 0; segment < bez1->GetSegmentsNum(); segment++){
		for (int row = 0; row < resS + 1; row++) { // x
			glm::vec4 cp = bez1->GetPointOnCurve(segment, row);
			for (int col = 0; col < resT; col++) { // y
				glm::vec3 pos = rotatePoint(glm::vec3(cp.x, 0, 0), glm::vec3(cp), 2 * PI * col / resT);
				surface.positions.push_back(pos);
				surface.colors.push_back(glm::vec3(0, 1, 1));
				surface.normals.push_back(glm::vec3(0));
				surface.texCoords.push_back(glm::vec2(cp.x, cp.y));
			}
		}
	}

	for (int seg = 0; seg < bez1->GetSegmentsNum() - 1; seg++) {
		for (int row = 0; row < resS + 1; row++) { // x
			int i = seg * resS * resT + row * resT;
			surface.normals[i] = getNormal(
				surface.positions[i + resT - 1],
				surface.positions[i],
				surface.positions[i + resT]);
			for (int col = 1; col < resT; col++) { // y
				surface.normals[i + col] = getNormal(
					surface.positions[i + col - 1],
					surface.positions[i + col],
					surface.positions[i + resT + col]);
			}
		}
	}
	
	for (int row = 0; row < resS; row++) { // x
		int i = (bez1->GetSegmentsNum() - 1) * resS * resT + row * resT;
		surface.normals[i] = getNormal(
			surface.positions[i + resT - 1],
			surface.positions[i],
			surface.positions[i + resT]);
		for (int col = 1; col < resT; col++) { // y
			surface.normals[i + col] = getNormal(
				surface.positions[i + col - 1],
				surface.positions[i + col],
				surface.positions[i + resT + col]);
		}
	}

	for (int seg = 0; seg < bez1->GetSegmentsNum(); seg++) {
		for (int row = 0; row < resS + 1; row++) {
			for (int col = 0; col < resT - 1; col++) {
				surface.indices.push_back(seg * resS * resT + row * resT + col);
				surface.indices.push_back(seg * resS * resT + row * resT + (col + 1));
				surface.indices.push_back(seg * resS * resT + (row + 1) * resT + (col + 1));
				surface.indices.push_back(seg * resS * resT + (row + 1) * resT + col);
			}
			surface.indices.push_back(seg * resS * resT + row * resT + resT - 1);
			surface.indices.push_back(seg * resS * resT + row * resT + 0);
			surface.indices.push_back(seg * resS * resT + (row + 1) * resT + 0);
			surface.indices.push_back(seg * resS * resT + (row + 1) * resT + resT - 1);
		}
	}

	return surface;
}

glm::vec4 Bezier2D::GetPointOnSurface(int segmentS, int segmentT, int s, int t)
{
	return glm::vec4(0,0,0,0);
}

//we multiply a vector in " * controlPoints[_ij]"
//glm::vec4 Bezier2D::GetPointOnSurface(int segmentS, int segmentT, int s, int t)
//{
//	float tt = t / resT;
//	float ss = s / resS;
//	glm::vec4 sum(0, 0, 0, 0);
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			int _ij = i * 4 + j;
//			sum += nCr(4, i) * nCr(4, j) * pow(1 - ss, i) * pow(1 - tt, j) * pow(ss, 4 - i) * pow(tt, 4 - j) * controlPoints[_ij];
//		}
//	}
//	return sum;
//
//}

glm::vec3 Bezier2D::GetNormal(int segmentS, int segmentT, int s, int t)
{
	return glm::vec3();
	//return CalcNormal(s, t, controlPoints);
}

Bezier2D::~Bezier2D(void)
{
}


//void Bezier2D::MyTranslate(const glm::vec3 delta, int mode)
//{
//	trans = glm::translate(trans, delta);
//	pos = trans * pos;
//	std::cout << "myTranslate: pos.x: " << pos.x << std::endl;
//}


//void Bezier2D::rotateBezier(float xoffset, float yoffset)
//{
//	MyRotate(xoffset, glm::vec3(0, 1, 0), 0);
//	MyRotate(yoffset, glm::vec3(1, 0, 0), 0);
//}
//
//void Bezier2D::translateBezier(float xoffset, float yoffset)
//{
//	//pos.x += (xoffset / scaleMovement);
//	//pos.y += yoffset / scaleMovement;
//	MyTranslate(glm::vec3(xoffset, yoffset, 0), 0);
//}




