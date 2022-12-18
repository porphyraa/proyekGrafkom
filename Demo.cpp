#include "Demo.h"

Demo::Demo() {

}

Demo::~Demo() {
}

void Demo::Init() {
	// build and compile our shader program
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);

	BuildCube();
	BuildPyramid();
	BuildDiagonal();
	BuildPlane();
	BuildTiles();
	BuildTexture();

	InitCamera();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &planeEBO);
	glDeleteVertexArrays(1, &lampVAO);
	glDeleteBuffers(1, &lampVBO);
	glDeleteBuffers(1, &lampEBO);
	glDeleteVertexArrays(1, &tilesVAO);
	glDeleteBuffers(1, &tilesVBO);
	glDeleteBuffers(1, &tilesEBO);
	glDeleteVertexArrays(1, &stoolVAO);
	glDeleteBuffers(1, &stoolVBO);
	glDeleteBuffers(1, &stoolEBO);
}

void Demo::ProcessInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.001f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		float range = .3,
			div = RAND_MAX / range;

		for (int i = 0; i < 4; i++) {
			randX[i] = -.15 + (rand() / div);
			randZ[i] = -.15 + (rand() / div);
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		MoveYCamera(CAMERA_Y_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		MoveYCamera(-CAMERA_Y_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth / 2;
	double midY = screenHeight / 2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 750;
	angleZ = (float)((midY - mouseY)) / 750;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);
}

void Demo::Update(double deltaTime) {
	angle += (float)((deltaTime * 1.5f) / 1000);
}

void Demo::Render() {
	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// set lighting attributes
	// dir light
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.direction"), 0.0f, -1.0f, -1.0f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.ambient"), .15, .15, .15);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.diffuse"), .0, 0, 0);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "dirLight.specular"), 0, 0, 0);
	// point light 1
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].position"), -4.15, 2.25, -2.75);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].ambient"), .35, .285, .015);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].diffuse"), .35, .285, .015);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[0].specular"), .35, .285, .015);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].constant"), 1);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].linear"), 0.14f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[0].quadratic"), 0.07f);
	// point light 2
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].position"), -5.5, 2.25, -2.75);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].ambient"), .35, .285, .015);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].diffuse"), .35, .285, .015);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[1].specular"), .35, .285, .015);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].constant"), 1);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].linear"), 0.14f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[1].quadratic"), 0.07f);
	// point light 3
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].position"), -5.801, 3.1, 1.5);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].ambient"), .35, .285, .015);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].diffuse"), .7, .57, .03);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[2].specular"), .7, .57, .03);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].constant"), 1);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].linear"), .07);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[2].quadratic"), 1.8);
	// point light 4
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].position"), -5.801, 3.9, 1.5);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].ambient"), .35, .285, .015);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].diffuse"), .7, .57, .03);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[3].specular"), .7, .57, .03);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].constant"), 1);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].linear"), .07);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[3].quadratic"), 1.8);
	// point light 3
	//glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[4].position"), -5.551, 3.5, 1.5);
	//glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[4].ambient"), .35, .285, .015);
	//glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[4].diffuse"), .7, .57, .03);
	//glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[4].specular"), .7, .57, .03);
	//glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[4].constant"), 1);
	//glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[4].linear"), .07);
	//glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[4].quadratic"), 1.8);
	// point light 4
	//glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[5].position"), -6.051, 3.5, 1.5);
	//glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[5].ambient"), .35, .285, .015);
	//glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[5].diffuse"), .7, .57, .03);
	//glUniform3f(glGetUniformLocation(this->shaderProgram, "pointLights[5].specular"), .7, .57, .03);
	//glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[5].constant"), 1);
	//glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[5].linear"), .07);
	//glUniform1f(glGetUniformLocation(this->shaderProgram, "pointLights[5].quadratic"), 1.8);

	// spotLight
	glm::vec3 cameraPos = glm::vec3(-4.15, 3.75, -2.25);
	glm::vec3 cameraFront = glm::vec3(0, -1, 0);
	glUniform3fv(glGetUniformLocation(this->shaderProgram, "spotLights[0].position"), 1, &cameraPos[0]);
	glUniform3fv(glGetUniformLocation(this->shaderProgram, "spotLights[0].direction"), 1, &cameraFront[0]);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLights[0].ambient"), .35, .285, .015);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLights[0].diffuse"), .7, .57, .03);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLights[0].specular"), .7, .57, .03);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[0].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[0].quadratic"), 0.032f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[0].cutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[0].outerCutOff"), glm::cos(glm::radians(15.0f)));

	cameraPos = glm::vec3(-5.5, 3.75, -2.25);
	glUniform3fv(glGetUniformLocation(this->shaderProgram, "spotLights[1].position"), 1, &cameraPos[0]);
	glUniform3fv(glGetUniformLocation(this->shaderProgram, "spotLights[1].direction"), 1, &cameraFront[0]);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLights[1].ambient"), .35f, .285f, .015f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLights[1].diffuse"), .7f, .57f, .03f);
	glUniform3f(glGetUniformLocation(this->shaderProgram, "spotLights[1].specular"), .7, .57, .03);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[1].linear"), 0.09f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[1].quadratic"), 0.032f);
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[1].cutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(glGetUniformLocation(this->shaderProgram, "spotLights[1].outerCutOff"), glm::cos(glm::radians(15.0f)));

	DrawWall();
	DrawStairs(13);
	DrawBanister(13);
	DrawBar();
	DrawSink();
	DrawCafeTable();
	DrawStool();
	DrawLamp();
	DrawQopee();
	DrawShelf();
	DrawBoundary();
	DrawFloor();
	DrawPlane();

	glDisable(GL_DEPTH_TEST);
}

void Demo::BuildCube()
{
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &ywalltex);
	glBindTexture(GL_TEXTURE_2D, ywalltex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("ywallDTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &sywalltex);
	glBindTexture(GL_TEXTURE_2D, sywalltex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("ywallSTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// front
		-0.5, -0.5, 0.5, 0, 0, 0.0f,  0.0f,  1.0f, // 0
		0.5, -0.5, 0.5, 1, 0,  0.0f,  0.0f,  1.0f, // 1
		0.5,  0.5, 0.5, 1, 1,  0.0f,  0.0f,  1.0f, // 2
		-0.5,  0.5, 0.5, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// upper
		0.5, 0.5,  0.5, 0, 0,   0.0f,  1.0f,  0.0f, // 16
		-0.5, 0.5, 0.5, 1, 0,   0.0f,  1.0f,  0.0f, // 17
		-0.5, 0.5, -0.5, 1, 1,  0.0f,  1.0f,  0.0f, // 18
		0.5, 0.5, -0.5, 0, 1,   0.0f,  1.0f,  0.0f, // 19

		// back
		-0.5, -0.5, -0.5, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		0.5,  -0.5, -0.5, 1, 0, 0.0f,  0.0f,  -1.0f, // 9
		0.5,   0.5, -0.5, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		-0.5,  0.5, -0.5, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// bottom
		-0.5, -0.5, -0.5, 0, 0, 0.0f,  -1.0f,  0.0f, // 20
		0.5, -0.5, -0.5, 1, 0,  0.0f,  -1.0f,  0.0f, // 21
		0.5, -0.5,  0.5, 1, 1,  0.0f,  -1.0f,  0.0f, // 22
		-0.5, -0.5,  0.5, 0, 1, 0.0f,  -1.0f,  0.0f, // 23

		// right
		 0.5,  0.5,  0.5, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		 0.5,  0.5, -0.5, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		 0.5, -0.5, -0.5, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		 0.5, -0.5,  0.5, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		 // left
		 -0.5, -0.5, -0.5, 0, 0, -1.0f,  0.0f,  0.0f, // 12
		 -0.5, -0.5,  0.5, 1, 0, -1.0f,  0.0f,  0.0f, // 13
		 -0.5,  0.5,  0.5, 1, 1, -1.0f,  0.0f,  0.0f, // 14
		 -0.5,  0.5, -0.5, 0, 1, -1.0f,  0.0f,  0.0f, // 15
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Demo::BuildPyramid()
{
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &lamptex);
	glBindTexture(GL_TEXTURE_2D, lamptex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("lampTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &slamptex);
	glBindTexture(GL_TEXTURE_2D, slamptex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("lampTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// bottom
		-0.5, -0.5, -0.5, 0, 0, 0.0f,  -1.0f,  0.0f, // 20
		0.5, -0.5, -0.5, 1, 0,  0.0f,  -1.0f,  0.0f, // 21
		0.5, -0.5,  0.5, 1, 1,  0.0f,  -1.0f,  0.0f, // 22
		-0.5, -0.5,  0.5, 0, 1, 0.0f,  -1.0f,  0.0f, // 23

		// front
		-0.5, -0.5, 0.5, 0, 0, 0.0f,  0.0f,  1.0f, // 0
		 0.5, -0.5, 0.5, 1, 0,  0.0f,  0.0f,  1.0f, // 1
		 0,  0.5, 0, 1, 1,  0.0f,  0.0f,  1.0f, // 2
		-0,  0.5, 0, 0, 1, 0.0f,  0.0f,  1.0f, // 3

		// back
		-0.5, -0.5, -0.5, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		 0.5,  -0.5, -0.5, 1, 0, 0.0f,  0.0f,  -1.0f, // 9
		 0,   0.5, -0, 1, 1, 0.0f,  0.0f,  -1.0f, // 10
		-0,  0.5, -0, 0, 1, 0.0f,  0.0f,  -1.0f, // 11

		// right
		 0,  0.5,  0, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		 0,  0.5, -0, 1, 0, 1.0f,  0.0f,  0.0f, // 5
		 0.5, -0.5, -0.5, 1, 1, 1.0f,  0.0f,  0.0f, // 6
		 0.5, -0.5,  0.5, 0, 1, 1.0f,  0.0f,  0.0f, // 7

		 // left
		 -0.5, -0.5, -0.5, 0, 0, -1.0f,  0.0f,  0.0f, // 12
		 -0.5, -0.5,  0.5, 1, 0, -1.0f,  0.0f,  0.0f, // 13
		 -0,  0.5,  0, 1, 1, -1.0f,  0.0f,  0.0f, // 14
		 -0,  0.5, -0, 0, 1, -1.0f,  0.0f,  0.0f, // 15
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &lampVAO);
	glGenBuffers(1, &lampVBO);
	glGenBuffers(1, &lampEBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(lampVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Demo::BuildDiagonal()
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// front
				-.5, -.5, .5, 0, 0, 0,  0,  1, // 0
		 .5, -.5, .5, 1, 0, 0,  0,  1, // 1
		 1.5,  .5, 1.5, 1, 1, 0,  0,  1, // 2
		0.5,  .5, 1.5, 0, 1, 0,  0,  1, // 3

		// upper
		 1.5, .5,  1.5, 0, 0, 0,  1,  0, // 16
		0.5, .5,  1.5, 1, 0, 0,  1,  0, // 17
		0.5, .5, 0.5, 1, 1, 0,  1,  0, // 18
		 1.5, .5, 0.5, 0, 1, 0,  1,  0, // 19

		// back
		-.5, -.5, -.5, 0, 0, 0,  0,  -1, // 8 
		 .5, -.5, -.5, 1, 0, 0,  0,  -1, // 9
		 1.5,  .5, 0.5, 1, 1, 0,  0,  -1, // 10
		0.5,  .5, 0.5, 0, 1, 0,  0,  -1, // 11

		// bottom
		-.5, -.5, -.5, 0, 0, 0,  -1,  0, // 20
		 .5, -.5, -.5, 1, 0, 0,  -1,  0, // 21
		 .5, -.5,  .5, 1, 1, 0,  -1,  0, // 22
		-.5, -.5,  .5, 0, 1, 0,  -1,  0, // 23

		// right
		 1.5,  .5,  1.5, 0, 0, 1,  0,  0, // 4
		 1.5,  .5, 0.5, 1, 0, 1,  0,  0, // 5
		 .5, -.5, -.5, 1, 1, 1,  0,  0, // 6
		 .5, -.5,  .5, 0, 1, 1,  0,  0, // 7

		 // left
		 -.5, -.5, -.5, 0, 0, -1,  0,  0, // 12
		 -.5, -.5,  .5, 1, 0, -1,  0,  0, // 13
		 0.5,  .5,  1.5, 1, 1, -1,  0,  0, // 14
		 0.5,  .5, 0.5, 0, 1, -1,  0,  0, // 15
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &stoolVAO);
	glGenBuffers(1, &stoolVBO);
	glGenBuffers(1, &stoolEBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(stoolVAO);

	glBindBuffer(GL_ARRAY_BUFFER, stoolVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, stoolEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Demo::BuildTiles()
{
	glGenTextures(1, &tilestex);
	glBindTexture(GL_TEXTURE_2D, tilestex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("tilesDTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &stilestex);
	glBindTexture(GL_TEXTURE_2D, stilestex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("tilesSTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	float vertices[] = {
		// front
		-4.05, -.5, 3, 0, 0, 0.0f,  0.0f,  1.0f, // 0
		 4.05, -.5, 3, 3, 0,  0.0f,  0.0f,  1.0f, // 1
		 4.05,  .5, 3, 3, 3,  0.0f,  0.0f,  1.0f, // 2
		-4.05,  .5, 3, 0, 3, 0.0f,  0.0f,  1.0f, // 3

		// upper
		 4.05, .5,  3, 0, 0,   0.0f,  1.0f,  0.0f, // 16
		-4.05, .5, 3, 3, 0,   0.0f,  1.0f,  0.0f, // 17
		-4.05, .5, -3, 3, 3,  0.0f,  1.0f,  0.0f, // 18
		 4.05, .5, -3, 0, 3,   0.0f,  1.0f,  0.0f, // 19

		// back
		-4.05, -.5, -3, 0, 0, 0.0f,  0.0f,  -1.0f, // 8 
		 4.05, -.5, -3, 3, 0, 0.0f,  0.0f,  -1.0f, // 9
		 4.05,  .5, -3, 3, 3, 0.0f,  0.0f,  -1.0f, // 10
		-4.05,  .5, -3, 0, 3, 0.0f,  0.0f,  -1.0f, // 11

		// bottom
		-4.05, -.5, -3, 0, 0, 0.0f,  -1.0f,  0.0f, // 20
		 4.05, -.5, -3, 3, 0,  0.0f,  -1.0f,  0.0f, // 21
		 4.05, -.5,  3, 3, 3,  0.0f,  -1.0f,  0.0f, // 22
		-4.05, -.5,  3, 0, 3, 0.0f,  -1.0f,  0.0f, // 23

		// right
		 4.05,  .5,  3, 0, 0, 1.0f,  0.0f,  0.0f, // 4
		 4.05,  .5, -3, 3, 0, 1.0f,  0.0f,  0.0f, // 5
		 4.05, -.5, -3, 3, 3, 1.0f,  0.0f,  0.0f, // 6
		 4.05, -.5,  3, 0, 3, 1.0f,  0.0f,  0.0f, // 7

		 // left
		 -4.05, -.5, -3, 0, 0, -1.0f,  0.0f,  0.0f, // 12
		 -4.05, -.5,  3, 3, 0, -1.0f,  0.0f,  0.0f, // 13
		 -4.05,  .5,  3, 3, 3, -1.0f,  0.0f,  0.0f, // 14
		 -4.05,  .5, -3, 0, 3, -1.0f,  0.0f,  0.0f, // 15
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &tilesVAO);
	glGenBuffers(1, &tilesVBO);
	glGenBuffers(1, &tilesEBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(tilesVAO);

	glBindBuffer(GL_ARRAY_BUFFER, tilesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tilesEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// define normal pointer layout 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Demo::BuildTexture()
{
	// concrete texture
	glGenTextures(1, &concretetex);
	glBindTexture(GL_TEXTURE_2D, concretetex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("concreteDTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &sconcretetex);
	glBindTexture(GL_TEXTURE_2D, sconcretetex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("concreteSTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// tiles 2 texture
	glGenTextures(1, &tiles2tex);
	glBindTexture(GL_TEXTURE_2D, tiles2tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("tilesD2Tex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &stiles2tex);
	glBindTexture(GL_TEXTURE_2D, stiles2tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("tilesS2Tex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// table
	glGenTextures(1, &tabletex);
	glBindTexture(GL_TEXTURE_2D, tabletex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("tableDTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &stabletex);
	glBindTexture(GL_TEXTURE_2D, stabletex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("tableSTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// bar
	glGenTextures(1, &bartex);
	glBindTexture(GL_TEXTURE_2D, bartex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("tableD2Tex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &sbartex);
	glBindTexture(GL_TEXTURE_2D, sbartex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("tableS2Tex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// wood
	glGenTextures(1, &woodtex);
	glBindTexture(GL_TEXTURE_2D, woodtex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("woodDTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &swoodtex);
	glBindTexture(GL_TEXTURE_2D, swoodtex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("woodSTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// wood 2
	glGenTextures(1, &wood2tex);
	glBindTexture(GL_TEXTURE_2D, wood2tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("woodDTex2.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &swood2tex);
	glBindTexture(GL_TEXTURE_2D, swood2tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("woodSTex2.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// metal
	glGenTextures(1, &metaltex);
	glBindTexture(GL_TEXTURE_2D, metaltex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("metalDTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &smetaltex);
	glBindTexture(GL_TEXTURE_2D, smetaltex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("metalDTex.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Demo::BuildPlane()
{
	// Load and create a texture 
	glGenTextures(1, &plane_texture);
	glBindTexture(GL_TEXTURE_2D, plane_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("marble_diffusemap.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &stexture2);
	glBindTexture(GL_TEXTURE_2D, stexture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("marble_specularmap.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// bottom
		-50.0, -0.5, -50.0,  0,  0, 0.0f,  1.0f,  0.0f,
		50.0, -0.5, -50.0, 50,  0, 0.0f,  1.0f,  0.0f,
		50.0, -0.5,  50.0, 50, 50, 0.0f,  1.0f,  0.0f,
		-50.0, -0.5,  50.0,  0, 50, 0.0f,  1.0f,  0.0f,
	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glGenBuffers(1, &planeEBO);

	glBindVertexArray(planeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawWall()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, concretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sconcretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 1.0f);

	// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glBindVertexArray(cubeVAO);

	// stairs wall
	for (int i = 0; i < 13; i++) {
		glm::mat4 model;

		model = glm::translate(model, glm::vec3(0, -.375, -.45f*i));
		model = glm::scale(model, glm::vec3(3, .25* (1 + 2 * i), .45));

		GLint model2Loc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(model2Loc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int)));
	}

	for (int i = 0; i < 13; i++) {
		glm::mat4 model;

		model = glm::translate(model, glm::vec3(-4.85f, 2.75f + .25f*i, -5.45f + .45f*i));
		model = glm::scale(model, glm::vec3(3, .5f, .45f));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ywalltex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sywalltex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	glUniform1f(shininessMatLoc, 10.0f);

	// left-right wall
	for (int i = 0; i < 2; i++) {
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(1.55 - 7.95*i, 5, -3));
		model = glm::scale(model, glm::vec3(.15f, 12, 10));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		if (i == 0)
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		else {
			glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int)));
		}
	}

	glm::mat4 model, model2, model3, model4;

	// back wall
	model2 = glm::translate(model2, glm::vec3(-2.35, 3, -8));
	model2 = glm::scale(model2, glm::vec3(8, 7, .15));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, concretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sconcretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	glUniform1f(shininessMatLoc, 1.0f);

	// left wall
	model = glm::translate(model, glm::vec3(1.55 - 7.95, 5, -3));
	model = glm::scale(model, glm::vec3(.15f, 12, 10));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int)));

	// back wall
	model3 = glm::translate(model3, glm::vec3(-3.9, 1, -6.125));
	model3 = glm::scale(model3, glm::vec3(4.9, 3.5, 1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
	glDrawElements(GL_TRIANGLES, 8, GL_UNSIGNED_INT, 0);

	// top walll
	model4 = glm::translate(model4, glm::vec3(-2.425f, 5.55, .75));
	model4 = glm::scale(model4, glm::vec3(7.85, .75, 2.5));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// top
	float rot = .5;
	glm::mat4 top, top2;
	top = glm::translate(top, glm::vec3(-2.4, 8.5, -3.5));
	top = glm::rotate(top, rot, glm::vec3(-1, 0, 0));
	top = glm::scale(top, glm::vec3(8.1, .2, 10));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(top));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	top2 = glm::translate(top2, glm::vec3(-2.4, 5.25, 3.25));
	top2 = glm::scale(top2, glm::vec3(8.1, .2, 6));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(top2));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	top2 = glm::translate(top2, glm::vec3(0, 28, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(top2));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// beside bar
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, stabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	float posX = -6.3125,
		posY = 2,
		posZ = -2.2;

	glm::mat4 wall;
	wall = glm::translate(wall, glm::vec3(posX, posY, posZ));
	wall = glm::scale(wall, glm::vec3(.025, 6, 1.5));
	modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wall));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// top
	for (int i = 0; i < 10; i++) {
		glm::mat4 top;
		top = glm::translate(top, glm::vec3(1.05 - .775*i, 5.15, 3.3));
		top = glm::scale(top, glm::vec3(.725, .2, 6));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(top));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawStairs(int n)
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tiles2tex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, stiles2tex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 10.0f);

	// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glBindVertexArray(cubeVAO);

	// 1st
	for (int i = 0; i < n; i++) {
		glm::mat4 model;

		model = glm::translate(model, glm::vec3(0, -.375 + .25f*i, -.45f*i));
		model = glm::scale(model, glm::vec3(3, .25f, .45f));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	}

	// 2nd
	for (int i = 0; i < n; i++) {
		glm::mat4 model;

		model = glm::translate(model, glm::vec3(-4.85f, 2.75f + .25f*i, -5.4f + .45f*i));
		model = glm::scale(model, glm::vec3(3, .25f, .45f));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
	}

	// top wall
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, concretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sconcretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	glBindVertexArray(cubeVAO);

	glm::mat4 model;

	model = glm::translate(model, glm::vec3(-4.85f, 4, -3));
	float angle = 100;
	model = glm::rotate(model, angle, glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(3, .5, 6.75));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawBanister(int n)
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, slamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 10.0f);

	glBindVertexArray(cubeVAO);

	// 1st
	for (int i = 0; i < n + 1; i++) {
		glm::mat4 model;
		if (i % 2 == 0) {
			model = glm::translate(model, glm::vec3(-1.3, .45 + .25f*i, -.45f * i));
			model = glm::scale(model, glm::vec3(.075, 1.5, .075));

			GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

	// 2nd
	for (int i = 0; i < n; i++) {
		glm::mat4 model;
		if (i % 2 == 0) {
			model = glm::translate(model, glm::vec3(-3.5, 3.5 + .25f*i, -5.85f + .45f*i));
			model = glm::scale(model, glm::vec3(.075, 1.5, .075));

			GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

	// middle
	for (int i = 0; i < 5; i++) {
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(-1.3 + .45f*-i, 3.4, -5.85));
		model = glm::scale(model, glm::vec3(.075, 1.5, .075));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	float rot = .5,
		rot2 = .997;
	// 1st
	glm::mat4 model, model2, model3, model4, model5, model6, model7, model8;
	model = glm::translate(model, glm::vec3(-1.3, 1.55, -2.725));
	model = glm::rotate(model, rot, glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(.05, .05, 6.2));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// 2nd
	model2 = glm::translate(model2, glm::vec3(-3.5, 4.675, -2.725));
	model2 = glm::rotate(model2, rot, glm::vec3(-1, 0, 0));
	model2 = glm::scale(model2, glm::vec3(.05, .05, 7.2));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// middle
	model3 = glm::translate(model3, glm::vec3(-2.4, 3, -5.85));
	model3 = glm::scale(model3, glm::vec3(2.15, .05, .05));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model4 = glm::translate(model4, glm::vec3(-1.3, 3, -5.6));
	model4 = glm::scale(model4, glm::vec3(.05, .05, .45));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, stabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	// 1st
	model5 = glm::translate(model5, glm::vec3(-1.3, 2.65, -2.625));
	model5 = glm::rotate(model5, rot, glm::vec3(1, 0, 0));
	model5 = glm::scale(model5, glm::vec3(.125, .075, 6.4));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// 2nd
	model6 = glm::translate(model6, glm::vec3(-3.5, 5.865, -2.725));
	model6 = glm::rotate(model6, rot, glm::vec3(-1, 0, 0));
	model6 = glm::scale(model6, glm::vec3(.125, .075, 7.2));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// middle
	model7 = glm::translate(model7, glm::vec3(-2.4, 4.15, -5.85));
	model7 = glm::scale(model7, glm::vec3(2.15, .075, .125));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model8 = glm::translate(model8, glm::vec3(-1.3, 4.15, -5.6));
	model8 = glm::scale(model8, glm::vec3(.125, .075, .45));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawCafeTable()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, slamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 5.0f);

	// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glBindVertexArray(cubeVAO);

	float posX = -4,
		posZ = .75;

	// legs
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			glm::mat4 model;

			model = glm::translate(model, glm::vec3(posX + 2 * (-1 + i), 0, posZ + .75f * (-1 + j)));
			model = glm::scale(model, glm::vec3(.05, 1, .05));

			GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

	for (int i = 0; i < 2; i++) {
		glm::mat4 model;

		model = glm::translate(model, glm::vec3(posX + 2 * (-1 + i), -.475, posZ - .375));
		model = glm::scale(model, glm::vec3(.05, .05, .7));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	glm::mat4 model, model2;

	model = glm::translate(model, glm::vec3(posX - 1, -.475, posZ - .375));
	model = glm::scale(model, glm::vec3(2, .05, .05));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// top
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, stabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 10.0f);

	model2 = glm::translate(model2, glm::vec3(posX - 1, .5, posZ - .375));
	model2 = glm::scale(model2, glm::vec3(2.4, .075, 1.1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawBar()
{
	float posX = -6.19,
		posZ = -1.9;

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodtex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, swoodtex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 5.0f);

	// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glBindVertexArray(cubeVAO);

	// legs
	for (int i = 0; i < 40; i++) {
		glm::mat4 model;

		model = glm::translate(model, glm::vec3(posX + i * .075, .15, posZ));
		model = glm::scale(model, glm::vec3(.05, 1.3, .025));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	glm::mat4 model, model2, model3, model4, model5;

	// qopee
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wood2tex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, swood2tex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 5.0f);

	model3 = glm::translate(model3, glm::vec3(posX + 2.375, .775, posZ + .225));
	model3 = glm::scale(model3, glm::vec3(1.25, .45, .05));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model4 = glm::translate(model4, glm::vec3(posX + 2.375, .975, posZ + .1));
	model4 = glm::scale(model4, glm::vec3(1.25, .05, .25));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model5 = glm::translate(model5, glm::vec3(posX + 2.375, .95, posZ));
	model5 = glm::scale(model5, glm::vec3(1.25, .05, .05));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// bar
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, swood2tex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 5.0f);

	model = glm::translate(model, glm::vec3(posX + 1.4575, .15, posZ - .5));
	model = glm::scale(model, glm::vec3(2.975, 1.3, 1));

	modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

	// black
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bartex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, swood2tex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// top
	shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 10.0f);

	model2 = glm::translate(model2, glm::vec3(posX + 1.4575, .8, posZ - .425));
	model2 = glm::scale(model2, glm::vec3(3.075, .25, 1.25));

	modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawSink()
{
	float posX = -6.2,
		posZ = -4.95;

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, slamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 5.0f);

	// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glBindVertexArray(cubeVAO);

	// body
	glm::mat4 model, model2, model3, model4;

	model = glm::translate(model, glm::vec3(posX + 1.4575, -.45, posZ));
	model = glm::scale(model, glm::vec3(2.975, .1, 1.25));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(-.25, 10, 0));
	model = glm::scale(model, glm::vec3(.5, 1, 1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(1.35, 0, 0));
	model = glm::scale(model, glm::vec3(.1, 1, 1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(-4.5, 0, .4));
	model = glm::scale(model, glm::vec3(8, 1, .2));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0, 0, -4.33));
	model = glm::scale(model, glm::vec3(1, 1, .33));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model2 = glm::translate(model2, glm::vec3(posX + .07, .1, posZ));
	model2 = glm::scale(model2, glm::vec3(.2, 1, 1.25));

	modelLoc = glGetUniformLocation(this->shaderProgram, "model");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model2 = glm::translate(model2, glm::vec3(13.875, 0, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model3 = glm::translate(model3, glm::vec3(posX + 1.4575, 0, posZ - .55));
	model3 = glm::scale(model3, glm::vec3(2.975, 1, .1));

	modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model3 = glm::translate(model3, glm::vec3(0, 0, 11));
	model3 = glm::scale(model3, glm::vec3(0.02, 1, 1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// slider
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, stabletex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 5.0f);

	model4 = glm::translate(model4, glm::vec3(posX + 1.4575, 0, posZ + .525));
	model4 = glm::scale(model4, glm::vec3(2.75, 1, .1));

	modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// sink
	glm::mat4 model5;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, metaltex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, smetaltex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 10);

	model5 = glm::translate(model5, glm::vec3(posX + 1.4575, .3, posZ - .05));
	model5 = glm::scale(model5, glm::vec3(1, .5, 1));

	modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model5 = glm::translate(model5, glm::vec3(1.15, 0, 0));
	model5 = glm::scale(model5, glm::vec3(.1, 1, 1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model5 = glm::translate(model5, glm::vec3(-3.5, 0, -.4425));
	model5 = glm::scale(model5, glm::vec3(6, 1, -.1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model5 = glm::translate(model5, glm::vec3(0, 0, -8.4));
	model5 = glm::scale(model5, glm::vec3(1, 1, -1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model5 = glm::translate(model5, glm::vec3(0, -.5, -4.2));
	model5 = glm::scale(model5, glm::vec3(1, 1, 8));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// faucet
	glm::mat4 model6, model7, model8;

	model6 = glm::translate(model6, glm::vec3(posX + 2.275, .6, posZ - .495));
	model6 = glm::scale(model6, glm::vec3(.06, .1, .06));

	modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model6 = glm::translate(model6, glm::vec3(0, 2, 0));
	model6 = glm::scale(model6, glm::vec3(.5, 3, .5));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model6 = glm::translate(model6, glm::vec3(0, .68, 4.25));
	model6 = glm::scale(model6, glm::vec3(1, .1, 5));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model6 = glm::translate(model6, glm::vec3(0, -3.25, .875));
	model6 = glm::scale(model6, glm::vec3(1, 3, .2));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	float rot = .809;
	model7 = glm::translate(model7, glm::vec3(posX + 2.275, .975, posZ - .465));
	model7 = glm::rotate(model7, rot, glm::vec3(1, 0, 0));
	model7 = glm::scale(model7, glm::vec3(.03, .1, .03));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model8 = glm::translate(model8, glm::vec3(posX + 2.275, .975, posZ - .267));
	model8 = glm::rotate(model8, -rot, glm::vec3(1, 0, 0));
	model8 = glm::scale(model8, glm::vec3(.03, .1, .03));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawStool()
{
	glUseProgram(shaderProgram);

	// make 4 stool
	for (int i = 0; i < 4; i++) {

		float posX = 0,
			posZ = .585;

		if (i == 0) {
			posX = -4.25 + randX[i];
			posZ = 1.5 + randZ[i];
		}
		if (i == 1) {
			posX = -5.25 + randX[i];
			posZ = 1.5 + randZ[i];
		}
		else if (i == 2) {
			posZ = -.3 + randX[i];
			posX = -5.25 + randZ[i];
		}
		else if (i == 3) {
			posZ = -.3 + randX[i];
			posX = -4.25 + randZ[i];
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lamptex);
		glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, slamptex);
		glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

		GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
		glUniform1f(shininessMatLoc, 5.0f);

		// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glBindVertexArray(stoolVAO);

		//legs
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				glm::mat4 model;
				float  xx = i == 0 ? 1 : -1,
					zz = j == 0 ? 1 : -1;
				model = glm::translate(model, glm::vec3(posX + .5 * (-1 + i), -.125, posZ + .5 * (-1 + j)));
				model = glm::scale(model, glm::vec3(.05 * xx, .75, .05 * zz));

				GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
		}

		glBindVertexArray(cubeVAO);

		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				glm::mat4 model;
				if (i == 0) {
					model = glm::translate(model, glm::vec3(posX - .25, -.325, posZ + -.01 + .475 * (-1 + j)));
					model = glm::scale(model, glm::vec3(.5, .04, .04));
				}
				else {
					model = glm::translate(model, glm::vec3(posX - .01 + .475 * (-1 + j), -.325, posZ - .25));
					model = glm::scale(model, glm::vec3(.04, .04, .5));
				}

				GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
		}

		// top
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tabletex);
		glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, stabletex);
		glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

		shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
		glUniform1f(shininessMatLoc, 10.0f);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(posX - .25, .25, posZ - .25));
		model = glm::scale(model, glm::vec3(.5, .05, .5));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	}
}

void Demo::DrawLamp()
{
	float posZ = -2.25;

	glUseProgram(shaderProgram);

	for (int i = 0; i < 2; i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lamptex);
		glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, slamptex);
		glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

		GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
		glUniform1f(shininessMatLoc, 0);

		// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
		glBindVertexArray(cubeVAO);

		glm::mat4 model, model2, model3;

		model = glm::translate(model, glm::vec3(-4.15 - i * 1.35f, 4.15, posZ));
		model = glm::scale(model, glm::vec3(.1, .2, .1));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// cable
		model2 = glm::translate(model2, glm::vec3(-4.15 - i * 1.35f, 3.75, posZ));
		model2 = glm::scale(model2, glm::vec3(.025, 2, .025));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

		// lamp
		glBindVertexArray(lampVAO);

		model3 = glm::translate(model3, glm::vec3(-4.15 - i * 1.35f, 2.75, posZ));
		model3 = glm::scale(model3, glm::vec3(.2, -.2, .2));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

		glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);

		shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
		glUniform1f(shininessMatLoc, 100);

		model3 = glm::translate(model3, glm::vec3(0, .8, 0));
		model3 = glm::scale(model3, glm::vec3(2, -2, 2));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

		// light
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tilestex);
		glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, swood2tex);
		glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

		shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
		glUniform1f(shininessMatLoc, 10);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawQopee() {
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wood2tex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, swood2tex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 1.0f);

	glBindVertexArray(cubeVAO);

	float posX = -5.801,
		posY = 3.5,
		posZ = 1.5;
	for (int i = 0; i < 6; i++) {
		glm::mat4 model;
		float rot = i * .55;
		model = glm::translate(model, glm::vec3(posX, posY, posZ));
		model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
		model = glm::rotate(model, rot, glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(.215, .8, .15));

		GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sconcretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(posX, posY - .45, posZ));
	model = glm::scale(model, glm::vec3(.05, .1, .05));
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(-5.5, -.5, 0));
	model = glm::scale(model, glm::vec3(12, .5, 1));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(-.375, 4.5, 0));
	model = glm::scale(model, glm::vec3(.08, 12, 1));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(1, .375, 0));
	model = glm::scale(model, glm::vec3(1, .08, 1));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawShelf() {
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, slamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 100.0f);

	glBindVertexArray(cubeVAO);

	float posX = -6.075,
		posY = 2,
		posZ = -3;

	glm::mat4 model, model2, model3;
	model = glm::translate(model, glm::vec3(posX, posY, posZ));
	model = glm::scale(model, glm::vec3(.5, 1.5, .05));
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model = glm::translate(model, glm::vec3(0, posY - 2.48, -15));
	model = glm::scale(model, glm::vec3(1, .04, 30));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	float rot = .997;
	model2 = glm::translate(model2, glm::vec3(posX, posY + .2425, posZ - .755));
	model2 = glm::rotate(model2, rot, glm::vec3(1, 0, 0));
	model2 = glm::scale(model2, glm::vec3(.5, 1.8, .05));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model3 = glm::translate(model3, glm::vec3(posX, posY - .49, posZ - 1.5));
	model3 = glm::scale(model3, glm::vec3(.5, .525, .05));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	model3 = glm::translate(model3, glm::vec3(0, .75, 17));
	model3 = glm::scale(model3, glm::vec3(1, .1, 25));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawBoundary()
{
	glUseProgram(shaderProgram);

	// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glBindVertexArray(cubeVAO);

	glm::mat4 top, top2;
	top = glm::translate(top, glm::vec3(9, 5.35, -2));
	top = glm::scale(top, glm::vec3(15, .2, 20));
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(top));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));

	top = glm::translate(top, glm::vec3(-1.5, 0, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(top));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));

	top2 = glm::translate(top2, glm::vec3(-2.4, 5.35, 11));
	top2 = glm::scale(top2, glm::vec3(35, .2, 15));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(top2));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tabletex);
	//glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, stabletex);
	//glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	glm::mat4 down, down2;
	down = glm::translate(down, glm::vec3(-2.4, -5.65, 6.2));
	down = glm::scale(down, glm::vec3(20, 10, .2));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(down));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawFloor()
{
	// tiles
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tilestex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, stilestex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 10.0f);

	// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glBindVertexArray(tilesVAO);

	glm::mat4 model;

	model = glm::translate(model, glm::vec3(-2.435, -.625, 3.215));
	model = glm::scale(model, glm::vec3(1, .25, 1));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	// concrete floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, concretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sconcretetex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 1);

	glUniform1f(shininessMatLoc, 10.0f);

	// seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
	glBindVertexArray(tilesVAO);

	glm::mat4 model2;

	model2 = glm::translate(model2, glm::vec3(-2.435, -.625, -2.785));
	model2 = glm::scale(model2, glm::vec3(1, .25, 1));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::DrawPlane()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.diffuse"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, slamptex);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "material.specular"), 3);

	GLint shininessMatLoc = glGetUniformLocation(this->shaderProgram, "material.shininess");
	glUniform1f(shininessMatLoc, 0);

	glBindVertexArray(planeVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0, -.25, 0));
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

#pragma region Camera 
void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.25;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 1.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.0025f;
	CAMERA_Y_SPEED = .02;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::MoveYCamera(float speed) {
	float y = viewCamY - posCamY;
	posCamY = posCamY + y * speed;
	viewCamY = viewCamY + y * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}
#pragma endregion

int main(int argc, char** argv) {
	RenderEngine &app = Demo();
	app.Start("Qopee", 1366, 768, false, false);
}