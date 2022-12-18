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
	GLuint shaderProgram,
		cubeVBO, cubeVAO, cubeEBO, planeVBO, planeVAO, planeEBO,
		lampVAO, lampVBO, lampEBO, tilesVBO, tilesVAO, tilesEBO,
		stoolVAO, stoolVBO, stoolEBO, 
		ywalltex, sywalltex, plane_texture, stexture2,
		tilestex, stilestex, tiles2tex, stiles2tex, lamptex, slamptex,
		concretetex, sconcretetex, tabletex, stabletex,
		bartex, sbartex, woodtex, swoodtex, wood2tex, swood2tex,
		metaltex, smetaltex;

	float randX[4] = {0, 0, 0, 0},
		randZ[4] = { 0, 0, 0, 0 };

	float angle = 0;

	virtual void Init();
	virtual void DeInit();
	virtual void Update(double deltaTime);
	virtual void Render();
	virtual void ProcessInput(GLFWwindow *window);

	// camera
	float viewCamX, viewCamY, viewCamZ, upCamX, upCamY, upCamZ, posCamX, posCamY, posCamZ, CAMERA_SPEED, CAMERA_Y_SPEED, fovy;
	void MoveCamera(float speed);
	void MoveYCamera(float speed);
	void StrafeCamera(float speed);
	void RotateCamera(float speed);
	void InitCamera();

	// build 
	void BuildCube();
	void BuildPyramid();
	void BuildDiagonal();
	void BuildPlane();
	void BuildTiles();
	void BuildTexture();

	// draw
	void DrawWall();
	void DrawStairs(int n);
	void DrawBanister(int n);
	void DrawBar();
	void DrawSink();
	void DrawCafeTable();
	void DrawStool();
	void DrawLamp();
	void DrawQopee();
	void DrawShelf();
	void DrawBoundary();
	void DrawFloor();
	void DrawPlane();
};

