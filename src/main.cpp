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

void calculateBounce();

void keyboardTimer( int x );

void specialUpHandler( int key , int x , int y );

void checkCollisions();

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

TVec3<GLfloat> eye;
TVec3<GLfloat> at;

float currentAngle = 0;

int prevX = -1 , prevY = -1;

bool movingLeft = false , movingForward = false, movingBackwards = false, movingRight = false;

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

	vertexsource = parseGLSL((char*)"/home/njordan/Downloads/workspace/AirHockey/src/shaders/shapes.vert");
	fragmentsource = parseGLSL((char*)"/home/njordan/Downloads/workspace/AirHockey/src/shaders/shapes.frag");

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

	eye = TVec3<GLfloat>( 1.0 , 1.0 , 1.0 );

	glOrtho( -5.0 , 5.0 , -5.0 , 5.0 , -10.0 , 10.0 );

	gluLookAt( 0.0 , 2.0 , 0.0 , 0.0 , 0.0 , 0.0 , -1.0 , 0.0 , 0.0 );

	//glDisable(GL_LIGHTING);

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
    
    
    //clear the background to black
	glClearColor(1.0, 1.0 , 1.0 , 1.0 );
    
	}

void initWindow() {

	char** none = new char*[0];

	int noneCount = 0;

	glutInit(&noneCount,none);

	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutInitWindowSize(700,700);

	glutInitWindowPosition(0, 0);

	glutCreateWindow("Air Hockey");

	glutDisplayFunc( displayCallback );

	glewExperimental = GL_TRUE;

	glutSetCursor( GL_NONE );

	glutMouseFunc(mouseButtonHandler);

	glutPassiveMotionFunc(mouseMoveHandler);

	glewInit();

	glutKeyboardFunc( keyboardHandler );

	glutSpecialFunc( specialHandler );

	glutSpecialUpFunc( specialUpHandler );

	glutTimerFunc( 15 , timerTick , 0 );

	glutTimerFunc( 30 , keyboardTimer , 0 );

	}

void mouseMoveHandler(int x, int y) {

	if( prevX < 0 ) {

		prevX = x;

		prevY = y;

		return;

		}

	float relZ = (prevX - x) * 0.02;

	float relX = -(prevY - y) * 0.02;

	prevX = x;

	prevY = y;

	paddle1->vecVelocity[0] = relX;

	paddle1->vecVelocity[2] = relZ;

	}
void mouseButtonHandler( int button, int state, int x, int y) {

	}
void timerTick( int value ) {

	calculateBounce();

	for( unsigned int i = 0 ; i < objectList.size() ; i++ ) {

		objectList[i]->updatePosition();

		}

	paddle1->vecVelocity[0] = 0.0;
	paddle1->vecVelocity[2] = 0.0;

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

		currentAngle += 0.01;

		updateViewMatrix();

		}

	else if (key == ']') {

		currentAngle -= 0.01;

		updateViewMatrix();

		}

	}
void specialHandler( int key , int x , int y ) {

	if( key == GLUT_KEY_UP ) {

		movingForward = true;

		}

	if( key == GLUT_KEY_DOWN ) {

		movingBackwards = true;

		}

	if( key == GLUT_KEY_LEFT ) {

		movingLeft = true;

		}

	if( key == GLUT_KEY_RIGHT ) {

		movingRight = true;

		}

	}
void specialUpHandler( int key , int x , int y )  {

	if( key == GLUT_KEY_UP ) {

		movingForward = false;

		paddle2->vecVelocity[0] = 0.0;

		}

	if( key == GLUT_KEY_DOWN ) {

		movingBackwards = false;

		paddle2->vecVelocity[0] = 0.0;

		}

	if( key == GLUT_KEY_LEFT ) {

		movingLeft = false;

		paddle2->vecVelocity[2] = 0.0;

		}

	if( key == GLUT_KEY_RIGHT ) {

		movingRight = false;

		paddle2->vecVelocity[2] = 0.0;

		}

	}
void keyboardTimer( int x ) {

	if( movingForward ) {
		paddle2->vecVelocity[0] += 0.05;
	}
	else if( movingBackwards ) {
		paddle2->vecVelocity[0] -= 0.05;
	}

	if( movingLeft ) {
		paddle2->vecVelocity[2] -= 0.05;
	}
	else if( movingRight ) {
		paddle2->vecVelocity[2] += 0.05;
	}

	checkCollisions();

	glutTimerFunc( 30 , keyboardTimer , 0 );

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

	readVertices( (char*) "/home/njordan/Desktop/table.obj" , table );

	objectList.push_back( table );

	readVertices( (char*) "/home/njordan/Desktop/puck.obj" , puck );

	objectList.push_back( puck );

	readVertices( (char*) "/home/njordan/Desktop/paddle.obj" , paddle1 );

	objectList.push_back( paddle1 );

	readVertices( (char*) "/home/njordan/Desktop/paddle.obj" , paddle2 );

	objectList.push_back( paddle2 );

	TVec4<GLfloat> white = TVec4<GLfloat>( 1.0 , 1.0 , 1.0 , 1.0 );

	TVec4<GLfloat> red = TVec4<GLfloat>( 1.0 , 0.0 , 0.0 , 1.0 );

	TVec4<GLfloat> black = TVec4<GLfloat>( 0.0 , 0.0 , 0.0 , 1.0 );

	TVec4<GLfloat> gray = TVec4<GLfloat>( 0.6 , 0.6 , 0.6 , 1.0 );

	puck->setColor( &black );

	table->setColor( &gray );

	paddle1->setColor( &red );

	paddle2->setColor( &red );

	puck->vecVelocity[0] = 0.02;

	puck->vecVelocity[2] = 0.01;

	paddle1->constraints[0] = 4.0;

	paddle1->constraints[1] = 2.0;

	paddle1->constraints[2] = 2.0;

	paddle1->constraints[3] = -2.0;

	paddle1->isConstrained = true;

	paddle2->constraints[0] = -2.0;

	paddle2->constraints[1] = -4.0;

	paddle2->constraints[2] = 2.0;

	paddle2->constraints[3] = -2.0;

	paddle2->isConstrained = true;

	}

void initView() {

	/*eye = TVec4<GLfloat>( 10.0 , 10.0 , 10.0 , 1.0 );

	at = TVec4<GLfloat>( 0.0 , 0.0 , 0.0 , 1.0);

	viewMatrix = viewMatrix.I();

	TVec4<GLfloat> x  = norm( at - eye );
	TVec4<GLfloat> f  = norm( at - eye );

	TVec4<GLfloat> up  = norm( TVec3<GLfloat>( 0.0 , 1.0 , 0.0 ) );

	TVec4<GLfloat> s = cross( f , up );

	TVec4<GLfloat> u = cross( s , f );

	//view crap
	viewMatrix[0][0] = s[0];
	viewMatrix[0][1] = s[1];
	viewMatrix[0][2] = s[2];

	viewMatrix[1][0] = u[0];
	viewMatrix[1][1] = u[1];
	viewMatrix[1][2] = u[2];

	viewMatrix[2][0] = -f[0];
	viewMatrix[2][1] = -f[1];
	viewMatrix[2][2] = -f[2];

	//translation crap
	viewMatrix[0][3] = -eye[0];
	viewMatrix[1][3] = -eye[1];
	viewMatrix[2][3] = -eye[2];
*/
	TVec4<GLfloat> a = TVec4<GLfloat>( 0.707107 , 0.0 , - 0.707107 , 1.0 );
	TVec4<GLfloat> b = TVec4<GLfloat>( -0.4082 , 0.8164 , -0.4082 , 1.0 );
	TVec4<GLfloat> c = TVec4<GLfloat>( 0.577 , 0.577 , 0.577 , -17.3205 );
	TVec4<GLfloat> d = TVec4<GLfloat>( 0.0 , 0.0 , 0.0 , 1.0 );

	viewMatrix = Mat4( a,b,c,d );

	a = TVec4<GLfloat>( 0.5 , 0.0 , 0.0 , 0.0 );
	b = TVec4<GLfloat>( 0.0 , 0.5 , 0.0 , 0.0 );
	c = TVec4<GLfloat>( 0.0 , 0.0 , -0.5 , 0.0 );
	d = TVec4<GLfloat>( 0.0 , 0.0 , 0.0 , 1.0 );

	projMatrix = Mat4( a,b,c,d );

	}

void updateViewMatrix() {

	eye[0] = 2 * cos( currentAngle );

	eye[2] = 2 * sin( currentAngle );

	glMatrixMode( GL_MODELVIEW );

	glLoadIdentity();

	glOrtho( -5.0 , 5.0 , -5.0 , 5.0 , -10.0 , 10.0 );

	gluLookAt( eye[0] , eye[1] , eye[2] , 0.0 , 0.0 , 0.0 , 0.0 , 1.0 , 0.0 );


	}

void calculateBounce() {

	float radius = puck->width / 2;

	float maxX = 4.0 - radius, minX = -4.0 + radius, maxZ = 2.0 - radius, minZ = -2.0 + radius;

	if( puck->matTranslation[0][3] > maxX ) {
		puck->vecVelocity[0] = - puck->vecVelocity[0];
		if(puck->vecVelocity[0] > 0 )
			puck->vecVelocity[0] = - puck->vecVelocity[0];
		}


	else if( puck->matTranslation[0][3] < minX ) {
		puck->vecVelocity[0] = - puck->vecVelocity[0];
		if(puck->vecVelocity[0] < 0 )
			puck->vecVelocity[0] = - puck->vecVelocity[0];
		}

	if( puck->matTranslation[2][3] > maxZ ) {
			puck->vecVelocity[2] = - puck->vecVelocity[2];
			if(puck->vecVelocity[2] > 0 )
				puck->vecVelocity[2] = - puck->vecVelocity[2];
			}


		else if( puck->matTranslation[2][3] < minZ ) {
			puck->vecVelocity[2] = - puck->vecVelocity[2];
			if(puck->vecVelocity[2] < 0 )
				puck->vecVelocity[2] = - puck->vecVelocity[2];
			}

	}

void checkCollisions() {

	TVec2<GLfloat> puckPoint( puck->matTranslation[0][3] , puck->matTranslation[2][3] );

	TVec2<GLfloat> paddle1Point( paddle1->matTranslation[0][3] , paddle1->matTranslation[2][3] );

	TVec2<GLfloat> paddle2Point( paddle2->matTranslation[0][3] , paddle2->matTranslation[2][3] );

	GLfloat paddlePuckRadius = ( puck->width / 2 ) + (paddle1->width / 2);

	GLfloat paddle1Dist = sqrt( pow( puckPoint[0] - paddle1Point[0] , 2 ) + pow( puckPoint[1] - paddle1Point[1] , 2 ) );

	GLfloat paddle2Dist = sqrt( pow( puckPoint[0] - paddle2Point[0] , 2 ) + pow( puckPoint[1] - paddle2Point[1] , 2 ) );

	if( paddle1Dist < paddlePuckRadius ) {

		float y = puckPoint[1] - paddle1Point[1];

		float x = puckPoint[0] - paddle1Point[0];

		float theta = atan( y / x );

		float puckVel = sqrt( pow( puck->vecVelocity[0] , 2 ) + pow( puck->vecVelocity[1] , 2 ) );

		float vZ = puckVel * cos( theta );

		float vX = puckVel * sin( theta );

		puck->vecVelocity[0] = vZ;

		puck->vecVelocity[2] = vX;

		}

	if( paddle2Dist < paddlePuckRadius ) {

		float y = puckPoint[1] - paddle2Point[1];

		float x = puckPoint[0] - paddle2Point[0];

		float theta = atan( y / x );

		float puckVel = sqrt( pow( puck->vecVelocity[0] , 2 ) + pow( puck->vecVelocity[1] , 2 ) );

		float vZ = puckVel * cos( theta );

		float vX = puckVel * sin( theta );

		puck->vecVelocity[0] = vZ;

		puck->vecVelocity[2] = vX;
		}

	}
