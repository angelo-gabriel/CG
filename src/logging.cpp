#include "logging.h"

bool restart_gl_log()
{
	FILE* file;
	errno_t err = fopen_s(&file, GL_LOG_FILE, "w");
	if (err == 0 && file != NULL)
	{
		auto now = std::chrono::system_clock::now();
		auto legacyNow = std::chrono::system_clock::to_time_t(now);

		char date[30];
		ctime_s(date, sizeof(date), &legacyNow);
		fprintf(file, "GL_LOG_FILE log. local time %s\n", date);
		fclose(file);
		return true;
		fclose(file);
	}
	else {
		fprintf(
			stderr,
			"ERROR: could not open GL_LOG_FILE log file %s for writing\n",
			GL_LOG_FILE
		);
		return false;
	}
}

bool gl_log(const char* message, ...)
{
	va_list argptr;
	FILE* file;
	errno_t err = fopen_s(&file, GL_LOG_FILE, "a");
	if (err == 0 && file != NULL)
	{
		va_start(argptr, message);
		vfprintf(file, message, argptr);
		va_end(argptr);
		fclose(file);
		return true;
	}
	else {
		fprintf(
			stderr,
			"ERROR: could not open GL_LOG_FILE %s file for appending\n",
			GL_LOG_FILE
		);
		return false;
	}
}

bool gl_log_err(const char* message, ...)
{
	va_list argptr;
	FILE* file;
	errno_t err = fopen_s(&file, GL_LOG_FILE, "a");
	if (err == 0 && file != NULL)
	{
		va_start(argptr, message);
		vfprintf(file, message, argptr);
		va_end(argptr);
		va_start(argptr, message);
		vfprintf(stderr, message, argptr);
		va_end(argptr);
		fclose(file);
		return true;
	}
	else {
		fprintf(
			stderr,
			"ERROR: could not open GL_LOG_FILE %s file for appending\n",
			GL_LOG_FILE
		);
		return false;
	}
}

void log_gl_params()
{
	GLenum params[] = {
		GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		GL_MAX_DRAW_BUFFERS,
		GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		GL_MAX_TEXTURE_IMAGE_UNITS,
		GL_MAX_TEXTURE_SIZE,
		GL_MAX_VARYING_FLOATS,
		GL_MAX_VERTEX_ATTRIBS,
		GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		GL_MAX_VIEWPORT_DIMS,
		GL_STEREO,
	};
	
	const char* names[] = {
		"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
		"GL_MAX_CUBE_MAP_TEXTURE_SIZE",
		"GL_MAX_DRAW_BUFFERS",
		"GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
		"GL_MAX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_TEXTURE_SIZE",
		"GL_MAX_VARYING_FLOATS",
		"GL_MAX_VERTEX_ATTRIBS",
		"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
		"GL_MAX_VERTEX_UNIFORM_COMPONENTS",
		"GL_MAX_VIEWPORT_DIMS",
		"GL_STEREO",
	};
	
	gl_log("GL Context Params:\n");

	for (int i = 0; i < 10; i++)
	{
		int v = 0;
		glGetIntegerv(params[i], &v);
		gl_log("%s: %d\n", names[i], v);
	}

	int v[2]{};
	v[0] = v[1] = 0;
	glGetIntegerv(params[10], v);
	gl_log("%s %i %i\n", names[10], v[0], v[1]);
	unsigned char s = 0;
	glGetBooleanv(params[11], &s);
	gl_log("%s %u\n", names[11], (unsigned int)s);
	gl_log("-----------------------------\n");
}