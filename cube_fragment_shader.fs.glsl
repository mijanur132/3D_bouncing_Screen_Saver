#version 330 core

// fragment shader has to have a vec4 output.
out vec4 color;

void main() {
	color = vec4(0.1, 0.1, 0.9, 1.0);
}