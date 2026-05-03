#include "maths_funcs.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "logging.h"

void glfw_error_callback(int error, const char* description)
{
	gl_log_err("GLFW ERROR: code {} msg: {}\n", error, description);
}

int g_win_width = 640;
int g_win_height = 480;

int g_fb_width = 640;
int g_fb_height = 480;

GLfloat points[] = {
	0.0f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f
};

const char* GL_type_to_string(GLenum type)
{
	switch (type)
	{
	case GL_BOOL: return "bool";
	case GL_INT: return "int";
	case GL_FLOAT: return "float";
	case GL_FLOAT_VEC2: return "vec2";
	case GL_FLOAT_VEC3: return "vec3";
	case GL_FLOAT_VEC4: return "vec4";
	case GL_FLOAT_MAT2: return "mat2";
	case GL_FLOAT_MAT3: return "mat3";
	case GL_FLOAT_MAT4: return "mat4";
	case GL_SAMPLER_2D: return "sampler2D";
	case GL_SAMPLER_3D: return "sampler3D";
	case GL_SAMPLER_CUBE: return "samplerCube";
	case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
	default: break;
	}
	return "other";
}

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
		double fps = (double)frame_count / elapsed_seconds;
		std::string title = std::format("opengl @ fps: {:.2f}", fps);
		glfwSetWindowTitle(window, title.c_str());
		frame_count = 0;
	}
	frame_count++;
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

void print_all(GLuint program)
{
	printf("------------------------\nshader program %i info:\n", program);
	int params = -1;
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	printf("GL_LINK_STATUS = %i\n", params);

	glGetProgramiv(program, GL_ATTACHED_SHADERS, &params);
	printf("GL_ATTACHED_SHADERS = %i\n", params);

	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &params);
	printf("GL_ACTIVE_ATTRIBUTES = %i\n", params);

	for (GLuint i = 0; i < (GLuint)params; i++)
	{
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveAttrib(program, i, max_length, &actual_length, &size, &type, name);

		if (size > 1)
		{
			for (int j = 0; j < size; j++)
			{
				char long_name[64];
				sprintf(long_name, "%s[%i]", name, j);
				int location = glGetAttribLocation(program, long_name);
				printf(" %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), long_name, location);
			}
		}
		else
		{
			int location = glGetAttribLocation(program, name);
			printf(" %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), name, location);
		}
	}

	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &params);
	printf("GL_ACTIVE_UNIFORMS = %i\n", params);
	for (GLuint i = 0; i < (GLuint)params; i++)
	{
		char name[64];
		int max_length = 64;
		int actual_length = 0;
		int size = 0;
		GLenum type;
		glGetActiveUniform(program, i, max_length, &actual_length, &size, &type, name);
		
		if (size > 1)
		{
			for (int j = 0; j < size; j++)
			{
				char long_name[64];
				sprintf(long_name, "%s[%i]", name, j);
				int location = glGetUniformLocation(program, long_name);
				printf(" %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), long_name, location);
			}
		}
		else
		{
			int location = glGetUniformLocation(program, name);
			printf(" %i) type:%s name:%s location:%i\n", i, GL_type_to_string(type), name, location);
		}
	}

	_print_program_info_log(program);
}

bool is_valid(GLuint program)
{
	glValidateProgram(program);
	int params = -1;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &params);
	printf("program %i GL_VALIDATE_STATUS = %i\n", program, params);
	if (GL_TRUE != params)
	{
		_print_program_info_log(program);
		return false;
	}
	return true;
}

// read shader from external file
std::string read_shader_file(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		fprintf(stderr, "ERROR: Could not open shader file: %s\n", path.c_str());
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

GLuint load_shader_program(const char* vertPath, const char* fragPath)
{
	std::string vertexCode = read_shader_file(vertPath);
	if (vertexCode.empty())
	{
		fprintf(stderr, "ERROR: Failed to read vertex shader file!\n");
	}
	printf("Loaded vertex shader (%zu bytes)\n", vertexCode.size());

	std::string fragmentCode = read_shader_file(fragPath);
	if (fragmentCode.empty())
	{
		fprintf(stderr, "ERROR: Failed to read fragment shader file!\n");
	}
	printf("Loaded fragment shader (%zu bytes)\n", fragmentCode.size());
	
	const char* vsrc = vertexCode.c_str();
	const char* fsrc = fragmentCode.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsrc, NULL);
	glCompileShader(vs);

	GLint success;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		_print_shader_info_log(vs);
		return 0;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsrc, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		_print_shader_info_log(fs);
		return 0;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		_print_program_info_log(program);
		glDeleteProgram(program);
		return 0;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main() {
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
	{
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	restart_gl_log();
	gl_log("starting GLFW: {}\n", glfwGetVersionString());

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

	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	float matrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	GLuint points_vbo = 0;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	GLuint colors_vbo = 0;
	glGenBuffers(1, &colors_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLuint shader_program = load_shader_program("shaders/test.vert", "shaders/test.frag");
	if (!shader_program)
	{
		fprintf(stderr, "ERROR: could not load shaders\n");
		return 1;
	}

	int matrix_location = glGetUniformLocation(shader_program, "matrix");
	glUseProgram(shader_program);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, matrix);

	glfwSetWindowSizeCallback(window, glfw_window_size_callback);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_resize_callback);

	glClearColor(0.6f, 0.7f, 0.8f, 1.0f);

	float cam_speed = 1.0f;
	float cam_yaw_speed = 10.0f;

	// camera variables
	float cam_pos[] = { 0.0f, 0.0f, 2.0f };
	float cam_yaw = 0.0f;

	mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
	mat4 view_mat = R * T;

	// input variables
	float near = 0.1f;
	float far = 100.0f;
	float fov= 67.0f * ONE_DEG_IN_RAD;
	float aspect = (float)g_win_width / (float)g_win_height;
	// matrix components
	float range = tan(fov * 0.5f) * near;
	float Sx = (2.0f * near) / (range * aspect + range * aspect);
	float Sy = near / range;
	float Sz = -(far + near) / (far - near);
	float Pz = -(2.0f * far * near) / (far - near);

	GLfloat proj_mat[] = {
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, -1.0f,
		0.0f, 0.0f, Pz, 0.0f
	};

	// get location numbers of matrices
	GLint view_mat_location = glGetUniformLocation(shader_program, "view");
	GLint proj_mat_location = glGetUniformLocation(shader_program, "proj");
	glUseProgram(shader_program);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);

	// winding and back-face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	// ---------- rendering loop ----------
	while (!glfwWindowShouldClose(window))
	{
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		_update_fps_counter(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_fb_width, g_fb_height);
		glUseProgram(shader_program);
		glBindVertexArray(vao);

		static bool reloadPressed = false;
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			if (!reloadPressed)
			{
				GLuint new_program = load_shader_program("shaders/test.vert", "shaders/test.frag");

				if (new_program)
				{
					glDeleteProgram(shader_program);
					shader_program = new_program;
					glUseProgram(shader_program);

					matrix_location = glGetUniformLocation(shader_program, "matrix");
					glUniformMatrix4fv(matrix_location, 1, GL_FALSE, matrix);
					printf("Shaders successfully reloaded.\n");
				}
				else
				{
					printf("Failed to reload shaders.\n");
				}
				reloadPressed = true;
			}
		}
		else
		{
			reloadPressed = false;
		}

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwPollEvents();

		// ----- camera movement -----
		bool cam_moved = false;
		if (glfwGetKey(window, GLFW_KEY_A)) 
		{
			cam_pos[0] -= cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) 
		{
			cam_pos[0] += cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
		{
			cam_pos[1] += cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
		{
			cam_pos[1] -= cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_W))
		{
			cam_pos[2] -= cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S))
		{
			cam_pos[2] += cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) 
		{
			cam_yaw += cam_yaw_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) 
		{
			cam_yaw -= cam_yaw_speed * elapsed_seconds;
			cam_moved = true;
		}
		// update view matrix
		if (cam_moved)
		{
			mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
			mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
			mat4 view_mat = R * T;
			glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
		}

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, 1);
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
} 