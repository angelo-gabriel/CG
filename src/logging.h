#pragma once

#include <GL/glew.h>

#include <string_view>
#include <format>
#include <fstream>
#include <iostream>
#include <chrono>

inline constexpr std::string_view GL_LOG_FILE = "gl.log";

inline bool restart_gl_log()
{
	std::ofstream file(std::string(GL_LOG_FILE), std::ios::trunc);

	if (file.is_open())
	{
		auto now = std::chrono::system_clock::now();

		file << std::format("GL_LOG_FILE log. local time {:%Y-%m-%d %H:%M:%S}\n", now);
		file << "------------------------\n";
		return true;
	}

	std::cerr << std::format("ERROR: could not open log file {} for writing\n", GL_LOG_FILE);
	return false;
}

template <typename... Args>
bool gl_log(std::string_view fmt, Args&&... args)
{
	std::ofstream file(std::string(GL_LOG_FILE), std::ios::app);

	if (file.is_open())
	{
		std::string message = std::vformat(fmt, std::make_format_args(args...));
		file << message << std::endl;
		return true;
	}

	return false;
}

template<typename... Args>
bool gl_log_err(std::string_view fmt, Args&&... args)
{
	std::string message = std::vformat(fmt, std::make_format_args(args...));

	gl_log("ERROR: {}", message);

	std::cerr << "ERROR: " << message << std::endl;

	return true;
}

void log_gl_params();