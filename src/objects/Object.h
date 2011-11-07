/*
 * GL_Shape.h
 *
 *  Created on: Sep 21, 2011
 *      Author: njordan
 */

#ifndef GL_LINE_H_
#define GL_LINE_H_

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <limits>
#include <iostream>
#include "../math/vec4.h"
#include "../math/mat4.h"
#include <math.h>
#include <fstream>
#include <string>

class Object {

public:

	Object( );
	virtual ~Object();
	Object& operator=( Object& right );
	void draw( GLuint translationLocation, GLuint rotationLocation, GLuint scaleLocation, GLuint vertexLocation, GLuint colorLocation);
	void setVertices( int , TVec4<GLfloat>* );
	void setColors( int , TVec4<GLfloat>* );
	void rotate( float );

	Mat4 matTranslation;
	Mat4 matRotation;
	Mat4 matScale;
	TVec4<GLfloat>* vertices;
	TVec4<GLfloat>* colors;
	GLfloat height, width, depth;
	int numVertices;

protected:

	GLuint vao, vbo[2];

	};

#endif /* GL_SHAPE_H_ */
