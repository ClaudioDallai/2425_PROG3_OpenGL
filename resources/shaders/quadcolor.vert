#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 vert_col;

out vec4 vert_col_out;

void main()
{
    gl_Position = vec4(vert_pos, 1.f);
    vert_col_out = vec4(vert_col, 1.f);
}
