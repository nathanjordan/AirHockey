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

void initMenu();

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

bool checkCollisions();

void physicsTimer( int x );

void score( int player );

void checkForScore( GLfloat maxX , GLfloat minX ,GLfloat goalMinZ , GLfloat goalMaxZ );

void gameOver();

void nextPoint( int x );

void mainMenuCallback( int x );

void newGame();

void flyingStuff();

void resetGameboard();




shaderloc program1;

Object* puck;
Object* paddle1;
Object* paddle2;
Object* table;



TVec3<GLfloat> eye;
TVec3<GLfloat> at;

float currentAngle = 0;
TVec3<GLfloat> p2A1 = 0;
TVec3<GLfloat> p2A2 = 0;

int prevX = -1 , prevY = -1;

bool movingLeft = false , movingForward = false, movingBackwards = false, movingRight = false;

bool paused = false, playing = false;

std::vector<Object*> objectList;

int player1Score = 0;
int player2Score = 0;

int MENU_NEW_GAME = 0;
int MENU_STOP_GAME = 1;
int MENU_PAUSE_GAME = 2;
int MENU_QUIT_GAME = 3;


int main() {

	srand( time( NULL ) );

	initWindow();

	initShaders();

	initView();

	initObjects();

	initMenu();

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

	//lighting variables
	program1.light_position = point4( -5.0, 5.0, -5.0, 0.0 );
	program1.light_ambient = color4( 0.2, 0.2, 0.2, 1.0 );
	//program1.light_diffuse = color4( 1.0, 1.0, 1.0, 1.0 );
	program1.light_diffuse = color4(1.0, 1.0, 1.0, 1.0 );
	program1.light_specular = color4( 1.0, 1.0, 1.0, 1.0 );


	program1.vertexShader = glCreateShader(GL_VERTEX_SHADER);
	program1.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	program1.program = glCreateProgram();

	GLchar *vertexsource, *fragmentsource;

	vertexsource = parseGLSL((char*)"/home/njordan/Downloads/workspace/AirHockey/src/shaders/shapes.vert");
	fragmentsource = parseGLSL((char*)"/home/njordan/Downloads/workspace/AirHockey/src/shaders/shapes.frag");

	glShaderSource( program1.vertexShader , 1 , (const GLchar**)&vertexsource , NULL );
	glShaderSource( program1.fragmentShader , 1 , (const GLchar**)&fragmentsource , NULL );

	glCompileShader( program1.vertexShader );
	glCompileShader( program1.fragmentShader );

	int isCompiled_vertexShader;

	//get the shader status from the vertex shader
	glGetShaderiv( program1.vertexShader , GL_COMPILE_STATUS, &isCompiled_vertexShader );

	//if the vertex shader did not compile, get some info about why it didnt
	if( ! (bool) isCompiled_vertexShader ) {

		char *vertexInfoLog;
		int maxLength,logLen;

		//Get the shader logs length
		glGetShaderiv( program1.vertexShader , GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		vertexInfoLog = (char *)malloc(maxLength);

		//get the log info
		glGetShaderInfoLog( program1.vertexShader , maxLength , &logLen , vertexInfoLog );

		std::string log( vertexInfoLog , logLen );

		std::cout << "ERROR COMPILING VERTEX SHADER: \n\n" << log;

		//exit indicating error
		exit(1);

		}

	int isCompiled_fragmentShader;

	//get the shader status from the fragment shader
	glGetShaderiv( program1.fragmentShader , GL_COMPILE_STATUS, &isCompiled_fragmentShader );

	//if the fragment shader did not compile, get some info about why it didnt
	if( ! (bool) isCompiled_fragmentShader ) {

		char *fragmentInfoLog;
		int maxLength,logLen;

		//Get the shader logs length
		glGetShaderiv(program1.fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		fragmentInfoLog = (char *)malloc(maxLength);

		//get the log info
		glGetShaderInfoLog( program1.fragmentShader , maxLength , &logLen , fragmentInfoLog );

		std::string log( fragmentInfoLog , logLen );

		std::cout << "ERROR COMPILING FRAG SHADER: \n\n" << log;

		//exit indicating error
		exit(1);

		}

	glAttachShader( program1.program , program1.vertexShader );
	glAttachShader( program1.program , program1.fragmentShader );

	glLinkProgram( program1.program );

	int isLinked;

	glGetProgramiv( program1.program , GL_LINK_STATUS , (int *)&isLinked );

	if( ! (bool) isLinked) {

		char *shaderProgramInfoLog;
		int maxLength,logLen;

		/* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
		glGetProgramiv( program1.program, GL_INFO_LOG_LENGTH, &maxLength);

		/* The maxLength includes the NULL character */
		shaderProgramInfoLog = (char *)malloc(maxLength);

		/* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
		glGetProgramInfoLog( program1.program , maxLength, &logLen, shaderProgramInfoLog );

		std::string log( shaderProgramInfoLog , logLen );

		std::cout << "ERROR LINKING: \n\n" << log;

		exit(1);
		}

	glUseProgram( program1.program );

	program1.modelLocation = glGetUniformLocation( program1.program , "model" );

	program1.viewLocation = glGetUniformLocation( program1.program , "view" );

	program1.projectionLocation = glGetUniformLocation( program1.program , "projection" );

	program1.vertexLocation = glGetAttribLocation( program1.program , "vertex" );

	program1.normalLocation = glGetAttribLocation( program1.program , "normal" );

	program1.ambientLocation = glGetUniformLocation(program1.program, "AmbientProduct");

	program1.diffuseLocation = glGetUniformLocation(program1.program, "DiffuseProduct");

	program1.specularLocation = glGetUniformLocation(program1.program, "SpecularProduct");

	program1.lightLocation = glGetUniformLocation(program1.program, "LightPosition");

	program1.shininessLocation = glGetUniformLocation(program1.program, "Shininess");

	eye = TVec3<GLfloat>( 1.0 , 1.0 , 1.0 );

	glOrtho( -5.0 , 5.0 , -5.0 , 5.0 , -10.0 , 10.0 );

	gluLookAt( 2.0 , 2.0 , 2.0 , 0.0 , 0.0 , 0.0 , 0.0 , 1.0 , 0.0 );

	glEnable(GL_LIGHTING);

	glEnable( GL_DEPTH_TEST );
    
    //clear the background to white
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

	glutTimerFunc( 15 , physicsTimer , 0 );

	}

void mouseMoveHandler(int x, int y) {
	if( !playing ) {
		return;
		}

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

	//crappyMovingPhysicsP1 = true;

	}

void mouseButtonHandler( int button, int state, int x, int y) {

	}

void timerTick( int value ) {

	if( !playing ) {

		glutTimerFunc( 15 , timerTick , 0 );

		return;

		}

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

		//objectList[i]->draw(modelLocation , viewLocation, projectionLocation, vertexLocation, colorLocation , &viewMatrix , &projMatrix );
		objectList[i]->draw(program1);
		}

	//glutStrokeString( GLUT_STROKE_ROMAN , (unsigned char*) "text to render" );

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
	if( !playing ) {
			return;
			}
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
	if( !playing ) {

			return;

			}
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
void physicsTimer( int x ) {

	if( !playing ) {

		glutTimerFunc( 40 , physicsTimer , 0 );

		return;

		}

	if ( checkCollisions() ) {

		glutTimerFunc( 40 , physicsTimer , 0 );

		}
	else {

		glutTimerFunc( 15 , physicsTimer , 0 );

		}


	}

void keyboardTimer( int x ) {

	if( !playing ) {

		glutTimerFunc( 30 , keyboardTimer , 0 );

		return;
		}

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

	glutTimerFunc( 30 , keyboardTimer , 0 );

	}

void readVertices( char* filename , Object* object ) {

	objLoader* loader = new objLoader();

	loader->load( filename );

	object->setFaces( loader );

	delete loader;

	}

void initObjects() {

	material paddleColor, puckColor, tableColor;

	//paddle color stuff
	paddleColor.material_ambient = color4(0.4, 0.0, 0.0, 1.0);
	paddleColor.material_diffuse = color4(0.8, 0.0, 0.0, 1.0);
	paddleColor.material_specular = color4(1.0, 1.0, 1.0, 1.0);
	paddleColor.material_shininess = 30.0;

	//puck color stuff
	puckColor.material_ambient = color4(0.0, 0.0, 0.0, 1.0);
	puckColor.material_diffuse = color4(0.2, 0.2, 0.2, 1.0);
	puckColor.material_specular = color4(1.0, 1.0, 1.0, 1.0);
	puckColor.material_shininess = 30.0;

	//table color stuff
	tableColor.material_ambient = color4(0.3, 0.3, 0.3, 1.0);
	tableColor.material_diffuse = color4(0.8, 0.8, 0.8, 1.0);
	tableColor.material_specular = color4(1.0, 1.0, 1.0, 1.0);
	tableColor.material_shininess = 30.0;

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

	puck->setColor( puckColor );

	table->setColor( tableColor );

	paddle1->setColor( paddleColor );

	paddle2->setColor( paddleColor );

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

	resetGameboard();

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

	program1.viewMatrix = Mat4( a,b,c,d );

	a = TVec4<GLfloat>( 0.5 , 0.0 , 0.0 , 0.0 );
	b = TVec4<GLfloat>( 0.0 , 0.5 , 0.0 , 0.0 );
	c = TVec4<GLfloat>( 0.0 , 0.0 , -0.5 , 0.0 );
	d = TVec4<GLfloat>( 0.0 , 0.0 , 0.0 , 1.0 );

	program1.projMatrix = Mat4( a,b,c,d );

	}

void initMenu() {

	glutCreateMenu( mainMenuCallback );

	glutAddMenuEntry( "New Game" , MENU_NEW_GAME );

	glutAddMenuEntry( "Stop Game" , MENU_STOP_GAME );

	glutAddMenuEntry( "Pause/Resume Game" , MENU_PAUSE_GAME );

	glutAddMenuEntry( "Quit Game" , MENU_QUIT_GAME );

	glutAttachMenu( GLUT_RIGHT_BUTTON );

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

	float goalMinZ = -1.0 + radius, goalMaxZ = 1.0 - radius;

	checkForScore( maxX , minX , goalMinZ , goalMaxZ );

	//bouncing stuff
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

bool checkCollisions() {

	TVec2<GLfloat> puckPoint( puck->matTranslation[0][3] , puck->matTranslation[2][3] );

	TVec2<GLfloat> paddle1Point( paddle1->matTranslation[0][3] , paddle1->matTranslation[2][3] );

	TVec2<GLfloat> paddle2Point( paddle2->matTranslation[0][3] , paddle2->matTranslation[2][3] );

	GLfloat paddlePuckRadius = ( puck->width / 2 ) + (paddle1->width / 2);

	GLfloat paddle1Dist = sqrt( pow( puckPoint[0] - paddle1Point[0] , 2 ) + pow( puckPoint[1] - paddle1Point[1] , 2 ) );

	GLfloat paddle2Dist = sqrt( pow( puckPoint[0] - paddle2Point[0] , 2 ) + pow( puckPoint[1] - paddle2Point[1] , 2 ) );

	if( paddle1Dist < paddlePuckRadius ) {

		float y = puckPoint[1] - paddle1Point[1];

		float x = puckPoint[0] - paddle1Point[0];

		float theta = atan2(y, x);

		float puckVel = sqrt( pow( puck->vecVelocity[0] , 2 ) + pow( puck->vecVelocity[1] , 2 ) );

		float vZ = puckVel * cos( theta );

		float vX = puckVel * sin( theta );

		puck->vecVelocity[0] = vZ;

		puck->vecVelocity[2] = vX;

		float paddleVel = sqrt( pow( paddle1->vecVelocity[0] , 2 ) + pow( paddle1->vecVelocity[1] , 2 ) );

		float vZp = paddleVel * cos( theta );

		float vXp = paddleVel * sin( theta );

		puck->vecVelocity[0] = vZ + vZp;

		puck->vecVelocity[2] = vX + vXp;

		return true;
		}

	if( paddle2Dist < paddlePuckRadius ) {

		float y = puckPoint[1] - paddle2Point[1];

		float x = puckPoint[0] - paddle2Point[0];

		float theta = atan( y / x );

		float puckVel = sqrt( pow( puck->vecVelocity[0] , 2 ) + pow( puck->vecVelocity[1] , 2 ) );

		float vZ = puckVel * cos( theta );

		float vX = puckVel * sin( theta );

		float paddleVel = sqrt( pow( paddle2->vecVelocity[0] , 2 ) + pow( paddle2->vecVelocity[1] , 2 ) );

		float vZp = paddleVel * cos( theta );

		float vXp = paddleVel * sin( theta );

		puck->vecVelocity[0] = vZ + vZp;

		puck->vecVelocity[2] = vX + vXp;

		return true;

		}

	return false;

	}

void checkForScore( GLfloat maxX , GLfloat minX ,GLfloat goalMinZ , GLfloat goalMaxZ ) {

	if( puck->matTranslation[0][3] > maxX ) {

		if( puck->matTranslation[2][3] > goalMinZ  && puck->matTranslation[2][3] < goalMaxZ ) {

			score( 1 );

			}

		}

	if( puck->matTranslation[0][3] < minX ) {

		if( puck->matTranslation[2][3] > goalMinZ  && puck->matTranslation[2][3] < goalMaxZ ) {

			score( 2 );

			}

		}

	}

void score( int player ) {

	puck->matTranslation[0][3] = 0.0;
	puck->matTranslation[2][3] = 0.0;

	puck->vecVelocity[0] = 0.0;
	puck->vecVelocity[2] = 0.0;

	if( player == 1 )
		player1Score += 1;

	if( player == 2 )
		player2Score += 1;

	if( player1Score > 2 || player2Score > 2 )
		gameOver();
	else
		glutTimerFunc( 2000 , nextPoint , 0 );

	}

void gameOver() {

	//playing = false;

	flyingStuff();

	}

void nextPoint( int x ) {

	float vel = 0.06;

	float angle =  2* 3.14159 * ( (rand() % 20000) / 20000.0 );

	puck->vecVelocity[0] = vel * cos( angle );
	puck->vecVelocity[2] = vel * sin( angle );

	}

void mainMenuCallback( int x ) {

	if( x == MENU_NEW_GAME ) {

		newGame();

		}
	else if( x == MENU_PAUSE_GAME ) {
		if( playing )
			playing = false;
		else
			playing = true;

		}
	else if( x == MENU_STOP_GAME ) {

		resetGameboard();

		playing = false;

		}
	else if( x == MENU_QUIT_GAME ) {

		exit(0);

		}

	}

void newGame() {

	resetGameboard();

	player1Score = 0;
	player2Score = 0;

	playing = true;

	glutTimerFunc( 2000 , nextPoint , 0 );

	}

void flyingStuff() {

	paddle1->vecAcceleration[1] = 0.003;

	paddle2->vecAcceleration[1] = 0.003;

	puck->vecAcceleration[1] = 0.003;

	table->vecAcceleration[1] = -0.003;

	}

void resetGameboard() {

	puck->matTranslation[0][3] = 0.0;
	puck->matTranslation[2][3] = 0.0;

	paddle1->matTranslation[0][3] = 3.0;
	paddle1->matTranslation[2][3] = 0.0;

	paddle2->matTranslation[0][3] = -3.0;
	paddle2->matTranslation[2][3] = 0.0;

	puck->vecVelocity[0] = 0.0;
	puck->vecVelocity[2] = 0.0;

	for( unsigned int i = 0 ; i < objectList.size() ; i++ ) {

		objectList[i]->updatePosition();

		}

	glutPostRedisplay();
}
