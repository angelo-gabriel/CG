#version 410

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 atex_coord;

uniform mat4 view, proj;

out vec2 tex_coord;

void main()
{
	gl_Position = proj * view * vec4(vertex_position, 1.0);
	tex_coord = atex_coord;
}