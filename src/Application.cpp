#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <iomanip>

#include "shader.h"

// callback for when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// resize complex plane based on mouse position
void resizePlane(float xpos, float ypos);
// callback for mouse scrollwheel to change zoom
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// listen for key presses, hit spacebar to reset to center
void processInput(GLFWwindow *window);
// Processing map function
float rescale(float value, float value_min, float value_max, float new_min, float new_max);
// prints x and y ranges, max number of iterations and current zoom
void printInfo(void);

// window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// portion of complex plane
float minX = -2, minY = -2;
float maxX = 2, maxY = 2;

const float ZOOM = 0.75f; // how much to zoom each time (75%)
unsigned int iterations = 50; // max number of iterations

unsigned int FPS = 0; // FPS counter

int main()
{
    glfwInit();

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }    

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    float vertices[] = {
        1.0f,  1.0f,  // top right
        1.0f, -1.0f,  // bottom right
        -1.0f, -1.0f, // bottom left
        -1.0f,  1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    unsigned int VAO; // vertex array object
    unsigned int VBO; //  vertex buffer object
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
   
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	// create and compile shaders
    Shader s("shaders/vertex.glsl", "shaders/fragment.glsl");

	// set mouse initially to center
	glfwSetCursorPos(window, (double)SCR_WIDTH / 2, (double)SCR_HEIGHT / 2);

	// for FPS 
	double previousTime = glfwGetTime(), currentTime;
	unsigned int frameCount = 0;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
		currentTime = glfwGetTime();
		frameCount++;
		
		if (currentTime - previousTime >= 1.0f) // if a second has passed
		{
			FPS = frameCount;

			frameCount = 0;
			previousTime = currentTime;
		}

        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        s.use();
		s.setFloat("minX", minX);
		s.setFloat("maxX", maxX);
		s.setFloat("minY", minY);
		s.setFloat("maxY", maxY);
		s.setUInt("iterations", iterations);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

		// update coordinates based on mouse position
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		resizePlane((float)xpos, (float)ypos);
    }

    glfwTerminate();
	std::cout << "Press ENTER to exit...";
	std::cin.get();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

	// hit spacebar to center with initial zoom
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		minX = minY = -2;
		maxX = maxY = 2;
		glfwSetCursorPos(window, (double)SCR_WIDTH / 2, (double)SCR_HEIGHT / 2);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// maps a value from an initial range to another range (Processing map function)
float rescale(float value, float value_min, float value_max, float new_min, float new_max)
{
	float scaled = (value - value_min) / (value_max - value_min);
	return scaled * (new_max - new_min) + new_min;
}

void resizePlane(float xpos, float ypos)
{
	float scaling = 0.001f * log10(iterations); // TODO find right scaling factor for mouse based on iterations

	float xRange = (maxX - minX) / 2;
	float yRange = (maxY - minY) / 2;
	float deltaX = rescale((float)xpos - (SCR_WIDTH / 2), -(signed int) SCR_WIDTH/2, SCR_WIDTH/2, -xRange, xRange) * scaling;
	float deltaY = rescale((float)ypos - (SCR_HEIGHT/ 2), SCR_HEIGHT / 2, -(signed int) SCR_HEIGHT/2, -yRange, yRange) * scaling;

	minX += deltaX;
	maxX += deltaX;
	minY += deltaY;
	maxY += deltaY;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// first center at mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	xpos = rescale((float)xpos, 0, SCR_WIDTH, minX, maxX);
	ypos = rescale((float)ypos, SCR_HEIGHT, 0, minY, maxY);

	float xRange = (maxX - minX) / 2;
	float yRange = (maxY - minY) / 2;

	minX = xpos - xRange;
	maxX = xpos + xRange;
	minY = ypos - yRange;
	maxY = ypos + yRange;

	// then update portion of complex plane (zoom is done relative to the center of the screen which is now at mouse position)
	float deltaX = (yoffset == 1 ? 1 : -1) * (maxX - minX) * (1 - ZOOM) / 2;
	float deltaY = (yoffset == 1 ? 1 : -1) * (maxY - minY) * (1 - ZOOM) / 2;

	// yoffset = 1 if scroll in, -1 if scroll out
	if (yoffset == 1)
	{
		deltaX = (maxX - minX) * (1 - ZOOM) / 2;
		deltaY = (maxY - minY) * (1 - ZOOM) / 2;

		iterations *= 1.05;
	}
	else
	{
		deltaX = -(maxX - minX) * (1 - ZOOM) / 2;
		deltaY = -(maxY - minY) * (1 - ZOOM) / 2;

		iterations /= 1.05;
	}

	minX += deltaX;
	maxX -= deltaX;
	minY += deltaY;
	maxY -= deltaY;

	printInfo();
}

void printInfo(void)
{
	std::cout << "FPS: " << FPS << std::endl;
	std::cout << std::setprecision(15);
	std::cout << "X(" << minX << ", " << maxX << ")\tY(" << minY << ", " << maxY << ")" << std::endl;
	std::cout << "zoom: " << 4 / (maxX - minX) <<	// 4 is initially maxX - minX
		"\tMax iterations:" << iterations << std::endl;

	std::cout << std::endl;
}