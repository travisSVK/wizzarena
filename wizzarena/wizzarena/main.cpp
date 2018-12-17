#include <GLFW/glfw3.h>
#include <glad/glad.h> 
#include <iostream>
#include "stb_image.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <chrono>

#include "core/ShaderLoader.h"
#include "core/Models.h"
#include "core/Vertex.h"
#include "core/TextureLoader.h"
#include "game/Player.h"

GLuint program;
GLuint playerProgram;

// view at settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool mouseRightPress = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, Game::Player &player, float height)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS)
    {
        mouseRightPress = true;
    }
    else if ((state == GLFW_RELEASE) && mouseRightPress)
    {
        // check cursor position and print it
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        //std::cout << xpos << ", " << abs(ypos - height) << std::endl;
        player.UpdatePosition(xpos, abs(ypos - height));
        mouseRightPress = false;
    }
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, 1920, 1080);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// add shaders
	Core::ShaderLoader shaderLoader;
	program = shaderLoader.CreateProgram("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    playerProgram = shaderLoader.CreateProgram("shaders/player_v_shader.glsl", "shaders/player_f_shader.glsl");
	// load and create a textures
	Core::TextureLoader textureLoader;
    unsigned int normalLavaTexture = textureLoader.LoadTexture("textures/Lava-Normal-1024x1024.png", true);
	unsigned int groundTexture = textureLoader.LoadTexture("textures/Ground-Normal-1024x1024.png", true);
    unsigned int highlightLavaTexture = textureLoader.LoadTexture("textures/Lava-Highligthed-1024x1024.png", true);
	//unsigned int texture2 = textureLoader->LoadTexture("textures/awesomeface.png", true);
	
	std::vector<Core::Vertex> vertices = {
        Core::Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Core::Vertex(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Core::Vertex(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Core::Vertex(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f))
	};
	std::vector<unsigned int> indices = {
		0, 1, 3,
		1, 2, 3
	};

	std::shared_ptr<Core::Models> modelManager = std::make_shared<Core::Models>();
	modelManager->CreateModel("baseModel", vertices, indices);

    Game::Player player(modelManager, textureLoader);
    auto startTime = std::chrono::system_clock::now();
    float radius = 700.0f;
    float sizeFactor = 0;

    float growingTime = 0.0f;
    float timeTillGrow = 0.0f;
    float lastTime = 0.0f;
    // render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window, player, 1080.0f);

        // update time since start
        std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;

        timeTillGrow += timeSinceStart.count() - lastTime;
        if (timeTillGrow >= 5)
        {
            if (growingTime < 10)
            {
                if (sizeFactor <= growingTime)
                {
                    radius -= 10;
                    ++sizeFactor;
                }
                growingTime += timeSinceStart.count() - lastTime;
            }
            else
            {
                timeTillGrow = 0.0f;
                growingTime = 0.0f;
                sizeFactor = 0;
            }
        }
        
        lastTime = timeSinceStart.count();

		// rendering
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use the created program
		glUseProgram(program);
        Core::ShaderLoader::setFloat(program, "radius", radius);
        Core::ShaderLoader::setFloat(program, "timeTillGrow", timeTillGrow / 5);
        Core::ShaderLoader::setFloat(program, "width", 1920.0f);
        Core::ShaderLoader::setFloat(program, "height", 1080.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalLavaTexture);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, highlightLavaTexture);
		glBindVertexArray(modelManager->GetModel("baseModel"));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // draw player
        player.Render(playerProgram);

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}