/*
 * GL_Object.cpp
 *
 *  Created on: Sep 21, 2011
 *      Author: njordan
 */

#include "Object.h"
#include <limits>

Object::Object( bool stuff ) {
	
	glGenBuffers( 3, vbo );

	glGenTextures(1, textures);

	glGenVertexArrays( 1, &vao );

	//texturing stuff
	// texture data

	// allocate buffer
	colorAttributes.data = new unsigned char[ TextureSize * TextureSize * 3 ];
	char filename[] = {"/home/nitish/Desktop/AirHockey/wood.raw"};

	FILE * file;

	// open and read texture data
	file = fopen( filename, "rb" );
	fread( colorAttributes.data, TextureSize * TextureSize * 3, 1, file );
	fclose( file );

	colorAttributes.texturized = stuff;

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

	vecAcceleration = TVec3<GLfloat>( 0.0 , 0.0 , 0.0 );

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

        colorAttributes = right.colorAttributes;

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

void Object::draw( shaderloc &prog) {

	color4 ambient_product = mul( prog.light_ambient , colorAttributes.material_ambient);
	color4 diffuse_product = mul( prog.light_diffuse , colorAttributes.material_diffuse);
	color4 specular_product = mul( prog.light_specular , colorAttributes.material_specular);

	GLfloat* tempVertices = new GLfloat[ numFaces * 3 * 4];


	for( int i = 0 ; i < numFaces * 3 ; i++ ) {

		for( int j = 0 ; j < 4 ; j++ ) {

			tempVertices[i * 4 + j] = vertexDrawList[i][j];

			}

		}

	TVec2<GLfloat>* tempTexils = new TVec2<GLfloat>[numFaces * 3];
	for( int i = 0; i< numFaces * 3; i+=6) {
		tempTexils[i] = point2(0.0, 0.0);
		tempTexils[i+1] = point2(0.0, 1.0);
		tempTexils[i+2] =point2(1.0, 1.0);
		tempTexils[i+3] = point2(0.0, 0.0);
		tempTexils[i+4] = point2(1.0, 1.0);
		tempTexils[i+5] =point2(1.0, 0.0);
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



	if( colorAttributes.texturized) {

		//texturing stuff
		glBindTexture(GL_TEXTURE_2D, textures[0]);

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, TextureSize, TextureSize, 0, GL_RGB, GL_UNSIGNED_BYTE, colorAttributes.data );

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

		//glActiveTexture( GL_TEXTURE0 );





	}


	glBindVertexArray( vao );


	// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)
	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );

	// Copy the vertex data from diamond to our buffer
	// 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values
	glBufferData( GL_ARRAY_BUFFER , numFaces * 3 * 4 * sizeof(GLfloat) , tempVertices , GL_STATIC_DRAW );

	// Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex
	glVertexAttribPointer( prog.vertexLocation , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

	// Enable attribute index 0 as being used
	glEnableVertexAttribArray( prog.vertexLocation );

	//Bind our second VBO as being the active buffer and storing vertex attributes (normals)
	glBindBuffer( GL_ARRAY_BUFFER , vbo[1] );

	// Copy the color data from colors to our buffer
	// 12 * sizeof(GLfloat) is the size of the colors array, since it contains 12 GLfloat values
	glBufferData( GL_ARRAY_BUFFER , numFaces * 3 * 4 * sizeof(GLfloat) , tempNormals , GL_STATIC_DRAW );

	// Specify that our color data is going into attribute index 1, and contains three floats per vertex
	glVertexAttribPointer( prog.normalLocation , 4 , GL_FLOAT, GL_FALSE, 0, 0);

	// Enable attribute index 1 as being used
	glEnableVertexAttribArray( prog.normalLocation );

	if(colorAttributes.texturized) {


			// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)
			glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );

			// Copy the vertex data from diamond to our buffer
			// 8 * sizeof(GLfloat) is the size of the diamond array, since it contains 8 GLfloat values
			glBufferData( GL_ARRAY_BUFFER , numFaces * 3 * 2 * sizeof(GLfloat) , tempTexils , GL_STATIC_DRAW );

			glEnableVertexAttribArray( prog.vTextCoordLocation );

			glVertexAttribPointer( prog.vTextCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0 );


			/*
			// Specify that our coordinate data is going into attribute index 0, and contains two floats per vertex
			glVertexAttribPointer( prog.textureLocation , 4 , GL_FLOAT , GL_FALSE , 0 , 0 );

			// Enable attribute index 0 as being used
			glEnableVertexAttribArray( prog.textureLocation );
	*/
	}

	glUniformMatrix4fv( prog.modelLocation , 1 , true , matModel );

	glUniformMatrix4fv( prog.viewLocation, 1 , true , prog.viewMatrix );

	glUniformMatrix4fv( prog.projectionLocation, 1 , true , prog.projMatrix );

	glUniform4fv( prog.ambientLocation, 1, ambient_product );

	glUniform4fv( prog.diffuseLocation, 1, diffuse_product );

	glUniform4fv( prog.specularLocation, 1, specular_product );

	glUniform4fv( prog.lightLocation, 1, prog.light_position );

	glUniform1f( prog.shininessLocation, colorAttributes.material_shininess );

	if (colorAttributes.texturized)
		glUniform1i( prog.textureLocation, 0 );


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
	GLfloat minX = vertices[0][0], minY = vertices[0][1] , minZ = vertices[0][2] , maxX = vertices[0][0], maxY = vertices[0][1] , maxZ = vertices[0][2];

	for( int i = 0 ; i < numVertices ; i++ ) {
		//x
		if( vertices[i][0] <  minX )
			minX = vertices[i][0];

		if( vertices[i][0] >  maxX )
			maxX = vertices[i][0];
		//y
		if( vertices[i][1] <  minY )
			minY = vertices[i][1];

		if( vertices[i][1] >  maxY )
			maxY = vertices[i][1];
		//z
		if( vertices[i][2] <  minZ )
			minZ = vertices[i][2];

		if( vertices[i][2] >  maxZ )
			maxZ = vertices[i][2];

		}

	width = maxX - minX;

	height = maxY - minY;

	depth = maxZ - minZ;

	}

void Object::setColor( material &colorStuff ) {

	colorAttributes.material_ambient = colorStuff.material_ambient;
	colorAttributes.material_diffuse = colorStuff.material_diffuse;
	colorAttributes.material_specular = colorStuff.material_specular;
	colorAttributes.material_shininess = colorStuff.material_shininess;

	/*
	for( int i = 0 ; i < numVertices ; i++ ) {

		colors[i] = *color;

		}

	for( int i = 0 ; i < numFaces * 3 ; i++ ) {

		colorDrawList[i] = *color;

		}
		*/

	}

void Object::updatePosition() {

	vecVelocity[0] += vecAcceleration[0];
	vecVelocity[1] += vecAcceleration[1];
	vecVelocity[2] += vecAcceleration[2];

	matTranslation[0][3] += vecVelocity[0];
	matTranslation[1][3] += vecVelocity[1];
	matTranslation[2][3] += vecVelocity[2];

	if( isConstrained ) {

		//x
		if(matTranslation[0][3] > (constraints[0] - width / 2 ) )
			matTranslation[0][3] = constraints[0] - width / 2;

		if(matTranslation[0][3] < (constraints[1] + width / 2) )
			matTranslation[0][3] = constraints[1] + width / 2;

		//z
		if(matTranslation[2][3] > (constraints[2] - depth / 2) )
			matTranslation[2][3] = constraints[2] - depth / 2;

		if(matTranslation[2][3] < (constraints[3] + depth / 2 ) )
			matTranslation[2][3] = constraints[3] + depth / 2;

		}

	}
