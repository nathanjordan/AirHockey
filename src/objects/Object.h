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


typedef TVec4<GLfloat> point4;
typedef TVec4<GLfloat> color4;

//struct stuff

struct shaderloc {
	GLuint program;
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint modelLocation;
	GLuint viewLocation;
	GLuint projectionLocation;
	GLuint normalLocation;
	GLuint vertexLocation;
	GLuint ambientLocation;
	GLuint diffuseLocation;
	GLuint specularLocation;
	GLuint lightLocation;
	GLuint shininessLocation;

	//lighting variables
	point4 light_position;
	color4 light_ambient;
	color4 light_diffuse;
	color4 light_specular;

	//matrixes
	Mat4 viewMatrix;
	Mat4 projMatrix;
	};

struct material {
	color4 material_ambient;
	color4 material_diffuse;
	color4 material_specular;
	float material_shininess;

};

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
	void draw( shaderloc&);
	void setFaces( objLoader* loader );
	void rotate( float , float , float );
	void setColor( material& );
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

	material colorAttributes;

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
