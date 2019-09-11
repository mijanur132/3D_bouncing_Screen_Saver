#include <iostream>
#include <vector>
#include<iostream>
#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/string_cast.hpp>

#include "util.hpp"
#include "mesh.hpp"

using namespace std;
// Window handle.
void initGLFW();
GLFWwindow* window = nullptr;
int width = 800, height = 600;

// Shaders, location to uniform variables, and vertex array objects.
void initOpenGL();
void prepareScene();
GLuint VAO, VBO;
GLuint shader;
GLuint transformLocation;
Mesh* mesh;

// Creates a Vertex Array Object with a cube
void prepareCube();
GLuint cubeVAO;
void prepareCube() {
	// buffer object, holding geometry data in GPU memory
	GLuint VBO;
	glGenBuffers(1, &VBO); // generate 1 buffer: VBO
	//GLuint VBOs[2];
	//glGenBuffers(2, VBOs);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// cube has 8 vertices, 3 component for each vertex
	GLfloat positions[8 * 3] = {
		-1, -1, -1,
		-1, -1,  1,
		-1,  1, -1,
		-1,  1,  1,
		1, -1, -1,
		1, -1,  1,
		1,  1, -1,
		1,  1,  1,
	};

	GLuint indices[6 * 2 * 3] = {
		0, 1, 2, 1, 2, 3, 1, 5, 7,1, 7 , 3, 5, 4, 6, 5, 6, 7,
		0, 4, 6, 0, 6, 2, 3, 7, 6, 3, 6, 2, 1, 5, 4, 1, 4, 0
	};

	// send the position data to GPU, handled by VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);


	// Manage the vertex array object.
	// ---------------------------------------
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	// create index buffer and bind to VAO
	GLuint IBO;
	glGenBuffers(1, &IBO);
	// Binding the element array buffer affects VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	int positionAttributeNumber = 0;

	// tell the GPU how to interpret the data in VBO
	// This call affects the cubeVAO.
	// The data will be retrieved from `VBO`, interpreted in the following way:
	glVertexAttribPointer(
		positionAttributeNumber, // attribute number
		3, // 3 components for each vertex, x, y, z as "position"
		GL_FLOAT, // data should be interpreted in IEEE 754 float format
		GL_FALSE, // don't normalize the vertex data
		0, // spacing: data is arranged compactly in the memory, don't worry now
		(GLvoid*)(0)
	);

	// Enable the attribute #0
	glEnableVertexAttribArray(positionAttributeNumber);

	// Unbind the VAO: done with cubeVAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); // don't change VBO.

	// Prepare the shader
	std::vector<GLuint> shaders;
	shaders.push_back(compileShader(GL_VERTEX_SHADER, "cube_vertex_shader.vs.glsl"));
	shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "cube_fragment_shader.fs.glsl"));
	shader = linkProgram(shaders);

	assert(glGetError() == GL_NO_ERROR);
}

// camera
glm::vec3 camCoords = glm::vec3(0.0, 0.0, 1.0);
bool camRot = false;
glm::vec2 camOrigin;
glm::vec2 mouseOrigin;

// View mode. When running the application, press 'M' key to switch mode.
const int VIEWMODE_TRIANGLE = 0;
const int VIEWMODE_OBJ = 1;
int viewMode = 0;

// GLFW window callbacks to handle keyboard and mouse input.
void scrollCallback(GLFWwindow* w, double x, double y);
void keyCallback(GLFWwindow* w, int key, int sc, int action, int mode);
void mouseButtonCallback(GLFWwindow* w, int b, int action, int mode);
void cursorPosCallback(GLFWwindow* w, double xp, double yp);
void framebufferSizeCallback(GLFWwindow* w, int width, int height);

int main() {
	std::cout << "Hello, OpenGL!" << std::endl;
	initGLFW();
	initOpenGL();
	//prepareScene();
	prepareCube();
	float x = 0;
	float y = 0;
	float addvalue = 0.1f;
	float addvalueY = 0.1f;
	float oldTime = 0;
	GLfloat positions_copy[8 * 3] = {
	-1, -1, -1,
	-1, -1,  1,
	-1,  1, -1,
	-1,  1,  1,
	1, -1, -1,
	1, -1,  1,
	1,  1, -1,
	1,  1,  1,
	};
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		glClearColor(0.1, 0.2, 0.25, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Render pass
		glUseProgram(shader);
		glm::mat4 transform;
		float aspect = (float)width / (float)height;
		glm::mat4 proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, -camCoords.z });
		glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(camCoords.y), { 1.0f, 0.0f, 0.0f });
		model = glm::rotate(model, glm::radians(camCoords.x), { 0.0f, 1.0f, 0.0f });
		transform = proj * view * model;
		
		if (viewMode == VIEWMODE_TRIANGLE) {
			float now = glfwGetTime();
			float deltaTime = now - oldTime;
			oldTime = now;
			std::cout << "delta time = " << deltaTime << std::endl;
			//cout << now << endl;
			if (true || deltaTime > 0.003) {
				//prepareCube();
				
				glBindVertexArray(cubeVAO);				
				GLuint translate_location = glGetUniformLocation(shader, "translate");
				glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(1.1f*x, y, 0));
				glm::vec4 translate_local= glm::vec4( 0, 0, 0,1);				
				translate_local = translate * translate_local;
				std::cout << glm::to_string(translate_local) << std::endl;
				glUniformMatrix4fv(translate_location, 1, GL_FALSE, glm::value_ptr(translate));
				
				glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, 0);
				glfwSwapBuffers(window);
				
				if (translate_local[0] > 0.9)
				{
					addvalue = -0.1f;
				}
				else if (translate_local[0] < -0.9)
				{
					addvalue = 0.1f;
				}

			
				x = x + addvalue * deltaTime * 3;
				if (translate_local[1] > 0.9)
				{
					addvalueY = -0.1f;
				}
				else if (translate_local[1] < -0.9)
				{
					addvalueY = 0.1f;
				}


				y = y + addvalueY * deltaTime * 3;
		
				
			}
			
		}

	
		else if (viewMode == VIEWMODE_OBJ) {
			auto meshBB = mesh->boundingBox();
			float bboxDiagLength = glm::length(meshBB.second - meshBB.first);
			glm::mat4 fixBB = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / bboxDiagLength));
			fixBB = glm::translate(fixBB, -(meshBB.first + meshBB.second) / 2.0f);
			glm::vec4 translate_local = glm::vec4(0, 0, 0, 1);
			glm::mat4 translatexy = glm::translate(glm::mat4(1.0f), glm::vec3(1.1f*x, y, 0));
			transform = transform * fixBB*translatexy;
			glm::vec4 local = transform * fixBB*translate_local*translatexy;
			std::cout << glm::to_string(local) << std::endl;
			glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transform));
			mesh->draw();
			glfwSwapBuffers(window);
			for (int i = 0; i < 10000000; i++)
			{
				int j = i;
			}
			if (local[0] > 0.9)
			{
				addvalue = -0.1f;
			}
			else if (local[0] < -0.9)
			{
				addvalue = 0.1f;
			}


			x = x + addvalue;
			if (local[1] > 0.9)
			{
				addvalueY = -0.1f;
			}
			else if (local[1] < -0.9)
			{
				addvalueY = 0.1f;
			}


			y = y + addvalueY;

		}
		
		glBindVertexArray(0);
		glUseProgram(0);
	

		//assert(glGetError() == GL_NO_ERROR);
	
		

	}

	return 0;
}

void initGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, "OpenGL Demo", nullptr, nullptr);
	if (!window) {
		std::cerr << "Cannot create window";
		std::exit(1);
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetScrollCallback(window, scrollCallback);
}

void initOpenGL() {
	assert(window);
	if (gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress)) == 0) {
		std::cerr << "Failed to intialize OpenGL loader" << std::endl;
		std::exit(1);
	}
	assert(glGetError() == GL_NO_ERROR);
}

void prepareScene() {
	glEnable(GL_DEPTH_TEST);
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 norm;
	};

	std::vector<Vertex> verts = {
		{{-0.433f, -0.25f, 0.0f}, {1.0, 0.0, 0.0}},
		{{ 0.433f, -0.25f, 0.0f}, {0.0, 1.0, 0.0}},
		{{ 0.0f,    0.5f,  0.0f}, {0.0, 0.0, 1.0}}
	};
	glm::vec3 color = glm::vec3(0.5, 1, .5);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(verts[0]), verts.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, norm));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Prepares the shader
	std::vector<GLuint> shaders;
	shaders.push_back(compileShader(GL_VERTEX_SHADER, "sh_v.glsl"));
	shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "sh_f.glsl"));
	shader = linkProgram(shaders);
	transformLocation = glGetUniformLocation(shader, "xform");

	assert(glGetError() == GL_NO_ERROR);
}
// GLFW window callbacks
// --------------------------------------------------------------------

void scrollCallback(GLFWwindow* w, double x, double y) {
	float offset = (y > 0) ? 0.1f : -0.1f;
	camCoords.z = glm::clamp(camCoords.z + offset, 0.1f, 10.0f);
}

void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(w, true);
	}
	else if (key == GLFW_KEY_M && action == GLFW_RELEASE) {
		viewMode = (viewMode == VIEWMODE_TRIANGLE ? VIEWMODE_OBJ : VIEWMODE_TRIANGLE);
		if (viewMode == VIEWMODE_OBJ && mesh == NULL) {
			mesh = new Mesh("models/cow.obj");
		}
	}
}

void mouseButtonCallback(GLFWwindow* w, int button, int action, int mode) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// Activate rotation mode
		camRot = true;
		camOrigin = glm::vec2(camCoords);
		double xpos, ypos;
		glfwGetCursorPos(w, &xpos, &ypos);
		mouseOrigin = glm::vec2(xpos, ypos);
	} if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		camRot = false;
	}
}

void cursorPosCallback(GLFWwindow* w, double xp, double yp) {
	if (camRot) {
		float rotScale = std::fmin(width / 450.f, height / 270.f);
		glm::vec2 mouseDelta = glm::vec2(xp, yp) - mouseOrigin;
		glm::vec2 newAngle = camOrigin + mouseDelta / rotScale;
		newAngle.y = glm::clamp(newAngle.y, -90.0f, 90.0f);
		while (newAngle.x > 180.0f) newAngle.x -= 360.0f;
		while (newAngle.y < -180.0f) newAngle.y += 360.0f;
		if (glm::length(newAngle - glm::vec2(camCoords)) > std::numeric_limits<float>::epsilon()) {
			camCoords.x = newAngle.x;
			camCoords.y = newAngle.y;
		}
	}
}

void framebufferSizeCallback(GLFWwindow* w, int width, int height) {
	::width = width;
	::height = height;
	glViewport(0, 0, width, height);
}
