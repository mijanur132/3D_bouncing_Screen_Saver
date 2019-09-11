#version 330 core

// setup the vertex attributes
layout (location = 0) in vec3 position;

uniform mat4 translate;
// uniform variables come from the application.


void main() {	
	gl_Position = translate*vec4(position*0.2, 1.0);
}