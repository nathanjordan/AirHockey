/*
 * GL_Object.cpp
 *
 *  Created on: Sep 21, 2011
 *      Author: njordan
 */

#include "Object.h"
#include <limits>

Object::Object( ) {
	
	glGenBuffers( 2, vbo );

	glGenVertexArrays( 1, &vao );

	numVertices = 0;

	colors = 0;

	vertices = 0;

	matTranslation = matTranslation.I();

	matRotation = matRotation.I();

	matScale = matScale.I();

	matModel = matTranslation * matScale * matRotation;

	}

Object::~Object() {

	delete [] vertices;
	delete [] colors;

	}
Object& Object::operator=( Object& right ) {

	this->setVertices( right.numVertices , right.vertices );

	this->setColors( right.numVertices , right.colors );

	this->matTranslation = right.matTranslation;

	this->matRotation = right.matRotation;

	this->matScale = right.matScale;

	this->height = right.height;

	this->width = right.width;

	this->depth = right.depth;

	return *this;

	}

void Object::draw( GLuint modelLocation, GLuint viewLocation, GLuint projectionLocation, GLuint vertexLocation, GLuint colorLocation , Mat4* viewMatrix, Mat4* projMatrix) {

	GLfloat* tempVertices = new GLfloat[ numVertices * 4];

	for( int i = 0 ; i < numVertices ; i++ ) {

		for( int j = 0 ; j < 4 ; j++ ) {

			tempVertices[i * 4 + j] = vertices[i][j];

			}

		}

	GLfloat* tempColors = new GLfloat[ numVertices * 4];

	for( int i = 0 ; i < numVertices ; i++ ) {

		for( int j = 0 ; j < 4 ; j++ ) {

			tempColors[i * 4 + j] = colors[i][j];

			}

		}

	matModel = matTranslation * matScale * matRotation;

	glBindVertexArray( vao );

	// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)
	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );

	// Copy the vertex data from diamond to our buffer
	// 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values
	glBufferData( GL_ARRAY_BUFFER , numVertices * 4 * sizeof(GLfloat) , tempVertices , GL_STATIC_DRAW );

	// Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex
	glVertexAttribPointer( vertexLocation , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

	// Enable attribute index 0 as being used
	glEnableVertexAttribArray( vertexLocation );

	//Bind our second VBO as being the active buffer and storing vertex attributes (colors)
	glBindBuffer( GL_ARRAY_BUFFER , vbo[1] );

	// Copy the color data from colors to our buffer
	// 12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values
	glBufferData( GL_ARRAY_BUFFER , numVertices * 4 * sizeof(GLfloat) , tempColors , GL_STATIC_DRAW );

	// Specify that our color data is going into attribute index 1, and contains three floats per vertex
	glVertexAttribPointer( colorLocation , 4 , GL_FLOAT, GL_FALSE, 0, 0);

	// Enable attribute index 1 as being used
	glEnableVertexAttribArray( colorLocation );

	glUniformMatrix4fv( modelLocation , 1 , true , matModel );

	glUniformMatrix4fv( viewLocation, 1 , true , *viewMatrix );

	glUniformMatrix4fv( projectionLocation, 1 , true , *projMatrix );

	glDrawArrays( GL_POLYGON , 0 , numVertices );

	delete [] tempColors;
	delete [] tempVertices;

	}

void Object::setVertices( int numVerts , TVec4<GLfloat>* verts ) {

	numVertices = numVerts;

	if(vertices)
		delete [] vertices;

	vertices = new TVec4<GLfloat>[numVertices];

	for( int i = 0 ; i < numVerts ; i++ ) {

		for( int j = 0 ; j < 4 ; j++ ) {

			vertices[i][j] = verts[i][j];

			}
		}

	float minX = vertices[0][0], minY = vertices[0][1], minZ = vertices[0][2], maxX = vertices[0][0], maxY = vertices[0][1], maxZ = vertices[0][2];

	for( int i = 0 ; i < numVerts ; i++ ) {

		if(vertices[i][0] < minX)
			minX = vertices[i][0];

		if(vertices[i][1] < minY)
			minY = vertices[i][1];

		if(vertices[i][2] < minZ)
			minZ = vertices[i][2];

		if(vertices[i][0] > maxX)
			maxX = vertices[i][0];

		if(vertices[i][1] > maxY)
			maxY = vertices[i][1];

		if(vertices[i][2] > maxZ)
			maxZ = vertices[i][2];

		}

	width = maxX - minX;
	height = maxY - minY;
	depth = maxZ - minZ;

	}

void Object::setColors( int numColors , TVec4<GLfloat>* newColors ) {

	if( numColors != numVertices )
		throw 30;

	if(colors)
		delete [] colors;

	colors = new TVec4<GLfloat>[numVertices];

	for( int i = 0 ; i < numColors ; i++ ) {

		for( int j = 0 ; j < 4 ; j++ ) {

			colors[i][j] = newColors[i][j];

			}
		}

	}
