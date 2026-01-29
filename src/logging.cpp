#include "logging.h"

struct GLParam { GLenum id; std::string_view name; };

void log_gl_params()
{
	const std::vector<GLParam> params = {
		{GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS"},
		{GL_MAX_CUBE_MAP_TEXTURE_SIZE, "GL_MAX_CUBE_MAP_TEXTURE_SIZE"},
		{GL_MAX_DRAW_BUFFERS, "GL_MAX_DRAW_BUFFERS"},
		{GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS"},
		{GL_MAX_TEXTURE_IMAGE_UNITS, "GL_MAX_TEXTURE_IMAGE_UNITS"},
		{GL_MAX_TEXTURE_SIZE, "GL_MAX_TEXTURE_SIZE"},
		{GL_MAX_VARYING_FLOATS, "GL_MAX_VARYING_FLOATS"},
		{GL_MAX_VERTEX_ATTRIBS, "GL_MAX_VERTEX_ATTRIBS"},
		{GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS"},
		{GL_MAX_VERTEX_UNIFORM_COMPONENTS, "GL_MAX_VERTEX_UNIFORM_COMPONENTS"},
		{GL_MAX_VIEWPORT_DIMS, "GL_MAX_VIEWPORT_DIMS"},
		{GL_STEREO, "GL_STEREO"}
	};
	
	gl_log("GL Context Params:\n");

	for (const auto& p : params)
	{
		if (p.id == GL_MAX_VIEWPORT_DIMS)
		{
			int v[2]{ 0, 0 };
			glGetIntegerv(p.id, v);
			gl_log("{}: {} x {}", p.name, v[0], v[1]);
		}
		else if (p.id == GL_STEREO)
		{
			unsigned char b = 0;
			glGetBooleanv(p.id, &b);
			gl_log("{}: {}", p.name, static_cast<bool>(b));
		}
		else
		{
			int v = 0;
			glGetIntegerv(p.id, &v);
			gl_log("{}: {}", p.name, v);
		}
	}
	gl_log("------------------------------\n");
}