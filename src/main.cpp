#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "logging.h"

void glfw_error_callback(int error, const char* description)
{
	gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}

int g_win_width = 640;
int g_win_height = 480;

int g_fb_width = 640;
int g_fb_height = 480;

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
	g_win_width = width;
	g_win_height = height;
}

void glfw_framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	g_fb_width = width;
	g_fb_height = height;
}

double previous_seconds;
int frame_count;

void _update_fps_counter(GLFWwindow* window)
{
	double current_seconds;
	double elapsed_seconds;
	current_seconds = glfwGetTime();
	elapsed_seconds = current_seconds - previous_seconds;

	if (elapsed_seconds > 0.25)
	{
		previous_seconds = current_seconds;
		char tmp[128];
		double fps = (double)frame_count / elapsed_seconds;
		sprintf_s(tmp, sizeof(tmp), "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

// read shader from external file
std::string read_shader_file(const std::string& path)
{
	std::ifstream file;
	file.open(path);

	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

void _print_shader_info_log(GLuint shader_index)
{
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetShaderInfoLog(shader_index, max_length, &actual_length, log);
	printf("shader info log for GL index %u:\n%s\n", shader_index, log);
}

void _print_program_info_log(GLuint program)
{
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetProgramInfoLog(program, max_length, &actual_length, log);
	printf("program info log for GL index %u:\n%s\n", program, log);
}

int main() {
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
	{
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	gl_log("starting GLFW\n%s\n", glfwGetVersionString());

	glfwWindowHint(GLFW_SAMPLES, 8);

	// GLFWmonitor* mon = glfwGetPrimaryMonitor();
	// const GLFWvidmode* vmode = glfwGetVideoMode(mon);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
	
	if (!window)
	{
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	log_gl_params();

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLfloat points[] = {
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	std::string vertexCode = read_shader_file("shaders/test.vert");
	std::string fragmentCode = read_shader_file("shaders/test.frag");

	const char* vShaderSource = vertexCode.c_str();
	const char* fShaderSource = fragmentCode.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vShaderSource, NULL);
	glCompileShader(vs);

	// check for compilation errors
	int params = -1;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params)
	{
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
		_print_shader_info_log(vs);
		return false;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fShaderSource, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params)
	{
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
		_print_shader_info_log(fs);
		return false;
	}

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	// check for linking errors
	glGetProgramiv(shader_program, GL_LINK_STATUS, &params);
	if (GL_TRUE != params)
	{
		fprintf(stderr, "ERROR: could not link shader program GL index %u\n", shader_program);
		_print_program_info_log(shader_program);
		return false;
	}

	int vertexColorLocation = glGetUniformLocation(shader_program, "inputColor");
	glUseProgram(shader_program);
	glUniform4f(vertexColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);

	glfwSetWindowSizeCallback(window, glfw_window_size_callback);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_resize_callback);

	glClearColor(0.6f, 0.7f, 0.8f, 1.0f);
	
	// rendering loop
	while (!glfwWindowShouldClose(window))
	{
		_update_fps_counter(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_fb_width, g_fb_height);
		glUseProgram(shader_program);
		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}
	}

	glfwTerminate();
	return 0;
} 