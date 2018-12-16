#include <iostream>
#include <math.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLUT/GLUT.h>

using namespace std;

#define SCREEN_WIDTH 1960
#define SCREEN_HEIGHT 1080
#define SCREEN_DEEPTH 2000

#define domino_dis 9
#define domino_x 2
#define domino_y 25
#define domino_z 9

GLvoid keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );

vector<vector<GLfloat>> getVertex(GLfloat leftBottomX, GLfloat leftBottomY, GLfloat leftBottomZ, GLfloat edgex, GLfloat edgey, GLfloat edgez, GLfloat angleYZ, GLfloat angleXZ);
vector<GLfloat> DrawCuboid( GLfloat leftBottomX, GLfloat leftBottomY, GLfloat leftBottomZ, GLfloat edgex, GLfloat edgey, GLfloat edgez, GLfloat *color, GLfloat angleXZ, GLfloat angleYZ);
vector<vector<GLfloat> > getDominos(GLfloat x, GLfloat y, GLfloat z, GLfloat anglexz, GLfloat angleyz);
void DominoFall(vector<vector<GLfloat>> &dominoPos);
void checkCollision();

vector<GLfloat> draw_straight(vector<GLfloat> prePos, GLfloat degree ,GLint direction);
vector<GLfloat> draw_slope(vector<GLfloat> prePos, GLfloat degree, GLint direction);
vector<GLfloat> draw_down(vector<GLfloat> prePos);
vector<GLfloat> draw_clockwise(vector<GLfloat> prePos);
vector<GLfloat> draw_anticlockwise(vector<GLfloat> prePos);
vector<GLfloat> draw_split(vector<GLfloat> prePos,GLint type);
vector<GLfloat> RB2LT(vector<GLfloat> prePos, GLfloat degree, GLint direction);
vector<GLfloat> LT2RB(vector<GLfloat> prePos, GLfloat degree, GLint direction);
bool pointInCuboid(vector<GLfloat> pt, vector<GLfloat> cuboid);

GLfloat rotationX = 40.0f;
GLfloat rotationY = 40.0f;

GLfloat c1[3] = {0.19, 0.19, 0.8}; // MediumBlue
GLfloat c2[3] = {0.36, 0.25, 0.20}; // DarkBrown
GLfloat c3[3] = {0.73, 0.16, 0.96}; // Med_Purple
GLfloat c4[3] = {0.95, 0.23, 0.37}; //
GLfloat c5[3] = {.55, 0.09, 0.09}; // Scarlet

const double pi = acos(-1);

vector<bool> dominoHit;
//vector<vector<GLfloat>> boundingBox;
vector<vector<GLfloat>> domino_pts;
vector<GLfloat> getTopPoints(vector<GLfloat> cuboid);




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
    GLfloat PlaneEdgeX = 1400, PlaneEdgeY = 5, PlaneEdgeZ = 1000;
    GLfloat PlaneX = halfScreenWidth - PlaneEdgeX/2 , PlaneY = halfScreenHeight, PlaneZ = -halfScreenDeepth - PlaneEdgeZ/2;
    
    GLfloat domino_px = PlaneX + 10;
    GLfloat domino_py = PlaneY + PlaneEdgeY + 3;
    GLfloat domino_pz = PlaneZ + PlaneEdgeZ/2 - domino_z - 3;
    
    GLfloat sphere_r = 10, sphere_slices = 100, sphere_stacks = 100;
    GLfloat sphere_x = PlaneX + sphere_r - 100;
    GLfloat sphere_y = PlaneY + domino_y/2 + PlaneEdgeY + 3 + 20;
    GLfloat sphere_z = PlaneZ + PlaneEdgeZ - domino_z/2 - 3 - PlaneEdgeZ/2;
    
    GLfloat angle_xz = pi/2;
    GLfloat angle_yz = 0;
    
    vector<vector<GLfloat> > dominoPos(1000, vector<GLfloat>(6));
    //dominoPos[i][3] --> angleYZ
    dominoPos= getDominos(domino_px, domino_py, domino_pz, angle_yz, angle_xz);
    dominoHit.resize(dominoPos.size());
    float acc = 0.0;
    
    while ( !glfwWindowShouldClose( window ) )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        glClearColor(0.80f, 0.80f, 0.80f, 0.0f);
        glPushMatrix( );
        glTranslatef( halfScreenWidth, halfScreenHeight, -halfScreenDeepth );
        glRotatef( rotationX, 1, 0, 0 );
        glRotatef( rotationY, 0, -1, 0 );
        //glTranslatef( -halfScreenWidth, -halfScreenHeight, halfScreenDeepth );
        glTranslatef( -halfScreenWidth, -halfScreenHeight, halfScreenDeepth );
        
        //draw plane
        //DrawCuboid(PlaneX, PlaneY, PlaneZ, PlaneEdgeX, PlaneEdgeY, PlaneEdgeZ, c1, 0, pi/2);
        
        //draw sphere
        glTranslatef(sphere_x, sphere_y, sphere_z);
        glColor3f(0,1,0);
        glutSolidSphere(sphere_r, sphere_slices, sphere_stacks);
        glTranslatef(-sphere_x, -sphere_y, -sphere_z);
        domino_pts.clear();
        //boundingBox.clear();
        for (int i = 0; i< 1000; i++){
            if(i < 125){
                domino_pts.push_back(DrawCuboid(dominoPos[i][0], dominoPos[i][1], dominoPos[i][2], domino_x, domino_y, domino_z, c1, dominoPos[i][3], dominoPos[i][4])) ;
            }
            else if(i < 235){
                domino_pts.push_back(DrawCuboid(dominoPos[i][0], dominoPos[i][1], dominoPos[i][2], domino_x, domino_y, domino_z, c2, dominoPos[i][3], dominoPos[i][4])) ;
            }
            else if(i < 346){
                domino_pts.push_back(DrawCuboid(dominoPos[i][0], dominoPos[i][1], dominoPos[i][2], domino_x, domino_y, domino_z, c3, dominoPos[i][3], dominoPos[i][4])) ;
            }
            else if(i < 490){
                domino_pts.push_back(DrawCuboid(dominoPos[i][0], dominoPos[i][1], dominoPos[i][2], domino_x, domino_y, domino_z, c1, dominoPos[i][3], dominoPos[i][4])) ;
            }
            else if(i < 635){
                domino_pts.push_back(DrawCuboid(dominoPos[i][0], dominoPos[i][1], dominoPos[i][2], domino_x, domino_y, domino_z, c4, dominoPos[i][3], dominoPos[i][4])) ;
            }
            else{
                domino_pts.push_back(DrawCuboid(dominoPos[i][0], dominoPos[i][1], dominoPos[i][2], domino_x, domino_y, domino_z, c5, dominoPos[i][3], dominoPos[i][4])) ;
            }
            
        }

        if(sphere_x >= dominoPos[0][0] - sphere_r){
            dominoHit[0] = true;
        }
        DominoFall(dominoPos);
        if(sphere_y <= PlaneY + PlaneEdgeY + sphere_r){
            acc = -acc;
        }
        acc += 9.8 * 0.0005;
        sphere_y += -acc;
        if(!dominoHit[0]){
            sphere_x += 1;
        }
        else{
            
            sphere_x -= 1;
        }
        
        glPopMatrix();
        glfwSwapBuffers( window );
        glfwPollEvents( );
    }
    glfwTerminate( );
    return 0;
}


vector<GLfloat> draw_straight(vector<GLfloat> prePos, GLfloat degree ,GLint direction)
{
    vector<GLfloat> p(6);
    p[0] = prePos[0] + domino_dis * direction;
    p[1] = prePos[1];
    p[2] = prePos[2];
    //p[3] = prePos[3];
    p[3] = degree;
    p[4] = prePos[4];
    p[5] = prePos[5];
    return p;
}

vector<GLfloat> draw_slope(vector<GLfloat> prePos, GLfloat degree, GLint direction)
{
    vector<GLfloat> p(6);
    p[0] = prePos[0] + domino_dis * cos(prePos[3]) * direction;
    p[1] = prePos[1];
    p[2] = prePos[2] - domino_dis * sin(prePos[3]) * direction;
    p[3] = degree;
    p[4] = prePos[4];
    p[5] = prePos[5];
    return p;
}

vector<GLfloat> draw_down(vector<GLfloat> prePos)
{
    vector<GLfloat> p(6);
    p[0] = prePos[0];
    p[1] = prePos[1];
    p[2] = prePos[2] + domino_dis;
    p[3] = prePos[3];
    p[4] = prePos[4];
    p[5] = prePos[5];
    return p;
}

vector<GLfloat> draw_clockwise(vector<GLfloat> prePos)
{
    vector<GLfloat> p(6);
    p[3] = prePos[3] - pi/6;
    p[0] = prePos[0] - (domino_dis-domino_x) * cos(prePos[3]) - domino_x * cos(p[3]) ;
    p[1] = prePos[1];
    p[2] = prePos[2] + (domino_dis-domino_x) * sin(prePos[3]) + domino_x * sin(p[3]) ;
    p[4] = prePos[4];
    p[5] = prePos[5];
    return p;
}

vector<GLfloat> draw_anticlockwise(vector<GLfloat> prePos)
{
    vector<GLfloat> p(6);
    p[0] = prePos[0] + domino_dis * cos(prePos[3]);
    p[1] = prePos[1];
    p[2] = prePos[2] - domino_dis * sin(prePos[3]);
    p[3] = prePos[3] + pi/6;
    p[4] = prePos[4];
    p[5] = prePos[5];
    return p;
}

vector<GLfloat> draw_split(vector<GLfloat> prePos,GLint type)
{
    vector<GLfloat> p(6);
    if(type == 1){
        p[0] = prePos[0] - domino_z * cos(pi/6);
        p[1] = prePos[1];
        p[2] = prePos[2] + domino_dis - domino_z * sin(pi/6);
        p[3] = prePos[3] - pi/6;
        p[4] = prePos[4];
        p[5] = prePos[5];
    }
    else if(type == 2){
        p[0] = prePos[0] - domino_dis + domino_x;
        p[1] = prePos[1];
        p[2] = prePos[2] - domino_z * 2/3;
        p[3] =  - pi/6;
        p[4] = prePos[4];
        p[5] = prePos[5];
    }
    else if(type == 3){
        p[0] = prePos[0] - domino_x;
        p[1] = prePos[1];
        p[2] = prePos[2] + domino_z * 2/3 + domino_z;
        p[3] = 7*pi/6;
        p[4] = prePos[4];
        p[5] = prePos[5] * (-1);
    }
    else if(type == 4){
        p[0] = prePos[0] + domino_z * 5/3;
        p[1] = prePos[1];
        p[2] = prePos[2] + domino_dis - domino_x * 2;
        p[3] =  - pi/3;
        p[4] = prePos[4];
        p[5] = prePos[5] * (-1);
    }
    else if(type == 5){
        p[0] = prePos[0] + domino_dis - domino_x;
        p[1] = prePos[1];
        p[2] = prePos[2] - domino_z * 2/3;
        p[3] = pi/6;
        p[4] = prePos[4];
        p[5] = prePos[5];
    }
    else if(type == 6){
        p[0] = prePos[0] + domino_dis;
        p[1] = prePos[1];
        p[2] = prePos[2] + domino_z * 2/3 + domino_z;
        p[3] = 5*pi/6;
        p[4] = prePos[4];
        p[5] = prePos[5] * (-1);
    }
    return p;
}

vector<GLfloat> RB2LT(vector<GLfloat> prePos, GLfloat degree, GLint direction)
{
    vector<GLfloat> p(6);
    p[3] = degree;
    p[0] = prePos[0] - domino_z* sin(p[3]) + domino_dis * cos(p[3])*direction - domino_x*cos(p[3]);
    p[1] = prePos[1];
    p[2] = prePos[2] - domino_z* cos(p[3]) - domino_dis * sin(p[3])*direction + domino_x*sin(p[3]);
    p[4] = prePos[4];
    p[5] = prePos[5] * (-1);
    return p;
}

vector<GLfloat> LT2RB(vector<GLfloat> prePos, GLfloat degree, GLint direction)
{
    vector<GLfloat> p(6);
    p[3] = degree;
    p[0] = prePos[0] - domino_z * sin(p[3]) - domino_dis * cos(p[3])*direction - domino_x * cos(p[3]);
    p[1] = prePos[1];
    p[2] = prePos[2] - domino_z * cos(p[3]) + domino_dis * sin(p[3])*direction + domino_x * sin(p[3]);
    p[4] = prePos[4];
    p[5] = prePos[5] * (-1);
    return p;
}


vector<vector<GLfloat>> getDominos(GLfloat x, GLfloat y, GLfloat z, GLfloat angleyz, GLfloat anglexz)
{
    vector<vector<GLfloat> > dominoPos(1000, vector<GLfloat>(6));
    vector<GLfloat> angleYZ(1000);
    
    dominoPos[0][0] = x;
    dominoPos[0][1] = y;
    dominoPos[0][2] = z;
    dominoPos[0][3] = angleyz;
    dominoPos[0][4] = anglexz;
    dominoPos[0][5] = 1;
    
    
    for(int i = 1; i<1000; i++){
        
        if(i < 10){dominoPos[i] = draw_straight(dominoPos[i-1], 0 ,1);}  //平
        else if(i < 80){dominoPos[i] = draw_slope(dominoPos[i-1], pi/6, 1);}//30度右上斜
        ////改变顶点: left top to right bottom in top view
        else if(i == 80){
            dominoPos[i][3] = pi;
            dominoPos[i][0] = dominoPos[i-1][0] + domino_z * sin(dominoPos[i-1][3]) * 1 + domino_dis * cos(dominoPos[i-1][3]) - domino_x*cos(dominoPos[i][3]);
            dominoPos[i][1] = dominoPos[i-1][1];
            dominoPos[i][2] = dominoPos[i-1][2] + domino_z * cos(dominoPos[i-1][3]) * 1 - domino_dis * sin(dominoPos[i-1][3]) + domino_x*sin(dominoPos[i][3]);
            dominoPos[i][4] = dominoPos[i-1][4];
            dominoPos[i][5] = -1;
            
        }
        else if(i < 84){dominoPos[i] = draw_clockwise(dominoPos[i-1]);}//顺时针转
        else if(i < 115){dominoPos[i] = draw_down(dominoPos[i-1]);}//竖下
        
        
        
        else if(i == 115){dominoPos[i] = draw_split(dominoPos[i-1], 1);}//主分裂 第一支
        else if(i < 118){dominoPos[i] = draw_clockwise(dominoPos[i-1]);}//顺时针转
        else if(i < 125){dominoPos[i] = draw_straight(dominoPos[i-1],0,-1);}//直左
        
        
        else if(i == 125){dominoPos[i] = draw_split(dominoPos[i-1], 2);}//次分裂1 第一支
        else if(i < 130){dominoPos[i] = draw_clockwise(dominoPos[i-1]);}//顺时针转
        else if(i == 130){dominoPos[i] = RB2LT(dominoPos[i-1], pi/6 ,1);}//改变顶点 from right bottom to left top in direction 1 in top view
        else if(i < 135){dominoPos[i] = draw_slope(dominoPos[i-1], pi/6, 1);}//30度右上斜
        else if(i < 141){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);}//逆时针转
        else if(i == 141){dominoPos[i] = RB2LT(dominoPos[i-1], pi/6, -1);}//改变顶点 from right bottom to left top in direction -1 in top view
        else if(i < 161){dominoPos[i] = draw_slope(dominoPos[i-1], pi/6, -1);}//30度左下斜
        else if(i < 167){dominoPos[i] = draw_clockwise(dominoPos[i-1]);}//顺时针转
        else if(i == 167){ dominoPos[i] = RB2LT(dominoPos[i-1], pi/6 ,1);} //改变顶点 from right bottom to left top in direction 1 in top view
        else if(i < 191){dominoPos[i] = draw_slope(dominoPos[i-1], pi/6, 1);}//30度右上斜
        else if(i < 197){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);}//逆时针转
        else if(i == 197){ dominoPos[i] = RB2LT(dominoPos[i-1], pi/6, -1);}//改变顶点 from right bottom to left top in direction -1 in top view
        else if(i < 235){dominoPos[i] = draw_slope(dominoPos[i-1], pi/6, -1);}//30度左下斜
        
        
        
        else if(i == 235){dominoPos[i] = draw_split(dominoPos[124], 3);}//次分裂1 第二支
        else if(i < 240){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);}//逆时针转
        else if(i == 240){dominoPos[i] = LT2RB(dominoPos[i-1], 5*pi/6, 1);}//改变顶点 left top to right bottom in direction 1
        else if(i < 245){dominoPos[i] = draw_slope(dominoPos[i-1], 5*pi/6, -1);}//30右下斜
        else if(i < 251){dominoPos[i] = draw_clockwise(dominoPos[i-1]);} //顺时针转
        else if(i == 251){dominoPos[i] = LT2RB(dominoPos[i-1], 5*pi/6, -1);}//改变顶点 left top to right bottom in direction -1
        else if(i < 270){dominoPos[i] = draw_slope(dominoPos[i-1], 5*pi/6, 1);}//30左上斜
        else if(i < 276){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]); }//逆时针转
        else if(i == 276){dominoPos[i] = LT2RB(dominoPos[i-1], 5*pi/6, 1);}//改变顶点 left top to right bottom in direction 1
        else if(i < 297){dominoPos[i] = draw_slope(dominoPos[i-1], 5*pi/6, -1);}//30右下斜
        else if(i < 303){dominoPos[i] = draw_clockwise(dominoPos[i-1]);} //顺时针转
        else if(i == 303){dominoPos[i] = LT2RB(dominoPos[i-1], 5*pi/6, -1);}//改变顶点 left top to right bottom in direction -1
        else if(i < 340){dominoPos[i] = draw_slope(dominoPos[i-1], 5*pi/6, 1);}//30左上斜
        else if(i < 346){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]); }//逆时针转
        else if(i == 346){dominoPos[i] = LT2RB(dominoPos[i-1], 5*pi/6, 1);}//改变顶点 left top to right bottom in direction 1
        else if(i < 398){dominoPos[i] = draw_slope(dominoPos[i-1], 5*pi/6, -1);}//30右下斜
        
        else if (i == 398) {dominoPos[i] = RB2LT(dominoPos[i-1], -pi/6, 1);}// 改变顶点
        else if(i < 401){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);}//顺时针转
        else if(i < 460){dominoPos[i] = draw_slope(dominoPos[i-1], pi/6, 1);}//30度右上斜
        else if(i == 460){dominoPos[i] = LT2RB(dominoPos[i-1], pi + pi/6, 1);}//改变顶点
        else if(i < 480){dominoPos[i] = draw_straight(dominoPos[i-1], pi ,1);}
        
        else if(i == 480){dominoPos[i] = draw_split(dominoPos[114], 4);}//main split -- second
        else if(i < 483){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);} // anticlockwise
        else if(i < 490){dominoPos[i] = draw_straight(dominoPos[i-1], 0 ,1);} // straight right
        
        
        
        else if(i== 490){dominoPos[i] = draw_split(dominoPos[i-1], 5);}//minor split2 -- first
        else if(i < 495){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);}//逆时针转
        else if(i == 495){dominoPos[i] = LT2RB(dominoPos[i-1], -pi/6, 1);}//改变顶点 left top to right bottom in direction 1
        else if(i < 500){dominoPos[i] = draw_slope(dominoPos[i-1], -pi/6, -1);}//30右下斜
        else if(i < 506){dominoPos[i] = draw_clockwise(dominoPos[i-1]);} //顺时针转
        else if(i == 506){dominoPos[i] = LT2RB(dominoPos[i-1], -pi/6, -1);}//改变顶点 left top to right bottom in direction -1
        else if(i < 522){dominoPos[i] = draw_slope(dominoPos[i-1], -pi/6, 1);}//30左上斜
        else if(i < 528){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]); }//逆时针转
        else if(i == 528){dominoPos[i] = LT2RB(dominoPos[i-1], -pi/6, 1);}//改变顶点 left top to right bottom in direction 1
        else if(i < 549){dominoPos[i] = draw_slope(dominoPos[i-1], -pi/6, -1);}//30右下斜
        else if(i < 555){dominoPos[i] = draw_clockwise(dominoPos[i-1]);} //顺时针转
        else if(i == 555){dominoPos[i] = LT2RB(dominoPos[i-1], -pi/6, -1);}//改变顶点 left top to right bottom in direction -1
        else if(i < 589){dominoPos[i] = draw_slope(dominoPos[i-1], -pi/6, 1);}//30左上斜
        else if(i < 595){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);}//逆时针转
        else if(i == 595){dominoPos[i] = LT2RB(dominoPos[i-1], -pi/6, 1);}//改变顶点 left top to right bottom in direction 1
        else if(i < 635){dominoPos[i] = draw_slope(dominoPos[i-1], -pi/6, -1);}//30右下斜
        
        
        else if(i == 635){dominoPos[i] = draw_split(dominoPos[489], 6);}//minor split2 -- first
        else if(i < 640){dominoPos[i] = draw_clockwise(dominoPos[i-1]);}//顺时针转
        else if(i == 640){dominoPos[i] = RB2LT(dominoPos[i-1], -5*pi/6 ,1);}//改变顶点 from right bottom to left top in direction 1 in top view
        else if(i < 644){dominoPos[i] = draw_slope(dominoPos[i-1], -5*pi/6, 1);}//30度右上斜
        else if(i < 650){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);}//逆时针转
        else if(i == 650){dominoPos[i] = RB2LT(dominoPos[i-1], -5*pi/6, -1);}//改变顶点 from right bottom to left top in direction -1 in top view
        else if(i < 665){dominoPos[i] = draw_slope(dominoPos[i-1], -5*pi/6, -1);}//30度左下斜
        else if(i < 671){dominoPos[i] = draw_clockwise(dominoPos[i-1]);}//顺时针转
        else if(i == 671){ dominoPos[i] = RB2LT(dominoPos[i-1], -5*pi/6 ,1);} //改变顶点 from right bottom to left top in direction 1 in top view
        else if(i < 690){dominoPos[i] = draw_slope(dominoPos[i-1], -5*pi/6, 1);}//30度右上斜
        else if(i < 696){dominoPos[i] = draw_anticlockwise(dominoPos[i-1]);}//逆时针转
        else if(i == 696){ dominoPos[i] = RB2LT(dominoPos[i-1], -5*pi/6, -1);}//改变顶点 from right bottom to left top in direction -1 in top view
        else if(i < 740){dominoPos[i] = draw_slope(dominoPos[i-1], -5*pi/6, -1);}//30度左下斜
        //
        
        
    }
    
    return dominoPos;
}

vector<vector<GLfloat>> getVertex(GLfloat leftBottomX, GLfloat leftBottomY, GLfloat leftBottomZ, GLfloat edgex, GLfloat edgey, GLfloat edgez, GLfloat angleYZ, GLfloat angleXZ)
{
    vector<vector<GLfloat>> vertex(8, vector<GLfloat>(3));
    vertex[0] = {leftBottomX + edgez*sin(angleYZ) + (edgex*(1-sin(angleXZ)) + edgey*cos(angleXZ))*cos(angleYZ),
        leftBottomY + edgey -edgey+edgey*sin(angleXZ)+edgex*cos(angleXZ),
        leftBottomZ + edgez*cos(angleYZ)  - (edgex*(1-sin(angleXZ)) + edgey*cos(angleXZ))*sin(angleYZ)};
    
    vertex[1] = {leftBottomX + edgez*sin(angleYZ) + edgex*cos(angleYZ) + edgey*cos(angleXZ)*cos(angleYZ),
        leftBottomY + edgey -edgey+edgey*sin(angleXZ),
        leftBottomZ + edgez*cos(angleYZ) - edgex*sin(angleYZ) - edgey*cos(angleXZ)*sin(angleYZ)};
    
    vertex[2] = {leftBottomX + edgez*sin(angleYZ) + edgex*cos(angleYZ),
        leftBottomY,
        leftBottomZ + edgez*cos(angleYZ) - edgex*sin(angleYZ)};
    
    vertex[3] = {leftBottomX + edgez*sin(angleYZ)  + edgex*(1-sin(angleXZ))*cos(angleYZ),
        leftBottomY  +edgex*cos(angleXZ),
        leftBottomZ + edgez*cos(angleYZ) - edgex*(1-sin(angleXZ))*sin(angleYZ)};
    
    vertex[4] = {leftBottomX  + (edgex*(1-sin(angleXZ)) + edgey*cos(angleXZ))*cos(angleYZ),
        leftBottomY + edgey -edgey+edgey*sin(angleXZ)+edgex*cos(angleXZ),
        leftBottomZ - (edgex*(1-sin(angleXZ)) + edgey*cos(angleXZ))*sin(angleYZ)};
    
    vertex[5] = {leftBottomX + edgex*cos(angleYZ) + edgey*cos(angleXZ)*cos(angleYZ),
        leftBottomY + edgey -edgey+edgey*sin(angleXZ),
        leftBottomZ - edgex*sin(angleYZ) - edgey*cos(angleXZ)*sin(angleYZ)};
    
    vertex[6] = {leftBottomX + edgex*cos(angleYZ),
        leftBottomY,
        leftBottomZ - edgex*sin(angleYZ)};
    
    vertex[7] = {leftBottomX + edgex*(1-sin(angleXZ))*cos(angleYZ),
        leftBottomY +edgex*cos(angleXZ),
        leftBottomZ - edgex*(1-sin(angleXZ))*sin(angleYZ)};
    return vertex;
}



vector<GLfloat> DrawCuboid( GLfloat leftBottomX, GLfloat leftBottomY, GLfloat leftBottomZ, GLfloat edgex, GLfloat edgey, GLfloat edgez, GLfloat *color, GLfloat angleYZ, GLfloat angleXZ)
{
    
    vector<vector<GLfloat>> p(8, vector<GLfloat>(3));
    p = getVertex(leftBottomX, leftBottomY, leftBottomZ, edgex, edgey, edgez, angleYZ, angleXZ);
    
    GLfloat vertices[] =
    {
        // front face
        p[0][0], p[0][1], p[0][2], // top left
        p[1][0], p[1][1], p[1][2], // top right
        p[2][0], p[2][1], p[2][2], // bottom right
        p[3][0], p[3][1], p[3][2], // bottom left
        
        // back face
        p[4][0], p[4][1], p[4][2], // top left
        p[5][0], p[5][1], p[5][2], // top right
        p[6][0], p[6][1], p[6][2], // bottom right
        p[7][0], p[7][1], p[7][2], // bottom left
        
        // left face
        p[0][0], p[0][1], p[0][2], // top left
        p[4][0], p[4][1], p[4][2], // top right
        p[7][0], p[7][1], p[7][2], // bottom right
        p[3][0], p[3][1], p[3][2], // bottom left
        
        // right face
        p[1][0], p[1][1], p[1][2], // top left
        p[5][0], p[5][1], p[5][2], // top right
        p[6][0], p[6][1], p[6][2], // bottom right
        p[2][0], p[2][1], p[2][2], // bottom left
        
        //bottom face
        p[0][0], p[0][1], p[0][2], // top left
        p[4][0], p[4][1], p[4][2], // top right
        p[5][0], p[5][1], p[5][2], // bottom right
        p[1][0], p[1][1], p[1][2],// bottom left
        
        //top face
        p[3][0], p[3][1], p[3][2], // top left
        p[7][0], p[7][1], p[7][2], // top right
        p[6][0], p[6][1], p[6][2], // bottom right
        p[2][0], p[2][1], p[2][2], // bottom left
    };
    
    //                 v7______________________v6
    //                  /|                    /|
    //                 / |                   / |
    //                /  |                  /  |
    //               /___|_________________/   |
    //            v3|    |                 |v2 |
    //              |    |                 |   |
    //              |    |                 |   |
    //              |    |                 |   |
    //              |    |_________________|___|
    //              |   / v4               |   /v5
    //              |  /                   |  /
    //              | /                    | /
    //              |/_____________________|/
    //              v0                     v1
    
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    //glColor3f(1.0, 0.0, 0.0);
    glColor3f(color[0],color[1],color[2]);
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );
    glDrawArrays( GL_QUADS, 0, 24 );
    glDisableClientState( GL_VERTEX_ARRAY );
    
    
    /*
     // return the most right, left, top, bottom, front and back value for the bounding box
     vector<float> result;
     // Bouding Box: [0] MaxX [1] MinX [2] MaxY [3] MinY [4] MaxZ [5] MinZ
     for(int i = 0; i < 3; i++)
     {
     result.push_back(max({p[0][i], p[1][i], p[2][i], p[3][i], p[4][i], p[5][i], p[6][i], p[7][i]}));
     result.push_back(min({p[0][i], p[1][i], p[2][i], p[3][i], p[4][i], p[5][i], p[6][i], p[7][i]}));
     }
     */
    
    // return the eight coordinates of a cuboid
    vector<GLfloat> result;
    
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            result.push_back(p[i][j]);
        }
    }
    
    return result;
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

void DominoFall(vector<vector<GLfloat>> &dominoPos)
{
    //cout << dominoHit[235] << "\n";
    int k = 1;
    for(int i = 0; i < dominoPos.size(); i++)
    {
        k = 1;
        
        
        if(dominoPos[i][5] == -1){
            k = -1;
        }
        
        // starts to fall only when the domino gets hit
        //if(i == 235) cout << i << " is falling? " << dominoHit[i] << "\n";
        
        if(dominoHit[i])
        {
            //if(i < 250 && i > 200) cout << i << " is falling!!" << "\n";
            if((k == -1 && dominoPos[i][4] > (pi - pi/7)) || dominoPos[i][4] < pi/7){
                continue;
            }
            //cout << "got in" << "\n";
            dominoPos[i][4] -= k * 0.01 * 5;
            if(dominoPos[i][4] < pi/3){
                dominoPos[i][4] += k * 0.004 * 5;
            }
            
        }
    }
    checkCollision();
}

void checkCollision()
{
    // check if there is a collision between the current domino and the previous domino
    // so checking from the 2nd domino to the last one
    for(int i = 1; i < dominoHit.size(); i ++)
    {
        //for(int j = 0; j < i; j ++)
        //{
        vector<GLfloat> prevCuboidPoints;
        if(i == 235) {prevCuboidPoints = getTopPoints(domino_pts[124]);}
        else if(i == 635) {prevCuboidPoints = getTopPoints(domino_pts[489]);}
        else if(i == 480) {prevCuboidPoints = getTopPoints(domino_pts[114]);}
        else {prevCuboidPoints = getTopPoints(domino_pts[i-1]);}
        for(int k = 0; k < 8; k++)
        {
            vector<GLfloat> temp;
            temp.assign({prevCuboidPoints[k*3+0], prevCuboidPoints[k*3+1], prevCuboidPoints[k*3+2]});
            //if(i == 1 && dominoHit[0])cout << "with points: " << temp[0] << ", " << temp[1] << ", " << temp[2] << "\n";
            if(pointInCuboid(temp, domino_pts[i])) {
                //cout << i << " got hit\n";
                dominoHit[i] = true;
            }
        }
        
    }
}


float dot_product(vector<float> p1, vector<float> p2)
{
    float result = 0.0;
    for(int i = 0; i < 3; i++)
    {
        result += p1[i] * p2[i];
    }
    return result;
}

bool pointInCuboid(vector<GLfloat> pt, vector<GLfloat> cuboid)
{
    vector<GLfloat> u, v, w;
    
    // Step 1: u = p0 - p4, v = p0 - p1, w = p0 - p3
    u.assign({cuboid[0] - cuboid[12], cuboid[1] - cuboid[13] , cuboid[2] - cuboid[14]});
    v.assign({cuboid[0] - cuboid[3], cuboid[1] - cuboid[4] , cuboid[2] - cuboid[5]});
    w.assign({cuboid[0] - cuboid[9], cuboid[1] - cuboid[10] , cuboid[2] - cuboid[11]});
    
    // Temp value...
    vector<GLfloat> p0, p4, p1, p3;
    p0.assign({cuboid[0], cuboid[1], cuboid[2]});
    p4.assign({cuboid[12], cuboid[13], cuboid[14]});
    p1.assign({cuboid[3], cuboid[4], cuboid[5]});
    p3.assign({cuboid[9], cuboid[10], cuboid[11]});
    
    // Step 2:
    float ux = dot_product(u, pt);
    float up0 = dot_product(u, p0);
    float up4 = dot_product(u, p4);
    if(!(ux > min(up0, up4) && ux < max(up0, up4))) return false;
    
    float vx = dot_product(v, pt);
    float vp0 = dot_product(v, p0);
    float vp1 = dot_product(v, p1);
    
    if(!(vx > min(vp0, vp1) && vx < max(vp0, vp1))) return false;
    
    float wx = dot_product(w, pt);
    float wp0 = dot_product(w, p0);
    float wp3 = dot_product(w, p3);
    if(!(wx > min(wp0, wp3) && wx < max(wp0, wp3))) return false;
    
    
    //cout << "collision!!" << "\n";
    return true;
}

// return the top four points and the four mid points of the top face
vector<GLfloat> getTopPoints(vector<GLfloat> cuboid)
{
    
    //                 v7______________________v6
    //                  /|                    /|
    //                 / |                   / |
    //                /  |                  /  |
    //               /___|_________________/   |
    //            v3|    |                 |v2 |
    //              |    |                 |   |
    //              |    |                 |   |
    //              |    |                 |   |
    //              |    |_________________|___|
    //              |   / v4               |   /v5
    //              |  /                   |  /
    //              | /                    | /
    //              |/_____________________|/
    //              v0                     v1
    vector<GLfloat> result;
    
    result.assign({(cuboid[0*3+0] + cuboid[4*3+0])/2, (cuboid[0*3+1] + cuboid[4*3+1])/2, (cuboid[0*3+2] + cuboid[4*3+2])/2,
        (cuboid[0*3+0] + cuboid[1*3+0])/2, (cuboid[0*3+1] + cuboid[1*3+1])/2, (cuboid[0*3+2] + cuboid[1*3+2])/2,
        (cuboid[1*3+0] + cuboid[5*3+0])/2, (cuboid[1*3+1] + cuboid[5*3+1])/2, (cuboid[1*3+2] + cuboid[5*3+2])/2,
        (cuboid[5*3+0] + cuboid[4*3+0])/2, (cuboid[5*3+1] + cuboid[4*3+1])/2, (cuboid[5*3+2] + cuboid[4*3+2])/2,
        cuboid[0*3+0], cuboid[0*3+1], cuboid[0*3+2],
        cuboid[4*3+0], cuboid[4*3+1], cuboid[4*3+2],
        cuboid[5*3+0], cuboid[5*3+1], cuboid[5*3+2],
        cuboid[1*3+0], cuboid[1*3+1], cuboid[1*3+2]
    });
    
    /*
     for(int i = 13; i < 24; i++)
     {
     result.push_back(cuboid[i]);
     }
     */
    return result;
}

