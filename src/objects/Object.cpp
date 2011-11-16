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

	normals = 0;

	faces = 0;

	vertexDrawList = 0;

	colorDrawList = 0;

	normalDrawList = 0;

	width = 0;

	height = 0;

	depth = 0;

	vecVelocity = TVec3<GLfloat>( 0.0 , 0.0 , 0.0 );

	matTranslation = matTranslation.I();

	matRotation = matRotation.I();

	matScale = matScale.I();

	matModel = matTranslation * matScale * matRotation;

	}

Object::Object( const Object& right ) {

    	numVertices = right.numVertices;

    	numFaces = right.numFaces;

        this->matTranslation = right.matTranslation;

        this->matRotation = right.matRotation;

        this->matScale = right.matScale;

        this->matModel = right.matModel;

        for( int i = 0 ; i < numFaces ; i++ ) {

        	faces[i] = right.faces[i];

			}

        for( int i = 0 ; i < numFaces * 3 ; i++ ) {

        	vertexDrawList[i] = right.vertexDrawList[i];

			colorDrawList[i] = right.colorDrawList[i];

			normalDrawList[i] = right.normalDrawList[i];

			}

        for( int i = 0 ; i < numVertices ; i++ ) {

        	vertices[i] = right.vertices[i];

        	colors[i] = right.colors[i];

        	normals[i] = right.normals[i];

			}

        this->height = right.height;

        this->width = right.width;

        this->depth = right.depth;


	}

Object::~Object() {

	delete [] vertices;
	delete [] colors;

	}
Object& Object::operator=( const Object& right ) {
    
    if (this != &right) {

    	numVertices = right.numVertices;

    	numFaces = right.numFaces;

        this->matTranslation = right.matTranslation;
        
        this->matRotation = right.matRotation;
        
        this->matScale = right.matScale;
        
        this->matModel = right.matModel;

        for( int i = 0 ; i < numFaces ; i++ ) {

        	faces[i] = right.faces[i];

			}

        for( int i = 0 ; i < numFaces * 3 ; i++ ) {

        	vertexDrawList[i] = right.vertexDrawList[i];

			colorDrawList[i] = right.colorDrawList[i];

			normalDrawList[i] = right.normalDrawList[i];

			}

        for( int i = 0 ; i < numVertices ; i++ ) {

        	vertices[i] = right.vertices[i];

        	colors[i] = right.colors[i];

        	normals[i] = right.normals[i];

			}

        this->height = right.height;
        
        this->width = right.width;
        
        this->depth = right.depth;

		}

	return *this;

	}

void Object::draw( GLuint modelLocation, GLuint viewLocation, GLuint projectionLocation, GLuint vertexLocation, GLuint colorLocation , Mat4* viewMatrix, Mat4* projMatrix) {

	GLfloat* tempVertices = new GLfloat[ numFaces * 3 * 4];

	for( int i = 0 ; i < numFaces * 3 ; i++ ) {

		for( int j = 0 ; j < 4 ; j++ ) {

			tempVertices[i * 4 + j] = vertexDrawList[i][j];

			}

		}

	GLfloat* tempColors = new GLfloat[ numFaces * 3 * 4];

	for( int i = 0 ; i < numFaces * 3 ; i++ ) {

		for( int j = 0 ; j < 4 ; j++ ) {

			tempColors[i * 4 + j] = colorDrawList[i][j];

			}

		}

	GLfloat* tempNormals = new GLfloat[ numFaces * 3 * 4 ];

	for( int i = 0 ; i < numFaces * 3 ; i++ ) {

		for( int j = 0 ; j < 4 ; j++ ) {

			tempNormals[i * 4 + j] = normalDrawList[i][j];

			}

		}

	matModel = matTranslation * matScale * matRotation;

	glBindVertexArray( vao );

	// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)
	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );

	// Copy the vertex data from diamond to our buffer
	// 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values
	glBufferData( GL_ARRAY_BUFFER , numFaces * 3 * 4 * sizeof(GLfloat) , tempVertices , GL_STATIC_DRAW );

	// Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex
	glVertexAttribPointer( vertexLocation , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

	// Enable attribute index 0 as being used
	glEnableVertexAttribArray( vertexLocation );

	//Bind our second VBO as being the active buffer and storing vertex attributes (colors)
	glBindBuffer( GL_ARRAY_BUFFER , vbo[1] );

	// Copy the color data from colors to our buffer
	// 12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values
	glBufferData( GL_ARRAY_BUFFER , numFaces * 3 * 4 * sizeof(GLfloat) , tempColors , GL_STATIC_DRAW );

	// Specify that our color data is going into attribute index 1, and contains three floats per vertex
	glVertexAttribPointer( colorLocation , 4 , GL_FLOAT, GL_FALSE, 0, 0);

	// Enable attribute index 1 as being used
	glEnableVertexAttribArray( colorLocation );

	glUniformMatrix4fv( modelLocation , 1 , true , matModel );

	glUniformMatrix4fv( viewLocation, 1 , true , *viewMatrix );

	glUniformMatrix4fv( projectionLocation, 1 , true , *projMatrix );

	glDrawArrays( GL_TRIANGLES, 0 , numFaces * 3 );

	delete [] tempColors;
	delete [] tempVertices;
	delete [] tempNormals;

	}

void Object::setFaces( objLoader* loader ) {

	//Asumes 3 vertices/normals per face (triangle)

	numFaces = loader->faceCount;

	faces = new face[loader->faceCount];

	//vertex and normal indices
	for( int i = 0 ; i < loader->faceCount ; i++ ) {

		//vertex indices
		faces[i].vertIndices[0] = (loader->faceList[i])->vertex_index[0];

		faces[i].vertIndices[1] = (loader->faceList[i])->vertex_index[1];

		faces[i].vertIndices[2] = (loader->faceList[i])->vertex_index[2];

		//normal indices
		faces[i].normalIndices[0] = (loader->faceList[i])->normal_index[0];

		faces[i].normalIndices[1] = (loader->faceList[i])->normal_index[1];

		faces[i].normalIndices[2] = (loader->faceList[i])->normal_index[2];
		}

	numVertices = loader->vertexCount;

	vertices = new TVec4<GLfloat>[loader->vertexCount];

	for( int i = 0 ; i < loader->vertexCount ; i++ ) {

		vertices[i][0] = (loader->vertexList[i])->e[0];

		vertices[i][1] = (loader->vertexList[i])->e[1];

		vertices[i][2] = (loader->vertexList[i])->e[2];

		vertices[i][3] = 1.0;

		}

	colors = new TVec4<GLfloat>[loader->vertexCount];

	for( int i = 0 ; i < loader->vertexCount ; i++ ) {

		colors[i][0] = 1.0;

		colors[i][1] = 0.0;

		colors[i][2] = 0.0;

		colors[i][3] = 1.0;

		}

	normals = new TVec4<GLfloat>[loader->normalCount];

	for( int i = 0 ; i < loader->normalCount ; i++ ) {

		normals[i][0] = (loader->normalList[i])->e[0];

		normals[i][1] = (loader->normalList[i])->e[1];

		normals[i][2] = (loader->normalList[i])->e[2];

		normals[i][3] = 0.0;

		}

	//construct draw arrays

	vertexDrawList = new TVec4<GLfloat>[loader->faceCount * 3];

	for( int i = 0 ; i < numFaces ; i++ ) {

		vertexDrawList[ i * 3 + 0 ] = vertices[ faces[i].vertIndices[0] ];

		vertexDrawList[ i * 3 + 1 ] = vertices[ faces[i].vertIndices[1] ];

		vertexDrawList[ i * 3 + 2 ] = vertices[ faces[i].vertIndices[2] ];

		}

	colorDrawList = new TVec4<GLfloat>[loader->faceCount * 3];

	for( int i = 0 ; i < numFaces ; i++ ) {

		colorDrawList[ i * 3 + 0 ] = colors[ faces[i].vertIndices[0] ];

		colorDrawList[ i * 3 + 1 ] = colors[ faces[i].vertIndices[1] ];

		colorDrawList[ i * 3 + 2 ] = colors[ faces[i].vertIndices[2] ];

		}

	normalDrawList = new TVec4<GLfloat>[loader->faceCount * 3];

	for( int i = 0 ; i < numFaces ; i++ ) {

		normalDrawList[ i * 3 + 0 ] = normals[ faces[i].normalIndices[0] ];

		normalDrawList[ i * 3 + 1 ] = normals[ faces[i].normalIndices[1] ];

		normalDrawList[ i * 3 + 2 ] = normals[ faces[i].normalIndices[2] ];

		}

	}

void Object::setColor( TVec4<GLfloat>* color ) {

	for( int i = 0 ; i < numVertices ; i++ ) {

		colors[i] = *color;

		}

	for( int i = 0 ; i < numFaces * 3 ; i++ ) {

		colorDrawList[i] = *color;

		}

	}

void Object::updatePosition() {
	matTranslation[0][3] += vecVelocity[0];
	matTranslation[1][3] += vecVelocity[1];
	matTranslation[2][3] += vecVelocity[2];
	}
