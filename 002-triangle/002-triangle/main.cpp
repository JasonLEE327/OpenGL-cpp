#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_DEEPTH -500
#define CELL_SZIE 30

#define GRID_SCALE 3

//3D shape param
#define SPHERE_R 100
#define ELLIPSE_X 100
#define ELLIPSE_Y 60
#define ELLIPSE_Z 70
#define CUBE_R 100

#define MAX_CHAR 128


GLvoid DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength );
GLvoid DrawGrid3D( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLint edgeLength );
GLvoid DrawLines3D( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength );

GLvoid DrawPoints(GLfloat x, GLfloat y, GLfloat z);
GLvoid DrawPoint(GLfloat x, GLfloat y, GLfloat z);
//GLvoid DrawLine(GLfloat *p1, GLfloat *p2);

GLvoid DrawPoints();
vector<GLfloat> movePoint(vector<GLfloat> point, GLfloat timestep, string dir);


GLvoid printtext(int x, int y, std::string String);

GLvoid DrawSquare(GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength );
GLvoid DrawGrid2D(GLint num, GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength );
GLvoid DrawLines2D( GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength);

GLint signJudge(GLfloat num1, GLfloat num2);
GLfloat *Interp(GLfloat *p1,GLfloat *p2, GLfloat *newp, GLfloat sdfm, GLfloat sdf1);
GLfloat *EdgePos(GLint edge, GLfloat **vertices);
GLfloat **cellVertices3D(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength);
GLfloat **cellVertices2D(GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength);
GLfloat sdf(GLfloat x, GLfloat y, GLfloat z);

GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );

static const GLint EdgeConnection[12][2] =
{
    {0, 1},{1, 2},{2, 3},{3, 0},
    {4, 5},{5, 6},{6, 7},{7, 4},
    {0, 4},{1, 5},{2, 6},{3, 7}
};

GLfloat rotationX = 40.0f;
GLfloat rotationY = 40.0f;

GLint cubeNum = 1;
GLint shapeId = 0; //0:sphere  1:
GLint Dimension = 3;   //0:2d   1:3d
GLint p_click = 0;

GLfloat randx = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_WIDTH/2 - (2*GRID_SCALE +1)/2.0*CELL_SZIE);
GLfloat randy = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_HEIGHT/2 - (2*GRID_SCALE +1)/2.0*CELL_SZIE);
GLfloat randz = rand() % ((2*GRID_SCALE +1) * CELL_SZIE) + (SCREEN_DEEPTH - (2*GRID_SCALE +1)/2.0*CELL_SZIE);

//GLfloat *sdfDis = new GLfloat[pow(2*GRID_SCALE+1,Dimension)];
//GLfloat sdfDis[7*7*7][8];



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
    vector<GLfloat> newp = {randx,randy,randz};
    
    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        
//        char string[64];
//        sprintf(string, "something");
//        printtext(10,10,string);
        
        switch (Dimension) {
            case 2:
                DrawGrid2D(cubeNum, halfScreenWidth, halfScreenHeight, CELL_SZIE);
                break;
            case 3:
                glPushMatrix( );
                glTranslatef( halfScreenWidth, halfScreenHeight, -500 );
                glRotatef( rotationX, 1, 0, 0 );
                glRotatef( rotationY, 0, 1, 0 );
                glTranslatef( -halfScreenWidth, -halfScreenHeight, 500 );
                
                //draw marching cube
                DrawGrid3D(halfScreenWidth, halfScreenHeight, -500, CELL_SZIE);
            
                glPopMatrix();
                break;
        }
        //draw points
        if (p_click){
            newp = {randx,randy,randz};
            p_click = 0;
        }
        if(cubeNum > (2*GRID_SCALE +1)){
            float dis = sdf(newp[0],newp[1],newp[2]);
            if (abs(dis) > 1){
                if (dis > 0){
                    newp = movePoint(newp,0.001,"in");
                }
                else{
                    newp = movePoint(newp,0.001,"out");
                }
            }
            GLfloat temparr[6] = {randx,randy,randz, newp.at(0),newp.at(1),newp.at(2)};
            glColor3f(0.0,1.0,1.0);
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize(12);
            glEnableClientState( GL_VERTEX_ARRAY );
            glVertexPointer( 3, GL_FLOAT, 0, temparr);
            glDrawArrays(GL_POINTS, 0, 2);
            glDrawArrays(GL_LINES, 0, 2);
        }
        
        glfwSwapBuffers( window );
        glfwPollEvents( );
    }
    glfwTerminate( );
    return 0;
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
                cubeNum = 1;
                shapeId++;
                if (shapeId == 3){
                    shapeId = 0;
                }
                break;
            case GLFW_KEY_D:
                Dimension = 5-Dimension;
                cubeNum = 1;
                break;
            case GLFW_KEY_P:
                p_click = 1;
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
GLvoid DrawGrid2D(GLint num, GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength )
{
    GLint count = 0;
    for (int i = -GRID_SCALE; i <= GRID_SCALE; i++)
    {
        for (int j = -GRID_SCALE; j <= GRID_SCALE; j++)
        {
            DrawSquare( centerPosX+edgeLength*i, centerPosY+edgeLength*j, edgeLength );
            DrawLines2D( centerPosX+edgeLength*i, centerPosY+edgeLength*j, edgeLength );
        }
        count++;
        if(count == num){
            return ;
        }
    }
    if(cubeNum > (2*GRID_SCALE +1)){
        DrawPoint(randx, randy, randz);
    }
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

GLvoid DrawLines2D( GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength)
{
    extern GLint triTable2D[16][4];
    GLfloat lineVertices[4*2];
    GLint edgeCount = 0;
    GLint vertices = 0;
    
    GLfloat **verticelist = cellVertices2D(centerPosX,centerPosY,edgeLength);
    
    for (int i =0; i<4; i++)
    {
        if(sdf(verticelist[i][0], verticelist[i][1], 0) >= 0)
        {
            vertices += pow(2,i);
        }
    }
    //printf("%d/n", vertices);
    for (int i =0,j =0; i< 4; i++)
    {
        if (triTable2D[vertices][i] != -1)
        {
            lineVertices[j] = EdgePos(triTable2D[vertices][i], verticelist)[0];
            lineVertices[j+1] = EdgePos(triTable2D[vertices][i], verticelist)[1];
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




//********************************
//*******              ***********
//*******    3D draw   ***********
//*******              ***********
//********************************
GLvoid DrawGrid3D(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLint edgeLength )
{
    extern GLfloat sdfDis[7*7*7][8];
    //GLfloat *sdfDis_new = new GLfloat[8];
    GLint count = 0;
    for (int i = -GRID_SCALE; i <= GRID_SCALE; i++)
    {
        for (int j = -GRID_SCALE; j <= GRID_SCALE; j++)
        {
            for (int k = -GRID_SCALE; k <= GRID_SCALE; k++)
            {
//                GLfloat **verticelist = cellVertices3D(centerPosX+edgeLength*i, centerPosY+edgeLength*j, centerPosZ+edgeLength*k, edgeLength);
//
//                for (int v = 0; v<8; v++){
//                    sdfDis[(i+3)*(j+3)*(k+3)][v] = sdf(verticelist[v][0], verticelist[v][1], verticelist[v][2]);
//                }
//
//                //check point in cube
//                if (randx <= (centerPosX+edgeLength*i +edgeLength/2) && randx >= (centerPosX+edgeLength*i -edgeLength/2) &&
//                    randy <= (centerPosY+edgeLength*j +edgeLength/2) && randy >= (centerPosY+edgeLength*j -edgeLength/2) &&
//                    randz <= (centerPosZ+edgeLength*k +edgeLength/2) && randz >= (centerPosZ+edgeLength*k -edgeLength/2))
//                {
//                    //printf("%d,%d,%d/n", i,j,k);
//                    for (int v = 0; v<8; v++){
//                        sdfDis_new[v] = sdf(verticelist[v][0], verticelist[v][1], verticelist[v][2]);
//                    }
//                    //printf("%f %f %f %f****",sdfDis_new[0],sdfDis_new[1],sdfDis_new[2],sdfDis_new[3]);
//                }
                DrawCube( centerPosX+edgeLength*i, centerPosY+edgeLength*j, centerPosZ+edgeLength*k, edgeLength );
                DrawLines3D( centerPosX+edgeLength*i, centerPosY+edgeLength*j, centerPosZ+edgeLength*k, edgeLength );
            }
        }
        count++;
        if(count == cubeNum){
            return ;
        }
    }
//    if(cubeNum > (2*GRID_SCALE +1)){
//        DrawPoints();
//    }
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

GLvoid DrawLines3D( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength)
{
    extern GLint triTable3D[256][16];
    GLfloat *lineVertices = new GLfloat[12*3];
    GLint edgeCount = 0;
    GLint vertices = 0;
    GLfloat **verticelist = cellVertices3D(centerPosX,centerPosY,centerPosZ,edgeLength);
    
    for (int i =0; i<8; i++)
    {
        if(sdf(verticelist[i][0], verticelist[i][1], verticelist[i][2]) >= 0)
        {
            vertices += pow(2,i);
        }
    }
    
    for (int i =0,j =0; i< 16; i++)
    {
        if (triTable3D[vertices][i] != -1)
        {
            lineVertices[j] = EdgePos(triTable3D[vertices][i], verticelist)[0];
            lineVertices[j+1] = EdgePos(triTable3D[vertices][i], verticelist)[1];
            lineVertices[j+2] = EdgePos(triTable3D[vertices][i], verticelist)[2];
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


vector<GLfloat> movePoint(vector<GLfloat> point, GLfloat timestep, string dir)
{
    vector<GLfloat> newp;
    
    if (dir == "in"){
        newp.push_back(point.at(0) + timestep * (SCREEN_WIDTH/2 - point.at(0)));
        newp.push_back(point.at(1) + timestep * (SCREEN_HEIGHT/2 - point.at(1)));
        newp.push_back(point.at(2) + timestep * (SCREEN_DEEPTH - point.at(2)));
    }
    else{
        newp.push_back(point.at(0) + timestep * -(SCREEN_WIDTH/2 - point.at(0)));
        newp.push_back(point.at(1) + timestep * -(SCREEN_HEIGHT/2 - point.at(1)));
        newp.push_back(point.at(2) + timestep * -(SCREEN_DEEPTH - point.at(2)));
    }
    
    return newp;
}


GLvoid DrawPoints()
{
    GLfloat newDot[] = {randx,randy,randz};
    
    
    while (sdf(newDot[0],newDot[1],newDot[2])  > 0.1){
        newDot[0]++;
        newDot[1]++;
        newDot[2]++;
        
        
        GLfloat temparr[6] = {randx,randy,randz, newDot[0],newDot[1],newDot[2]};
        // Draw a random point
        glColor3f(1.0,0.0,0.0);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointSize(12);
        glEnableClientState( GL_VERTEX_ARRAY );
        glVertexPointer( 3, GL_FLOAT, 0, temparr);
        glDrawArrays(GL_POINTS, 0, 2);
        glDrawArrays(GL_LINES, 0, 2);
    }
   
}

GLvoid DrawPoint(GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat pointVertex[] = {x,y,z};
    glColor3f(0.0, 1.0, 1.0);
    glEnable( GL_POINT_SMOOTH );
    glEnableClientState( GL_VERTEX_ARRAY );
    glPointSize( 10 );
    glVertexPointer( 3, GL_FLOAT, 0, pointVertex );
    glDrawArrays( GL_POINTS, 0, 1 );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisable( GL_POINT_SMOOTH );
}



//********************************
//*******              ***********
//*******  interpolate ***********
//*******  & otherfunc ***********
//********************************
GLint signJudge(GLfloat num1, GLfloat num2)
{
    if ((num1 < 0 && num2 >0) || (num1 >0 && num2 <0)){
        return 1;
    }
    else{
        return 0;
    }
}

GLfloat *Interp(GLfloat *p1,GLfloat *p2, GLfloat *newp, GLfloat sdfm, GLfloat sdf1)
{
    if ( signJudge(sdfm, sdf1) ){
        p2 = newp;
        newp[0] = (p1[0] + p2[0]) / 2;
        newp[1] = (p1[1] + p2[1]) / 2;
        if (Dimension == 3){
            newp[2] = (p1[2] + p2[2]) / 2;
        }
    }
    else{
        p1 = newp;
        newp[0] = (p1[0] + p2[0]) / 2;
        newp[1] = (p1[1] + p2[1]) / 2;
        if (Dimension == 3){
            newp[2] = (p1[2] + p2[2]) / 2;
        }
    }
    return newp;
}

GLfloat *EdgePos(GLint edge, GLfloat **vertices)
{
    GLfloat *newp = new GLfloat[Dimension];
    GLfloat *p1 = vertices[EdgeConnection[edge][0]];
    GLfloat *p2 = vertices[EdgeConnection[edge][1]];
    GLfloat sdfm;
    GLfloat sdf1;
    
    newp[0] = (p1[0] + p2[0]) / 2;
    newp[1] = (p1[1] + p2[1]) / 2;
    if (Dimension == 3){
        newp[2] = (p1[2] + p2[2]) / 2;
    }
    else{
        newp[2] = 0;
    }

    //GLfloat sdf2 = sdf(shapeId,p2[0],p2[1],p2[2]);
    do {
        sdfm = sdf(newp[0],newp[1],newp[2]);
        sdf1 = sdf(p1[0],p1[1],p1[2]);
        newp = Interp(p1, p2, newp, sdfm, sdf1);
    } while (sdfm<0.001);

    return newp;
}

GLfloat sdf(GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat formula = 0.0;
    if(Dimension == 3){
        if (shapeId == 0){
            //formula = pow(x-SCREEN_WIDTH/2,2) + pow(y-SCREEN_HEIGHT/2,2) + pow(z-(-500),2) - pow(SPHERE_R,2);   //sphere
            formula = sqrt(pow(x-SCREEN_WIDTH/2,2) + pow(y-SCREEN_HEIGHT/2,2) + pow(z-(-500),2)) - SPHERE_R;
        }
        else if (shapeId == 1){
            formula = sqrt(pow((x-SCREEN_WIDTH/2),2)/pow(ELLIPSE_X,2) + pow(y-SCREEN_HEIGHT/2,2)/pow(ELLIPSE_Y,2) + pow(z-(-500),2)/pow(ELLIPSE_Z,2)) - 1;
        }
        else if (shapeId == 2){
            formula = abs(x-SCREEN_WIDTH/2) + abs(y-SCREEN_HEIGHT/2) + abs(z-(-500)) -CUBE_R;
        }
    }
    else{
        if (shapeId == 0){
            //formula = pow(x-SCREEN_WIDTH/2,2) + pow(y-SCREEN_HEIGHT/2,2) + pow(z-(-500),2) - pow(SPHERE_R,2);   //sphere
            formula = sqrt(pow(x-SCREEN_WIDTH/2,2) + pow(y-SCREEN_HEIGHT/2,2)) - SPHERE_R;
        }
        else if (shapeId == 1){
            formula = sqrt(pow((x-SCREEN_WIDTH/2),2)/pow(ELLIPSE_X,2) + pow(y-SCREEN_HEIGHT/2,2)/pow(ELLIPSE_Y,2)) - 1;
        }
        else if (shapeId == 2){
            formula = abs(x-SCREEN_WIDTH/2) + abs(y-SCREEN_HEIGHT/2) -CUBE_R;
            //formula = sqrt(pow(x-SCREEN_WIDTH/2,2) + pow(y-SCREEN_HEIGHT/2,2)) - SPHERE_R;
        }
    }
    return formula;
}

//find vertices of a cell with given center point
GLfloat **cellVertices2D(GLfloat centerPosX, GLfloat centerPosY, GLfloat edgeLength)
{
    GLfloat **vertices = new GLfloat*[4];
    GLint i = 0;
    
    for (int i=0;i<4;i++)
    {
        vertices[i] = new GLfloat[2];
    }
    
    vertices[i][0] = centerPosX+(-1)*(edgeLength/2);
    vertices[i][1] = centerPosY+(-1)*(edgeLength/2);
    
    vertices[i+1][0] = centerPosX+1*(edgeLength/2);
    vertices[i+1][1] = centerPosY+(-1)*(edgeLength/2);
    
    vertices[i+2][0] = centerPosX+1*(edgeLength/2);
    vertices[i+2][1] = centerPosY+1*(edgeLength/2);
    
    vertices[i+3][0] = centerPosX+(-1)*(edgeLength/2);
    vertices[i+3][1] = centerPosY+1*(edgeLength/2);
    
    return vertices;
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
        vertices[i+2][2] = centerPosZ+1*(edgeLength/2);
        
        vertices[i+3][0] = centerPosX+(-1)*(edgeLength/2);
        vertices[i+3][1] = centerPosY+y*(edgeLength/2);
        vertices[i+3][2] = centerPosZ+1*(edgeLength/2);
        
        i= i +4;
    }
    return vertices;
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


