#version 120

uniform vec2 translation;
uniform float rotation;
uniform float scale;

in vec4 vPosition;
in vec4 in_Color;

varying vec4 ex_Color;

void main() {
	mat4 matTranslation = mat4( 1.0 , 0.0 , 0.0 , 0.0 , 0.0 , 1.0 , 0.0 , 0.0 , 0.0 , 0.0 , 1.0 , 0.0 , translation[0] , translation[1] , 0.0 , 1.0 );
	mat4 matRotation = mat4( cos( rotation ) , sin( rotation ) , 0.0 , 0.0 , -sin( rotation ) , cos( rotation ), 0.0 , 0.0, 0.0 , 0.0 , 1.0 , 0.0 , 0.0 , 0.0 , 0.0 , 1.0 );
	mat4 matScale = mat4( scale , 0.0 , 0.0 , 0.0 , 0.0 , scale , 0.0 , 0.0 , 0.0 , 0.0 , scale , 0.0 , 0.0 , 0.0 , 0.0 , 1.0 );
	
	gl_Position =  gl_ModelViewMatrix * matTranslation * matRotation * matScale * vPosition;
	
	//gl_Position =  gl_ModelViewMatrix * matRotation * (vPosition + vec4( translation[0] , translation[1] , 0.0 , 0.0 ) );
	
	ex_Color = in_Color;
	
	}