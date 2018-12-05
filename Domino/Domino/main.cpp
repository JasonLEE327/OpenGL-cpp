#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#include <cstdlib>
#include <math.h>
//#include <unistd.h>
#include <GLUT/GLUT.h>
#include <vector>

using namespace std;

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960
#define SCREEN_DEEPTH 2000

GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );
GLvoid DrawCuboid( GLfloat leftBottomX, GLfloat leftBottomY, GLfloat leftBottomZ, GLfloat edgex, GLfloat edgey, GLfloat edgez, GLfloat *color, GLfloat angleXZ, GLfloat angleYZ);

GLfloat rotationX = 40.0f;
GLfloat rotationY = 40.0f;

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
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Domino", NULL, NULL );
    
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
    
    //3 is the thickness of the line
    GLfloat PlaneEdgeX = 900, PlaneEdgeY = 5, PlaneEdgeZ = 900;
    GLfloat PlaneX = halfScreenWidth - PlaneEdgeX/2 , PlaneY = halfScreenHeight, PlaneZ = -halfScreenDeepth - PlaneEdgeZ/2;
    
    GLfloat domino_x = 10, domino_y = 60, domino_z = 30;
    GLfloat domino_px = PlaneX + 100;
    GLfloat domino_py = PlaneY + PlaneEdgeY + 3;
    GLfloat domino_pz = PlaneZ + PlaneEdgeZ - domino_z - 3;
    GLfloat domino_dis = 30;
    
    GLfloat sphere_r = 10, sphere_slices = 100, sphere_stacks = 100;
    GLfloat sphere_x = PlaneX + sphere_r;
    GLfloat sphere_y = PlaneY + domino_y/2 + PlaneEdgeY + 3;
    GLfloat sphere_z = PlaneZ + PlaneEdgeZ - domino_z/2 - 3;
    
    GLfloat angle_xz = 0.0;
    GLfloat angle_yz = 0.0;
    
//    GLfloat **dominoPos = new GLfloat*[50];
//    for(int i = 0; i<50; i++) dominoPos[i] = new GLfloat[3];
    
    vector<vector<GLfloat> > dominoPos(50, vector<GLfloat>(3));
    vector<GLfloat> angleYZ(50);
    
    while ( !glfwWindowShouldClose( window ) )
    {
        
        angle_yz = 0.0;
        domino_px = PlaneX + 100;
        domino_pz = PlaneZ + PlaneEdgeZ - domino_z;
        
        glClear( GL_COLOR_BUFFER_BIT );
        glPushMatrix( );
        glTranslatef( halfScreenWidth, halfScreenHeight, -halfScreenDeepth );
        glRotatef( rotationX, 1, 0, 0 );
        glRotatef( rotationY, 0, -1, 0 );
        glTranslatef( -halfScreenWidth, -halfScreenHeight, halfScreenDeepth );
        
        //draw plane
        DrawCuboid(PlaneX, PlaneY, PlaneZ, PlaneEdgeX, PlaneEdgeY, PlaneEdgeZ, c1, 0, 0);
        
        //draw sphere
        glTranslatef(sphere_x, sphere_y, sphere_z);
        glColor3f(0,1,0);
        glutSolidSphere(sphere_r, sphere_slices, sphere_stacks);
        glTranslatef(-sphere_x, -sphere_y, -sphere_z);
        
        
        dominoPos[0][0] = domino_px;
        dominoPos[0][1] = domino_py;
        dominoPos[0][2] = domino_pz;
        angleYZ[0] = angle_yz;
        
        for(int i = 1; i<50; i++){
            if(i<20){
                dominoPos[i][0] = dominoPos[i-1][0] + domino_dis;
                dominoPos[i][1] = dominoPos[i-1][1];
                dominoPos[i][2] = dominoPos[i-1][2];
                angleYZ[i] = angleYZ[i-1];
            }
            else if (i <= 25){
                dominoPos[i][0] = dominoPos[i-1][0] + domino_dis * cos(angleYZ[i-1]);
                dominoPos[i][1] = dominoPos[i-1][1];
                dominoPos[i][2] = dominoPos[i-1][2] - domino_dis * sin(angleYZ[i-1]);
                angleYZ[i] = angleYZ[i-1] + pi/6;
            }
            else if (i == 26){
                angleYZ[i] = 0;
                dominoPos[i][0] = dominoPos[i-1][0] - domino_x - domino_dis;
                dominoPos[i][1] = dominoPos[i-1][1];
                dominoPos[i][2] = dominoPos[i-1][2] - domino_z;
            }
            else if (i < 35){
                dominoPos[i][0] = dominoPos[i-1][0] - domino_dis;
                dominoPos[i][1] = dominoPos[i-1][1];
                dominoPos[i][2] = dominoPos[i-1][2];
                angleYZ[i] = angleYZ[i-1];
            }
            else if (i < 41){
                angleYZ[i] = angleYZ[i-1] - pi/6;
                dominoPos[i][0] = dominoPos[i-1][0] - ((domino_dis - domino_x) * cos(-angleYZ[i] - pi/6) + domino_x * cos(-angleYZ[i]));
                dominoPos[i][1] = dominoPos[i-1][1];
                dominoPos[i][2] = dominoPos[i-1][2] - ((domino_dis - domino_x) * sin(-angleYZ[i] - pi/6) + domino_x * sin(-angleYZ[i]));
            }
            else if(i == 41){
                angleYZ[i] = 0;
                dominoPos[i][0] = dominoPos[i-1][0] - domino_x + domino_dis;
                dominoPos[i][1] = dominoPos[i-1][1];
                dominoPos[i][2] = dominoPos[i-1][2] - domino_z;
            }
            else{
                angleYZ[i] = 0;
                dominoPos[i][0] = dominoPos[i-1][0] + domino_dis;
                dominoPos[i][1] = dominoPos[i-1][1];
                dominoPos[i][2] = dominoPos[i-1][2];
            }
            
        }
        
        for (int i = 0; i< 50; i++){
            DrawCuboid(dominoPos[i][0], dominoPos[i][1], dominoPos[i][2], domino_x, domino_y, domino_z, c1, angleYZ[i], angle_xz);
        }
        

        //draw dominos
//        for(int i = 0; i<50; i++){
//            // left upper
//            if(i<20){  //20
//
//                DrawCuboid(domino_px, domino_py, domino_pz, domino_x, domino_y, domino_z, c1, angle_yz, angle_xz);
//                domino_px += domino_dis;
//            }
//            else if(i<25){  //5
//                angle_yz += pi/6;
//
//                DrawCuboid(domino_px, domino_py, domino_pz, domino_x, domino_y, domino_z, c2, angle_yz, angle_xz);
//                domino_px += domino_dis * cos(angle_yz);
//                domino_pz -= domino_dis * sin(angle_yz);
//            }
//
//            //left upper
//            else if(i == 25){  //1
//                angle_yz = 0;
//                domino_px -= domino_x;
//                domino_pz -= domino_z;
//                DrawCuboid(domino_px, domino_py, domino_pz, domino_x, domino_y, domino_z, c1, angle_yz, angle_xz);
//            }
//            else if(i < 35){    //10
//                domino_px -= domino_dis;
//                DrawCuboid(domino_px, domino_py, domino_pz, domino_x, domino_y, domino_z, c2, angle_yz, angle_xz);
//            }
//
//            //left upper
//            else if(i < 41){  //5
//                angle_yz -= pi/6;
//                domino_px -= (domino_dis - domino_x) * cos(-angle_yz - pi/6) + domino_x * cos(-angle_yz);
//                domino_pz -= (domino_dis - domino_x) * sin(- angle_yz - pi/6) + domino_x * sin(-angle_yz);
//                DrawCuboid(domino_px, domino_py, domino_pz, domino_x, domino_y, domino_z, c1, angle_yz, angle_xz);
//            }
//
//            //left upper
//            else if(i == 41){
//                angle_yz = 0;
//                domino_px -= domino_x;
//                domino_pz -= domino_z;
//                DrawCuboid(domino_px, domino_py, domino_pz, domino_x, domino_y, domino_z, c2, angle_yz, angle_xz);
//
//            }
//
//            //repeat
//            else{
//                domino_px += domino_dis;
//                DrawCuboid(domino_px, domino_py, domino_pz, domino_x, domino_y, domino_z, c1, angle_yz, angle_xz);
//                //domino_px += domino_dis;
//            }
//        }
        
//        move
//        if(sphere_x < domino_px - domino_x/2 - sphere_r){
//            sphere_x = sphere_x + 0.2;
//        }
//        else{
//            if(angle_xz<pi/2){angle_xz = angle_xz + pi/2/900;}
//            //else{angle_xz = 0;}
//        }
        
        glPopMatrix();
        glfwSwapBuffers( window );
        glfwPollEvents( );
    }
    glfwTerminate( );
    return 0;
}




GLvoid DrawCuboid( GLfloat leftBottomX, GLfloat leftBottomY, GLfloat leftBottomZ, GLfloat edgex, GLfloat edgey, GLfloat edgez, GLfloat *color, GLfloat angleYZ, GLfloat angleXZ)
{
    //GLfloat halfx = edgex/2, halfy = edgey/2, halfz = edgez/2;

//    GLfloat p1[3] = {leftBottomX - halfx + edgex*(1-cos(angleXZ)) + edgey*sin(angleXZ) + edgez*sin(angleYZ),
//                     leftBottomY - halfy + edgex*sin(angleXZ) + edgey*cos(angleXZ),
//                     leftBottomZ - halfz + edgez*cos(angleYZ)};
//
//    GLfloat p2[3] = {leftBottomX + halfx + edgey*sin(angleXZ) - edgex + edgez*sin(angleYZ) + edgex*cos(angleYZ),
//                     leftBottomY - halfy + edgey*cos(angleXZ),
//                     leftBottomZ - halfz + edgez*cos(angleYZ) - edgex*sin(angleYZ)};
//
//    GLfloat p3[3] = {leftBottomX + halfx - edgex + edgez*sin(angleYZ) + edgex*cos(angleYZ),
//                     leftBottomY - halfy,
//                     leftBottomZ - halfz + edgez*cos(angleYZ) - edgex*sin(angleYZ)};
//
//    GLfloat p4[3] = {leftBottomX - halfx + edgex*(1-cos(angleXZ)) + edgez*sin(angleYZ),
//                     leftBottomY - halfy + edgex*sin(angleXZ),
//                     leftBottomZ - halfz + edgez*cos(angleYZ)};
//
//    GLfloat p5[3] = {leftBottomX - halfx + edgex*(1-cos(angleXZ)) + edgey*sin(angleXZ) ,
//                     leftBottomY - halfy + edgex*sin(angleXZ) + edgey*cos(angleXZ),
//                     leftBottomZ - halfz};
//
//    GLfloat p6[3] = {leftBottomX + halfx + edgey*sin(angleXZ) - edgex + edgex*cos(angleYZ),
//                     leftBottomY - halfy + edgey*cos(angleXZ),
//                     leftBottomZ - halfz - edgex*sin(angleYZ)};
//
//    GLfloat p7[3] = {leftBottomX + halfx - edgex + edgex*cos(angleYZ),
//                     leftBottomY - halfy,
//                     leftBottomZ - halfz - edgex*sin(angleYZ)};
//
//    GLfloat p8[3] = {leftBottomX - halfx + edgex*(1-cos(angleXZ)),
//                     leftBottomY - halfy + edgex*sin(angleXZ),
//                     leftBottomZ - halfz};
    
    GLfloat p1[3] = {leftBottomX + edgez*sin(angleYZ),
        leftBottomY + edgey,
        leftBottomZ + edgez*cos(angleYZ)};
    
    GLfloat p2[3] = {leftBottomX + edgez*sin(angleYZ) + edgex*cos(angleYZ),
        leftBottomY + edgey,
        leftBottomZ + edgez*cos(angleYZ) - edgex*sin(angleYZ)};
    
    GLfloat p3[3] = {leftBottomX + edgez*sin(angleYZ) + edgex*cos(angleYZ),
        leftBottomY,
        leftBottomZ + edgez*cos(angleYZ) - edgex*sin(angleYZ)};
    
    GLfloat p4[3] = {leftBottomX + edgez*sin(angleYZ),
        leftBottomY,
        leftBottomZ + edgez*cos(angleYZ)};
    
    GLfloat p5[3] = {leftBottomX,
        leftBottomY + edgey,
        leftBottomZ};
    
    GLfloat p6[3] = {leftBottomX + edgex*cos(angleYZ),
        leftBottomY + edgey,
        leftBottomZ - edgex*sin(angleYZ)};
    
    GLfloat p7[3] = {leftBottomX + edgex*cos(angleYZ),
        leftBottomY,
        leftBottomZ - edgex*sin(angleYZ)};
    
    GLfloat p8[3] = {leftBottomX,
        leftBottomY,
        leftBottomZ};
    
    GLfloat vertices[] =
    {
        // front face
        p1[0], p1[1], p1[2], // top left
        p2[0], p2[1], p2[2], // top right
        p3[0], p3[1], p3[2], // bottom right
        p4[0], p4[1], p4[2], // bottom left

        // back face
        p5[0], p5[1], p5[2], // top left
        p6[0], p6[1], p6[2], // top right
        p7[0], p7[1], p7[2], // bottom right
        p8[0], p8[1], p8[2], // bottom left
        
        // left face
        p1[0], p1[1], p1[2], // top left
        p5[0], p5[1], p5[2], // top right
        p8[0], p8[1], p8[2], // bottom right
        p4[0], p4[1], p4[2], // bottom left

        // right face
        p2[0], p2[1], p2[2], // top left
        p6[0], p6[1], p6[2], // top right
        p7[0], p7[1], p7[2], // bottom right
        p3[0], p3[1], p3[2], // bottom left

        //top face
        p1[0], p1[1], p1[2], // top left
        p5[0], p5[1], p5[2], // top right
        p6[0], p6[1], p6[2], // bottom right
        p2[0], p2[1], p2[2], // bottom left

        //top face
        p4[0], p4[1], p4[2], // top left
        p8[0], p8[1], p8[2], // top right
        p7[0], p7[1], p7[2], // bottom right
        p3[0], p3[1], p3[2]  // bottom left
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

