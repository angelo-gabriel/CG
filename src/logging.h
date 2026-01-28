#pragma once

#include <GL/glew.h>

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <cstdarg>

#define GL_LOG_FILE "gl.log"

bool restart_gl_log();

bool gl_log(const char* message, ...);

bool gl_log_err(const char* message, ...);

void log_gl_params();