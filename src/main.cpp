/*
 * main.cpp
 *
 *  Created on: Nov 6, 2011
 *      Author: njordan
 */

#include <vector>
#include <fstream>
#include <math.h>

//OpenGL
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>

//OpenAL
#include <AL/al.h>
#include <AL/alut.h>

#include "objects/Object.h"
#include "kixor/objLoader.h"

char* parseGLSL( char* file );

void initWindow();

void initShaders();

void initObjects();

void initView();

void mouseMoveHandler(int x, int y);

void mouseButtonHandler( int button, int state, int x, int y);

void timerTick( int value );

void displayCallback();

void keyboardHandler(unsigned char key , int x , int y );

void specialHandler( int key , int x , int y );

TVec3<GLfloat>* crossProduct( TVec3<GLfloat>* a, TVec3<GLfloat>* b);

void updateViewMatrix();

GLuint program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint modelLocation;
GLuint viewLocation;
GLuint projectionLocation;
GLuint colorLocation;
GLuint vertexLocation;

Object* puck;
Object* paddle1;
Object* paddle2;
Object* table;

Mat4 viewMatrix;
Mat4 projMatrix;

TVec4<GLfloat> eye;
TVec4<GLfloat> at;

float currentAngle = 0;

std::vector<Object*> objectList;

int main() {

	initWindow();

	initShaders();

	initView();

	initObjects();

	glutMainLoop();

	return 0;
	}

char* parseGLSL( char* file ) {
	FILE *fptr;
	long length;
	char *buf;

	fptr = fopen(file, "rb"); /* Open file for reading */
	if (!fptr) /* Return NULL on failure */
		return NULL;
	fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
	length = ftell(fptr); /* Find out how many bytes into the file we are */
	buf = (char*)malloc(length+1); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
	fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
	fclose(fptr); /* Close the file */
	buf[length] = 0; /* Null terminator */

	return buf; /* Return the buffer */
	}

void initShaders() {

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	program = glCreateProgram();

	GLchar *vertexsource, *fragmentsource;

	vertexsource = parseGLSL((char*)"/home/njordan/Downloads/workspace/Air Hockey/src/shaders/shapes.vert");
	fragmentsource = parseGLSL((char*)"/home/njordan/Downloads/workspace/Air Hockey/src/shaders/shapes.frag");

	glShaderSource( vertexShader , 1 , (const GLchar**)&vertexsource , NULL );
	glShaderSource( fragmentShader , 1 , (const GLchar**)&fragmentsource , NULL );

	glCompileShader( vertexShader );
	glCompileShader( fragmentShader );

	int isCompiled_vertexShader;

	//get the shader status from the vertex shader
	glGetShaderiv( vertexShader , GL_COMPILE_STATUS, &isCompiled_vertexShader );

	//if the vertex shader did not compile, get some info about why it didnt
	if( ! (bool) isCompiled_vertexShader ) {

		char *vertexInfoLog;
		int maxLength,logLen;

		//Get the shader logs length
		glGetShaderiv( vertexShader , GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		vertexInfoLog = (char *)malloc(maxLength);

		//get the log info
		glGetShaderInfoLog( vertexShader , maxLength , &logLen , vertexInfoLog );

		std::string log( vertexInfoLog , logLen );

		std::cout << "ERROR COMPILING VERTEX SHADER: \n\n" << log;

		//exit indicating error
		exit(1);

		}

	int isCompiled_fragmentShader;

	//get the shader status from the fragment shader
	glGetShaderiv( fragmentShader , GL_COMPILE_STATUS, &isCompiled_fragmentShader );

	//if the fragment shader did not compile, get some info about why it didnt
	if( ! (bool) isCompiled_fragmentShader ) {

		char *fragmentInfoLog;
		int maxLength,logLen;

		//Get the shader logs length
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		fragmentInfoLog = (char *)malloc(maxLength);

		//get the log info
		glGetShaderInfoLog( fragmentShader , maxLength , &logLen , fragmentInfoLog );

		std::string log( fragmentInfoLog , logLen );

		std::cout << "ERROR COMPILING FRAG SHADER: \n\n" << log;

		//exit indicating error
		exit(1);

		}

	glAttachShader( program , vertexShader );
	glAttachShader( program , fragmentShader );

	glLinkProgram( program );

	int isLinked;

	glGetProgramiv( program , GL_LINK_STATUS , (int *)&isLinked );

	if( ! (bool) isLinked) {

		char *shaderProgramInfoLog;
		int maxLength,logLen;

		/* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		shaderProgramInfoLog = (char *)malloc(maxLength);

		/* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
		glGetProgramInfoLog( program , maxLength, &logLen, shaderProgramInfoLog );

		std::string log( shaderProgramInfoLog , logLen );

		std::cout << "ERROR LINKING: \n\n" << log;

		exit(1);
		}

	glUseProgram( program );

	modelLocation = glGetUniformLocation( program , "model" );

	viewLocation = glGetUniformLocation( program , "view" );

	projectionLocation = glGetUniformLocation( program , "projection" );

	vertexLocation = glGetAttribLocation( program , "vertex" );

	colorLocation = glGetAttribLocation( program , "color" );

	glDisable(GL_LIGHTING);

	/*glEnable(GL_LIGHTING);

	glEnable(GL_LIGHT0);

	GLfloat ambient[4] = { 0.2 , 0.2 , 0.2 , 1.0 };
	GLfloat diffuse[4] = { 1.0 , 1.0 , 1.0 , 1.0 };
	GLfloat specular[4] = { 1.0 , 1.0 , 1.0 , 1.0 };
	GLfloat pos[4] = { 0.0 , 0.0 , -1.0 , 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient );
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse );
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular );
	glLightfv(GL_LIGHT0, GL_POSITION, pos );
*/
    glEnable( GL_DEPTH_TEST );
    
    
    //clear the background to black
	glClearColor(1.0, 1.0 , 1.0 , 1.0 );
    
	}

void initWindow() {

	char** none = new char*[0];

	int noneCount = 0;

	glutInit(&noneCount,none);

	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutInitWindowSize(640,480);

	glutInitWindowPosition(0, 0);

	glutCreateWindow("Air Hockey");

	glutDisplayFunc( displayCallback );

	glewExperimental = GL_TRUE;

	glutMouseFunc(mouseButtonHandler);

	glutPassiveMotionFunc(mouseMoveHandler);

	glewInit();

	glutKeyboardFunc( keyboardHandler );

	glutSpecialFunc( specialHandler );

	glutTimerFunc( 15 , timerTick , 0 );

	}

void mouseMoveHandler(int x, int y) {

	}
void mouseButtonHandler( int button, int state, int x, int y) {

	}
void timerTick( int value ) {

	//redraw the screen
	glutPostRedisplay();

	//come back in 15ms
	glutTimerFunc( 15 , timerTick , 0 );

	}

void displayCallback() {

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//draw all the shapes
	for( unsigned int i = 0 ; i < objectList.size() ; i++ ) {

		objectList[i]->draw( modelLocation , viewLocation, projectionLocation, vertexLocation, colorLocation , &viewMatrix , &projMatrix );

		}

	//show the changes
	glutSwapBuffers();

	}

void keyboardHandler(unsigned char key , int x , int y ) {

	if( key == '[' ) {

		currentAngle += 0.1;

		updateViewMatrix();

		}

	else if (key == ']') {

		currentAngle -= 0.1;

		updateViewMatrix();

		}

	}
void specialHandler( int key , int x , int y ) {

	}

void readVertices( char* filename , Object* object ) {

	objLoader* loader = new objLoader();

	loader->load( filename );

	object->setFaces( loader );

	delete loader;

	}

void initObjects() {

	table = new Object();

	puck = new Object();

	paddle1 = new Object();

	paddle2 = new Object();

	readVertices( (char*) "/home/njordan/Desktop/test.obj" , table );

	objectList.push_back( table );

	table->matScale[0][0] = 1.0;

	table->matScale[1][1] = 1.0;

	table->matScale[2][2] = 1.0;

	table->matTranslation[0][3] = 0.0;

	table->matTranslation[1][3] = 0.0;

	}

void initView() {

	eye = TVec4<GLfloat>( 10.0 , 10.0 , 10.0 , 1.0 );

	at = TVec4<GLfloat>( 0.0 , 0.0 , 0.0 , 1.0 );

	viewMatrix = viewMatrix.I();

	TVec4<GLfloat> f  = norm( at - eye );

	TVec4<GLfloat> up  = norm( TVec4<GLfloat>( 0.0 , 1.0 , 0.0 , 1.0 ) );

	}

void updateViewMatrix() {

	}
