#version 150

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec4 vertex;
inout vec4 color;

void main() {

	gl_Position =  projection * view * model * vPosition;
	
	}