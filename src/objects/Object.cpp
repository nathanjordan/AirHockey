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

	numFaces = 0;

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
    
    if (this != &right) {

        //*this->loader = *right.loader;
        
        this->matTranslation = right.matTranslation;
        
        this->matRotation = right.matRotation;
        
        this->matScale = right.matScale;
        
        this->height = right.height;
        
        this->width = right.width;
        
        this->depth = right.depth;

		}

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

	int offset = 0;

	for( int i = 0 ; i < numFaces ; i++ ) {

		glDrawArrays( GL_POLYGON , offset , offset + faces[i].vertex_count );

		offset += faces[i].vertex_count;

		}

	//glDrawArrays( GL_POLYGON , 0 , numVertices );

	delete [] tempColors;
	delete [] tempVertices;

	}

void Object::setFaces( int numFaces , int numVertices, obj_face** newFaces , obj_vector** newVerts ) {

	faces = new obj_face[numFaces];

	vertices = new TVec4<GLfloat>[numVertices];

	colors = new TVec4<GLfloat>[numVertices];

	normals = new TVec4<GLfloat>[numVertices];

	this->numFaces = numFaces;

	this->numVertices = numVertices;

	for( int i = 0 ; i < numFaces ; i++ ) {

		faces[i] = *newFaces[i];

		}


	for( int i = 0, offset = 0 ; i < numFaces ; i++ ) {

		for( int j = 0 ; j < faces[i].vertex_count ; j++ ) {

			vertices[offset] = TVec4<GLfloat>( newVerts[ faces[i].vertex_index[j] ]->e[0] , newVerts[ faces[i].vertex_index[j] ]->e[1] , newVerts[ faces[i].vertex_index[j] ]->e[2] , 1.0  );

			normals[offset] = TVec4<GLfloat>( newVerts[ faces[i].normal_index[j] ]->e[0] , newVerts[ faces[i].normal_index[j] ]->e[1] , newVerts[ faces[i].normal_index[j] ]->e[2] , 0.0  );

			colors[offset++] = TVec4<GLfloat>( 1.0 , 1.0 , 1.0 , 1.0 );

			}

		}

	}
