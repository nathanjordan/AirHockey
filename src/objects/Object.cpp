/*
 * GL_Object.cpp
 *
 *  Created on: Sep 21, 2011
 *      Author: njordan
 */

#include "Object.h"

Object::Object( ) {
	
	glGenBuffers( 2, vbo );

	glGenVertexArrays( 1, &vao );

	numVertices = 0;

	colors = 0;

	vertices = 0;

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

void Object::draw( GLuint translationLocation, GLuint rotationLocation, GLuint scaleLocation, GLuint vertexLocation, GLuint colorLocation) {

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

	glUniformMatrix4fv( translationLocation , 1 , true , matTranslation );

	glUniformMatrix4fv( rotationLocation, 1 , true , matRotation );

	glUniformMatrix4fv( scaleLocation , 1 , true , matScale );

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
