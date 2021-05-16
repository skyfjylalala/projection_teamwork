// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <GLM/glm.hpp>
using namespace glm;

#include "shader.hpp"

void saveImage(const unsigned int imageWidth, const unsigned int imageHeight)
{
	const unsigned int channelNum = 3; // RGBなら3, RGBAなら4
	void* dataBuffer = NULL;
	dataBuffer = (GLubyte*)malloc(imageWidth * imageHeight * channelNum);

	// 読み取るOpneGLのバッファを指定 GL_FRONT:フロントバッファ　GL_BACK:バックバッファ
	glReadBuffer(GL_BACK);

	// OpenGLで画面に描画されている内容をバッファに格納
	glReadPixels(
		0,                 //読み取る領域の左下隅のx座標
		0,                 //読み取る領域の左下隅のy座標 //0 or getCurrentWidth() - 1
		imageWidth,             //読み取る領域の幅
		imageHeight,            //読み取る領域の高さ
		GL_BGR, //it means GL_BGR,           //取得したい色情報の形式
		GL_UNSIGNED_BYTE,  //読み取ったデータを保存する配列の型
		dataBuffer      //ビットマップのピクセルデータ（実際にはバイト配列）へのポインタ
	);

	GLubyte* p = static_cast<GLubyte*>(dataBuffer);
	std::string fname = "outputImage.jpg";
	//IplImage* outImage = cvCreateImage(cvSize(imageWidth, imageHeight), IPL_DEPTH_8U, 3);

	cv::Mat outImage(imageHeight,imageWidth,  CV_8UC3);

	for (unsigned int j = 0; j < imageHeight; ++j)
	{
		for (unsigned int i = 0; i < imageWidth; ++i)
		{
			outImage.at<cv::Vec3b>((imageHeight - j - 1), i)[0] = *p;
			outImage.at<cv::Vec3b>((imageHeight - j - 1), i)[1] = *(p+1);
			outImage.at<cv::Vec3b>((imageHeight - j - 1), i)[2] = *(p+2);
			//outImage->imageData[(imageHeight - j - 1) * outImage->widthStep + i * 3 + 1] = *(p + 1);
			//outImage->imageData[(imageHeight - j - 1) * outImage->widthStep + i * 3 + 2] = *(p + 2);
			p += 3;
		}
	}

	cv::imwrite(fname.c_str(), outImage);
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int window_width = 1024, window_height = 768 ;
	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Tutorial 02 - Red triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");


	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
		
		glDisableVertexAttribArray(0);
		
		// Swap buffers
		glfwSwapBuffers(window);
		saveImage( window_width, window_height);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);
	
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
