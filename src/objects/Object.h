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
#include <vector>
#include "../kixor/objLoader.h"

struct face {
			int vertIndices[3];
			int normalIndices[3];
			};

class Object {

public:

	Object();
	Object(const Object&);
	virtual ~Object();
	Object& operator=(const Object& right );
	void draw( GLuint modelLocation, GLuint viewLocation, GLuint, GLuint vertexLocation, GLuint colorLocation , Mat4* , Mat4*);
	void setFaces( objLoader* loader );
	void rotate( float , float , float );
	void setColor( TVec4<GLfloat>* );
	void updatePosition();

	Mat4 matTranslation;
	Mat4 matRotation;
	Mat4 matScale;
	Mat4 matModel;

	TVec4<GLfloat>* vertexDrawList;
	TVec4<GLfloat>* colorDrawList;
	TVec4<GLfloat>* normalDrawList;

	TVec3<GLfloat> vecVelocity;
	TVec3<GLfloat> vecAcceleration;

	TVec4<GLfloat>* vertices;
	TVec4<GLfloat>* colors;
	TVec4<GLfloat>* normals;

	bool isConstrained;
	GLfloat constraints[4];

	face* faces;
	int numFaces;

	int numVertices;
	GLfloat height, width, depth;

protected:

	GLuint vao, vbo[2];

	};

#endif /* GL_SHAPE_H_ */
