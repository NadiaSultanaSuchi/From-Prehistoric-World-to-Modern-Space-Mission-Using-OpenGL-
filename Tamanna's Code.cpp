#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

// ===== GLOBAL =====
GLfloat angle = 0.0f;
GLfloat rocketPos = 0.0f;
GLfloat flame = 0.0f;

int phase = 0;
float landY = 0.5f;

GLfloat moonAngle = 0.0f;
float orbitAngle = 0.0f;
float orbitTime = 0.0f;

// ===== CAMERA =====
float camZoom = 1.0f;
float camTargetZoom = 1.0f;
float camX = 0.0f;
float camY = 0.0f;
int camMode = 0;

// ===== ROCKET TRACK =====
float rocketX = 0.0f;
float rocketY = 0.0f;

// ===== STARS =====
#define STAR_COUNT 120
float starX[STAR_COUNT], starY[STAR_COUNT], starBlink[STAR_COUNT];

// ===== DUST =====
#define DUST_COUNT 80
float dustY[DUST_COUNT], dustLife[DUST_COUNT];

// ================= BASIC =================
void drawCircle(float r){
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++){
        float th = 2*3.1416f*i/200;
        glVertex2f(r*cos(th), r*sin(th));
    }
    glEnd();
}

// ================= STARS =================
void initStars(){
    for(int i=0;i<STAR_COUNT;i++){
        starX[i]=((rand()%200)/100.0f)-1;
        starY[i]=((rand()%200)/100.0f)-1;
        starBlink[i]=rand()%100;
    }
}

void drawStars(){
    for(int i=0;i<STAR_COUNT;i++){
        float b=0.5f+0.5f*sin(starBlink[i]);
        glColor3f(b,b,b);
        glBegin(GL_POINTS);
        glVertex2f(starX[i],starY[i]);
        glEnd();
    }
}

// ================= DUST =================
void initDust(){
    for(int i=0;i<DUST_COUNT;i++){
        dustY[i]=0;
        dustLife[i]=rand()%100;
    }
}

void drawDust(float x,float y){
    for(int i=0;i<DUST_COUNT;i++){
        float spread=((rand()%100)/100.0f-0.5f)*0.25f;

        glColor3f(0.7,0.7,0.7);
        glBegin(GL_POINTS);
        glVertex2f(x+spread,y-0.25f+dustY[i]);
        glEnd();

        dustY[i]+=0.002f;
        dustLife[i]--;

        if(dustLife[i]<=0){
            dustY[i]=0;
            dustLife[i]=rand()%100;
        }
    }
}

// ================= EARTH =================
void drawEarth(){
    glPushMatrix();
    glRotatef(angle,0,0,1);

    glColor3f(0.2,0.6,1);
    drawCircle(0.4);

    glColor3f(0,0.3,0.8);
    drawCircle(0.35);

    glColor3f(0.2,0.7,0.2);
    glBegin(GL_POLYGON);
        glVertex2f(-0.15,0.1);
        glVertex2f(0.1,0.2);
        glVertex2f(0.2,0.0);
        glVertex2f(0.0,-0.1);
    glEnd();

    glPopMatrix();
}

// ================= MOON =================
void drawMoonSurface(){
    glColor3f(0.75,0.75,0.75);
    drawCircle(1.2f);

    for(int i=0;i<20;i++){
        float x = sin(i*7.3f)*0.8f;
        float y = cos(i*5.7f)*0.8f;

        glPushMatrix();
        glTranslatef(x,y,0);

        glColor3f(0.5,0.5,0.5);
        drawCircle(0.1f);

        glColor3f(0.3,0.3,0.3);
        drawCircle(0.05f);

        glPopMatrix();
    }
}

// ================= ROCKET =================
void drawRocket(float x,float y,float rot){

    rocketX = x;
    rocketY = y;

    glPushMatrix();
    glTranslatef(x,y,0);
    glRotatef(rot,0,0,1);

    glColor3f(0.95,0.95,0.95);
    glBegin(GL_POLYGON);
        glVertex2f(-0.04,-0.25);
        glVertex2f(0.04,-0.25);
        glVertex2f(0.05,0.2);
        glVertex2f(-0.05,0.2);
    glEnd();

    glColor3f(0.8,0.1,0.1);
    glBegin(GL_TRIANGLES);
        glVertex2f(0,0.35);
        glVertex2f(-0.05,0.2);
        glVertex2f(0.05,0.2);
    glEnd();

    glColor3f(0.2,0.6,1);
    glPushMatrix();
        glTranslatef(0,0.05,0);
        drawCircle(0.025);
    glPopMatrix();

    glColor3f(0.8,0.2,0.2);
    glBegin(GL_POLYGON);
        glVertex2f(-0.04,-0.05);
        glVertex2f(0.04,-0.05);
        glVertex2f(0.04,0.0);
        glVertex2f(-0.04,0.0);
    glEnd();

    glColor3f(0.7,0.1,0.1);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.04,-0.2);
        glVertex2f(-0.12,-0.3);
        glVertex2f(-0.04,-0.1);

        glVertex2f(0.04,-0.2);
        glVertex2f(0.12,-0.3);
        glVertex2f(0.04,-0.1);
    glEnd();

    float f=0.12+0.06*sin(flame);

    glColor3f(1,0.7,0);
    glBegin(GL_TRIANGLES);
        glVertex2f(0,-0.25);
        glVertex2f(-0.03,-0.38-f);
        glVertex2f(0.03,-0.38-f);
    glEnd();

    glColor3f(1,0.3,0);
    glBegin(GL_TRIANGLES);
        glVertex2f(0,-0.28);
        glVertex2f(-0.02,-0.34-f);
        glVertex2f(0.02,-0.34-f);
    glEnd();

    glPopMatrix();
}

// ================= KEYBOARD =================
void keyboard(unsigned char key, int x, int y){

    if(key=='c' || key=='C'){
        camMode = 1;
        camTargetZoom = 2.0f;
    }

    if(key=='m' || key=='M'){
        camMode = 2;
        camTargetZoom = 3.0f;
    }

    if(key=='r' || key=='R'){
        camMode = 0;
        camTargetZoom = 1.0f;
        camX = 0;
        camY = 0;
    }
}

// ================= IDLE =================
void idle(){

    if(phase==0){
        rocketPos += 0.00003f;
        if(rocketPos>=1.0f) phase=1;
    }
    else if(phase==1){
        landY -= 0.001f;
        if(landY <= 0.0f){
            landY = 0.0f;
            phase=2;
        }
    }
    else if(phase==2){
        orbitAngle += 0.5f;
        orbitTime += 0.01f;

        if(orbitTime > 6.0f){
            phase = 3;
        }
    }
    else if(phase==3){
        landY -= 0.001f;
        if(landY <= -0.2f){
            landY = -0.2f;
        }
    }

    angle += 0.05f;
    flame += 0.08f;
    moonAngle += 0.0005f;

    camZoom += (camTargetZoom-camZoom)*0.05f;

    for(int i=0;i<STAR_COUNT;i++)
        starBlink[i]+=0.03f;

    glutPostRedisplay();
}

// ================= DISPLAY =================
void display(){

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if(camMode == 1){
        camX = -rocketX;
        camY = -rocketY;
    }
    else if(camMode == 2){
        camX = -0.45f;
        camY = -0.55f;
    }

    glTranslatef(camX, camY, 0);
    glScalef(camZoom,camZoom,1);

    drawStars();

    glPushMatrix();
    glTranslatef(-0.35,-0.45,0);
    drawEarth();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.45,0.55,0);
    glColor3f(0.85,0.85,0.85);
    drawCircle(0.15);
    glPopMatrix();

    if(phase==0){

        float t=rocketPos;

        float x0=-0.35f,y0=-0.45f;
        float x1=-0.6f,y1=0.4f;
        float x2=0.2f,y2=0.9f;
        float x3=0.45f,y3=0.55f;

        float x=pow(1-t,3)*x0 + 3*pow(1-t,2)*t*x1 +
                3*(1-t)*t*t*x2 + t*t*t*x3;

        float y=pow(1-t,3)*y0 + 3*pow(1-t,2)*t*y1 +
                3*(1-t)*t*t*y2 + t*t*t*y3;

        float rot=atan2(y3-y0,x3-x0)*180/3.1416;

        drawRocket(x,y,rot-90);
    }
    else{
        glPushMatrix();
        glRotatef(moonAngle,0,0,1);
        drawMoonSurface();
        glPopMatrix();

        if(phase==2){
            float r = 0.5f;
            float x = r * cos(orbitAngle * 3.1416/180);
            float y = r * sin(orbitAngle * 3.1416/180);
            drawRocket(x,y,orbitAngle+90);
        }
        else if(phase==3){
            drawRocket(0,landY,0);
            drawDust(0,landY);
        }
        else{
            drawRocket(0,landY,0);
        }
    }

    glutSwapBuffers();
}

// ================= MAIN =================
int main(int argc,char** argv){

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(800,600);
    glutCreateWindow("Orbit Then Landing Rocket");

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);

    initStars();
    initDust();

    glutMainLoop();
    return 0;
}
