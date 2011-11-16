#version 120

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec4 vertex;
in vec4 color;

varying vec4 fColor;

void main() {

	fColor = color;
	
	//gl_Position =  projection * view * model * vertex;
	
	gl_Position = gl_ModelViewProjectionMatrix * model * vertex;
	
	}