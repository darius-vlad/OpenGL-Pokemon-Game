#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include <glm/gtx/norm.hpp>
#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include <chrono>
#include <random>

#include <iostream>
#include <time.h>
#include "Texture_2D.h"
#include "Particle_Generator.h"

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

glm::mat4 orcModel;
glm::mat3 orcNormalMatrix;



glm::mat4 weaponModel;
glm::mat3 weaponNormalMatrix;

glm::mat4 arceusModel;
glm::mat3 arceusNormalMatrix;

glm::mat4 pokemonGroundModel;
glm::mat3 pokemonGroundNormalMatrix;

glm::mat4 snorlaxModel;
glm::mat3 snorlaxNormalMatrix;

glm::mat4 magnemiteModel;
glm::mat3 magnemiteNormalMatrix;

glm::mat4 pokeballModel;
glm::mat3 pokeballNormalMatrix;

glm::mat4 eveeModel;
glm::mat3 eveeNormalMatrix;


glm::mat4 dragoniteModel;
glm::mat3 dragoniteNormalMatrix;

glm::mat4 gengarModel;
glm::mat3 gengarNormalMatrix;


glm::mat4 kyuremModel;
glm::mat3 kyuremNormalMatrix;

glm::mat4 lucarioModel;
glm::mat3 lucarioNormalMatrix;

glm::mat4 gyaradosModel;
glm::mat3 gyaradosNormalMatrix;

glm::mat4 laprasModel;
glm::mat3 laprasNormalMatrix;

glm::mat4 geodudeModel;
glm::mat3 geodudeNormalMatrix;

glm::vec3 triangleVertices[3] = {
	glm::vec3(-13.0f, 7.0f, 20.0f),   // Vertex 1
	glm::vec3(2.0f, 7.0f, -30.0f), // Vertex 2
	glm::vec3(56.0f, 7.0f, -15.0f)   // Vertex 3
};

// Track time and movement phase
static float timeCounter = 0.0f;
static int currentVertex = 0;

//models for scenes
glm::mat4 scene1Model;
glm::mat3 scene1NormalMatrix;


glm::mat4 scene2Model;
glm::mat3 scene2NormalMatrix;


glm::mat4 scene3Model;
glm::mat3 scene3NormalMatrix;


glm::mat4 scene4Model;
glm::mat3 scene4NormalMatrix;


glm::mat4 forceFieldModel;

glm::mat4 volcanoModel;
glm::mat3 volcanoNormalMatrix;

glm::mat4 cryptModel;
glm::mat3 cryptNormalMatrix;
static float cryptRotationAngle = 0.1f;

glm::mat4 waterModel;

// light parameters
glm::vec3 lightDir = glm::vec3(1.0f , 20.0f , 1.0f);
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

GLint particleModelLoc;
GLint particleViewLoc;
GLint particleProjectionLoc;
GLint particleTintLoc;

GLint modelOutliningLoc;
GLint normalMatrixOutliningLoc;
GLint viewOutliningLoc;
GLint projectionOutliningLoc;
GLuint transparencyLoc;

const unsigned int SHADOW_WIDTH =  8192;
const unsigned int SHADOW_HEIGHT = 8192;

bool isWeaponPickedUp = false;
// camera
gps::Camera myCamera(
	glm::vec3(0.0f, 2.0f, 35.0f),
	glm::vec3(0.0f, 5.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.08f;

GLboolean pressedKeys[1024];

float lastX = myWindow.getWindowDimensions().width / 2;
float lastY = myWindow.getWindowDimensions().height / 2;
float fov = 45.0f;
float yaw = -90.f;
float pitch = 0.0f;
bool firstMouse = true;

bool isScene2 = false;
bool flying = false;

//variables for the attack animation

bool isAttacking = false;
float attackTime = 0.0f; // Track animation progress (in seconds)
const float attackDuration = 0.5f; // Total duration of the animation



// models

gps::Model3D orc;


gps::Model3D arceus;
gps::Model3D snorlax;
gps::Model3D magnemite;
gps::Model3D evee;
gps::Model3D dragonite;
gps::Model3D gengar;
gps::Model3D lucario;
gps::Model3D gyarados;
gps::Model3D lapras;
gps::Model3D geodude;

gps::Model3D water;

gps::Model3D pokemonGround;
gps::Model3D pokeball;

gps::Model3D scene1;
gps::Model3D scene2;
gps::Model3D scene3;
gps::Model3D scene4;
gps::Model3D force_field;
gps::Model3D volcano;
gps::Model3D crypt;
gps::Model3D kyurem;

GLfloat angle;

// shaders
gps::Shader myBasicShader;
gps::Shader outliningShader;
gps::Shader simpleDepthShader;
gps::Shader particleShader;
gps::Shader waterShader;




//for shadow
GLuint shadowMapFBO;
GLuint depthMapTexture;
GLuint textureID;
bool showDepthMap;


//skybox
gps::SkyBox mySkyBox;
gps::SkyBox nightSkyBox;
gps::SkyBox redSkyBox;
gps::SkyBox hillSkyBox;
gps::Shader skyboxShader;


int currentScene = 1;




float magnemiteMovingSpeed = 0.12f;
glm::vec3 magnemiteVelocity = glm::vec3(1.0f, 0.0f, 0.0f);
float magnemiteRotationAngle = 0.0f;

GLuint ReadTextureFromFile(const char* file_name) {

	int x, y, n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);

	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// NPOT check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(
			stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
		);
	}

	int width_in_bytes = x * 4;
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {

		top = image_data + row * width_in_bytes;
		bottom = image_data + (y - row - 1) * width_in_bytes;

		for (int col = 0; col < width_in_bytes; col++) {

			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_SRGB, //GL_SRGB,//GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


ParticleGenerator* Particles;

GLuint quadVAO;
//FOR SIMULATING FIRE
struct FireParticle {
	glm::vec3 position;  // Position of the particle
	glm::vec3 velocity;  // Velocity of the particle
	float size;          // Size of the particle
	float life;          // Remaining lifetime
	glm::vec4 color;     // Color (RGBA)
};
std::vector<FireParticle> fireParticles;
const int maxFireParticles = 1000;




bool cutsceneActive = false; // To track if the cutscene is active
std::vector<glm::vec3> keyPoints = {
	glm::vec3(-10.4714f, 2.568f, 44.8213f),
	glm::vec3(-0.9495f, 3.8052f, 31.621f),
	glm::vec3(8.565f, 5.4394f, 17.579f),
	glm::vec3(14.17f, 6.1902f, 2.1963f),
	glm::vec3(15.963f, 0.8503f, -16.18979f),
	
};
float cutsceneSpeed = 0.2f; // Speed factor for the cutscene

GLuint createQuadVAO() {
	float quadVertices[] = {
		// Positions        // Texture Coords
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // Bottom-left
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // Bottom-right
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // Top-left
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f   // Top-right
	};

	GLuint VAO, VBO;

	// Generate and bind VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Generate and bind VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	// Define vertex attributes
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture Coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0);

	return VAO; // Return the VAO for later use
}

void initFireParticles() {
	for (int i = 0; i < maxFireParticles; ++i) {
		FireParticle particle;
		particle.position = glm::vec3(0.0f, 2.0f, 35.0f); // Base of the flame
		particle.velocity = glm::vec3(
			static_cast<float>(rand() % 20 - 10) / 100.0f, // Random sway (X)
			static_cast<float>(rand() % 50 + 50) / 100.0f, // Upward velocity (Y)
			static_cast<float>(rand() % 20 - 10) / 100.0f  // Random sway (Z)
		);
		particle.size = static_cast<float>(rand() % 10 + 5) / 50.0f; // Size between 0.1 and 0.3
		particle.life = static_cast<float>(rand() % 50 + 50) / 100.0f; // Lifetime between 0.5 and 1.0 seconds
		particle.color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // Initial orange color
		fireParticles.push_back(particle);
	}
}

// Update fire particles
void updateFireParticles(float deltaTime) {
	for (auto& particle : fireParticles) {
		particle.position += particle.velocity * deltaTime; // Update position
		particle.life -= deltaTime; // Decrease lifetime

		// Fade out color over lifetime
		particle.color.a = particle.life / 1.0f; // Alpha fades with life

		// Reset particle if its life ends
		if (particle.life <= 0.0f) {
			particle.position = glm::vec3(0.0f, 2.0f, 35.0f); // Reset to base
			particle.velocity = glm::vec3(
				static_cast<float>(rand() % 20 - 10) / 100.0f,
				static_cast<float>(rand() % 50 + 50) / 100.0f,
				static_cast<float>(rand() % 20 - 10) / 100.0f
			);
			particle.size = static_cast<float>(rand() % 10 + 5) / 50.0f;
			particle.life = static_cast<float>(rand() % 50 + 50) / 100.0f;
			particle.color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
		}
	}
}


// Render fire particles
void renderFireParticles(gps::Shader shader, GLuint textureID) {
	shader.useShaderProgram();
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindVertexArray(quadVAO);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram , "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	for (const auto& particle : fireParticles) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, particle.position);
		model = glm::scale(model, glm::vec3(particle.size));

		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform4fv(glGetUniformLocation(shader.shaderProgram, "particleColor"), 1, glm::value_ptr(particle.color));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Render quad
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


glm::vec3 interpolate(glm::vec3 p0, glm::vec3 p1, float t) {
	return p0 * (1.0f - t) + p1 * t;
}


void updateCamera(float deltaTime, GLint viewLoc, float speed) {
	static float t = 0.0f; // Interpolation time
	static int currentIndex = 0; // Current key point index

	int nextIndex = (currentIndex + 1) % keyPoints.size();

	t += deltaTime * speed; // Adjust speed
	if (t > 1.0f) {
		t = 0.0f;
		currentIndex = nextIndex; // Move to the next key point
	}

	// Interpolate camera position and target
	glm::vec3 cameraPosition = interpolate(keyPoints[currentIndex], keyPoints[nextIndex], t);
	glm::vec3 target = keyPoints[nextIndex]; // Simplified target logic

	// Create view matrix
	view = glm::lookAt(cameraPosition, target, glm::vec3(0.0f, 1.0f, 0.0f));

	// Send view matrix to shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void sendLightsForScene1()
{
	
	glm::vec3 directionalLightColor = glm::vec3(0.8f , 0.8f ,0.8f);
	float directionalAmbientStrength = 0.07f;
	float directionalSpecularStrength = 0.4f;
	lightDir = glm::vec3(19.0f, 20.0f, 39.0f);
	myBasicShader.useShaderProgram();
	glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.direction"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.color"), 1, glm::value_ptr(directionalLightColor));
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.ambientStrength"), directionalAmbientStrength);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.specularStrength"), directionalSpecularStrength);

	glm::vec3 pointLightPositions[] =
	{
		glm::vec3(-18.0f , 0.0f ,-15.0f),
		glm::vec3(34.0f , 1.0f , -29.0f),
		glm::vec3(45.0f , 0.0f , 5.0f),
		glm::vec3(22.0f , 2.0f , -41.0f)

	};

	glm::vec3 pointLightColors[] =
	{
		glm::vec3(1.0f , 1.0f , 1.0f),
		glm::vec3(1.0f , 1.0f , 1.0f),
		glm::vec3(1.0f , 1.0f , 1.0f),
		glm::vec3(1.0f , 0.0f , 1.0f),
	};

	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].color"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].specularStrength"), 0.2f);

	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].color"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].specularStrength"), 0.2f);


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].color"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].specularStrength"), 0.2f);


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].color"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].specularStrength"), 0.2f);



	glm::vec3 spotlightPosition = glm::vec3(42.0f, 4.0f, 29.0f);
	glm::vec3 spotlightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
	glm::vec3 spotlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float spotlightAmbientStrength = 0.1f;
	float spotlightSpecularStrength = 0.2f;
	float spotlightConstant = 1.0f;
	float spotlightLinear = 0.09f;
	float spotlightQuadratic = 0.032f;

	float innerCutoff = glm::cos(glm::radians(30.5f));
	float outterCutoff = glm::cos(glm::radians(50.5f));

	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.position"), spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.direction"), spotlightDirection.x, spotlightDirection.y, spotlightDirection.z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.color"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.ambientStrength"), spotlightAmbientStrength);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.specularStrength"), spotlightSpecularStrength);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.constant"), spotlightConstant);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.linear"), spotlightLinear);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.quadratic"), spotlightQuadratic);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.innerCutoff"), innerCutoff);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.outerCutoff"), outterCutoff);



	

}


void sendLightsForScene2()
{

	glm::vec3 directionalLightColor = glm::vec3(0.1f, 0.1f, 0.1f);
	float directionalAmbientStrength = 0.1f;
	float directionalSpecularStrength = 0.4f;
	lightDir = glm::vec3(-36.0f, 20.0f, 53.0f);
	myBasicShader.useShaderProgram();
	glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.direction"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.color"), 1, glm::value_ptr(directionalLightColor));
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.ambientStrength"), directionalAmbientStrength);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.specularStrength"), directionalSpecularStrength);

	glm::vec3 pointLightPositions[] =
	{
		glm::vec3(-3.0f , 2.0f ,53.0f),
		glm::vec3(1.0f , 2.0f , 80.0f),
		glm::vec3(36.0f , 2.0f , 87.0f),
		glm::vec3(55.0f , 2.0f , 47.0f)

	};


	glm::vec3 pointLightColors[] =
	{
		glm::vec3(1.0f , 0.0f , 1.0f),
		glm::vec3(1.0f , 0.0f , 1.0f),
		glm::vec3(1.0f , 0.0f , 1.0f),
		glm::vec3(1.0f , 0.0f , 1.0f)
	};



	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].color"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].specularStrength"), 0.2f);

	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].color"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].specularStrength"), 0.2f);


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].color"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].specularStrength"), 0.2f);


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].color"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].specularStrength"), 0.2f);


	glm::vec3 spotlightPosition = glm::vec3(42.0f, 4.0f, 29.0f);
	glm::vec3 spotlightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
	glm::vec3 spotlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float spotlightAmbientStrength = 0.1f;
	float spotlightSpecularStrength = 0.2f;
	float spotlightConstant = 1.0f;
	float spotlightLinear = 0.09f;
	float spotlightQuadratic = 0.032f;

	float innerCutoff = glm::cos(glm::radians(30.5f));
	float outterCutoff = glm::cos(glm::radians(50.5f));

	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.position"), spotlightPosition.x, spotlightPosition.y, spotlightPosition.z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.direction"), spotlightDirection.x, spotlightDirection.y, spotlightDirection.z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.color"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.ambientStrength"), spotlightAmbientStrength);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.specularStrength"), spotlightSpecularStrength);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.constant"), spotlightConstant);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.linear"), spotlightLinear);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.quadratic"), spotlightQuadratic);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.innerCutoff"), innerCutoff);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "spotlight.outerCutoff"), outterCutoff);

}


void sendLightsForScene3()
{

	glm::vec3 directionalLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float directionalAmbientStrength = 0.1f;
	float directionalSpecularStrength = 0.4f;
	lightDir = glm::vec3(62.0f, 20.0f, 67.0f);
	myBasicShader.useShaderProgram();
	glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.direction"), 1, glm::value_ptr (lightDir));
	glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.color"), 1, glm::value_ptr(directionalLightColor));
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.ambientStrength"), directionalAmbientStrength);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.specularStrength"), directionalSpecularStrength);


	glm::vec3 pointLightPositions[] =
	{
		glm::vec3(2.0f , 2.0f ,13.0f),
		glm::vec3(38.0f , 2.0f , 50.0f),
		glm::vec3(63.0f , 2.0f , 87.0f),
		glm::vec3(88.0f , 4.0f , 96.0f)

	};

	glm::vec3 pointLightColors[] =
	{
		glm::vec3(0.0f , 0.0f , 1.0f),
		glm::vec3(0.0f , 0.0f , 1.0f),
		glm::vec3(0.0f , 0.0f , 1.0f),
		glm::vec3(0.0f , 0.0f , 1.0f)
	};


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].color"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].specularStrength"), 0.2f);

	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].color"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].specularStrength"), 0.2f);


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].color"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].specularStrength"), 0.2f);


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].color"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].specularStrength"), 0.2f);


}


void sendLightsForScene4()
{

	glm::vec3 directionalLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float directionalAmbientStrength = 0.1f;
	float directionalSpecularStrength = 0.4f;
	lightDir = glm::vec3(-7.0f, 20.0f, 12.0f);
	myBasicShader.useShaderProgram();
	glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.direction"), 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
	glUniform3fv(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.color"), 1, glm::value_ptr(directionalLightColor));
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.ambientStrength"), directionalAmbientStrength);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "dirLight.specularStrength"), directionalSpecularStrength);

	glm::vec3 pointLightPositions[] =
	{
		glm::vec3(25.0f , 1.0f , 15.0f),
		glm::vec3(31.0f , 1.0f , -6.0f),
		glm::vec3(50.0f , 0.0f , -27.0f),
		glm::vec3(65.0f , 0.0f , -14.0f)

	};

	glm::vec3 pointLightColors[] =
	{
		glm::vec3(1.0f , 0.0f , 0.0f),
		glm::vec3(1.0f , 0.0f , 0.0f),
		glm::vec3(0.0f , 1.0f , 1.0f),
		glm::vec3(0.0f , 1.0f , 1.0f)
	};





	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].color"), pointLightColors[0].x, pointLightColors[0].y, pointLightColors[0].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0].specularStrength"), 0.2f);

	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].color"), pointLightColors[1].x, pointLightColors[1].y, pointLightColors[1].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1].specularStrength"), 0.2f);


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].color"), pointLightColors[2].x, pointLightColors[2].y, pointLightColors[2].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2].specularStrength"), 0.2f);


	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
	glUniform3f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].color"), pointLightColors[3].x, pointLightColors[3].y, pointLightColors[3].z);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].constant"), 1.0f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].linear"), 0.25f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].quadratic"), 0.017f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].ambientStrength"), 0.15f);
	glUniform1f(glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[3].specularStrength"), 0.2f);


}





void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;


	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		currentScene = 1;
		myCamera.setPosition(glm::vec3(0.0f, 2.0f, 35.0f));
		isScene2 = false;
		sendLightsForScene1();
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		currentScene = 2;
		myCamera.setPosition(glm::vec3(0.0f, 2.0f, 35.0f));
		isScene2 = true;
		sendLightsForScene2();
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		currentScene = 3;
		myCamera.setPosition(glm::vec3(0.0f, 2.0f, 35.0f));
		isScene2 = false;
		sendLightsForScene3();
	}

	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		currentScene = 4;
		myCamera.setPosition(glm::vec3(0.0f, 2.0f, 35.0f));
		isScene2 = false;
		sendLightsForScene4();
	}

	if (pressedKeys[GLFW_KEY_KP_0]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
	}

	if (pressedKeys[GLFW_KEY_KP_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
	}

	if (pressedKeys[GLFW_KEY_KP_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);	
	}




	if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		cutsceneActive = !cutsceneActive; // Toggle cutscene state
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		flying = !flying; // Toggle flying
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		glm::vec3 cameraCoordinates = myCamera.getCameraPosition();
		std::cout << cameraCoordinates.x << " " << cameraCoordinates.y << " " << cameraCoordinates.z << std::endl;
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 45.0f)
		pitch = 45.0f;
	if (pitch < -45.0f)
		pitch = -45.0f;

	myCamera.rotate(pitch, yaw);

}


void handleMouseInput(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!isAttacking) {
			isAttacking = true;
			attackTime = 0.0f; // Reset animation timer
		}
	}

}


void processMovement() {
	if (pressedKeys[GLFW_KEY_W]) {
		if (pressedKeys[GLFW_KEY_LEFT_SHIFT])
		{
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 2.0f, flying); 
		}
		else
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed , flying);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed , flying);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed , flying);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed , flying);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		// compute normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angle -= 1.0f;
		// update model matrix for teapot
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
		// update normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle += 1.0f;
		// update model matrix for teapot
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
		// update normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}
}

void processWeaponPickUp()
{

	if (glm::distance(myCamera.getCameraPosition(), glm::vec3(0.0f, 0.0f, 0.0f)) <= 10)
	{
		if (pressedKeys[GLFW_KEY_L])
		{
			isWeaponPickedUp = true;
		}
	}
}



void initOpenGLWindow() {
	myWindow.Create(1024, 768, "OpenGL Project Core");
}


void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
	glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(myWindow.getWindow(), scroll_callback);
	glfwSetMouseButtonCallback(myWindow.getWindow(), handleMouseInput);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	glEnable(GL_STENCIL_TEST); // stencil test
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void initTextures()
{
	 textureID = ReadTextureFromFile("textures/fire.jpg");
}

void initModels() {

	orc.LoadModel("models/orc/orc.obj");
	force_field.LoadModel("models/force_field/field.obj");
	volcano.LoadModel("models/volcano/volcano.obj");
	crypt.LoadModel("models/crypt/crypt.obj");
	water.LoadModel("models/wave/water.obj");

	arceus.LoadModel("models/Pokemons/arceus/arceus.obj");
	snorlax.LoadModel("models/Pokemons/snorlax/snorlax.obj");
	magnemite.LoadModel("models/Pokemons/magnemite/magnemite.obj");
	dragonite.LoadModel("models/Pokemons/dragonite/dragonite.obj");
	gengar.LoadModel("models/Pokemons/gengar/gengarv1.obj");
	kyurem.LoadModel("models/Pokemons/kyurem/kyurem.obj");
	lucario.LoadModel("models/Pokemons/lucario/lucario.obj");
	gyarados.LoadModel("models/Pokemons/gyarados/gyarados.obj");
	lapras.LoadModel("models/Pokemons/lapras/lplc.obj");
	geodude.LoadModel("models/Pokemons/geodude/geodude.obj");

	pokemonGround.LoadModel("models/pokemon-ground/pokemon-ground.obj");
	pokeball.LoadModel("models/pokeball/pokeball.obj");
	evee.LoadModel("models/Pokemons/evee/evee.obj");
	scene1.LoadModel("models/scenes/scene1/ground1.obj");
	scene2.LoadModel("models/scenes/scene2/ground2.obj");
	scene3.LoadModel("models/scenes/scene3/ground3.obj");
	scene4.LoadModel("models/scenes/scene4/ground4.obj");

}

void initShaders() {
	myBasicShader.loadShader(
		"shaders/basic.vert",
		"shaders/basic.frag");

	outliningShader.loadShader(
		"shaders/outlining.vert",
		"shaders/outlining.frag"
	);

	simpleDepthShader.loadShader(
		"shaders/simpleDepthShader.vert",
		"shaders/simpleDepthShader.frag"
	);

	particleShader.loadShader(
		"shaders/particle.vert",
		"shaders/particle.frag"
	);

	waterShader.loadShader(
		"shaders/water.vert",
		"shaders/water.frag"
	);


	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

}

void initFBO()
{
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 computeLightSpaceTrMatrix() {

	glm::mat4 lightView = glm::lookAt( lightDir, glm::vec3(0.0f , 0.0f , 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 150.0f;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void initSkyBox()
{
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/right.tga");
	faces.push_back("skybox/left.tga");
	faces.push_back("skybox/top.tga");
	faces.push_back("skybox/bottom.tga");
	faces.push_back("skybox/back.tga");
	faces.push_back("skybox/front.tga");
	mySkyBox.Load(faces);

	std::vector<const GLchar*> Nightfaces;
	Nightfaces.push_back("skybox/nightsky_rt.tga");
	Nightfaces.push_back("skybox/nightsky_lf.tga");
	Nightfaces.push_back("skybox/nightsky_up.tga");
	Nightfaces.push_back("skybox/nightsky_dn.tga");
	Nightfaces.push_back("skybox/nightsky_bk.tga");
	Nightfaces.push_back("skybox/nightsky_ft.tga");
	nightSkyBox.Load(Nightfaces);

	std::vector<const GLchar*> RedFaces;
	RedFaces.push_back("skybox/redplanet_rt.tga");
	RedFaces.push_back("skybox/redplanet_lf.tga");
	RedFaces.push_back("skybox/redplanet_up.tga");
	RedFaces.push_back("skybox/redplanet_dn.tga");
	RedFaces.push_back("skybox/redplanet_bk.tga");
	RedFaces.push_back("skybox/redplanet_ft.tga");
	redSkyBox.Load(RedFaces);

	std::vector<const GLchar*> hillFaces;
	hillFaces.push_back("skybox/hills_rt.tga");
	hillFaces.push_back("skybox/hills_lf.tga");
	hillFaces.push_back("skybox/hills_up.tga");
	hillFaces.push_back("skybox/hills_dn.tga");
	hillFaces.push_back("skybox/hills_bk.tga");
	hillFaces.push_back("skybox/hills_ft.tga");
	hillSkyBox.Load(hillFaces);




}

void initUniforms() {
	myBasicShader.useShaderProgram();

	// create model matrix for teapot
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// compute normal matrix for teapot
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");

	projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	


	transparencyLoc = glGetUniformLocation(myBasicShader.shaderProgram, "transparency");

	// Set transparency to fully opaque (alpha = 1.0)
	glUniform1f(transparencyLoc, 1.0f);


	pokeballModel = glm::mat4(1.0f);
	pokeballModel = glm::translate(pokeballModel, glm::vec3(2.0f, 0.0f, 3.0f));
	pokeballModel = glm::scale(pokeballModel, glm::vec3(4.0f, 4.0f, 4.0f));

	magnemiteModel = glm::mat4(1.0f);
	magnemiteModel = glm::translate(magnemiteModel, glm::vec3(5.0f, 0.0f, -15.0f));


	//model for weapon
	weaponModel = glm::mat4(1.0f);
	weaponModel = glm::translate(weaponModel, glm::vec3(0.0f, 1.0f, 12.0f));
	weaponModel = glm::scale(weaponModel, glm::vec3(0.01f, 0.01f, 0.01f));
	weaponModel = glm::rotate(weaponModel, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	////set the light direction (direction towards the light)
	
	//lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	//// send light dir to shader
	//glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	////set light color
	//lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	//lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	//// send light color to shader
	//glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	sendLightsForScene1();

	waterShader.useShaderProgram();
	//send projection to water
	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	

	outliningShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(outliningShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
	



}


void renderPokemonGround(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();

	pokemonGroundModel = glm::mat4(1.0f);
	pokemonGroundModel = glm::translate(pokemonGroundModel, glm::vec3(0.0f, -5.0f, 0.0f));

	pokemonGroundNormalMatrix = glm::mat3(glm::inverseTranspose(view * pokemonGroundModel));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(pokemonGroundModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(pokemonGroundNormalMatrix));

	pokemonGround.Draw(shader);
}

void renderGengar(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	//gengar 1
	gengarModel = glm::mat4(1.0f);
	gengarModel = glm::translate(gengarModel, glm::vec3(-4.0f, 1.0f, 54.0f));
	gengarModel = glm::rotate(gengarModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	gengarNormalMatrix = glm::mat3(glm::inverseTranspose(view * gengarModel));


	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(gengarModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(gengarNormalMatrix));

	gengar.Draw(shader);



	//gengar2
	gengarModel = glm::mat4(1.0f);
	gengarModel = glm::translate(gengarModel, glm::vec3(2.0f, 1.0f, 81.0f));
	gengarModel = glm::rotate(gengarModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	gengarNormalMatrix = glm::mat3(glm::inverseTranspose(view * gengarModel));


	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(gengarModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(gengarNormalMatrix));

	gengar.Draw(shader);


	//gengar 3
	gengarModel = glm::mat4(1.0f);
	gengarModel = glm::translate(gengarModel, glm::vec3(37.0f, 1.0f, 88.0f));
	gengarModel = glm::rotate(gengarModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	gengarNormalMatrix = glm::mat3(glm::inverseTranspose(view * gengarModel));


	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(gengarModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(gengarNormalMatrix));

	gengar.Draw(shader);

	//gengar4
	gengarModel = glm::mat4(1.0f);
	gengarModel = glm::translate(gengarModel, glm::vec3(55.0f, 1.0f, 47.0f));
	gengarModel = glm::rotate(gengarModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	gengarNormalMatrix = glm::mat3(glm::inverseTranspose(view * gengarModel));


	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(gengarModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(gengarNormalMatrix));

	gengar.Draw(shader);
}


void renderLucario(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	lucarioModel = glm::mat4(1.0f);
	lucarioModel = glm::translate(lucarioModel, glm::vec3(35.0f, -1.0f, 56.0f));
	lucarioModel = glm::scale(lucarioModel, glm::vec3(3.0f, 3.0f, 3.0f));
	lucarioModel = glm::rotate(lucarioModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lucarioNormalMatrix = glm::mat3(glm::inverseTranspose(view * lucarioModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lucarioModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(lucarioNormalMatrix));

	lucario.Draw(shader);


	lucarioModel = glm::mat4(1.0f);
	lucarioModel = glm::translate(lucarioModel, glm::vec3(5.0f, -0.8f, 18.0f));
	lucarioModel = glm::scale(lucarioModel, glm::vec3(3.0f, 3.0f, 3.0f));
	lucarioModel = glm::rotate(lucarioModel, glm::radians(250.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lucarioNormalMatrix = glm::mat3(glm::inverseTranspose(view * lucarioModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lucarioModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(lucarioNormalMatrix));

	lucario.Draw(shader);




	lucarioModel = glm::mat4(1.0f);
	lucarioModel = glm::translate(lucarioModel, glm::vec3(84.0f, 1.0f, 92.0f));
	lucarioModel = glm::scale(lucarioModel, glm::vec3(3.0f, 3.0f, 3.0f));
	lucarioModel = glm::rotate(lucarioModel, glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lucarioNormalMatrix = glm::mat3(glm::inverseTranspose(view * lucarioModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lucarioModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(lucarioNormalMatrix));

	lucario.Draw(shader);



	lucarioModel = glm::mat4(1.0f);
	lucarioModel = glm::translate(lucarioModel, glm::vec3(67.0f, -1.5f, 80.0f));
	lucarioModel = glm::scale(lucarioModel, glm::vec3(3.0f, 3.0f, 3.0f));
	lucarioNormalMatrix = glm::mat3(glm::inverseTranspose(view * lucarioModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lucarioModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(lucarioNormalMatrix));

	lucario.Draw(shader);

}


void renderGeodude(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	geodudeModel = glm::mat4(1.0f);
	geodudeModel = glm::translate(geodudeModel, glm::vec3(21.0f, 2.0f, 34.0f));
	geodudeModel = glm::rotate(geodudeModel, glm::radians(65.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	geodudeNormalMatrix = glm::mat3(glm::inverseTranspose(view * geodudeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(geodudeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(geodudeNormalMatrix));

	geodude.Draw(shader);






	geodudeModel = glm::mat4(1.0f);
	geodudeModel = glm::translate(geodudeModel, glm::vec3(42.0f, 2.0f, 27.0f));
	geodudeModel = glm::rotate(geodudeModel, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	geodudeNormalMatrix = glm::mat3(glm::inverseTranspose(view * geodudeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(geodudeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(geodudeNormalMatrix));

	geodude.Draw(shader);



	geodudeModel = glm::mat4(1.0f);
	geodudeModel = glm::translate(geodudeModel, glm::vec3(62.0f, 2.0f, -1.0f));
	geodudeModel = glm::rotate(geodudeModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	geodudeNormalMatrix = glm::mat3(glm::inverseTranspose(view * geodudeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(geodudeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(geodudeNormalMatrix));

	geodude.Draw(shader);





	geodudeModel = glm::mat4(1.0f);
	geodudeModel = glm::translate(geodudeModel, glm::vec3(45.0f, 2.0f, -37.0f));
	geodudeModel = glm::rotate(geodudeModel, glm::radians(100.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	geodudeNormalMatrix = glm::mat3(glm::inverseTranspose(view * geodudeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(geodudeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(geodudeNormalMatrix));

	geodude.Draw(shader);




}
void renderGyarados(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	gyaradosModel = glm::mat4(1.0f);
	gyaradosModel = glm::translate(gyaradosModel, glm::vec3(40.0f, -3.0f, -240.0f));
	gyaradosModel = glm::rotate(gyaradosModel, glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	gyaradosModel = glm::scale(gyaradosModel, glm::vec3(0.8f, 0.8f, 0.8f));
	gyaradosNormalMatrix = glm::mat3(glm::inverseTranspose(view * gyaradosModel));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(gyaradosModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(gyaradosNormalMatrix));

	gyarados.Draw(shader);


}




void renderLapras(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	laprasModel = glm::mat4(1.0f);
	laprasModel = glm::translate(laprasModel, glm::vec3(61.0f, -1.0f, -70.0f));
	laprasModel = glm::rotate(laprasModel, glm::radians(-120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	laprasModel = glm::scale(laprasModel, glm::vec3(3.0f, 3.0f, 3.0f));
	laprasNormalMatrix = glm::mat3(glm::inverseTranspose(view * laprasModel));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(laprasModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(laprasNormalMatrix));

	lapras.Draw(shader);


}
glm::vec3 triangleVerticesKyurem[3] = {
	glm::vec3(-13.0f, 0.0f, 20.0f),   // Vertex 1
	glm::vec3(2.0f, 0.0f, -30.0f), // Vertex 2
	glm::vec3(56.0f, 0.0f, -15.0f)   // Vertex 3
};

void renderKyurem(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	
	
	float speed = 0.001f; // Adjust this for faster/slower movement
	timeCounter += speed;

	if (timeCounter >= 1.0f) {
		// Move to the next vertex in the triangle
		timeCounter = 0.0f;
		currentVertex = (currentVertex + 1) % 3;
	}

	// Interpolate between currentVertex and the next vertex
	glm::vec3 start = triangleVerticesKyurem[currentVertex];
	glm::vec3 end = triangleVerticesKyurem[(currentVertex + 1) % 3];
	glm::vec3 currentPosition = glm::mix(start, end, timeCounter);

	// Update the model matrix with the new position
	kyuremModel = glm::mat4(1.0f);
	kyuremModel = glm::translate(kyuremModel,currentPosition);
	kyuremModel = glm::rotate(kyuremModel, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	kyuremNormalMatrix = glm::mat3(glm::inverseTranspose(view * kyuremModel));

	// Pass matrices to shader
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(kyuremModel));

	if (isDepthMap == false) {
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(kyuremNormalMatrix));
	}

	// Draw Kyurem
	kyurem.Draw(shader);
}

void renderSnorlax(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();

	snorlaxModel = glm::mat4(1.0f);
	snorlaxModel = glm::translate(snorlaxModel, glm::vec3(-19.0f, -1.0f, -14.0f));
	snorlaxNormalMatrix = glm::mat3(glm::inverseTranspose(view * snorlaxModel));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snorlaxModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(snorlaxNormalMatrix));

	snorlax.Draw(shader);


	snorlaxModel = glm::mat4(1.0f);
	snorlaxModel = glm::translate(snorlaxModel, glm::vec3(46.0f, -0.5f, 7.0f));
	snorlaxNormalMatrix = glm::mat3(glm::inverseTranspose(view * snorlaxModel));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snorlaxModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(snorlaxNormalMatrix));

	snorlax.Draw(shader);


	snorlaxModel = glm::mat4(1.0f);
	snorlaxModel = glm::translate(snorlaxModel, glm::vec3(35.0f, -0.5f, -30.0f));
	snorlaxModel = glm::rotate(snorlaxModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	snorlaxNormalMatrix = glm::mat3(glm::inverseTranspose(view * snorlaxModel));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(snorlaxModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(snorlaxNormalMatrix));

	snorlax.Draw(shader);


}

void renderWater()
{   
	waterShader.useShaderProgram();
	waterModel = glm::mat4(1.0f);
	waterModel = glm::scale(waterModel, glm::vec3(1.0f, 1.0, 0.5f));
	waterModel = glm::translate(waterModel, glm::vec3(60.0f, -1.0f, -150.0f));

	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(waterModel));
	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glCheckError();
	
	float time = glfwGetTime();
	// Send wave parameters to the vertex shader
	glUniform1f(glGetUniformLocation(waterShader.shaderProgram, "time"), time);
	glUniform1f(glGetUniformLocation(waterShader.shaderProgram, "waveAmplitude"), 0.9f); // Example amplitude
	glUniform1f(glGetUniformLocation(waterShader.shaderProgram, "waveFrequency"), 2.0f); // Example frequency
	glUniform1f(glGetUniformLocation(waterShader.shaderProgram, "waveSpeed"), 1.0f);     // Example speed

	glCheckError();

	water.Draw(waterShader);



	

}

float eveeScaleFactor = 1.0f;

void processEveeCatch()
{
	if ((glm::distance(myCamera.getCameraPosition(), glm::vec3(-5.0f, 2.0f, 5.0f)) <= 8.0f && isAttacking == true))
	{
		eveeScaleFactor = 0.0f;
	}
}

void renderEvee(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();

	eveeModel = glm::mat4(1.0f);
	eveeModel = glm::translate(eveeModel, glm::vec3(5.0f, -1.1f, 5.0f));
	eveeModel = glm::scale(eveeModel, glm::vec3(eveeScaleFactor, eveeScaleFactor, eveeScaleFactor));
	eveeNormalMatrix = glm::mat3(glm::inverseTranspose(view * eveeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(eveeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(eveeNormalMatrix));

	evee.Draw(shader);



	eveeModel = glm::mat4(1.0f);
	eveeModel = glm::translate(eveeModel, glm::vec3(-17.0f, -0.85f, 14.0f));
	eveeModel = glm::rotate(eveeModel, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	eveeNormalMatrix = glm::mat3(glm::inverseTranspose(view * eveeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(eveeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(eveeNormalMatrix));

	evee.Draw(shader);




	eveeModel = glm::mat4(1.0f);
	eveeModel = glm::translate(eveeModel, glm::vec3(3.0f, -1.8f, -20.0f));
	eveeNormalMatrix = glm::mat3(glm::inverseTranspose(view * eveeModel));
	eveeModel = glm::rotate(eveeModel, glm::radians(60.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(eveeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(eveeNormalMatrix));

	evee.Draw(shader);


	eveeModel = glm::mat4(1.0f);
	eveeModel = glm::translate(eveeModel, glm::vec3(19.0f, -1.5f, -16.0f));
	eveeModel = glm::rotate(eveeModel, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	eveeNormalMatrix = glm::mat3(glm::inverseTranspose(view * eveeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(eveeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(eveeNormalMatrix));

	evee.Draw(shader);



	eveeModel = glm::mat4(1.0f);
	eveeModel = glm::translate(eveeModel, glm::vec3(44.0f, 0.4f, -11.0f));
	eveeModel = glm::rotate(eveeModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	eveeNormalMatrix = glm::mat3(glm::inverseTranspose(view * eveeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(eveeModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(eveeNormalMatrix));

	evee.Draw(shader);
}
void renderDragonite(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();

	// Calculate current position between two vertices
	float speed = 0.0008f; // Adjust this for faster/slower movement
	timeCounter += speed;

	if (timeCounter >= 1.0f) {
		// Move to the next vertex in the triangle
		timeCounter = 0.0f;
		currentVertex = (currentVertex + 1) % 3;
	}

	// Interpolate between currentVertex and the next vertex
	glm::vec3 start = triangleVertices[currentVertex];
	glm::vec3 end = triangleVertices[(currentVertex + 1) % 3];
	glm::vec3 currentPosition = glm::mix(start, end, timeCounter);

	dragoniteModel = glm::mat4(1.0f);
	dragoniteModel = glm::translate(dragoniteModel, currentPosition);
	dragoniteModel = glm::scale(dragoniteModel, glm::vec3(0.02f, 0.02f, 0.02f));
	dragoniteModel = glm::rotate(dragoniteModel, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	dragoniteNormalMatrix = glm::mat3(glm::inverseTranspose(view * dragoniteModel));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(dragoniteModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(dragoniteNormalMatrix));

	dragonite.Draw(shader);
}

void renderVolcano(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();

	volcanoModel = glm::mat4(1.0f);
	volcanoModel = glm::translate(volcanoModel, glm::vec3(25.0f, 2.0f, -44.0f));
	volcanoModel = glm::scale(volcanoModel, glm::vec3(0.3f, 0.3f, 0.3f));
	volcanoNormalMatrix = glm::mat3(glm::inverseTranspose(view * volcanoModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(volcanoModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(volcanoNormalMatrix));

	volcano.Draw(shader);
}
void renderCrypt(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();

	cryptModel = glm::mat4(1.0f);

	// Increment the rotation angle
	cryptRotationAngle += 0.5f; // Adjust the speed of rotation by changing the increment value

	// Apply translation
	cryptModel = glm::translate(cryptModel, glm::vec3(25.3f, 1.5f, -44.0f));

	// Apply rotation around the Y-axis
	cryptModel = glm::rotate(cryptModel, glm::radians(cryptRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	// Apply scaling
	cryptModel = glm::scale(cryptModel, glm::vec3(0.5f, 0.5f, 0.5f));

	cryptNormalMatrix = glm::mat3(glm::inverseTranspose(view * cryptModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(cryptModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(cryptNormalMatrix));

	crypt.Draw(shader);
}



void renderPokeball(gps::Shader shader, bool isDepthMap) {
	
	if (isWeaponPickedUp == false)
	{
		// Use the shader program
		shader.useShaderProgram();
		// First pass: Render the Pokéball normally
		glStencilFunc(GL_ALWAYS, 1, 0xFF); // Always pass and set stencil to 1
		glStencilMask(0xFF);              // Enable writing to stencil buffer
		pokeballNormalMatrix = glm::mat3(glm::inverseTranspose(view * pokeballModel));

		// Pass the model matrix to the shader
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(pokeballModel));

		// If not depth map, pass the normal matrix
		if (!isDepthMap) {
			glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(pokeballNormalMatrix));
		}

		// Draw the Pokéball
		pokeball.Draw(shader);

		// Second pass: Render the outline
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Pass only where stencil is not 1
		glStencilMask(0x00);                // Disable writing to stencil buffer
		glDisable(GL_DEPTH_TEST);           // Disable depth test for outline

		outliningShader.useShaderProgram(); // Use the outlining shader
		float scale = 1.1f;
		glm::mat4 scaledModel = glm::scale(pokeballModel, glm::vec3(scale, scale, scale));
		glUniformMatrix4fv(glGetUniformLocation(outliningShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(scaledModel));
		glUniformMatrix4fv(glGetUniformLocation(outliningShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glCheckError();
		// Render the scaled outline
		pokeball.Draw(outliningShader);

		// Restore state
		glStencilMask(0xFF);                // Re-enable writing to stencil buffer
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glCheckError();// Reset stencil function
		glEnable(GL_DEPTH_TEST);            // Re-enable depth test
		glCheckError();
	}
	else
	{
		// Use the shader program
		shader.useShaderProgram();
		
		
		pokeballNormalMatrix = glm::mat3(glm::inverseTranspose(view * pokeballModel));

		// Pass the model matrix to the shader
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(pokeballModel));

		// If not depth map, pass the normal matrix
		if (!isDepthMap) {
			glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(pokeballNormalMatrix));
		}

		// Draw the Pokéball
		pokeball.Draw(shader);
	}
}


void renderMagnemite(gps::Shader shader, bool isDepthMap, float deltaTime)
{
	shader.useShaderProgram();

	// Define boundaries
	const float minX = 0.0f;
	const float maxX = 30.0f;

	// Extract the current position from the model matrix
	glm::vec3 currentPosition = glm::vec3(magnemiteModel[3]);

	// Check if Magnemite reaches the boundaries and reverse direction if necessary
	if (currentPosition.x <= minX || currentPosition.x >= maxX) {
		magnemiteVelocity.x = -magnemiteVelocity.x; // Reverse X velocity
		magnemiteRotationAngle += 90.0f;
	}

	// Immediately update the position after reversing the velocity
	currentPosition += magnemiteVelocity * deltaTime * magnemiteMovingSpeed;

	// Rebuild the model matrix with the updated position
	magnemiteModel = glm::mat4(1.0f); // Reset to identity matrix
	magnemiteModel = glm::translate(magnemiteModel, currentPosition);
	magnemiteModel = glm::rotate(magnemiteModel, glm::radians(magnemiteRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	// Update the normal matrix
	magnemiteNormalMatrix = glm::mat3(glm::inverseTranspose(view * magnemiteModel));

	// Pass updated matrices to the shader
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(magnemiteModel));

	if (!isDepthMap)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(magnemiteNormalMatrix));

	// Draw Magnemite
	magnemite.Draw(shader);
}




void renderArceus(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	arceusModel = glm::mat4(1.0f);
	arceusModel = glm::translate(arceusModel, glm::vec3(72.0f, 8.0f, -28.0f));
	arceusModel = glm::scale(arceusModel, glm::vec3(3.0f, 3.0f, 3.0f));
	arceusModel = glm::rotate(arceusModel, glm::radians(-70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	arceusNormalMatrix = glm::mat3(glm::inverseTranspose(view * arceusModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(arceusModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(arceusNormalMatrix));

	arceus.Draw(shader);
}


void renderOrc(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	orcModel = glm::mat4(1.0f);

	orcNormalMatrix = glm::mat3(glm::inverseTranspose(view * orcModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(orcModel));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(orcNormalMatrix));

	//orc.Draw(shader);
}




void renderPokemonGround1(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	scene1Model = glm::mat4(1.0f);
	scene1Model = glm::translate(scene1Model, glm::vec3(0.0f, -5.5f, 0.0f));
	scene1NormalMatrix = glm::mat3(glm::inverseTranspose(view * scene1Model));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(scene1Model));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(scene1NormalMatrix));

	scene1.Draw(shader);

}


void renderPokemonGround2(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	scene2Model = glm::mat4(1.0f);
	scene2Model = glm::translate(scene2Model, glm::vec3(0.0f, -6.0f, -90.0f));
	scene2NormalMatrix = glm::mat3(glm::inverseTranspose(view * scene2Model));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(scene2Model));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(scene2NormalMatrix));

	scene2.Draw(shader);

}

void renderPokemonGround3(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	scene3Model = glm::mat4(1.0f);
	scene3Model = glm::translate(scene3Model, glm::vec3(-150.0f, -6.0f, -90.0f));
	scene3NormalMatrix = glm::mat3(glm::inverseTranspose(view * scene3Model));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(scene3Model));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(scene3NormalMatrix));

	scene3.Draw(shader);

}

void renderPokemonGround4(gps::Shader shader, bool isDepthMap)
{
	shader.useShaderProgram();
	scene4Model = glm::mat4(1.0f);
	scene4Model = glm::translate(scene4Model, glm::vec3(-150.0f, -6.0f, 0.0f));
	scene4NormalMatrix = glm::mat3(glm::inverseTranspose(view * scene4Model));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(scene4Model));

	if (isDepthMap == false)
		glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgram, "nomralMatrix"), 1, GL_FALSE, glm::value_ptr(scene4NormalMatrix));

	scene4.Draw(shader);

}

void renderForceField(gps::Shader shader)
{
	shader.useShaderProgram();
	forceFieldModel = glm::mat4(1.0f);
	forceFieldModel = glm::translate(forceFieldModel, glm::vec3(25.0f, 2.2f, -44.0f));
	forceFieldModel = glm::scale(forceFieldModel, glm::vec3(3.5f, 3.5f, 3.5f));

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(forceFieldModel));

	force_field.Draw(shader);
}

// Function to calculate a point on a cubic Bezier curve
glm::vec3 calculateBezierPoint(float t, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3) {
	float u = 1.0f - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	glm::vec3 point = uuu * P0; // (1 - t)^3 * P0
	point += 3 * uu * t * P1;   // 3 * (1 - t)^2 * t * P1
	point += 3 * u * tt * P2;   // 3 * (1 - t) * t^2 * P2
	point += ttt * P3;          // t^3 * P3

	return point;
}


void updatePokeballPosition(float deltaTime) {
	// Set weapon relative to the camera's position
	glm::vec3 cameraPos = myCamera.getCameraPosition();
	glm::vec3 cameraFront = myCamera.getCameraFront();
	glm::vec3 cameraUp = myCamera.getCameraUp();
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

	// Offset to position the weapon in front of the camera
	glm::vec3 weaponOffset = cameraFront * 1.5f + cameraRight * 0.5f - cameraUp * 0.5f;

	// If attacking, adjust weapon offset for animation
	if (isAttacking) {
		attackTime += deltaTime;

		// Calculate a sinusoidal movement for the up-down motion
		float attackProgress = attackTime / attackDuration;
		float verticalOffset = sin(glm::pi<float>() * attackProgress) * 0.3f;

		// Apply vertical offset
		weaponOffset -= cameraUp * verticalOffset;

		// Stop animation when finished
		if (attackTime >= attackDuration) {
			isAttacking = false;
			attackTime = 0.0f;
		}
	}

	// Update weapon model matrix
    pokeballModel = glm::mat4(1.0f);
	pokeballModel = glm::translate(pokeballModel, cameraPos + weaponOffset);

	// Align weapon orientation with the camera
	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0] = glm::vec4(cameraRight, 0.0f);  // X-axis
	rotation[1] = glm::vec4(cameraUp, 0.0f);     // Y-axis
	rotation[2] = glm::vec4(-cameraFront, 0.0f); // Z-axis (negative for OpenGL coordinate system)
	pokeballModel *= rotation;

	// Scale the Pokéball
	pokeballModel = glm::scale(pokeballModel, glm::vec3(4.0f, 4.0f, 4.0f));
}



void renderScene1ToDepthMap()
{
	glCheckError();
	simpleDepthShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glCheckError();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCheckError();

	float lastFrameTime = 0.0f;
	float currentFrameTime = glfwGetTime();
	float deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	glCheckError();

	processEveeCatch();
	renderSnorlax(simpleDepthShader, true);
	renderMagnemite(simpleDepthShader, true, 0.4f);
	//renderGround(simpleDepthShader, true);
	renderPokemonGround1(simpleDepthShader, true);
	renderEvee(simpleDepthShader, true);
	renderDragonite(simpleDepthShader, true);

	renderOrc(simpleDepthShader, true);
	renderCrypt(simpleDepthShader, true);

	glCheckError();

	

	glCheckError();


	renderPokeball(simpleDepthShader, true);


	glCheckError();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderScene1(float deltaTime, gps::Shader shader)
{
	glCheckError();

	renderScene1ToDepthMap();

	glCheckError();

	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myBasicShader.useShaderProgram();

	if (cutsceneActive)
	{
		updateCamera(deltaTime, viewLoc, cutsceneSpeed);
	}
	else
	{
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	glCheckError();
	/* lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	 glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));*/

	 //bind the shadow map

	glActiveTexture(GL_TEXTURE3);

	glCheckError();

	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glCheckError();
	glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

	glCheckError();
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glUniform1i(glGetUniformLocation(shader.shaderProgram, "isScene2"), isScene2);

	glCheckError();

	// renderGround(shader, false);

	renderPokemonGround1(shader, false);

	renderSnorlax(shader, false);
	
	processEveeCatch();
	renderMagnemite(shader, false, 0.4f);
	renderEvee(shader, false);
	renderDragonite(shader, false);

	renderOrc(shader, false);
	renderCrypt(shader, false);

	glCheckError();

	if (isWeaponPickedUp == true)
	{
		updatePokeballPosition(deltaTime);

	}


	glCheckError();

	renderPokeball(shader, false);
	glCheckError();

	/*glUniform1f(transparencyLoc, 0.5f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);*/

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND); // Ensure blending is disabled if not needed

	// Set transparency and render force field
	shader.useShaderProgram(); // Ensure correct shader is active
	glUniform1f(transparencyLoc, 0.7f);
	glCheckError(); // Check for errors after setting transparency uniform

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderForceField(shader); // Render the force field with transparency
	glCheckError(); // Final error check


	glDisable(GL_BLEND);
	glCheckError();
	if (glm::distance(myCamera.getCameraPosition(), glm::vec3(25.0f, 2.0f, -44.0f)) <= 0.4f)
	{
		currentScene = 2;
		myCamera.setPosition(glm::vec3(0.0f, 2.0f, 35.0f));
		isScene2 = true;
		sendLightsForScene2();
	}
	glCheckError();
	hillSkyBox.Draw(skyboxShader, view, projection);

}


void renderScene2ToDepthMap()
{
	glCheckError();
	simpleDepthShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glCheckError();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCheckError();

	float lastFrameTime = 0.0f;
	float currentFrameTime = glfwGetTime();
	float deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	glCheckError();


	//renderGround(simpleDepthShader, true);
	renderPokemonGround2(simpleDepthShader, true);
	renderGengar(simpleDepthShader, true);
	renderKyurem(simpleDepthShader, true);
	renderOrc(simpleDepthShader, true);

	glCheckError();

	if (isWeaponPickedUp == true)
	{
		updatePokeballPosition(deltaTime);

	}

	glCheckError();


	renderPokeball(simpleDepthShader, true);
	

	glCheckError();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderScene2(float deltaTime, gps::Shader shader)
{
	glCheckError();

	renderScene2ToDepthMap();

	glCheckError();

	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myBasicShader.useShaderProgram();

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glCheckError();
	/* lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	 glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));*/

	 //bind the shadow map

	glActiveTexture(GL_TEXTURE3);

	glCheckError();

	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glCheckError();
	glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);
	glUniform1i(glGetUniformLocation(shader.shaderProgram, "isScene2"), isScene2);
	glCheckError();
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));



	glCheckError();

	// renderGround(shader, false);

	renderPokemonGround2(shader, false);
	renderGengar(shader, false);
	renderKyurem(shader, false);
	renderOrc(shader, false);



	if (isWeaponPickedUp == true)
	{
		updatePokeballPosition(deltaTime);

	}


	glCheckError();

	renderPokeball(shader, false);

	
	/*glUniform1f(transparencyLoc, 0.5f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);*/

	//renderParticles();
	glCheckError();
	nightSkyBox.Draw(skyboxShader, view, projection);
}


void renderScene3ToDepthMap()
{
	glCheckError();
	simpleDepthShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glCheckError();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCheckError();

	float lastFrameTime = 0.0f;
	float currentFrameTime = glfwGetTime();
	float deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	glCheckError();


	//renderGround(simpleDepthShader, true);
	renderPokemonGround3(simpleDepthShader, true);
	renderLucario(simpleDepthShader, true);
	renderGyarados(simpleDepthShader, true);
	renderLapras(simpleDepthShader, true);
	renderOrc(simpleDepthShader, true);

	glCheckError();

	if (isWeaponPickedUp == true)
	{
		updatePokeballPosition(deltaTime);

	}

	glCheckError();


	renderPokeball(simpleDepthShader, true);


	glCheckError();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderScene3(float deltaTime, gps::Shader shader)
{
	glCheckError();

	renderScene3ToDepthMap();

	glCheckError();

	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myBasicShader.useShaderProgram();

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glCheckError();
	/* lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	 glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));*/

	 //bind the shadow map

	glActiveTexture(GL_TEXTURE3);

	glCheckError();

	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glCheckError();
	glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

	glCheckError();
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glUniform1i(glGetUniformLocation(shader.shaderProgram, "isScene2"), isScene2);


	glCheckError();

	// renderGround(shader, false);

	renderPokemonGround3(shader, false);
	renderLucario(shader, false);
	renderGyarados(shader, false);
	renderLapras(shader, false);
	renderOrc(shader, false);



	if (isWeaponPickedUp == true)
	{
		updatePokeballPosition(deltaTime);

	}


	glCheckError();

	renderPokeball(shader, false);
	renderWater();

	/*glUniform1f(transparencyLoc, 0.5f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);*/

	//renderParticles();
	glCheckError();
	mySkyBox.Draw(skyboxShader, view, projection);
}

void renderScene4ToDepthMap()
{
	glCheckError();
	simpleDepthShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glCheckError();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCheckError();

	float lastFrameTime = 0.0f;
	float currentFrameTime = glfwGetTime();
	float deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	glCheckError();


	//renderGround(simpleDepthShader, true);
	renderPokemonGround4(simpleDepthShader, true);
	renderArceus(simpleDepthShader, true);
	renderGeodude(simpleDepthShader, true);

	renderOrc(simpleDepthShader, true);

	glCheckError();

	if (isWeaponPickedUp == true)
	{
		updatePokeballPosition(deltaTime);

	}

	glCheckError();


	renderPokeball(simpleDepthShader, true);


	glCheckError();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void renderScene4(float deltaTime, gps::Shader shader)
{
	glCheckError();

	renderScene4ToDepthMap();

	glCheckError();

	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	myBasicShader.useShaderProgram();

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glCheckError();
	/* lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	 glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));*/

	 //bind the shadow map

	glActiveTexture(GL_TEXTURE3);

	glCheckError();

	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glCheckError();
	glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

	glCheckError();
	glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glUniform1i(glGetUniformLocation(shader.shaderProgram, "isScene2"), isScene2);

	glCheckError();

	// renderGround(shader, false);

	renderPokemonGround4(shader, false);
	renderGeodude(shader, false);
	renderArceus(shader, false);
	renderOrc(shader, false);



	if (isWeaponPickedUp == true)
	{
		updatePokeballPosition(deltaTime);

	}


	glCheckError();

	renderPokeball(shader, false);


	/*glUniform1f(transparencyLoc, 0.5f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);*/

	//renderParticles();
	glCheckError();
	redSkyBox.Draw(skyboxShader, view, projection);
}

void renderParticles(float deltaTime, glm::vec3 position) {
	particleShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(particleShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(particleShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	Particles->Update(deltaTime, position, 10, glm::vec3(0.0f, 0.0f, 0.0f)); // Spawn 10 particles
	Particles->Draw();
}


void cleanup() {
	myWindow.Delete();
	//cleanup code for your own data
}

int main(int argc, const char* argv[]) {

	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	quadVAO = createQuadVAO();
	initFireParticles();
	initOpenGLState();
	initModels();
	initShaders();
	initTextures();
	initUniforms();
	initSkyBox();
	initFBO();
	setWindowCallbacks();

	glCheckError();

	float lastFrameTime = 0.0f;


	/*createParticleBuffer();
	initParticles();*/
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {


		float currentFrameTime = glfwGetTime();

		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		//simulateParticles(deltaTime);

		processMovement();


		//se verifica distanta pana la arma doar daca arma nu este inca ridicata
		if (isWeaponPickedUp == false)
		{
			processWeaponPickUp();
		}


		switch (currentScene)
		{
		case 1:
			renderScene1(deltaTime, myBasicShader);
			break;
		case 2:
			renderScene2(deltaTime, myBasicShader);
			break;
		case 3:
			renderScene3(deltaTime, myBasicShader);
			break;
		case 4:
			renderScene4(deltaTime, myBasicShader);
			break;
		}

		
		glCheckError();
		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());


	}

	cleanup();

	return EXIT_SUCCESS;
}


