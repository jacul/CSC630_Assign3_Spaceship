//
//  main.cpp
//  CSC630_Assign3_Spaceship
//
//

#include <stdlib.h>			// standard definitions
#include <stdio.h>			// C I/O (for sprintf) 
#include <math.h>
#include <list>
#include <iostream>
using namespace std;

#ifdef _WIN32
#include <GL/glut.h>
#include <GL/GLU.h>
#include <GL/GL.h>
#else
#include <GLUT/glut.h>			// GLUT
#include <OpenGL/glu.h>			// GLU
#include <OpenGL/gl.h>			// OpenGL
#endif

void reshape(int w,int h);
void display();
void mouse(int button,int status,int x,int y);
void keyboard(unsigned char c, int x, int y);
void quit();
void pauseGame();
void timerfunc(int status);
void customInit();
void drawFloor();
void drawSpaceShips();
void drawSpaceShip(int id);
void drawClouds();
void drawCloud(int id);
void setViewPosition();
void shipMovement();
void debugOutput();

#define INTERVAL 50
#define RUN 1
#define PAUSE 0

/*******************The below part is from lecture*************/
#define FOVY 60
#define NO_SHIPS 2           /*number of ships */
#define NO_CLOUDS 8          /*number of clouds */
#define SHIP_TVEL_INC 2      /*incremen/decrement amounts for ship velocities */
#define SHIP_TVEL_DEC -2
#define SHIP_RVEL_INC 2
#define SHIP_RVEL_DEC -2
#define INNER 1              /*inner radius of torus */
#define OUTER 4              /*outer radius of torus */ 
#define SPHERERADIUS 10            /*radius of sphere */
#define NEAR 1               /*clipping plane near and far */
#define FAR 10000


/* window dimensions */
int wh=600;
int ww=800;

/* parameters for drawing sphere and torus */
GLint NO_STACKS=20;
GLint NO_SLICES=20;
GLint NO_SIDES=20;
GLint NO_RINGS=20;


GLfloat ship_color[4]={1.0,0.627,0.478, 1.0};  /*color of a ship */
GLfloat ground_color[4]={0.545, 0.537, 0.537, 1.0}; 

/* The ground should be located at z=0 plane, you can use (0,0,0) as the lower left corner and (200, 200, 0) as the upper right corner. */

/*several initializations */
/* cloud coordinates(x,y,z) for each cloud */ 
GLfloat cloud_coords[NO_CLOUDS][3]={{30, 30, 30},
    {30, 170, 15},
    {80, 110, 25},
    {70,  60, 12},
    {90, 150, 13},
    {120,  80, 17},
    {150,  40, 15},
    {160, 170, 22}};

/*cloud colors(rgb) for each cloud */
GLfloat cloud_colors[NO_CLOUDS][3]={{0.30, 0.30, 0.30},
    {1.00, 0.00, 0.00},
    {0.00, 1.00, 0.00},
    {0.00, 0.00, 1.00},
    {1.00, 1.00, 0.00},
    {1.00, 0.00, 1.00},
    {0.00, 1.00, 1.00},
    {0.00, 0.00, 0.00}};

/*ship coordinates for all ships, (x,y,z) for each ship*/
GLfloat ship_coords[NO_SHIPS][3]={{100.0,10.0,10.0},
    {105.0,0.0,15.0}};


/*velocites (translational and rotational) for each ship */
GLfloat ship_velocities[NO_SHIPS][2]={{0.0,2.0},
    {-2.0,-2.0}};


/*heading initializations for each ship */
GLfloat ship_headings[NO_SHIPS]={0.0, 180.0};

/******************************************/

//to see a horizon, the floor must have a relatively big size
GLfloat groundsize=10000;
GLfloat groundVertices[4][3] = {
    {-groundsize,-groundsize,0},
    {-groundsize,groundsize,0},
    {groundsize,groundsize,0},
    {groundsize,-groundsize,0}
};

GLdouble eyeX,eyeY,eyeZ;
GLint currentShipId = 0;
//Represents the initial status of the two ships.(Rotating angle)
GLint ship_spin[2] = {0,0};

GLint gamestatus = RUN;
bool FULLSCREEN = false;

void customInit(){
    eyeX = 0;
    eyeY = 0;
    eyeZ = 100;
    
    setViewPosition();
    
    glClearColor(1, 1, 1, 1);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluPerspective(FOVY, (GLdouble) ww / wh, NEAR, FAR);
}

//invoked when the window is resized
void reshape(int newWidth, int newHeight){
    ww=newWidth;
    wh=newHeight;
    glViewport(0, 0, newWidth, newHeight);
    glMatrixMode(GL_PROJECTION);     // Choose projection matrix.
    glLoadIdentity();
    
    gluPerspective(FOVY, (GLdouble) ww / wh, NEAR, FAR);
}

//display callback
void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    setViewPosition();
    
    //viewer's looking position
    GLint dirX=eyeX+groundsize*sin(ship_headings[currentShipId]*3.14/180);
    GLint dirY=eyeY+groundsize*cos(ship_headings[currentShipId]*3.14/180);
    GLint dirZ=eyeZ;
    //set camera
    gluLookAt(eyeX, eyeY, eyeZ, dirX, dirY, dirZ, eyeX, eyeY, eyeZ+groundsize);
    
    drawFloor();
    
    drawSpaceShips();
    
    drawClouds();
    
    glFlush();
    
    glutSwapBuffers();
}

//draws spaceships
void drawSpaceShips(){
    drawSpaceShip(0);
    drawSpaceShip(1);
}

//draw a spaceship with given id
void drawSpaceShip(int id){
    glPushMatrix();
    glColor3fv(ship_color);
    glTranslatef(ship_coords[id][0], ship_coords[id][1], ship_coords[id][2]);
    glRotatef(90+ship_spin[id], 0, 1, 0);
    glutWireTorus(INNER, OUTER, NO_SIDES, NO_RINGS);
    
    glRotatef(180+2*ship_spin[id], 0, -1, 0);
    
    glutWireTorus(INNER, OUTER, NO_SIDES, NO_RINGS);
    
    glPopMatrix();
}

//draw all the clouds
void drawClouds(){
    for (int id=0; id<NO_CLOUDS; ++id) {
        drawCloud(id);
    }
}

//draw a cloud with given id
void drawCloud(int id){
    glPushMatrix();
    glTranslatef(cloud_coords[id][0], cloud_coords[id][1], cloud_coords[id][2]);
    glColor3fv(cloud_colors[id]);
    glutWireSphere(SPHERERADIUS, NO_SLICES, NO_STACKS);
    glPopMatrix();
}

//draw the floor
void drawFloor(){
    glPushMatrix();
    glColor3fv(ground_color);
    glBegin(GL_QUADS);
    
    glVertex3fv(groundVertices[0]);
    
    glVertex3fv(groundVertices[1]);
    
    glVertex3fv(groundVertices[2]);
    
    glVertex3fv(groundVertices[3]);
    glEnd();
    glPopMatrix();
}

//set the camera's position. It is always in front of the spaceship.
void setViewPosition(){
    eyeX = ship_coords[currentShipId][0];
    eyeY = ship_coords[currentShipId][1];
    eyeZ = ship_coords[currentShipId][2];
}

//Every time unit, ships move. This includes their traslation and rotation.
void shipMovement(){
    //These are translation
    ship_coords[0][0]+= ship_velocities[0][0]*sin(ship_headings[0]*3.14/180);
    ship_coords[0][1]+= ship_velocities[0][0]*cos(ship_headings[0]*3.14/180);
    ship_coords[1][0]+= ship_velocities[1][0]*sin(ship_headings[1]*3.14/180);
    ship_coords[1][1]+= ship_velocities[1][0]*cos(ship_headings[1]*3.14/180);
    
    //These are rotation
    ship_spin[0]+=ship_velocities[0][1];
    ship_spin[0] %= 360;
    ship_spin[1]+=ship_velocities[1][1];
    ship_spin[1] %= 360;
}

void mouse(int button,int status,int x,int y){
    if(status==GLUT_UP){
        switch (button) {
            case GLUT_LEFT_BUTTON://beam from this ship to another ship
                currentShipId = 1-currentShipId;
                setViewPosition();
                break;
            case GLUT_MIDDLE_BUTTON:
                pauseGame();
                break;
            case GLUT_RIGHT_BUTTON:
                if(gamestatus==RUN){
                    pauseGame();
                }
                shipMovement();
                debugOutput();
                break;
            default:
                break;
        }
        glutPostRedisplay();
    }
}

void keyboard(unsigned char c, int x, int y){
    switch (c) {
        case 'q':
        case 'Q':
            quit();
            break;
        case 'p':
        case 'P':
            pauseGame();
            break;
        case 13://enter key
			if (FULLSCREEN) {//toggle between full screen and window mode
				glutReshapeWindow(800, 600);
				glutPositionWindow(0, 0);
				FULLSCREEN=false;
			}else {
				glutFullScreen();
				FULLSCREEN=true;
			}
			break;
        case 'a'://Increase the velocity of the current ship by a constant amount
            ship_velocities[currentShipId][0]+=SHIP_TVEL_INC;
            break;
        case 's'://Decrease the velocity of the current ship by the same constant amount. Negative velocities are allowed, meaning the ship moves backwards.
            ship_velocities[currentShipId][0]+=SHIP_TVEL_DEC;
            break;
        case 'l'://Increase the ship’s rightward rotation by constant amount.
            ship_velocities[currentShipId][1]+=SHIP_RVEL_INC;
            break;
        case 'k'://Decrease the ship’s rightward rotation by constant amount.
            ship_velocities[currentShipId][1]+=SHIP_RVEL_DEC;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

//quit the game
void quit(){
    exit(0);
}

//Pause & resume the game. Here is just switch between the status flags.
void pauseGame(){
    gamestatus=1-gamestatus;
}

//timer callback
void timerfunc(int status){
    glutTimerFunc(INTERVAL, timerfunc, 0);
    
    if(gamestatus==RUN){
        shipMovement();
    
        glutPostRedisplay();
    }
}

//Print all the information of the game.
void debugOutput(){
    for(int i=0;i<NO_SHIPS;i++){
        cout<<"Ship NO."<<i<<":"<<endl;
        cout<<"  Position: x "<<ship_coords[i][0]<<", y "<<ship_coords[i][1]<<", z "<<ship_coords[i][2]<<endl;
        cout<<"  Volocity: "<<ship_velocities[i][0]<<endl;
        cout<<"  Spin speed: "<<ship_velocities[i][1]<<endl;
        cout<<"  Rotation angle: "<<ship_spin[i]<<endl;
    }
}

int main (int argc, char * argv[])
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    
    //*  Here to initial window
    glutInitWindowSize(ww, wh);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Space ship");
    
    //Set callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(INTERVAL, timerfunc, 0);
    
    customInit();
    
    glutMainLoop();
    
    return 0;
}

