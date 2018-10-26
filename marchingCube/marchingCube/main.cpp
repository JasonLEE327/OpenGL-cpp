#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <vector>

#include <Eigen/Dense>


using namespace std;
using namespace Eigen;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_DEEPTH -500

#define CELL_SZIE 30
#define GRID_SCALE 3

#define SPHERE_R 80
#define ELLIPSE_X 100
#define ELLIPSE_Y 60
#define ELLIPSE_Z 70
#define CUBE_R 100

#define STEPSIZE 0.1

//for 2D
//GLfloat BilinearInter(GLfloat *dis, GLfloat *v);
GLvoid DrawGrid2D(GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength );
GLfloat *Interp2D(GLfloat *p1,GLfloat *p2, GLfloat d1, GLfloat d2);
GLfloat *EdgePos2D(GLint edge, GLfloat **vertices, GLint index);
GLfloat **cellVertices2D(GLfloat centerPosX, GLfloat centerPosY);
GLvoid DrawLines2D( GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength, GLint index);
GLvoid DrawSquare(GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength );

//for 3D
//GLfloat TrilinearInter(GLfloat *dis, GLfloat *v);
GLvoid DrawGrid3D( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLint edgeLength );
GLfloat *Interp3D(GLfloat *p1,GLfloat *p2, GLfloat d1, GLfloat d2);
GLfloat *EdgePos3D(GLint edge, GLfloat **vertices, GLint index);
GLfloat **cellVertices3D(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength);
GLvoid DrawLines3D( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength, GLint index);;
GLvoid DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength );

//for point
vector<GLfloat> movePoint(vector<GLfloat> point, GLfloat timestep, string dir);

//for computation
GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );
GLfloat sdf(GLfloat x, GLfloat y, GLfloat z);
GLfloat sdfDIS(GLfloat x, GLfloat y, GLfloat z, GLfloat edgeLength );
GLvoid GetDIS2D();
GLvoid GetDIS3D();
GLint PointInCell(GLfloat centerX, GLfloat centerY, GLfloat centerZ);

Vector4f BilinearInter(GLfloat *dis, GLfloat *v);
VectorXf TrilinearInter(GLfloat *dis, GLfloat *v);
Vector3f MoveByGD2D(const Vector4f& a, Vector3f p, GLint direction);
Vector3f MoveByGD3D(const VectorXf& a, Vector3f p, GLint direction);

GLfloat rotationX = 40.0f;
GLfloat rotationY = 40.0f;

GLint cubeNum = 1;
GLint nodeNum = 0;
GLint shapeId = 0; //0:sphere  1:
GLint Dimension = 2;   //2:2d   3:3d
GLboolean shapeChange = false;
GLboolean DimChange = false;
GLboolean p_click = false; //change point

GLfloat randx = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_WIDTH/2 - (2*GRID_SCALE +1)/2.0*CELL_SZIE);
GLfloat randy = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_HEIGHT/2 - (2*GRID_SCALE +1)/2.0*CELL_SZIE);
GLfloat randz = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_DEEPTH - (2*GRID_SCALE +1)/2.0*CELL_SZIE);

GLfloat sdfDis2D[8*8];
GLfloat sdfDis3D[8*8*8];

GLint RandomPointIndex = 0;
GLfloat *RandomPointCenter = new GLfloat[3];

int main( void )
{
    GLFWwindow *window;
    if ( !glfwInit( ) )
    {
        return -1;
    }
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Marching cube/suqare", NULL, NULL );
    glfwSetKeyCallback( window, keyCallback );
    glfwSetInputMode( window, GLFW_STICKY_KEYS, 1 );
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );
    if ( !window )
    {
        glfwTerminate( );
        return -1;
    }
    // Make the window's context current
    glfwMakeContextCurrent( window );
    glViewport( 0.0f, 0.0f, screenWidth, screenHeight );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1000 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    
    GLfloat halfScreenWidth = SCREEN_WIDTH / 2;
    GLfloat halfScreenHeight = SCREEN_HEIGHT / 2;
    
    Vector3f newp; newp << randx,randy,randz;
    
    GLboolean first = true;
    VectorXf a;
    
    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        
        if (first || p_click || shapeChange || DimChange){
            if (Dimension == 2){
                GetDIS2D();
                nodeNum = pow(2*GRID_SCALE+1+1,2);
            }else{
                GetDIS3D();
                nodeNum = pow(2*GRID_SCALE+1+1,3);
            }
        }
        
        switch (Dimension) {
            case 2:
                DrawGrid2D(halfScreenWidth, halfScreenHeight, CELL_SZIE);
                break;
            case 3:
                glPushMatrix( );
                glTranslatef( halfScreenWidth, halfScreenHeight, -500 );
                glRotatef( rotationX, 1, 0, 0 );
                glRotatef( rotationY, 0, 1, 0 );
                glTranslatef( -halfScreenWidth, -halfScreenHeight, 500 );
                DrawGrid3D(halfScreenWidth, halfScreenHeight, -500, CELL_SZIE);
                glPopMatrix();
                break;
        }
        
        if (cubeNum >= 2*GRID_SCALE+1 && (first || p_click || shapeChange || DimChange)){
            if (Dimension == 2){
                GLfloat curRanDI[] = {sdfDis2D[RandomPointIndex+1], sdfDis2D[RandomPointIndex+9], sdfDis2D[RandomPointIndex+8], sdfDis2D[RandomPointIndex]};
                //GLfloat RanDIS = BilinearInter(curRanDI, RandomPointCenter);
                a = BilinearInter(curRanDI, RandomPointCenter);
                GLfloat RanDIS = a[0] + a[1]*randx + a[2]*randy + a[3]*randx*randy;
                if (RanDIS > 0){ printf("The pos of the random point is (%f,%f), it is OUTSIDE the shape\n\n" , randx, randy);}
                else{ printf("The pos of the random point is (%f,%f), it is INSIDE the shape\n\n" , randx, randy); }
                //
            }
            else{
                GLfloat curRanDI[] = {sdfDis3D[RandomPointIndex], sdfDis3D[RandomPointIndex+64], sdfDis3D[RandomPointIndex+1+64], sdfDis3D[RandomPointIndex+1],
                    sdfDis3D[RandomPointIndex+8], sdfDis3D[RandomPointIndex+64+8], sdfDis3D[RandomPointIndex+1+64+8], sdfDis3D[RandomPointIndex+1+8]};
                //GLfloat RanDIS = TrilinearInter(curRanDI, RandomPointCenter);
                a = TrilinearInter(curRanDI, RandomPointCenter);
                GLfloat RanDIS = a[0] + a[1]*randx + a[2]*randy + a[3]*randz + a[4]*randx*randy + a[5]*randx*randz + a[6]*randy*randz + a[7]*randx*randy*randz;
                //printf("%f %f\n", RanDIS, sdf(randx, randy, randz));
                if (RanDIS > 0){ printf("The pos of the random point is (%f,%f,%f), it is OUTSIDE the shape\n\n" , randx, randy, randz);}
                else{ printf("The pos of the random point is (%f,%f,%f), it is INSIDE the shape\n\n" , randx, randy, randz); }
            }
            if (p_click){
                newp = {randx,randy,randz};
            }
            first = false;
            p_click = false;
            shapeChange = false;
            DimChange = false;
        }
        
        if (cubeNum > 2*GRID_SCALE+1){
            if (Dimension == 2){
                GLfloat curDis = a[0] + a[1]*newp[0] + a[2]*newp[1] + a[3]*newp[0]*newp[1];
                if ( abs(curDis) > 1){
                    if(curDis > 0){
                        newp = MoveByGD2D(a,newp,1);
                    }else{
                        newp = MoveByGD2D(a,newp,0);
                    }
                }
            }else{
                GLfloat curDis = a[0] + a[1]*newp[0] + a[2]*newp[1] + a[3]*newp[2] + a[4]*newp[0]*newp[1] + a[5]*newp[0]*newp[2] + a[6]*newp[1]*newp[2] + a[7]*newp[0]*newp[1]*newp[2];
                if ( abs(curDis) > 1){
                    if(curDis > 0){
                        newp = MoveByGD3D(a,newp,1);
                    }else{
                        newp = MoveByGD3D(a,newp,0);
                    }
                }
            }
            //printf("%f %f %f", newp[0],newp[1],newp[2]);
            GLfloat temparr[6] = {randx,randy,randz,  newp[0],newp[1],newp[2]};
            glColor3f(0.0,1.0,1.0);
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(12);
            glEnableClientState( GL_VERTEX_ARRAY );
            glVertexPointer( 3, GL_FLOAT, 0, temparr);
            glDrawArrays(GL_POINTS, 0, 2);
            glDrawArrays(GL_LINES, 0, 2);
        }
        
        //draw points
        
        glfwSwapBuffers( window );
        glfwPollEvents( );
    }
    glfwTerminate( );
    return 0;
}

Vector3f MoveByGD2D(const Vector4f& a, Vector3f p, GLint direction)
{
    // function: f(x,y) = a0 + a1*x + a2*y + a3*x*y
    //      df/dx = a1+a3*y    df/dy = a2+a3*x
    Vector3f Point; Point << 0,0,0;
    if (direction == 1){
        Point[0] = p[0] - STEPSIZE * (a[1]+a[3]*p[1]);
        Point[1] = p[1] - STEPSIZE * (a[2]+a[3]*p[0]);
    }else{
        Point[0] = p[0] + STEPSIZE * (a[1]+a[3]*p[1]);
        Point[1] = p[1] + STEPSIZE * (a[2]+a[3]*p[0]);
    }
    return Point;
}
Vector3f MoveByGD3D(const VectorXf& a, Vector3f p, GLint direction)
{
    // function: f(x,y) = a0 + a1*x + a2*y + a3*z + a4*x*y + a5*x*z + a6*y*z + a7*x*y*z
    //      df/dx = a1 + a4*y + a5*z + a7*y*z
    //      df/dy = a2 + a4*x + a6*z + a7*x*z
    //      df/dz = a3 + a5*x + a6*y + a7*x*y
    Vector3f Point; Point << 0,0,0;
    if (direction == 1){
        Point[0] = p[0] - STEPSIZE * (a[1] + a[4]*p[1] + a[5]*p[2] + a[7]*p[1]*p[2]);
        Point[1] = p[1] - STEPSIZE * (a[2] + a[4]*p[0] + a[6]*p[2] + a[7]*p[0]*p[2]);
        Point[2] = p[2] - STEPSIZE * (a[3] + a[5]*p[0] + a[6]*p[1] + a[7]*p[0]*p[1]);
    }else{
        Point[0] = p[0] + STEPSIZE * (a[1] + a[4]*p[1] + a[5]*p[2] + a[7]*p[1]*p[2]);
        Point[1] = p[1] + STEPSIZE * (a[2] + a[4]*p[0] + a[6]*p[2] + a[7]*p[0]*p[2]);
        Point[2] = p[2] + STEPSIZE * (a[3] + a[5]*p[0] + a[6]*p[1] + a[7]*p[0]*p[1]);
    }
    return Point;
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
            case GLFW_KEY_SPACE:
                cubeNum++;
                break;
            case GLFW_KEY_S:
                shapeChange = true;
                cubeNum = 1;
                shapeId++;
                if (shapeId == 3){
                    shapeId = 0;
                }
                break;
            case GLFW_KEY_D:
                DimChange = true;
                Dimension = 5-Dimension;
                cubeNum = 1;
                break;
            case GLFW_KEY_P:
                p_click = true;
                randx = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_WIDTH/2 - (2*GRID_SCALE +1)/2.0*CELL_SZIE);
                randy = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_HEIGHT/2 - (2*GRID_SCALE +1)/2.0*CELL_SZIE);
                randz = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_DEEPTH - (2*GRID_SCALE +1)/2.0*CELL_SZIE);
        }
    }
}

//********************************
//*******              ***********
//*******    2D draw   ***********
//*******              ***********
//********************************
//GLfloat BilinearInter(GLfloat *dis, GLfloat *v)
//{
//    GLfloat pdis = 0.0;
//    //first in x_direction
//    GLfloat fx1 = (v[0]+CELL_SZIE/2 - randx)/CELL_SZIE * dis[3] + (randx-v[0]+CELL_SZIE/2)/CELL_SZIE * dis[2];
//    GLfloat fx2 = (v[0]+CELL_SZIE/2 - randx)/CELL_SZIE * dis[0] + (randx-v[0]+CELL_SZIE/2)/CELL_SZIE * dis[1];
//    //next in y_direction
//    pdis = (v[1]+CELL_SZIE/2 - randy)/CELL_SZIE * fx1 + (randy-v[1]+CELL_SZIE/2)/CELL_SZIE * fx2;
//    return pdis;
//}

Vector4f BilinearInter(GLfloat *dis, GLfloat *v)
{
    Matrix4f X;
    Vector4f q; q << dis[3], dis[0], dis[2], dis[1];
    
    GLfloat x1 = v[0] - CELL_SZIE/2;
    GLfloat x2 = v[0] + CELL_SZIE/2;
    GLfloat y1 = v[1] - CELL_SZIE/2;
    GLfloat y2 = v[1] + CELL_SZIE/2;
    X << 1, x1, y1, x1*y1,
         1, x1, y2, x1*y2,
         1, x2, y1, x2*y1,
         1, x2, y2, x2*y2;
    
    Vector4f a = X.colPivHouseholderQr().solve(q);
    //printf("%f,%f,%f,%f\n",a[0],a[1],a[2],a[3]);
    return a;
}

GLvoid DrawGrid2D(GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength )
{
    GLint count = 0;
    GLint cubeIndex = 0;

    for (int i = -GRID_SCALE; i <= GRID_SCALE; i++)
    {
        for (int j = -GRID_SCALE; j <= GRID_SCALE; j++)
        {
            cubeIndex = (i+3)*8+(j+3);
            DrawSquare( centerPosX+edgeLength*i, centerPosY+edgeLength*j, edgeLength );
            DrawLines2D( centerPosX+edgeLength*i, centerPosY+edgeLength*j, edgeLength , cubeIndex);
            
            if (PointInCell(centerPosX+edgeLength*i, centerPosY+edgeLength*j, 0)){
                RandomPointIndex = cubeIndex;
                RandomPointCenter[0] = centerPosX+edgeLength*i;
                RandomPointCenter[1] = centerPosY+edgeLength*j;
            }
        }
        count++;
        if(count == cubeNum){return ; }
    }
}

GLfloat *Interp2D(GLfloat *p1,GLfloat *p2, GLfloat d1, GLfloat d2)
{
    GLfloat *p = new GLfloat[2];
    p[0] = p1[0] - abs(d1)/(abs(d1)+abs(d2)) * (p1[0]-p2[0]);
    p[1] = p1[1] - abs(d1)/(abs(d1)+abs(d2)) * (p1[1]-p2[1]);
    return p;
}

GLfloat *EdgePos2D(GLint edge, GLfloat **vertices, GLint index)
{
    GLfloat *p = new GLfloat[2];
    switch (edge) {
        case 0:
            p = Interp2D(vertices[0], vertices[1], sdfDis2D[index+1], sdfDis2D[index+9]);
            break;
        case 1:
            p = Interp2D(vertices[1], vertices[2], sdfDis2D[index+9], sdfDis2D[index+8]);
            break;
        case 2:
            p = Interp2D(vertices[2], vertices[3], sdfDis2D[index+8], sdfDis2D[index]);
            break;
        case 3:
            p = Interp2D(vertices[3], vertices[0], sdfDis2D[index], sdfDis2D[index+1]);
            break;
        default:
            break;
    }
    return p;
}

GLfloat **cellVertices2D(GLfloat centerPosX, GLfloat centerPosY)
{
    GLfloat **vertices = new GLfloat*[4];
    GLint i = 0;
    
    for (int i=0;i<4;i++)
    {
        vertices[i] = new GLfloat[2];
    }
    
    vertices[i][0] = centerPosX+(-1)*(CELL_SZIE/2);
    vertices[i][1] = centerPosY+(1)*(CELL_SZIE/2);
    
    vertices[i+1][0] = centerPosX+1*(CELL_SZIE/2);
    vertices[i+1][1] = centerPosY+1*(CELL_SZIE/2);
    
    vertices[i+2][0] = centerPosX+1*(CELL_SZIE/2);
    vertices[i+2][1] = centerPosY+(-1)*(CELL_SZIE/2);
    
    vertices[i+3][0] = centerPosX+(-1)*(CELL_SZIE/2);
    vertices[i+3][1] = centerPosY+(-1)*(CELL_SZIE/2);
    
    return vertices;
}

GLvoid DrawLines2D( GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength, GLint index)
{
    extern GLint triTable2D[16][4];
    GLfloat lineVertices[4*2];
    GLint edgeCount = 0;
    GLint vertices = 0;
    
    
    GLfloat curDIS[] = {sdfDis2D[index+1], sdfDis2D[index+9], sdfDis2D[index+8], sdfDis2D[index]};
    for (int i=0;i<4;i++){
        if (curDIS[i] < 0){
            vertices += pow(2,i);
        }
    }
    
    GLfloat **verticelist = cellVertices2D(centerPosX,centerPosY);
    
    for (int i =0,j =0; i< 4; i++)
    {
        if (triTable2D[vertices][i] != -1)
        {
            GLfloat *newp = new GLfloat[2];
            newp = EdgePos2D(triTable2D[vertices][i], verticelist,index);
            lineVertices[j] = newp[0];
            lineVertices[j+1] = newp[1];
            edgeCount++;
        }
        j= j+2;
    }
    glEnableClientState( GL_VERTEX_ARRAY );
    glColor3f(1.0, 0.0, 0.0);
    glVertexPointer( 2, GL_FLOAT, 0, lineVertices );
    glDrawArrays( GL_LINE_LOOP, 0, edgeCount );
    glDisableClientState( GL_VERTEX_ARRAY );
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


//********************************
//*******              ***********
//*******    3D draw   ***********
//*******              ***********
//********************************
//GLfloat TrilinearInter(GLfloat *dis, GLfloat *v)
//{
//    GLfloat xd = (randx - v[0]+CELL_SZIE/2)/CELL_SZIE;
//    GLfloat yd = (randy - v[1]+CELL_SZIE/2)/CELL_SZIE;
//    GLfloat zd = (randz - v[2]+CELL_SZIE/2)/CELL_SZIE;
////    GLfloat yd = (randz - v[2]+CELL_SZIE/2)/CELL_SZIE;
////    GLfloat zd = (randy - v[1]+CELL_SZIE/2)/CELL_SZIE;
//
//
//    GLfloat c00 = dis[3]*(1-xd) + dis[2]*xd;
//    GLfloat c01 = dis[7]*(1-xd) + dis[6]*xd;
//    GLfloat c10 = dis[0]*(1-xd) + dis[1]*xd;
//    GLfloat c11 = dis[4]*(1-xd) + dis[5]*xd;
//
//    GLfloat c0 = c00 * (1-yd) + c10 * yd;
//    GLfloat c1 = c01 * (1-yd) + c11 * yd;
//
//    GLfloat c = c0 * (1-zd) + c1 * zd;
//    return c;
//}

VectorXf TrilinearInter(GLfloat *dis, GLfloat *v)
{
    VectorXf c(8,1); c << dis[3],dis[2],dis[0],dis[1],dis[7],dis[6],dis[4],dis[5];
    MatrixXf X(8,8);
    
    GLfloat x0 = v[0] - CELL_SZIE/2;
    GLfloat x1 = v[0] + CELL_SZIE/2;
    GLfloat y0 = v[1] - CELL_SZIE/2;
    GLfloat y1 = v[1] + CELL_SZIE/2;
    GLfloat z0 = v[2] - CELL_SZIE/2;
    GLfloat z1 = v[2] + CELL_SZIE/2;
    
    X << 1, x0, y0, z0, x0*y0, x0*z0, y0*z0, x0*y0*z0,
         1, x1, y0, z0, x1*y0, x1*z0, y0*z0, x1*y0*z0,
         1, x0, y1, z0, x0*y1, x0*z0, y1*z0, x0*y1*z0,
         1, x1, y1, z0, x1*y1, x1*z0, y1*z0, x1*y1*z0,
         1, x0, y0, z1, x0*y0, x0*z1, y0*z1, x0*y0*z1,
         1, x1, y0, z1, x1*y0, x1*z1, y0*z1, x1*y0*z1,
         1, x0, y1, z1, x0*y1, x0*z1, y1*z1, x0*y1*z1,
         1, x1, y1, z1, x1*y1, x1*z1, y1*z1, x1*y1*z1;
    
    VectorXf a(8,1); a = X.colPivHouseholderQr().solve(c);
    return a;
}

GLvoid DrawGrid3D(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLint edgeLength )
{
    GLint count = 0;
    GLint index = 0;
    for (int i = -GRID_SCALE; i <= GRID_SCALE; i++)
    {
        for (int j = -GRID_SCALE; j <= GRID_SCALE; j++)
        {
            for (int k = -GRID_SCALE; k <= GRID_SCALE; k++)
            {
                index = ((i+3)*8+(j+3))*8+(k+3);
                DrawCube( centerPosX+edgeLength*i, centerPosY+edgeLength*j, centerPosZ+edgeLength*k, edgeLength );
                DrawLines3D( centerPosX+edgeLength*i, centerPosY+edgeLength*j, centerPosZ+edgeLength*k, edgeLength, index );
                if (PointInCell(centerPosX+edgeLength*i, centerPosY+edgeLength*j, centerPosZ+edgeLength*k)){
                    RandomPointIndex = index;
                    RandomPointCenter[0] = centerPosX+edgeLength*i;
                    RandomPointCenter[1] = centerPosY+edgeLength*j;
                    RandomPointCenter[2] = centerPosZ+edgeLength*k;
                }
            }
            
        }
        
        count++;
        if(count == cubeNum){return ; }
    }
}

GLfloat *Interp3D(GLfloat *p1,GLfloat *p2, GLfloat d1, GLfloat d2)
{
    GLfloat *p = new GLfloat[3];
    p[0] = p1[0] - abs(d1)/(abs(d1)+abs(d2)) * (p1[0]-p2[0]);
    p[1] = p1[1] - abs(d1)/(abs(d1)+abs(d2)) * (p1[1]-p2[1]);
    p[2] = p1[2] - abs(d1)/(abs(d1)+abs(d2)) * (p1[2]-p2[2]);
    return p;
}

GLfloat *EdgePos3D(GLint edge, GLfloat **vertices, GLint index)
{
    GLfloat *p = new GLfloat[3];
    switch (edge) {
        case 0:
            p = Interp3D(vertices[0], vertices[1], sdfDis3D[index], sdfDis3D[index+64]);
            break;
        case 1:
            p = Interp3D(vertices[1], vertices[2], sdfDis3D[index+64], sdfDis3D[index+1+64]);
            break;
        case 2:
            p = Interp3D(vertices[2], vertices[3], sdfDis3D[index+1+64], sdfDis3D[index+1]);
            break;
        case 3:
            p = Interp3D(vertices[3], vertices[0], sdfDis3D[index+1], sdfDis3D[index]);
            break;
        case 4:
            p = Interp3D(vertices[4], vertices[5], sdfDis3D[index+8], sdfDis3D[index+8+64]);
            break;
        case 5:
            p = Interp3D(vertices[5], vertices[6], sdfDis3D[index+8+64], sdfDis3D[index+1+8+64]);
            break;
        case 6:
            p = Interp3D(vertices[6], vertices[7], sdfDis3D[index+1+8+64], sdfDis3D[index+1+8]);
            break;
        case 7:
            p = Interp3D(vertices[7], vertices[4], sdfDis3D[index+1+8], sdfDis3D[index+8]);
            break;
        case 8:
            p = Interp3D(vertices[0], vertices[4], sdfDis3D[index], sdfDis3D[index+8]);
            break;
        case 9:
            p = Interp3D(vertices[1], vertices[5], sdfDis3D[index+64], sdfDis3D[index+8+64]);
            break;
        case 10:
            p = Interp3D(vertices[2], vertices[6], sdfDis3D[index+1+64], sdfDis3D[index+1+64+8]);
            
            break;
        case 11:
            p = Interp3D(vertices[3], vertices[7], sdfDis2D[index+1], sdfDis2D[index+1+8]);
            break;
        default:
            break;
    }
    return p;
}

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
        vertices[i+2][2] = centerPosZ+(1)*(edgeLength/2);
        
        vertices[i+3][0] = centerPosX+(-1)*(edgeLength/2);
        vertices[i+3][1] = centerPosY+y*(edgeLength/2);
        vertices[i+3][2] = centerPosZ+(1)*(edgeLength/2);
        
        i= i +4;
    }
    return vertices;
}

GLvoid DrawLines3D( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength, GLint index)
{
    extern GLint triTable3D[256][16];
    GLfloat *lineVertices = new GLfloat[12*3];
    GLint edgeCount = 0;
    GLint vertices = 0;
    GLfloat **verticelist = cellVertices3D(centerPosX,centerPosY,centerPosZ,edgeLength);
    
    //x->+64  y->+8  z->+1
    GLfloat curDIS[] = {sdfDis3D[index], sdfDis3D[index+64], sdfDis3D[index+1+64], sdfDis3D[index+1],
        sdfDis3D[index+8], sdfDis3D[index+64+8], sdfDis3D[index+1+64+8], sdfDis3D[index+1+8]};
    for (int i=0;i<8;i++){
        if (curDIS[i] > 0){vertices += pow(2,i);}
    }
    
    for (int i =0,j =0; i< 16; i++)
    {
        if (triTable3D[vertices][i] != -1)
        {
            GLfloat *newp = new GLfloat[3];
            newp = EdgePos3D(triTable3D[vertices][i], verticelist,index);
            lineVertices[j] = newp[0];
            lineVertices[j+1] = newp[1];
            lineVertices[j+2] = newp[2];
            edgeCount++;
        }
        j= j+3;
    }
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glColor3f(1.0, 0.0, 0.0);
    glVertexPointer( 3, GL_FLOAT, 0, lineVertices );
    glDrawArrays( GL_LINE_LOOP, 0, edgeCount );
    glDisableClientState( GL_VERTEX_ARRAY );
}

GLvoid DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength)
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




//********************************
//*******              ***********
//*******  interpolate ***********
//*******  & otherfunc ***********
//********************************
GLfloat sdfDIS(GLfloat x, GLfloat y, GLfloat z, GLfloat edgeLength )
{
    GLfloat dis;
    if (Dimension==2){
        dis = sdf(x-edgeLength/2, y-edgeLength/2, 0);
    }else{
        dis = sdf(x-edgeLength/2, y-edgeLength/2, z-edgeLength/2);
    }
    
    return dis;
}

GLvoid GetDIS2D()
{
    for (int i = -GRID_SCALE; i <= GRID_SCALE+1; i++)
    {
        for (int j = -GRID_SCALE; j <= GRID_SCALE+1; j++)
        {
            sdfDis2D[(i+3)*8+(j+3)] = sdfDIS(SCREEN_WIDTH/2+CELL_SZIE*i, SCREEN_HEIGHT/2+CELL_SZIE*j, 0, CELL_SZIE);
        }
    }
}
GLvoid GetDIS3D()
{
    for (int i = -GRID_SCALE; i <= GRID_SCALE+1; i++){
        for (int j = -GRID_SCALE; j <= GRID_SCALE+1; j++){
            for (int k = -GRID_SCALE; k<= GRID_SCALE+1; k++){
                sdfDis3D[((i+3)*8+(j+3))*8+(k+3)] = sdfDIS(SCREEN_WIDTH/2+CELL_SZIE*i, SCREEN_HEIGHT/2+CELL_SZIE*j, SCREEN_DEEPTH+CELL_SZIE*k, CELL_SZIE);
            }
        }
    }
}

GLint PointInCell(GLfloat centerX, GLfloat centerY, GLfloat centerZ)
{
    GLint flag = 0;
    if (Dimension == 2){
        if (randx >= centerX-CELL_SZIE/2 && randx <= centerX+CELL_SZIE/2 && randy >= centerY-CELL_SZIE/2 && randy<=centerY+CELL_SZIE/2){
            flag = 1;
        }
    
    }else{
        if (randx >= centerX-CELL_SZIE/2 && randx <= centerX+CELL_SZIE/2 && randy >= centerY-CELL_SZIE/2 && randy<=centerY+CELL_SZIE/2 && randz >= centerZ-CELL_SZIE/2 && randz<=centerZ+CELL_SZIE/2){
            flag = 1;
        }
    }
    return flag;
}

GLfloat sdf(GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat formula = 0.0;
    if(Dimension == 3){
        if (shapeId == 0){
            //formula = sqrt(pow(x-SCREEN_WIDTH/2,2) + pow(y-SCREEN_HEIGHT/2,2) + pow(z-(-500),2)) - SPHERE_R;
            formula = sqrt(pow(x-SCREEN_WIDTH/2,2) + pow(y-SCREEN_HEIGHT/2,2) + pow(z-(-500),2)) - SPHERE_R;
        }
        else if (shapeId == 1){
            formula = abs(x-SCREEN_WIDTH/2) + abs(y-SCREEN_HEIGHT/2) + abs(z-(-500)) -CUBE_R;
        }
        else if (shapeId == 2){
            formula = sqrt(pow((x-SCREEN_WIDTH/2),2)/pow(ELLIPSE_X,2) + pow(y-SCREEN_HEIGHT/2,2)/pow(ELLIPSE_Y,2) + pow(z-(-500),2)/pow(ELLIPSE_Z,2)) - 1;
        }
    }
    else{
        if (shapeId == 0){
            formula = sqrt(pow(x-SCREEN_WIDTH/2,2) + pow(y-SCREEN_HEIGHT/2,2)) - SPHERE_R;
        }
        else if (shapeId == 1){
            formula = abs(x-SCREEN_WIDTH/2) + abs(y-SCREEN_HEIGHT/2) -CUBE_R;
        }
        else if (shapeId == 2){
            formula = sqrt(pow((x-SCREEN_WIDTH/2),2)/pow(ELLIPSE_X,2) + pow(y-SCREEN_HEIGHT/2,2)/pow(ELLIPSE_Y,2)) - 1;
        }
    }
    return formula;
}

//**************************
//**** lookup table *****
//
//
//**************************
GLint triTable3D[256][16] =
{
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
    {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
    {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
    {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
    {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
    {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
    {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
    {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
    {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
    {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
    {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
    {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
    {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
    {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
    {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
    {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
    {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
    {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
    {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
    {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
    {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
    {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
    {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
    {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
    {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
    {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
    {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
    {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
    {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
    {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
    {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
    {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
    {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
    {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
    {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
    {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
    {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
    {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
    {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
    {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
    {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
    {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
    {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
    {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
    {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
    {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
    {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
    {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
    {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
    {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
    {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
    {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
    {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
    {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
    {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
    {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
    {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
    {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
    {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
    {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
    {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
    {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
    {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
    {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
    {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
    {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
    {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
    {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
    {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
    {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
    {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
    {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
    {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
    {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
    {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
    {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
    {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
    {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
    {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
    {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
    {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
    {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
    {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
    {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
    {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
    {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
    {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
    {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
    {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
    {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
    {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
    {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
    {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
    {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
    {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
    {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
    {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
    {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
    {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
    {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
    {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
    {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
    {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
    {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

// 0 _________ 1
//  |        |
//  |        |
//  |________|
// 3           2
GLint triTable2D[16][4] =
{
    {-1,-1,-1,-1},
    {3,0,-1,-1},
    {0,1,-1,-1},
    {3,1,-1,-1},
    {1,2,-1,-1},
    {3,0,2,1},
    {2,0,-1,-1},
    {3,2,-1,-1},
    {3,2,-1,-1},
    {2,0,-1,-1},
    {3,0,2,1},
    {1,2,-1,-1},
    {3,1,-1,-1},
    {0,1,-1,-1},
    {3,0,-1,-1},
    {-1,-1,-1,-1}
};



