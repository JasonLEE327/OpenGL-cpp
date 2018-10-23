#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <math.h>

#include <unistd.h>

#include <GLUT/GLUT.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );
GLvoid DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength );
GLvoid DrawSquare(GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength );

GLvoid DrawPoint(GLfloat x, GLfloat y, GLfloat z);

GLfloat rotationX = 10.0f;
GLfloat rotationY = 10.0f;
GLfloat rotationSpeed = 4.0f;

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

    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        //glPushMatrix( );
        glTranslatef( halfScreenWidth, halfScreenHeight, -500 );
        glRotatef( rotationSpeed, 0, 1, 0 );
        glTranslatef( -halfScreenWidth, -halfScreenHeight, 500 );

        double seconds = glfwGetTime();
        std::cout << seconds << std::endl;
        
        glTranslatef( 0, -0.5*9.8*pow(seconds,2), 0);

        DrawCube( halfScreenWidth, halfScreenHeight+150, -500, 30 );
        //gluSphere(1,2,2);
        //glPopMatrix();
        DrawPoint(halfScreenWidth, halfScreenHeight, -500);

        glfwSwapBuffers( window );
        glfwPollEvents( );
    }

    glfwTerminate( );

    return 0;
}




GLvoid DrawSquare(GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength )
{
    GLfloat halfSideLength = edgeLength * 0.5f;

    GLfloat vertices[] =
    {
        centerPosX + halfSideLength, centerPosY + halfSideLength, 0.0,
        centerPosX - halfSideLength, centerPosY + halfSideLength, 0.0,
        centerPosX - halfSideLength, centerPosY - halfSideLength, 0.0,
        centerPosX + halfSideLength, centerPosY - halfSideLength, 0.0,
    };

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(1.0, 1.0, 1.0);
    glEnableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're using a vertex array for fixed-function attribute
    glVertexPointer( 3, GL_FLOAT, 0, vertices ); // point to the vertices to be used
    glDrawArrays( GL_QUADS, 0, 4 ); // draw the vertixes
    glDisableClientState( GL_VERTEX_ARRAY ); // tell OpenGL that you're finished using the vertex arrayattribute
}


GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods )
{
    //std::cout << key << std::endl;

    const GLfloat rotationSpeed = 10;

    // actions are GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
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
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glDrawArrays( GL_QUADS, 0, 24 );
    glDisableClientState( GL_VERTEX_ARRAY );
}

GLvoid DrawPoint(GLfloat x, GLfloat y, GLfloat z)
{
    //GLfloat pointVertex[] = { x,y,z};
    GLfloat *pointVertex = new GLfloat[3];
    pointVertex[0] = x;
    pointVertex[1] = y;
    pointVertex[1] = z;

    glColor3f(0.0, 1.0, 1.0);
    glEnable( GL_POINT_SMOOTH );
    glEnableClientState( GL_VERTEX_ARRAY );
    glPointSize( 10 );
    glVertexPointer( 3, GL_FLOAT, 0, pointVertex );
    glDrawArrays( GL_POINTS, 0, 1 );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisable( GL_POINT_SMOOTH );
}


//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <cstdlib>
//
//#define SCREEN_WIDTH 640
//#define SCREEN_HEIGHT 480
//
//int main( void )
//{
//    GLFWwindow *window;
//
//    // Initialize the library
//    if ( !glfwInit( ) )
//    {
//        return -1;
//    }
//
//    // Create a windowed mode window and its OpenGL context
//    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL );
//
//    int screenWidth, screenHeight;
//    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );
//
//    if ( !window )
//    {
//        glfwTerminate( );
//        return -1;
//    }
//
//    // Make the window's context current
//    glfwMakeContextCurrent( window );
//
//    glViewport( 0.0f, 0.0f, screenWidth, screenHeight ); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
//    glMatrixMode( GL_PROJECTION ); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
//    glLoadIdentity( ); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
//    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1 ); // essentially set coordinate system
//    glMatrixMode( GL_MODELVIEW ); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
//    glLoadIdentity( ); // same as above comment
//
//
//
//    // Loop until the user closes the window
//    while ( !glfwWindowShouldClose( window ) )
//    {
//        glClear( GL_COLOR_BUFFER_BIT );
//
//        // Render OpenGL here
//
//
//
//
//        // Swap front and back buffers
//        glfwSwapBuffers( window );
//
//        // Poll for and process events
//        glfwPollEvents( );
//    }
//
//    glfwTerminate( );
//
//    return 0;
//}
