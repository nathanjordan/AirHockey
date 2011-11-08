#version 120

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec4 vertex;
varying vec4 color;

varying vec3 N;
varying vec3 v;

void main() {
	
	v = vec3( view * model * vertex );       
	N = normalize( gl_NormalMatrix * gl_Normal );
	
	gl_Position =  projection * view * model * vertex;
	
	}