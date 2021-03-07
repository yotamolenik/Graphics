#pragma once
#include "MovableGLM.h"
#include "VertexArray.hpp"
#include "shader.h"
#include "MeshConstructor.h"

class Shape : public MovableGLM
{
private:

	MeshConstructor* mesh;
	unsigned int materialID;
	int shaderID;
	bool isCopy;
	unsigned int mode;
	unsigned int viewports;

public:

	Shape(const Shape& shape, unsigned int mode);

	Shape(const std::string& fileName, unsigned int mode);

	Shape(const int SimpleShapeType, unsigned int mode);

	void Draw(const Shader* shaders, bool isPicking);

	inline void AddViewport(int viewport) { viewports = viewports | (1<<viewport); }

	inline void RemoveViewport(int viewport) { viewports = viewports & ~(1 << viewport); }

	inline bool Is2Render(int viewport) { return  viewports & (1 << viewport); }

	inline void SetShader(const int id) { shaderID = id; }

	inline int GetShader() { return shaderID; }

	inline void SetMaterial(const unsigned int id) { materialID = id; }

	inline unsigned int GetMaterial() { return materialID; }

	virtual ~Shape(void);
};

