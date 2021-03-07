#pragma once
#include <vector>
#include <list>
#include "camera.h"
#include "scene.h"
#include "DrawBuffer.h"

struct DrawInfo
{
	int viewportIndx;
	int cameraIndx;
	int shaderIndx;
	int buffer;
	unsigned int flags;


	DrawInfo(int view, int camera, int shader, int buff, unsigned int _flags = 0)
	{
		viewportIndx = view;
		cameraIndx = camera;
		shaderIndx = shader;
		buffer = buff;
		flags = _flags;
	}

	inline void SetCamera(int indx)
	{
		cameraIndx = indx;
	}

	inline void SetFlags(unsigned int value) { flags = flags | value; }
	inline void ClearFlags(unsigned int value) { flags = flags - value; }


};

class Renderer
{
public:
	enum buffersMode { COLOR, DEPTH, STENCIL, BACK, FRONT, NONE };
	enum transformations { xTranslate, yTranslate, zTranslate, xRotate, yRotate, zRotate, xScale, yScale, zScale, xCameraTranslate, yCameraTranslate, zCameraTranslate };
	enum drawFlags { toClear = 1, is2D = 2, inAction = 4, scissorTest = 8, depthTest = 16, stencilTest = 32, blend = 64, blackClear = 128, debugMode = 256};

	Renderer();
	Renderer(float angle, float relationWH, float near, float far);
	void Init(Scene* scene,  std::list<int>xViewport,  std::list<int>yViewport);
	void SetScene(Scene* userPointer);
	inline Scene* GetScene() const { return scn; };
	void Draw(int infoIndx = 1);
	void DrawAll();


	void Resize(int width, int height);
	void UpdatePosition(float xpos, float ypos);
	void AddCamera(const glm::vec3& pos, float fov, float relationWH, float zNear, float zFar, int infoIndx = -1);
	void AddViewport(int left, int bottom, int width, int height);
	unsigned int AddBuffer(int infoIndx, bool stencil = false);
	int Create2Dmaterial(int texsNum);
	void AddDraw(int view, int camera, int shader, int buff, unsigned int flags);
	void CopyDraw(int infoIndx, int buff = 0);
	void SetViewport(int left, int bottom, int width, int height, int indx);
	inline void BindViewport2D(int indx) { drawInfo[indx]->SetFlags(is2D); }
	void MoveCamera(int cameraIndx, int type, float amt);
	bool Picking(int x, int y);
	void MouseProccessing(int button);
	inline float GetNear(int cameraIndx) { return cameras[cameraIndx]->GetNear(); }
	inline float GetFar(int cameraIndx) { return cameras[cameraIndx]->GetFar(); }
	inline float GetAngle(int cameraIndx) { return cameras[cameraIndx]->GetAngle(); }
	inline void SetDrawFlag(int infoIndx,unsigned int flag) { drawInfo[infoIndx]->SetFlags(flag); }
	inline void ClearDrawFlag(int infoIndx, unsigned int flag) { drawInfo[infoIndx]->ClearFlags(flag); }
	bool checkViewport(int x, int y, int viewportIndx);
	~Renderer();
private:
	std::vector<Camera*> cameras;
	Scene* scn;
	float depth;
	int xold, yold, xrel, yrel;
	void Clear(float r, float g, float b, float a);
	void ActionDraw();
	std::vector<glm::ivec4> viewports;
	std::vector<DrawInfo*> drawInfo;
	std::vector<DrawBuffer*> buffers;
	int materialIndx2D;
	int toDrawIndx;
	
};

