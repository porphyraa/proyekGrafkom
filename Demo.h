#pragma once
#include "RenderEngine.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <SOIL/SOIL.h>
#include "glm/gtc/type_ptr.hpp"

class Demo :
	public RenderEngine
{
public:
	Demo();
	~Demo();
private:
	GLuint shaderProgram, cubeVBO, cubeVAO, cubeEBO, ywalltex, sywalltex, planeVBO, planeVAO, planeEBO, plane_texture,  stexture2,
		tilesVBO, tilesVAO, tilesEBO, tilestex, stilestex, tiles2tex, stiles2tex, lampVAO, lampVBO, lampEBO, lamptex, slamptex,
		concretetex, sconcretetex, tabletex, stabletex;
	
	virtual void Init();
	virtual void DeInit();
	virtual void Update(double deltaTime);
	virtual void Render();
	virtual void ProcessInput(GLFWwindow *window);

	// camera
	float viewCamX, viewCamY, viewCamZ, upCamX, upCamY, upCamZ, posCamX, posCamY, posCamZ, CAMERA_SPEED, fovy;
	void MoveCamera(float speed);
	void StrafeCamera(float speed);
	void RotateCamera(float speed);
	void InitCamera();

	// build 
	void BuildCube();
	void BuildPyramid();
	void BuildPlane();
	void BuildTiles();
	void BuildTexture();

	// draw
	void DrawWall();
	void DrawStairs(int n);
	void DrawCafeTable();
	void DrawLamp();
	void DrawFloor();
	void DrawPlane();
};

