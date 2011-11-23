#version 120

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec4 vertex;
in vec4 normal;
in vec2 vTexCoord;

// lighting stuff
varying  vec3 fN;
varying  vec3 fE;
varying  vec3 fL;
uniform vec4 LightPosition;

varying vec2 texCoord;

void main() {

	vec3 normal2;
	normal2.xyz = normal.xyz;

	//lighting stuff
	fN = normal2;
    fE = vertex.xyz;
    fL = LightPosition.xyz;

	if( LightPosition.w != 0.0 ) {
		fL = LightPosition.xyz - vertex.xyz;
    }

	
	//gl_Position =  projection * view * model * vertex;
	
	gl_Position = gl_ModelViewProjectionMatrix * model * vertex;
	
	texCoord    = vTexCoord;
	
	}