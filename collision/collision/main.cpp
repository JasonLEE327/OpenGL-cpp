#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <unistd.h>
#include <GLUT/GLUT.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

GLvoid DrawCuboid( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgex, GLfloat edgey, GLfloat edgez );

GLfloat rotationX = 10.0f;
GLfloat rotationY = 10.0f;
//GLfloat rotationSpeed = 4.0f;
//GLint collisionDetect = 0;

int main( void )
{
    GLFWwindow *window;
    if ( !glfwInit( ) )
    {
        return -1;
    }
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Collision", NULL, NULL );
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );
    
    if ( !window )
    {
        glfwTerminate( );
        return -1;
    }
    
    glfwMakeContextCurrent( window );
    
    glViewport( 0.0f, 0.0f, screenWidth, screenHeight );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    
    GLfloat halfScreenWidth = SCREEN_WIDTH / 2;
    GLfloat halfScreenHeight = SCREEN_HEIGHT / 2;
    
    glTranslatef( halfScreenWidth, halfScreenHeight, -500 );
    glRotatef( rotationX, 1, 0, 0 );
    glRotatef( rotationY, 0, 1, 0 );
    glTranslatef( -halfScreenWidth, -halfScreenHeight, 500 );
    
    glfwSetTime( 0 );
    GLfloat v = 0;
    GLint hit = 0;
    GLdouble hitS = 0;
 
    //#define CUBESIZE 30
    GLfloat cubesize = 30;
    GLfloat curX = SCREEN_WIDTH/2, curY = SCREEN_HEIGHT-15, curZ = -500;
    GLfloat PlaneX = SCREEN_WIDTH/2, PlaneY = 100, PlaneZ = -500, PlaneEdgeX = 500, PlaneEdgeY = 5, PlaneEdgeZ = 500;
    GLfloat boundary = PlaneY + cubesize/2 + PlaneEdgeY/2;
    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        
        double seconds = glfwGetTime();

        if (hit == 0){
            if (curY > boundary){
                curY = SCREEN_HEIGHT-15 - 0.5*9.8*pow((seconds-hitS),2);
            }
            else{
                hit = 1-hit;
                v = 9.8 * (seconds-hitS);
                hitS = seconds;
            }
        }
        else{
            if (curY < SCREEN_HEIGHT-15){
                curY = boundary + v*(seconds-hitS) - 0.5*9.8*pow((seconds-hitS),2);
            }
            else{
                hit = 1-hit;
                hitS = seconds;
            }
        }
        
        DrawCuboid(PlaneX, PlaneY, PlaneZ, PlaneEdgeX, PlaneEdgeY, PlaneEdgeZ);
        DrawCuboid( curX, curY, curZ, cubesize, cubesize, cubesize );
       
        
        glfwSwapBuffers( window );
        glfwPollEvents( );
    }
    glfwTerminate( );
    return 0;
}

GLvoid DrawCuboid( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgex, GLfloat edgey, GLfloat edgez )
{
    //GLfloat halfSideLength = edgeLength * 0.5f;
    GLfloat halfx = edgex/2, halfy = edgey/2, halfz = edgez/2;
    
    GLfloat vertices[] =
    {
        // front face
        centerPosX - halfx, centerPosY + halfy, centerPosZ + halfz, // top left
        centerPosX + halfx, centerPosY + halfy, centerPosZ + halfz, // top right
        centerPosX + halfx, centerPosY - halfy, centerPosZ + halfz, // bottom right
        centerPosX - halfx, centerPosY - halfy, centerPosZ + halfz, // bottom left
        
        // back face
        centerPosX - halfx, centerPosY + halfy, centerPosZ - halfz, // top left
        centerPosX + halfx, centerPosY + halfy, centerPosZ - halfz, // top right
        centerPosX + halfx, centerPosY - halfy, centerPosZ - halfz, // bottom right
        centerPosX - halfx, centerPosY - halfy, centerPosZ - halfz, // bottom left
        
        // left face
        centerPosX - halfx, centerPosY + halfy, centerPosZ + halfz, // top left
        centerPosX - halfx, centerPosY + halfy, centerPosZ - halfz, // top right
        centerPosX - halfx, centerPosY - halfy, centerPosZ - halfz, // bottom right
        centerPosX - halfx, centerPosY - halfy, centerPosZ + halfz, // bottom left
        
        // right face
        centerPosX + halfx, centerPosY + halfy, centerPosZ + halfz, // top left
        centerPosX + halfx, centerPosY + halfy, centerPosZ - halfz, // top right
        centerPosX + halfx, centerPosY - halfy, centerPosZ - halfz, // bottom right
        centerPosX + halfx, centerPosY - halfy, centerPosZ + halfz, // bottom left
        
        // top face
        centerPosX - halfx, centerPosY + halfy, centerPosZ + halfz, // top left
        centerPosX - halfx, centerPosY + halfy, centerPosZ - halfz, // top right
        centerPosX + halfx, centerPosY + halfy, centerPosZ - halfz, // bottom right
        centerPosX + halfx, centerPosY + halfy, centerPosZ + halfz, // bottom left
        
        // top face
        centerPosX - halfx, centerPosY - halfy, centerPosZ + halfz, // top left
        centerPosX - halfx, centerPosY - halfy, centerPosZ - halfz, // top right
        centerPosX + halfx, centerPosY - halfy, centerPosZ - halfz, // bottom right
        centerPosX + halfx, centerPosY - halfy, centerPosZ + halfz  // bottom left
    };
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(1.0, 0.0, 0.0);
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glDrawArrays( GL_QUADS, 0, 24 );
    glDisableClientState( GL_VERTEX_ARRAY );
}


