#version 410

in vec3 color;
in vec2 tex_coord;

uniform sampler2D my_texture;

out vec4 frag_color;

void main()
{
	frag_color = texture(my_texture, tex_coord);
}