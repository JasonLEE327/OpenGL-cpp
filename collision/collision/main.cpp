#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <unistd.h>
#include <GLUT/GLUT.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CUBESIZE 30

GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );
GLvoid DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength );
GLvoid DrawSquare(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength );
GLint CollisionDetect(GLfloat x, GLfloat y, GLfloat z);
GLfloat **cellVertices3D(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength);

GLfloat rotationX = 10.0f;
GLfloat rotationY = 10.0f;
GLfloat rotationSpeed = 4.0f;
GLint collisionDetect = 0;
GLint hit = 0;


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
    GLdouble hitS = 0;
 
    GLfloat curX = SCREEN_WIDTH/2,curY = SCREEN_HEIGHT-15,curZ = -500;
    
    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        
        double seconds = glfwGetTime();

        if (hit == 0){
            if (curY > 15+100){
                curY = SCREEN_HEIGHT-15 - 0.5*9.8*pow((seconds-hitS),2);
                //curX = curX + 0.5;
            }
            else{
                hit = 1-hit;
                v = 9.8 * (seconds-hitS);
                hitS = seconds;
            }
        }
        else{
            if (curY < SCREEN_HEIGHT-15){
                curY = 15+100 + v*(seconds-hitS) - 0.5*9.8*pow((seconds-hitS),2);
            }
            else{
                hit = 1-hit;
                hitS = seconds;
            }
        }
        
        DrawCube( curX, curY, curZ, CUBESIZE );
        DrawSquare(320, 100, curZ, 200);
        
        glfwSwapBuffers( window );
        glfwPollEvents( );
    }
    glfwTerminate( );
    return 0;
}

//GLint CollisionDetect(GLfloat x, GLfloat y, GLfloat z)
//{
//    GLfloat **nodes = cellVertices3D(x, y, z, CUBESIZE);
//    for (i=0;i<8;i++){
//
//    }
//    return 1;
//}

GLfloat **cellVertices3D(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength)
{
    GLfloat **vertices = new GLfloat*[8];
    GLint i = 0;
    
    for (int i=0;i<8;i++)
    {
        vertices[i] = new GLfloat[3];
    }
    
    for (int y = -1; y<=1; y = y+2 )
    {
        vertices[i][0] = centerPosX+(-1)*(edgeLength/2);
        vertices[i][1] = centerPosY+y*(edgeLength/2);
        vertices[i][2] = centerPosZ+(-1)*(edgeLength/2);
        
        vertices[i+1][0] = centerPosX+1*(edgeLength/2);
        vertices[i+1][1] = centerPosY+y*(edgeLength/2);
        vertices[i+1][2] = centerPosZ+(-1)*(edgeLength/2);
        
        vertices[i+2][0] = centerPosX+1*(edgeLength/2);
        vertices[i+2][1] = centerPosY+y*(edgeLength/2);
        vertices[i+2][2] = centerPosZ+1*(edgeLength/2);
        
        vertices[i+3][0] = centerPosX+(-1)*(edgeLength/2);
        vertices[i+3][1] = centerPosY+y*(edgeLength/2);
        vertices[i+3][2] = centerPosZ+1*(edgeLength/2);
        
        i= i +4;
    }
    return vertices;
}

GLvoid DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength )
{
    GLfloat halfSideLength = edgeLength * 0.5f;
    
    GLfloat vertices[] =
    {
        // front face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left
        
        // back face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom left
        
        // left face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left
        
        // right face
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left
        
        // top face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // bottom left
        
        // top face
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength  // bottom left
    };
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(1.0, 1.0, 1.0);
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glDrawArrays( GL_QUADS, 0, 24 );
    glDisableClientState( GL_VERTEX_ARRAY );
}


GLvoid DrawSquare(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength )
{
    GLfloat vertices[] =
    {
        centerPosX-edgeLength, centerPosY , -500-edgeLength,
        centerPosX+edgeLength, centerPosY , -500-edgeLength,
        centerPosX+edgeLength, centerPosY , -500+edgeLength,
        centerPosX-edgeLength, centerPosY , -500+edgeLength,
    };
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(1.0, 0.0, 0.0);
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glDrawArrays( GL_QUADS, 0, 4 );
    glDisableClientState( GL_VERTEX_ARRAY );
}
