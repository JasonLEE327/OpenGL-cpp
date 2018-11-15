#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <unistd.h>
#include <GLUT/GLUT.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
#define SCREEN_DEEPTH 2000

GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );
GLvoid DrawCuboid( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgex, GLfloat edgey, GLfloat edgez, GLfloat *color, GLfloat angleX);

GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;
GLfloat rotationSpeed = 10.0f;

GLfloat c1[3] = {1.0, 0.0, 0.0};
GLfloat c2[3] = {0.0, 1.0, 0.0};

const double pi = acos(-1);

int main( void )
{
    GLFWwindow *window;
    if ( !glfwInit( ) )
    {
        return -1;
    }
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Collision", NULL, NULL );
    
    glfwSetKeyCallback( window, keyCallback );
    glfwSetInputMode( window, GLFW_STICKY_KEYS, 1 );
    
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
    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, SCREEN_DEEPTH);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    
    GLfloat halfScreenWidth = SCREEN_WIDTH / 2;
    GLfloat halfScreenHeight = SCREEN_HEIGHT / 2;
    GLfloat halfScreenDeepth = SCREEN_DEEPTH / 2;
    
    glTranslatef( halfScreenWidth, halfScreenHeight, -halfScreenDeepth );
    glRotatef( rotationX, 1, 0, 0 );
    glRotatef( rotationY, 0, -1, 0 );
    glTranslatef( -halfScreenWidth, -halfScreenHeight, halfScreenDeepth );
    
    
    GLfloat PlaneX = halfScreenWidth, PlaneY = halfScreenHeight, PlaneZ = -halfScreenDeepth, PlaneEdgeX = 900, PlaneEdgeY = 5, PlaneEdgeZ = 900;
    
    GLfloat angle_x = pi/2;
    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        glPushMatrix( );
        glTranslatef( halfScreenWidth, halfScreenHeight, -halfScreenDeepth );
        glRotatef( rotationX, 1, 0, 0 );
        glRotatef( rotationY, 0, -1, 0 );
        glTranslatef( -halfScreenWidth, -halfScreenHeight, halfScreenDeepth );
        
        DrawCuboid(PlaneX, PlaneY, PlaneZ, PlaneEdgeX, PlaneEdgeY, PlaneEdgeZ, c1, pi/2);
        
        for(int i = 0; i<1; i++){
            DrawCuboid(PlaneX + 50*i, PlaneY+PlaneEdgeY/2+3+60/2, PlaneZ, 10, 60, 30, c2, angle_x);
        }
        
        if(angle_x>0){angle_x = angle_x - pi/2/900;}
        else{angle_x = pi/2;}
        
        
        glPopMatrix();
        glfwSwapBuffers( window );
        glfwPollEvents( );
    }
    glfwTerminate( );
    return 0;
}




GLvoid DrawCuboid( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgex, GLfloat edgey, GLfloat edgez, GLfloat *color, GLfloat angleX )
{
    //GLfloat halfSideLength = edgeLength * 0.5f;
    GLfloat halfx = edgex/2, halfy = edgey/2, halfz = edgez/2;
    
    
    GLfloat p1[2] = {centerPosX - halfx + edgex*(1-sin(angleX)) + edgey*cos(angleX),
                     centerPosY - halfy + edgex*cos(angleX) + edgey*sin(angleX)};
    GLfloat p2[2] = {centerPosX + halfx + edgey*cos(angleX),
                     centerPosY - halfy + edgey*sin(angleX)};
    GLfloat p3[2] = {centerPosX + halfx, centerPosY - halfy};
    GLfloat p4[2] = {centerPosX - halfx + edgex*(1-sin(angleX)),
                     centerPosY - halfy + edgex*cos(angleX)};
    
    GLfloat vertices[] =
    {
        
        // front face
        p1[0], p1[1], centerPosZ + halfz, // top left
        p2[0], p2[1], centerPosZ + halfz, // top right
        p3[0], p3[1], centerPosZ + halfz, // bottom right
        p4[0], p4[1], centerPosZ + halfz, // bottom left

        // back face
        p1[0], p1[1], centerPosZ - halfz, // top left
        p2[0], p2[1], centerPosZ - halfz, // top right
        p3[0], p3[1], centerPosZ - halfz, // bottom right
        p4[0], p4[1], centerPosZ - halfz, // bottom left
        
        // left face
        p1[0], p1[1], centerPosZ + halfz, // top left
        p1[0], p1[1], centerPosZ - halfz, // top right
        p4[0], p4[1], centerPosZ - halfz, // bottom right
        p4[0], p4[1], centerPosZ + halfz, // bottom left

        // right face
        p2[0], p2[1], centerPosZ + halfz, // top left
        p2[0], p2[1], centerPosZ - halfz, // top right
        p3[0], p3[1], centerPosZ - halfz, // bottom right
        p3[0], p3[1], centerPosZ + halfz, // bottom left

        //top face
        p1[0], p1[1], centerPosZ + halfz, // top left
        p1[0], p1[1], centerPosZ - halfz, // top right
        p2[0], p2[1], centerPosZ - halfz, // bottom right
        p2[0], p2[1], centerPosZ + halfz, // bottom left

        //top face
        p4[0], p4[1], centerPosZ + halfz, // top left
        p4[0], p4[1], centerPosZ - halfz, // top right
        p3[0], p3[1], centerPosZ - halfz, // bottom right
        p3[0], p3[1], centerPosZ + halfz  // bottom left


//        // front face
//        centerPosX - halfx, centerPosY + halfy, centerPosZ + halfz, // top left
//        centerPosX + halfx, centerPosY + halfy, centerPosZ + halfz, // top right
//        centerPosX + halfx, centerPosY - halfy, centerPosZ + halfz, // bottom right
//        centerPosX - halfx, centerPosY - halfy, centerPosZ + halfz, // bottom left
//
//        // back face
//        centerPosX - halfx, centerPosY + halfy, centerPosZ - halfz, // top left
//        centerPosX + halfx, centerPosY + halfy, centerPosZ - halfz, // top right
//        centerPosX + halfx, centerPosY - halfy, centerPosZ - halfz, // bottom right
//        centerPosX - halfx, centerPosY - halfy, centerPosZ - halfz, // bottom left

//        // left face
//        centerPosX - halfx, centerPosY + halfy, centerPosZ + halfz, // top left
//        centerPosX - halfx, centerPosY + halfy, centerPosZ - halfz, // top right
//        centerPosX - halfx, centerPosY - halfy, centerPosZ - halfz, // bottom right
//        centerPosX - halfx, centerPosY - halfy, centerPosZ + halfz, // bottom left
//
//        // right face
//        centerPosX + halfx, centerPosY + halfy, centerPosZ + halfz, // top left
//        centerPosX + halfx, centerPosY + halfy, centerPosZ - halfz, // top right
//        centerPosX + halfx, centerPosY - halfy, centerPosZ - halfz, // bottom right
//        centerPosX + halfx, centerPosY - halfy, centerPosZ + halfz, // bottom left
//
//        //top face
//        centerPosX - halfx, centerPosY + halfy, centerPosZ + halfz, // top left
//        centerPosX - halfx, centerPosY + halfy, centerPosZ - halfz, // top right
//        centerPosX + halfx, centerPosY + halfy, centerPosZ - halfz, // bottom right
//        centerPosX + halfx, centerPosY + halfy, centerPosZ + halfz, // bottom left
//
//        //top face
//        centerPosX - halfx, centerPosY - halfy, centerPosZ + halfz, // top left
//        centerPosX - halfx, centerPosY - halfy, centerPosZ - halfz, // top right
//        centerPosX + halfx, centerPosY - halfy, centerPosZ - halfz, // bottom right
//        centerPosX + halfx, centerPosY - halfy, centerPosZ + halfz  // bottom left

    };
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //glColor3f(1.0, 0.0, 0.0);
    glColor3f(color[0],color[1],color[2]);
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glDrawArrays( GL_QUADS, 0, 24 );
    glDisableClientState( GL_VERTEX_ARRAY );
}

GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods )
{
    const GLfloat rotationSpeed = 10;

    if ( action == GLFW_PRESS || action == GLFW_REPEAT )
    {
        switch ( key )
        {
            case GLFW_KEY_UP:
                rotationX -= rotationSpeed;
                break;
            case GLFW_KEY_DOWN:
                rotationX += rotationSpeed;
                break;
            case GLFW_KEY_RIGHT:
                rotationY += rotationSpeed;
                break;
            case GLFW_KEY_LEFT:
                rotationY -= rotationSpeed;
                break;
        }


    }
}

