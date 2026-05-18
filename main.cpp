#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

//   GLOBAL STATE

bool isNight            = false;
bool showPresentWorld   = false;
float cameraScale       = 1.0f;
float spaceBlendAmount  = 0.0f;

float dinoMoveX         = 0.0f;
float dinoLegAngle      = 0.0f;
float dinoBodyBounce    = 0.0f;
bool  dinoPaused        = false;

//egg h
bool  eggHatching       = false;
float eggCrackProgress  = 0.0f;
float babyDinoMoveX     = 0.0f;
float babyDinoHeadBob   = 0.0f;
float babyDinoLegAngle  = 0.0f;

// air ani
float windTimer         = 0.0f;

// world trans
bool  transitioning     = false;
float transitionProgress= 0.0f;

// rocket
float rocketPositionY   = -0.62f;
float smokeFlowTimer    = 0.0f;
bool  rocketLaunched    = false;
float rocketSupportArmX = 0.0f;
int   rocketStage       = 0;
float countdownTimer    = 0.0f;

// bird
float birdPositionX     = 1.2f;
float birdWingAngle     = 0.0f;

// cloud
float cloud1X = -1.2f;
float cloud2X =  0.0f;
float cloud3X =  1.2f;

const float PI = 3.1415926535f;

struct Point2D {
    float x, y;
};


//  GROUPMATE GLOBALS
GLfloat gAngle = 0.0f;
GLfloat gFlame = 0.0f;
int gPhase = 0;
float gLandY = 0.5f;
GLfloat gMoonAngle = 0.0f;
float gOrbitAngle = 0.0f;
float gOrbitTime = 0.0f;
float gCamZoom = 1.0f;
float gCamTargetZoom = 1.0f;
float gCamX = 0.0f;
float gCamY = 0.0f;
int gCamMode = 0;
float gRocketX = 0.0f;
float gRocketY = 0.0f;

#define STAR_COUNT 120
#define DUST_COUNT 80

// stars
float starPositions[60][2];
float starX[STAR_COUNT], starY[STAR_COUNT], starBlink[STAR_COUNT];
float dustY[DUST_COUNT], dustLife[DUST_COUNT];

bool groupmateStarted = false;


void initGL()
{
    for (int i = 0; i < 60; i++) {
        starPositions[i][0] = -1.0f + (rand() % 200) / 100.0f;
        starPositions[i][1] = (rand() % 100) / 100.0f;
    }

    // groupmate init
    for(int i=0;i<STAR_COUNT;i++){
        starX[i]=((rand()%200)/100.0f)-1;
        starY[i]=((rand()%200)/100.0f)-1;
        starBlink[i]=rand()%100;
    }
    for(int i=0;i<DUST_COUNT;i++){
        dustY[i]=0;
        dustLife[i]=rand()%100;
    }
}


//basic shape
void drawFilledCircle(float centerX, float centerY, float radius)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);
    for (int i = 0; i <= 100; i++) {
        float angle = 2 * PI * i / 100;
        glVertex2f(centerX + radius * cos(angle),
                   centerY + radius * sin(angle));
    }
    glEnd();
}

void drawFilledOval(float centerX, float centerY, float radiusX, float radiusY)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);
    for (int i = 0; i <= 100; i++) {
        float angle = 2 * PI * i / 100;
        glVertex2f(centerX + radiusX * cos(angle),
                   centerY + radiusY * sin(angle));
    }
    glEnd();
}

void drawSkinSpot(float x, float y, float radius)
{
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 20; i++) {
        float angle = 2 * PI * i / 20;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

void drawSimpleCircle(float x, float y, float radius)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 40; i++) {
        float angle = 2 * PI * i / 40.0f;
        glVertex2f(x + radius * cosf(angle), y + radius * sinf(angle));
    }
    glEnd();
}

void drawFilledTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}



//bezier

Point2D getBezierPoint(Point2D startPt, Point2D controlPt, Point2D endPt, float t)
{
    Point2D result;
    result.x = (1 - t)*(1 - t)*startPt.x + 2*(1 - t)*t*controlPt.x + t*t*endPt.x;
    result.y = (1 - t)*(1 - t)*startPt.y + 2*(1 - t)*t*controlPt.y + t*t*endPt.y;
    return result;
}

// leg bez
void drawBezierStrip(Point2D startPt, Point2D controlPt, Point2D endPt, float halfWidth)
{
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= 40; i++) {
        float t = i / 40.0f;
        Point2D p = getBezierPoint(startPt, controlPt, endPt, t);
        glVertex2f(p.x - halfWidth, p.y);
        glVertex2f(p.x + halfWidth, p.y);
    }
    glEnd();
}

//sky draw

void drawSky()
{

    if (!isNight)
        glColor3f(0.70f, 0.85f, 0.95f);
    else
        glColor3f(0.03f, 0.03f, 0.12f);

    glBegin(GL_QUADS);
    glVertex2f(-1, 0);
    glVertex2f( 1, 0);
    glVertex2f( 1, 1);
    glVertex2f(-1, 1);
    glEnd();

    if (!isNight) {
        glColor3f(1.0f, 0.95f, 0.6f);
        drawFilledCircle(-0.75f, 0.8f, 0.08f);
    } else {
        glColor3f(0.95f, 0.95f, 0.85f);
        drawFilledCircle(0.75f, 0.8f, 0.08f);
        glColor3f(1, 1, 1);
        glPointSize(1.5f);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glBegin(GL_POINTS);
        for (int i = 0; i < 60; i++)
            glVertex2f(starPositions[i][0], starPositions[i][1]);
        glEnd();
        glDisable(GL_POINT_SMOOTH);
    }
}
//cloud
void drawOneCloud(float posX, float posY)
{
    glColor3f(1, 1, 1);
    drawFilledCircle(posX,        posY,        0.06f);
    drawFilledCircle(posX + 0.07f, posY + 0.03f, 0.08f);
    drawFilledCircle(posX + 0.14f, posY,        0.06f);
}

//landscape

void drawBackgroundHills()
{
    glColor3f(0.65f, 0.82f, 0.55f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1,    0);
    glVertex2f(-0.6f, 0.45f);
    glVertex2f(-0.2f, 0);

    glVertex2f(-0.2f, 0);
    glVertex2f( 0.4f, 0.55f);
    glVertex2f( 1,    0);
    glEnd();
}

void drawMiddleHills()
{
    if (!isNight)
        glColor3f(0.45f, 0.70f, 0.40f);
    else
        glColor3f(0.25f, 0.45f, 0.30f);

    glBegin(GL_TRIANGLES);
    glVertex2f(-1,    -0.05f);
    glVertex2f(-0.3f,  0.25f);
    glVertex2f( 0.3f, -0.05f);

    glVertex2f( 0.2f, -0.05f);
    glVertex2f( 0.8f,  0.30f);
    glVertex2f( 1.2f, -0.05f);
    glEnd();
}

void drawGrassGround()
{
    if (!isNight)
        glColor3f(0.30f, 0.65f, 0.30f);
    else
        glColor3f(0.15f, 0.40f, 0.20f);

    glBegin(GL_QUADS);
    glVertex2f(-1,  0);
    glVertex2f( 1,  0);
    glVertex2f( 1, -1);
    glVertex2f(-1, -1);
    glEnd();
}

void drawRiver()
{
    glColor3f(0.2f, 0.5f, 0.75f);

    Point2D riverTop    = {0.25f,  0.0f};
    Point2D riverMiddle = {0.55f, -0.35f};
    Point2D riverBottom = {0.30f, -1.0f};

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= 40; i++) {
        float t = i / 40.0f;
        Point2D p = getBezierPoint(riverTop, riverMiddle, riverBottom, t);
        float width = 0.06f + 0.18f * t;
        glVertex2f(p.x - width, p.y);
        glVertex2f(p.x + width, p.y);
    }
    glEnd();
}

 //tree

void setLeafColor(int treeStyle)
{
    if (isNight)
        glColor3f(0.10f, 0.30f, 0.10f);
    else if (treeStyle == 0)
        glColor3f(0.15f, 0.55f, 0.20f);
    else
        glColor3f(0.18f, 0.62f, 0.22f);
}

void setLeafHighlightColor(int treeStyle)
{
    if (isNight)
        glColor3f(0.12f, 0.35f, 0.12f);
    else if (treeStyle == 0)
        glColor3f(0.22f, 0.68f, 0.28f);
    else
        glColor3f(0.28f, 0.75f, 0.32f);
}

void setBarkColor(int treeStyle)
{
    if (isNight)
        glColor3f(0.20f, 0.12f, 0.06f);
    else
        glColor3f(0.42f, 0.22f, 0.10f);
}

void drawRoundLeafTree(float posX, float posY, float size, int treeStyle)
{
    float windSway = 0.002f * sin(windTimer + posX * 10.0f) + 0.001f * cos(windTimer * 1.5f);
    if (treeStyle == 0) windSway = 0.0f;

    setLeafColor(treeStyle);
    drawFilledCircle(posX - 0.10f*size + windSway, posY + 0.35f*size, 0.13f*size);

    setLeafHighlightColor(treeStyle);
    drawFilledCircle(posX - 0.01f*size + windSway, posY + 0.56f*size, 0.13f*size);
    drawFilledCircle(posX + 0.10f*size + windSway, posY + 0.49f*size, 0.10f*size);
    drawFilledCircle(posX - 0.08f*size + windSway, posY + 0.38f*size, 0.13f*size);
    drawFilledCircle(posX + 0.10f*size + windSway, posY + 0.35f*size, 0.13f*size);

    setBarkColor(treeStyle);
    drawFilledTriangle(posX, posY + 0.5f*size,
                       posX - 0.02f*size, posY,
                       posX + 0.02f*size, posY);

    float midY = posY + 0.2f*size;
    drawFilledTriangle(posX, midY + 0.03f*size,
                       posX, midY - 0.03f*size,
                       posX + 0.08f*size, posY + 0.35f*size);
    drawFilledTriangle(posX, midY + 0.03f*size,
                       posX, midY - 0.03f*size,
                       posX - 0.08f*size, posY + 0.33f*size);
}

void drawBushyTree(float posX, float posY, float size, int treeStyle)
{
    float windSway = 0.002f * sin(windTimer + posX * 10.0f) + 0.001f * cos(windTimer * 1.5f);
    if (treeStyle == 0) windSway = 0.0f;

    setLeafColor(treeStyle);
    drawFilledCircle(posX - 0.02f*size + windSway, posY + 0.52f*size, 0.18f*size);
    drawFilledCircle(posX + 0.10f*size + windSway, posY + 0.33f*size, 0.15f*size);
    drawFilledCircle(posX - 0.12f*size + windSway, posY + 0.36f*size, 0.16f*size);
    drawFilledCircle(posX - 0.22f*size + windSway, posY + 0.20f*size, 0.12f*size);

    setLeafHighlightColor(treeStyle);
    drawFilledCircle(posX + 0.02f*size + windSway, posY + 0.56f*size, 0.18f*size);
    drawFilledCircle(posX + 0.14f*size + windSway, posY + 0.37f*size, 0.15f*size);
    drawFilledCircle(posX - 0.08f*size + windSway, posY + 0.40f*size, 0.16f*size);
    drawFilledCircle(posX - 0.18f*size + windSway, posY + 0.24f*size, 0.12f*size);

    setBarkColor(treeStyle);
    drawFilledTriangle(posX, posY + 0.60f*size,
                       posX - 0.03f*size, posY,
                       posX + 0.03f*size, posY);
    drawFilledTriangle(posX, posY + 0.20f*size,
                       posX, posY + 0.12f*size,
                       posX - 0.19f*size, posY + 0.26f*size);
}



void drawFlyingBird()
{
    if (isNight) return;

    glPushMatrix();
    glTranslatef(birdPositionX, 0.78f, 0);

    float wingFlap = sin(birdWingAngle) * 0.04f;

    glColor3f(0.12f, 0.08f, 0.03f);
    drawFilledOval(0, 0, 0.025f, 0.015f);

    // left
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.01f,  0);
    glVertex2f(-0.12f,  0.03f + wingFlap);
    glVertex2f(-0.06f, -0.03f);
    glEnd();

    // right
    glBegin(GL_TRIANGLES);
    glVertex2f( 0.01f,  0);
    glVertex2f( 0.12f,  0.03f + wingFlap);
    glVertex2f( 0.06f, -0.03f);
    glEnd();

    glPopMatrix();
}




void idle(int value)
{
    if (transitioning && transitionProgress < 1.0f)
        transitionProgress += 0.003f;

    if (transitionProgress >= 1.0f)
        showPresentWorld = true;

    birdPositionX -= 0.002f;
    if (birdPositionX < -1.2f) birdPositionX = 1.2f;
    birdWingAngle += 0.2f;

    cloud1X += 0.0008f;
    cloud2X += 0.0006f;
    cloud3X += 0.0005f;
    if (cloud1X > 1.2f) cloud1X = -1.2f;
    if (cloud2X > 1.2f) cloud2X = -1.2f;
    if (cloud3X > 1.2f) cloud3X = -1.2f;

    if (eggHatching && eggCrackProgress < 1.0f)
        eggCrackProgress += 0.003f;

    if (eggCrackProgress > 0.75f && babyDinoMoveX < 0.45f) {
        babyDinoMoveX    += 0.002f;
        babyDinoHeadBob   = sinf(babyDinoMoveX * 30.0f) * 0.008f;
        babyDinoLegAngle  = sinf(babyDinoMoveX * 30.0f) * 0.15f;
    }

    windTimer += 0.02f;

    if (!dinoPaused) {
        dinoMoveX    += 0.003f;
        dinoLegAngle  = sinf(dinoMoveX * 40.0f) * 0.22f;
        dinoBodyBounce = sinf(dinoMoveX * 40.0f) * 0.012f;
        if (dinoMoveX >= 0.50f)
            dinoPaused = true;
    } else {
        dinoBodyBounce = sinf(glutGet(GLUT_ELAPSED_TIME) * 0.003f) * 0.005f;
    }
    if (rocketStage == 1) {
        countdownTimer += 0.01f;
        if (countdownTimer >= 1.0f) rocketStage = 2;
    }
    if (rocketStage == 2) {
        smokeFlowTimer += 0.03f;
        countdownTimer += 0.01f;
        if (countdownTimer >= 2.0f) {
            rocketStage = 3;
            rocketSupportArmX -= 0.005f;
        }
    }

    if (rocketStage == 3) {
        smokeFlowTimer += 0.05f;
        if (rocketSupportArmX > -0.25f) rocketSupportArmX -= 0.008f;
        rocketPositionY += 0.005f;
        if (spaceBlendAmount < 1.0f) spaceBlendAmount += 0.003f;
        if (rocketPositionY > 2.0f) {
            rocketStage = 4;
            groupmateStarted = true;  // trigger groupmate scene
            gLandY = 0.0f;
        }
    }

    // groupmate animation update
    if(groupmateStarted){
        if(gPhase==0){
            gLandY += 0.003f;
            if(gLandY >= 1.0f) gPhase=1;
        }
        else if(gPhase==1){
            gLandY -= 0.001f;
            if(gLandY <= 0.0f){ gLandY=0.0f; gPhase=2; }
        }
        else if(gPhase==2){
            gOrbitAngle += 0.5f;
            gOrbitTime += 0.01f;
            if(gOrbitTime > 6.0f) gPhase=3;
        }
        else if(gPhase==3){
            gLandY -= 0.001f;
            if(gLandY <= -0.2f) gLandY=-0.2f;
        }
        gAngle += 0.05f;
        gFlame += 0.08f;
        gMoonAngle += 0.0005f;
        gCamZoom += (gCamTargetZoom - gCamZoom)*0.05f;
        for(int i=0;i<STAR_COUNT;i++)
            starBlink[i] += 0.03f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, idle, 0);
}



void keyboard(unsigned char key, int x, int y)
{
    if (key == 'n' || key == 'N') isNight = true;
    if (key == 'd' || key == 'D') isNight = false;
    if (key == 'e' || key == 'E') eggHatching = true;
    if (key == 'r' || key == 'R') {
        if(!groupmateStarted)
            rocketStage = 1;
    }
    if (key == 't' || key == 'T') {
        transitioning    = true;
        showPresentWorld = true;
    }

    // groupmate camera controls
    if(groupmateStarted){
        if(key=='c' || key=='C'){ gCamMode=1; gCamTargetZoom=2.0f; }
        if(key=='m' || key=='M'){ gCamMode=2; gCamTargetZoom=3.0f; }
        if(key=='r' || key=='R'){ gCamMode=0; gCamTargetZoom=1.0f; gCamX=0; gCamY=0; }
    }
}



void drawBackSpike(float baseX, float baseY, float spikeHeight, float spikeWidth)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(baseX, baseY + spikeHeight);
    for (int i = 0; i <= 30; i++) {
        float t     = (float)i / 30.0f;
        float angle = PI + PI * t;
        glVertex2f(baseX + spikeWidth * cosf(angle),
                   baseY + spikeWidth * 0.5f * sinf(angle));
    }
    glEnd();
}

void drawSingleDinosaur(float posX, float posY, float scale)
{
    glPushMatrix();
    glTranslatef(posX, posY, 0.0f);
    glScalef(scale, scale, 1.0f);

    float lightGreen[] = { 0.45f, 0.85f, 0.25f };
    float darkGreen[]  = { 0.25f, 0.55f, 0.10f };
    float bellyColor[] = { 0.75f, 0.95f, 0.55f };

    // tail
    for (float t = 0; t <= 1; t += 0.01f) {
        Point2D p = getBezierPoint({-0.65f, 0.15f}, {-1.2f, 0.15f}, {-1.6f, -0.3f}, t);
        glColor3f(0.45f - t*0.1f, 0.85f - t*0.2f, 0.25f);
        drawFilledCircle(p.x, p.y, 0.18f * (1.0f - t));
    }
//back leg
    glColor3fv(darkGreen);
    drawBezierStrip({-0.35f, -0.10f},
                    {-0.32f + dinoLegAngle*0.4f, -0.55f},
                    {-0.30f + dinoLegAngle*0.6f, -1.00f}, 0.09f);
    drawBezierStrip({ 0.28f, -0.10f},
                    { 0.32f - dinoLegAngle*0.4f, -0.55f},
                    { 0.30f - dinoLegAngle*0.6f, -1.00f}, 0.09f);

    drawFilledOval(-0.30f + dinoLegAngle*0.6f, -1.00f, 0.15f, 0.065f);
    drawFilledOval( 0.30f - dinoLegAngle*0.6f, -1.00f, 0.15f, 0.065f);

//body
    glColor3fv(lightGreen);
    for (float rx = 0.60f; rx <= 0.68f; rx += 0.01f) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.05f);
        for (int i = 0; i <= 100; i++) {
            float th = 2 * PI * i / 100.0f;
            glVertex2f(rx * cosf(th), 0.05f + 0.42f * sinf(th));
        }
        glEnd();
    }

    // belly
    glColor3fv(bellyColor);
    drawFilledOval(0.05f, -0.08f, 0.36f, 0.26f);

    // back
    glColor3f(0.20f, 0.50f, 0.10f);
    drawBackSpike(-0.38f, 0.43f, 0.08f,  0.030f);
    drawBackSpike(-0.22f, 0.45f, 0.11f,  0.035f);
    drawBackSpike(-0.06f, 0.46f, 0.14f,  0.038f);
    drawBackSpike( 0.10f, 0.46f, 0.13f,  0.036f);
    drawBackSpike( 0.25f, 0.44f, 0.10f,  0.032f);
    drawBackSpike( 0.38f, 0.42f, 0.07f,  0.028f);

    glColor3f(0.15f, 0.42f, 0.08f);
    drawBackSpike(-0.38f, 0.43f, 0.04f,  0.028f);
    drawBackSpike(-0.22f, 0.45f, 0.05f,  0.033f);
    drawBackSpike(-0.06f, 0.46f, 0.06f,  0.036f);
    drawBackSpike( 0.10f, 0.46f, 0.055f, 0.034f);
    drawBackSpike( 0.25f, 0.44f, 0.045f, 0.030f);
    drawBackSpike( 0.38f, 0.42f, 0.035f, 0.026f);

    // front leg
    glColor3fv(darkGreen);
    drawBezierStrip({-0.45f, -0.20f},
                    {-0.47f - dinoLegAngle*0.3f, -0.55f},
                    {-0.46f - dinoLegAngle*0.5f, -0.90f}, 0.075f);
    drawBezierStrip({ 0.12f, -0.20f},
                    { 0.14f + dinoLegAngle*0.3f, -0.55f},
                    { 0.13f + dinoLegAngle*0.5f, -0.90f}, 0.075f);

    drawFilledOval(-0.46f - dinoLegAngle*0.5f, -0.90f, 0.12f, 0.05f);
    drawFilledOval( 0.13f + dinoLegAngle*0.5f, -0.90f, 0.12f, 0.05f);

    // neck
    glColor3fv(lightGreen);
    drawBezierStrip({0.45f, 0.15f}, {0.55f, 0.70f}, {0.75f, 0.95f}, 0.13f);

    // head
    glColor3fv(lightGreen);
    drawFilledOval(0.80f, 1.00f, 0.20f, 0.15f);

    // Mouth
    glColor3fv(lightGreen);
    drawFilledOval(0.98f, 0.94f, 0.14f, 0.08f);
    glColor3fv(darkGreen);
    drawFilledOval(0.98f, 0.89f, 0.13f, 0.045f);

    // teeth
    glColor3f(0.95f, 0.95f, 0.88f);
    for (int i = 0; i < 3; i++) {
        float tx = 0.88f + i * 0.07f;
        glBegin(GL_TRIANGLES);
        glVertex2f(tx,           0.92f);
        glVertex2f(tx + 0.038f,  0.92f);
        glVertex2f(tx + 0.019f,  0.83f);
        glEnd();
    }

    // nose circle
    glColor3f(0.18f, 0.45f, 0.10f);
    drawFilledCircle(1.04f, 0.97f, 0.018f);

    // eye
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledCircle(0.88f, 1.05f, 0.062f);
    glColor3f(0.05f, 0.05f, 0.05f);
    drawFilledCircle(0.90f, 1.04f, 0.030f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawFilledCircle(0.915f, 1.055f, 0.011f);

    // body texture
    glColor3f(0.32f, 0.74f, 0.18f);
    drawSkinSpot(-0.15f,  0.20f, 0.045f);
    drawSkinSpot( 0.12f,  0.14f, 0.040f);
    drawSkinSpot(-0.02f, -0.05f, 0.038f);
    drawSkinSpot( 0.28f,  0.30f, 0.042f);

    glPopMatrix();
}

void drawDinosaurGroup()
{
    glPushMatrix();
    glTranslatef(dinoMoveX, dinoBodyBounce, 0.0f);

    drawSingleDinosaur(-0.70f, -0.22f, 0.24f);
    drawSingleDinosaur(-0.38f, -0.22f, 0.18f);
    drawSingleDinosaur(-0.62f, -0.45f, 0.12f);
    drawSingleDinosaur(-0.38f, -0.45f, 0.075f);

    glPopMatrix();
}


void drawCrackingEgg(float centerX, float centerY, float size)
{
    if (eggCrackProgress >= 1.0f) return;

    // egg shadow
    glColor3f(0.18f, 0.48f, 0.13f);
    drawFilledOval(centerX, centerY - size*0.52f, size*0.42f, size*0.06f);

    if (eggCrackProgress < 0.75f) {

        float shake = sinf(eggCrackProgress * 40.0f) * eggCrackProgress * 0.005f;

        glPushMatrix();
        glTranslatef(shake, 0, 0);

        glColor3f(0.95f, 0.93f, 0.85f);
        drawFilledOval(centerX, centerY, size*0.38f, size*0.48f);

        if (eggCrackProgress > 0.15f) {
            glColor3f(0.55f, 0.48f, 0.38f);
            glLineWidth(1.8f);

            float crackLength = (eggCrackProgress - 0.15f) * 2.0f;
            if (crackLength > 1.0f) crackLength = 1.0f;

            glBegin(GL_LINE_STRIP);
            glVertex2f(centerX,            centerY + size*0.40f);
            glVertex2f(centerX + size*0.07f, centerY + size*(0.40f - 0.20f*crackLength));
            glVertex2f(centerX + size*0.02f, centerY + size*(0.40f - 0.35f*crackLength));
            glVertex2f(centerX + size*0.09f, centerY + size*(0.40f - 0.50f*crackLength));
            glEnd();

            glBegin(GL_LINE_STRIP);
            glVertex2f(centerX,             centerY + size*0.38f);
            glVertex2f(centerX - size*0.08f, centerY + size*(0.38f - 0.18f*crackLength));
            glVertex2f(centerX - size*0.03f, centerY + size*(0.38f - 0.32f*crackLength));
            glEnd();

            if (eggCrackProgress > 0.40f) {
                glBegin(GL_LINE_STRIP);
                glVertex2f(centerX + size*0.20f, centerY + size*0.15f);
                glVertex2f(centerX + size*0.32f, centerY + size*0.05f);
                glEnd();

                glBegin(GL_LINE_STRIP);
                glVertex2f(centerX - size*0.18f, centerY + size*0.10f);
                glVertex2f(centerX - size*0.30f, centerY - size*0.05f);
                glEnd();
            }
        }
        glPopMatrix();

    } else {

        float openAmount = (eggCrackProgress - 0.75f) * 4.0f;
        if (openAmount > 1.0f) openAmount = 1.0f;

        float fallDistance = openAmount * size * 0.6f;
        float leftTilt  = openAmount * 80.0f;
        float rightTilt = openAmount * 75.0f;

        // left
        glPushMatrix();
        glTranslatef(centerX - openAmount*size*0.55f, centerY - fallDistance, 0);
        glRotatef(-leftTilt, 0, 0, 1);
        glColor3f(0.95f, 0.93f, 0.85f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int i = 0; i <= 60; i++) {
            float a = PI/2.0f + PI * i / 60.0f;
            glVertex2f(size*0.38f * cosf(a), size*0.48f * sinf(a));
        }
        glEnd();
        glColor3f(0.55f, 0.48f, 0.38f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(size*0.05f,  size*0.10f);
        glVertex2f(size*0.12f, -size*0.05f);
        glVertex2f(size*0.06f, -size*0.15f);
        glEnd();
        glPopMatrix();

        // right
        glPushMatrix();
        glTranslatef(centerX + openAmount*size*0.50f, centerY - fallDistance*0.85f, 0);
        glRotatef(rightTilt, 0, 0, 1);
        glColor3f(0.92f, 0.90f, 0.80f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int i = 0; i <= 60; i++) {
            float a = -PI/2.0f + PI * i / 60.0f;
            glVertex2f(size*0.38f * cosf(a), size*0.48f * sinf(a));
        }
        glEnd();
        glColor3f(0.55f, 0.48f, 0.38f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_STRIP);
        glVertex2f(-size*0.04f,  size*0.08f);
        glVertex2f(-size*0.10f, -size*0.06f);
        glEnd();
        glPopMatrix();
    }
}


void drawBabyDinosaur(float posX, float posY, float scale)
{
    glPushMatrix();
    glTranslatef(posX, posY + babyDinoHeadBob, 0);
    glScalef(scale, scale, 1.0f);

    float lightGreen[] = {0.50f, 0.90f, 0.28f};
    float darkGreen[]  = {0.28f, 0.58f, 0.12f};
    float bellyColor[] = {0.78f, 0.96f, 0.58f};

    // tail
    for (float t = 0; t <= 1; t += 0.02f) {
        Point2D p = getBezierPoint({-0.65f, 0.15f}, {-1.1f, 0.10f}, {-1.4f, -0.2f}, t);
        glColor3f(0.50f - t*0.1f, 0.90f - t*0.2f, 0.28f);
        drawFilledCircle(p.x, p.y, 0.15f * (1.0f - t));
    }

    // back leg
    glColor3fv(darkGreen);
    drawBezierStrip({-0.35f, -0.10f},
                    {-0.32f + babyDinoLegAngle*0.3f, -0.55f},
                    {-0.30f + babyDinoLegAngle*0.5f, -1.00f}, 0.09f);
    drawBezierStrip({ 0.28f, -0.10f},
                    { 0.32f - babyDinoLegAngle*0.3f, -0.55f},
                    { 0.30f - babyDinoLegAngle*0.5f, -1.00f}, 0.09f);
    drawFilledOval(-0.30f + babyDinoLegAngle*0.5f, -1.00f, 0.13f, 0.055f);
    drawFilledOval( 0.30f - babyDinoLegAngle*0.5f, -1.00f, 0.13f, 0.055f);

    // body
    glColor3fv(lightGreen);
    drawFilledOval(0.0f, 0.05f, 0.62f, 0.40f);
    glColor3fv(bellyColor);
    drawFilledOval(0.05f, -0.08f, 0.34f, 0.24f);

    glColor3f(0.22f, 0.52f, 0.12f);
    drawBackSpike(-0.25f, 0.42f, 0.07f, 0.025f);
    drawBackSpike(-0.08f, 0.44f, 0.09f, 0.028f);
    drawBackSpike( 0.08f, 0.43f, 0.08f, 0.026f);
    drawBackSpike( 0.22f, 0.41f, 0.06f, 0.023f);

    // front leg
    glColor3fv(darkGreen);
    drawBezierStrip({-0.45f, -0.20f},
                    {-0.47f - babyDinoLegAngle*0.2f, -0.55f},
                    {-0.46f - babyDinoLegAngle*0.4f, -0.88f}, 0.07f);
    drawBezierStrip({ 0.12f, -0.20f},
                    { 0.14f + babyDinoLegAngle*0.2f, -0.55f},
                    { 0.13f + babyDinoLegAngle*0.4f, -0.88f}, 0.07f);
    drawFilledOval(-0.46f - babyDinoLegAngle*0.4f, -0.88f, 0.11f, 0.045f);
    drawFilledOval( 0.13f + babyDinoLegAngle*0.4f, -0.88f, 0.11f, 0.045f);

    // neck
    glColor3fv(lightGreen);
    drawBezierStrip({0.45f, 0.15f}, {0.52f, 0.60f}, {0.70f, 0.88f}, 0.12f);

    // head moving
    glPushMatrix();
    glTranslatef(0.75f, 0.95f, 0);
    glRotatef(sinf(babyDinoMoveX * 25.0f) * 8.0f, 0, 0, 1);

    glColor3fv(lightGreen);
    drawFilledOval(0.0f, 0.0f, 0.20f, 0.14f);

    // mouth
    drawFilledOval(0.18f, -0.05f, 0.13f, 0.07f);
    glColor3fv(darkGreen);
    drawFilledOval(0.18f, -0.09f, 0.12f, 0.038f);

    // eye
    glColor3f(1, 1, 1);
    drawFilledCircle(0.08f, 0.06f, 0.055f);
    glColor3f(0.05f, 0.05f, 0.05f);
    drawFilledCircle(0.10f, 0.05f, 0.028f);
    glColor3f(1, 1, 1);
    drawFilledCircle(0.115f, 0.065f, 0.010f);

    // nose
    glColor3f(0.18f, 0.45f, 0.10f);
    drawFilledCircle(0.24f, -0.02f, 0.015f);

    glPopMatrix();
    glPopMatrix();
}


void drawEggHatchingScene()
{
    float eggX = -0.88f;
    float eggY = -0.55f;
    float eggSize = 0.13f;

    drawCrackingEgg(eggX, eggY, eggSize);

    if (eggCrackProgress > 0.75f) {
        float babyX = eggX - 0.02f + babyDinoMoveX;
        drawBabyDinosaur(babyX, eggY - 0.06f, 0.042f);
    }
}



void drawPrehistoricWorld()
{
    drawSky();

    if (!isNight) {
        drawOneCloud(cloud1X, 0.80f);
        drawOneCloud(cloud2X, 0.83f);
        drawOneCloud(cloud3X, 0.78f);
    }

    drawBackgroundHills();
    drawMiddleHills();
    drawGrassGround();
    drawRiver();

    int treeStyle = 1;

    // back trees
    drawRoundLeafTree(-0.90f,  0.05f, 0.40f, treeStyle);
    drawRoundLeafTree(-0.65f,  0.02f, 0.35f, treeStyle);
    drawRoundLeafTree(-0.40f,  0.04f, 0.38f, treeStyle);
    drawRoundLeafTree( 0.75f,  0.02f, 0.38f, treeStyle);
    drawRoundLeafTree( 0.92f,  0.04f, 0.33f, treeStyle);

    // middle trees
    drawRoundLeafTree(-0.88f, -0.18f, 0.75f, treeStyle);
    drawRoundLeafTree(-0.60f, -0.20f, 0.65f, treeStyle);
    drawRoundLeafTree( 0.45f, -0.22f, 0.58f, treeStyle);
    drawRoundLeafTree( 0.70f, -0.22f, 0.70f, treeStyle);

    // front trees
    drawRoundLeafTree(-0.95f, -0.35f, 0.90f, treeStyle);
    drawRoundLeafTree( 0.62f, -0.35f, 0.88f, treeStyle);

    drawDinosaurGroup();
    drawEggHatchingScene();
    drawFlyingBird();
}



void drawRocketSmoke()
{
    for (int i = 0; i < 6; i++) {
        float smokeY = -0.78f - fmodf(smokeFlowTimer + i*0.12f, 0.6f);
        float brightness = 0.75f + i * 0.05f;
        glColor3f(brightness, brightness, brightness);
        drawSimpleCircle(-0.03f, smokeY + rocketPositionY + 0.62f, 0.05f + i*0.01f);
        drawSimpleCircle( 0.03f, smokeY + rocketPositionY + 0.60f, 0.06f + i*0.01f);
    }
}

void drawRocket()
{
    glPushMatrix();
    glTranslatef(0.0f, rocketPositionY, 0.0f);

//rocket smoke
    if (rocketStage == 3 && rocketPositionY > -0.30f) {
        for (int i = 1; i <= 8; i++) {
            float trailY    = -0.62f - i * 0.12f;
            float intensity = 1.0f / i;
            float size      = 0.04f - i * 0.003f;
            if (size < 0.005f) size = 0.005f;
            glColor3f(intensity, intensity * 0.5f, 0.0f);
            drawSimpleCircle(0.0f, trailY, size);
        }
    }

    // body
    glColor3f(0.96f, 0.96f, 0.96f);
    glBegin(GL_QUADS);
    glVertex2f(-0.06f, -0.62f); glVertex2f(0.06f, -0.62f);
    glVertex2f( 0.06f,  0.30f); glVertex2f(-0.06f,  0.30f);
    glEnd();
//nose
    glColor3f(0.80f, 0.80f, 0.80f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.06f, 0.30f);
    glVertex2f( 0.06f, 0.30f);
    glVertex2f( 0.00f, 0.50f);
    glEnd();

    // rep st
    glColor3f(0.85f, 0.15f, 0.10f);
    glBegin(GL_QUADS);
    glVertex2f(-0.06f, 0.05f); glVertex2f(0.06f, 0.05f);
    glVertex2f( 0.06f, 0.10f); glVertex2f(-0.06f, 0.10f);
    glEnd();

    //blue window
    glColor3f(0.3f, 0.5f, 0.9f);
    drawSimpleCircle(0.0f, 0.18f, 0.03f);

    // fins
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.06f, -0.62f); glVertex2f(-0.14f, -0.72f); glVertex2f(-0.06f, -0.42f);
    glVertex2f( 0.06f, -0.62f); glVertex2f( 0.14f, -0.72f); glVertex2f( 0.06f, -0.42f);
    glEnd();

    // fire and smoke
    if (rocketStage >= 2) {
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.4f, 0.0f);
        glVertex2f(-0.05f, -0.62f);
        glVertex2f( 0.05f, -0.62f);
        glVertex2f( 0.0f,  -0.90f - (rand()%10)/200.0f);

        glColor3f(1.0f, 1.0f, 0.2f);
        glVertex2f(-0.025f, -0.62f);
        glVertex2f( 0.025f, -0.62f);
        glVertex2f( 0.0f,   -0.78f);
        glEnd();

        drawRocketSmoke();
    }

    glPopMatrix();
}

void drawLaunchTower()
{
    glPushMatrix();
    glTranslatef(rocketSupportArmX, 0.0f, 0.0f);

    glColor3f(0.50f, 0.50f, 0.52f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (float y = -0.82f; y < 0.60f; y += 0.08f) {
        glVertex2f(-0.20f, y); glVertex2f(-0.20f, y+0.08f);
        glVertex2f(-0.06f, y); glVertex2f(-0.06f, y+0.08f);
        glVertex2f(-0.20f, y); glVertex2f(-0.06f, y+0.08f);
        glVertex2f(-0.06f, y); glVertex2f(-0.20f, y+0.08f);
    }
    glEnd();

  //support arm
    glColor3f(0.70f, 0.18f, 0.10f);
    glBegin(GL_QUADS);
    glVertex2f(-0.06f,  0.30f); glVertex2f(0.08f,  0.30f);
    glVertex2f( 0.08f,  0.26f); glVertex2f(-0.06f,  0.26f);
    glVertex2f(-0.06f,  0.02f); glVertex2f(0.10f,  0.02f);
    glVertex2f( 0.10f, -0.02f); glVertex2f(-0.06f, -0.02f);
    glEnd();

    glPopMatrix();
}

void drawLaunchPad()
{
    glColor3f(0.40f, 0.40f, 0.42f);
    glBegin(GL_QUADS);
    glVertex2f(-0.18f, -0.82f); glVertex2f(0.18f, -0.82f);
    glVertex2f( 0.18f, -0.90f); glVertex2f(-0.18f, -0.90f);
    glEnd();


    glColor3f(0.35f, 0.35f, 0.38f);
    glBegin(GL_QUADS);
    glVertex2f(-0.14f, -0.90f); glVertex2f(-0.10f, -0.90f);
    glVertex2f(-0.10f, -1.00f); glVertex2f(-0.14f, -1.00f);
    glVertex2f( 0.10f, -0.90f); glVertex2f( 0.14f, -0.90f);
    glVertex2f( 0.14f, -1.00f); glVertex2f( 0.10f, -1.00f);
    glEnd();
}


void drawCityBuilding(float posX, float height, float width,
                      float colorR, float colorG, float colorB)
{

    glColor3f(colorR, colorG, colorB);
    glBegin(GL_QUADS);
    glVertex2f(posX,       -1.0f);
    glVertex2f(posX+width, -1.0f);
    glVertex2f(posX+width, -1.0f+height);
    glVertex2f(posX,       -1.0f+height);
    glEnd();

    glColor3f(0.85f, 0.90f, 0.50f);
    float windowWidth  = width * 0.15f;
    float windowHeight = 0.06f;
    for (float wy = -0.85f; wy < -1.0f+height-0.05f; wy += 0.10f)
        for (float wx = posX+0.04f; wx < posX+width-0.04f; wx += width*0.28f) {
            glBegin(GL_QUADS);
            glVertex2f(wx,             wy);
            glVertex2f(wx+windowWidth, wy);
            glVertex2f(wx+windowWidth, wy+windowHeight);
            glVertex2f(wx,             wy+windowHeight);
            glEnd();
        }
}


void drawCityTree(float posX, float posY)
{

    glColor3f(0.35f, 0.20f, 0.08f);
    glBegin(GL_QUADS);
    glVertex2f(posX,         posY);
    glVertex2f(posX+0.025f,  posY);
    glVertex2f(posX+0.025f,  posY+0.18f);
    glVertex2f(posX,         posY+0.18f);
    glEnd();

   //leaf
    glColor3f(0.15f, 0.55f, 0.18f);
    glBegin(GL_TRIANGLES);
    glVertex2f(posX-0.06f,   posY+0.12f);
    glVertex2f(posX+0.085f,  posY+0.12f);
    glVertex2f(posX+0.012f,  posY+0.28f);
    glEnd();

    glColor3f(0.12f, 0.48f, 0.15f);
    glBegin(GL_TRIANGLES);
    glVertex2f(posX-0.05f,   posY+0.20f);
    glVertex2f(posX+0.075f,  posY+0.20f);
    glVertex2f(posX+0.012f,  posY+0.34f);
    glEnd();

    glColor3f(0.10f, 0.42f, 0.12f);
    glBegin(GL_TRIANGLES);
    glVertex2f(posX-0.04f,   posY+0.27f);
    glVertex2f(posX+0.065f,  posY+0.27f);
    glVertex2f(posX+0.012f,  posY+0.40f);
    glEnd();
}


void drawSmallPerson(float posX, float posY, float size)
{

    glColor3f(0.20f, 0.40f, 0.80f);
    glBegin(GL_QUADS);
    glVertex2f(posX-0.012f*size, posY);
    glVertex2f(posX+0.012f*size, posY);
    glVertex2f(posX+0.012f*size, posY+0.055f*size);
    glVertex2f(posX-0.012f*size, posY+0.055f*size);
    glEnd();

    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_LINES);
    glVertex2f(posX, posY); glVertex2f(posX-0.010f*size, posY-0.045f*size);
    glVertex2f(posX, posY); glVertex2f(posX+0.010f*size, posY-0.045f*size);
    glEnd();

    glColor3f(0.20f, 0.40f, 0.80f);
    glBegin(GL_LINES);
    glVertex2f(posX-0.012f*size, posY+0.04f*size); glVertex2f(posX-0.025f*size, posY+0.015f*size);
    glVertex2f(posX+0.012f*size, posY+0.04f*size); glVertex2f(posX+0.025f*size, posY+0.015f*size);
    glEnd();


    glColor3f(0.90f, 0.72f, 0.55f);
    drawSimpleCircle(posX, posY+0.072f*size, 0.018f*size);
}


void drawPresentWorld()
{
    float cityMoveDown = 0.0f;
    if (rocketStage == 3)
        cityMoveDown = (rocketPositionY + 0.62f) * 0.25f;
    if (cityMoveDown > 0.4f) cityMoveDown = 0.4f;
//sky
    if (!isNight) {
        if (spaceBlendAmount < 0.3f) {
            float fade = 1.0f - (spaceBlendAmount / 0.3f);
            glBegin(GL_QUADS);
            glColor3f(0.70f*fade, 0.60f*fade, 0.85f*fade);
            glVertex2f(-1, 1); glVertex2f(1, 1);
            glColor3f(0.98f*fade, 0.78f*fade, 0.60f*fade);
            glVertex2f(1, 0); glVertex2f(-1, 0);
            glEnd();
            glBegin(GL_QUADS);
            glColor3f(0.98f*fade, 0.78f*fade, 0.60f*fade);
            glVertex2f(-1, 0); glVertex2f(1, 0);
            glColor3f(0.99f*fade, 0.88f*fade, 0.65f*fade);
            glVertex2f(1, -1); glVertex2f(-1, -1);
            glEnd();
        } else {
            float t = (spaceBlendAmount - 0.3f) / 0.7f;
            glColor3f(0.08f*t, 0.10f*t, 0.18f*t + 0.02f);
            glBegin(GL_QUADS);
            glVertex2f(-1,-1); glVertex2f(1,-1);
            glVertex2f( 1, 1); glVertex2f(-1, 1);
            glEnd();
        }
    } else {
        float sr = 0.08f*(1.0f-spaceBlendAmount);
        float sg = 0.10f*(1.0f-spaceBlendAmount);
        float sb = 0.18f*(1.0f-spaceBlendAmount) + 0.02f*spaceBlendAmount;
        glColor3f(sr, sg, sb);
        glBegin(GL_QUADS);
        glVertex2f(-1,-1); glVertex2f(1,-1);
        glVertex2f( 1, 1); glVertex2f(-1, 1);
        glEnd();
    }

    //sun
    if (!isNight && spaceBlendAmount < 0.1f) {
        glColor3f(1.0f, 0.75f, 0.40f);
        drawFilledCircle(0.72f, 0.15f, 0.10f);
        glColor3f(1.0f, 0.55f, 0.15f);
        drawFilledCircle(0.72f, 0.15f, 0.07f);
    }

    // moon
    if (isNight) {
        glColor3f(0.92f, 0.92f, 0.80f);
        drawSimpleCircle(-0.75f, 0.75f, 0.07f + spaceBlendAmount*0.03f);
    }

    //stars
    if (isNight || spaceBlendAmount > 0.5f) {
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glPointSize(1.5f);
        float starBrightness = isNight ? 1.0f : (spaceBlendAmount - 0.5f) * 2.0f;
        glColor3f(starBrightness, starBrightness, starBrightness);
        glBegin(GL_POINTS);
        for (int i = 0; i < 60; i++)
            glVertex2f(starPositions[i][0], starPositions[i][1]);
        glEnd();
        glDisable(GL_POINT_SMOOTH);
    }

 //cloud fade
    if (!isNight && spaceBlendAmount < 0.3f) {
        float fade = 1.0f - (spaceBlendAmount / 0.3f);
        glColor3f(0.98f*fade, 0.80f*fade, 0.70f*fade);
        drawFilledOval(cloud1X,       0.60f, 0.06f, 0.035f);
        drawFilledOval(cloud1X+0.07f, 0.63f, 0.08f, 0.045f);
        drawFilledOval(cloud1X+0.14f, 0.60f, 0.06f, 0.035f);

        glColor3f(0.96f*fade, 0.75f*fade, 0.65f*fade);
        drawFilledOval(cloud2X,       0.70f, 0.06f, 0.035f);
        drawFilledOval(cloud2X+0.07f, 0.73f, 0.08f, 0.045f);
        drawFilledOval(cloud2X+0.14f, 0.70f, 0.06f, 0.035f);

        glColor3f(0.95f*fade, 0.72f*fade, 0.60f*fade);
        drawFilledOval(cloud3X,       0.55f, 0.06f, 0.035f);
        drawFilledOval(cloud3X+0.07f, 0.58f, 0.08f, 0.045f);
        drawFilledOval(cloud3X+0.14f, 0.55f, 0.06f, 0.035f);
    }

//bird fade
    if (!isNight && spaceBlendAmount < 0.2f) {
        glPushMatrix();
        glTranslatef(birdPositionX, 0.72f, 0);
        float wingFlap = sin(birdWingAngle) * 0.04f;
        glColor3f(0.15f, 0.08f, 0.05f);
        drawFilledOval(0, 0, 0.025f, 0.015f);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.01f, 0); glVertex2f(-0.12f, 0.03f+wingFlap); glVertex2f(-0.06f,-0.03f);
        glEnd();
        glBegin(GL_TRIANGLES);
        glVertex2f( 0.01f, 0); glVertex2f( 0.12f, 0.03f+wingFlap); glVertex2f( 0.06f,-0.03f);
        glEnd();
        glPopMatrix();
    }

//city
    glPushMatrix();
    glTranslatef(0.0f, -cityMoveDown, 0.0f);

    // building
    drawCityBuilding(-0.95f, 0.55f, 0.16f, 0.28f, 0.22f, 0.20f);
    drawCityBuilding(-0.78f, 0.70f, 0.18f, 0.30f, 0.24f, 0.22f);
    drawCityBuilding(-0.58f, 0.50f, 0.15f, 0.26f, 0.20f, 0.18f);
    drawCityBuilding( 0.45f, 0.65f, 0.18f, 0.28f, 0.22f, 0.20f);
    drawCityBuilding( 0.65f, 0.80f, 0.20f, 0.32f, 0.25f, 0.22f);
    drawCityBuilding( 0.85f, 0.60f, 0.15f, 0.27f, 0.21f, 0.19f);

    drawCityTree(-0.90f, -0.82f);
    drawCityTree(-0.72f, -0.82f);
    drawCityTree( 0.48f, -0.82f);
    drawCityTree( 0.64f, -0.82f);
    drawCityTree( 0.80f, -0.82f);

    // ground
    if (!isNight)
        glColor3f(0.18f, 0.14f, 0.12f);
    else
        glColor3f(0.12f, 0.14f, 0.18f);
    glBegin(GL_QUADS);
    glVertex2f(-1, -1);    glVertex2f(1, -1);
    glVertex2f( 1, -0.82f); glVertex2f(-1, -0.82f);
    glEnd();

    // road
    glColor3f(0.15f, 0.15f, 0.17f);
    glBegin(GL_QUADS);
    glVertex2f(-0.35f, -1.0f);  glVertex2f(0.35f, -1.0f);
    glVertex2f( 0.35f, -0.82f); glVertex2f(-0.35f, -0.82f);
    glEnd();

    glColor3f(0.85f, 0.75f, 0.15f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, -1.0f);
    glVertex2f(0.0f, -0.82f);
    glEnd();

    drawLaunchPad();
    drawLaunchTower();

    drawSmallPerson(0.30f, -0.82f, 1.0f);
    drawSmallPerson(0.38f, -0.82f, 1.0f);
    drawSmallPerson(0.46f, -0.82f, 1.0f);
    drawSmallPerson(0.54f, -0.82f, 1.0f);

    glPopMatrix();

    drawRocket();

}


// GROUPMATE DRAW FUNCTIONS

void gDrawCircle(float r){
    glBegin(GL_POLYGON);
    for(int i=0;i<200;i++){
        float th = 2*3.1416f*i/200;
        glVertex2f(r*cos(th), r*sin(th));
    }
    glEnd();
}

void gDrawStars(){
    for(int i=0;i<STAR_COUNT;i++){
        float b=0.5f+0.5f*sin(starBlink[i]);
        glColor3f(b,b,b);
        glBegin(GL_POINTS);
        glVertex2f(starX[i],starY[i]);
        glEnd();
    }
}

void gDrawDust(float x,float y){
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

void gDrawEarth(){
    glPushMatrix();
    glRotatef(gAngle,0,0,1);
    glColor3f(0.2,0.6,1);
    gDrawCircle(0.4);
    glColor3f(0,0.3,0.8);
    gDrawCircle(0.35);
    glColor3f(0.2,0.7,0.2);
    glBegin(GL_POLYGON);
        glVertex2f(-0.15,0.1);
        glVertex2f(0.1,0.2);
        glVertex2f(0.2,0.0);
        glVertex2f(0.0,-0.1);
    glEnd();
    glPopMatrix();
}

void gDrawMoonSurface(){
    glColor3f(0.75,0.75,0.75);
    gDrawCircle(1.2f);
    for(int i=0;i<20;i++){
        float x = sin(i*7.3f)*0.8f;
        float y = cos(i*5.7f)*0.8f;
        glPushMatrix();
        glTranslatef(x,y,0);
        glColor3f(0.5,0.5,0.5);
        gDrawCircle(0.1f);
        glColor3f(0.3,0.3,0.3);
        gDrawCircle(0.05f);
        glPopMatrix();
    }
}

void gDrawRocket(float x,float y,float rot){
    gRocketX = x;
    gRocketY = y;
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
        gDrawCircle(0.025);
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
    float f=0.12+0.06*sin(gFlame);
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

void drawGroupmateWorld(){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if(gCamMode == 1){
        gCamX = -gRocketX;
        gCamY = -gRocketY;
    }
    else if(gCamMode == 2){
        gCamX = -0.45f;
        gCamY = -0.55f;
    }

    glTranslatef(gCamX, gCamY, 0);
    glScalef(gCamZoom, gCamZoom, 1);

    gDrawStars();

    glPushMatrix();
    glTranslatef(-0.35,-0.45,0);
    gDrawEarth();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.45,0.55,0);
    glColor3f(0.85,0.85,0.85);
    gDrawCircle(0.15);
    glPopMatrix();

    if(gPhase==0){
        float t = gLandY;
        float x0=-0.35f,y0=-0.45f;
        float x1=-0.6f,y1=0.4f;
        float x2=0.2f,y2=0.9f;
        float x3=0.45f,y3=0.55f;
        float x=pow(1-t,3)*x0+3*pow(1-t,2)*t*x1+3*(1-t)*t*t*x2+t*t*t*x3;
        float y=pow(1-t,3)*y0+3*pow(1-t,2)*t*y1+3*(1-t)*t*t*y2+t*t*t*y3;
        float rot=atan2(y3-y0,x3-x0)*180/3.1416;
        gDrawRocket(x,y,rot-90);
    }
    else{
        glPushMatrix();
        glRotatef(gMoonAngle,0,0,1);
        gDrawMoonSurface();
        glPopMatrix();

        if(gPhase==2){
            float r=0.5f;
            float x=r*cos(gOrbitAngle*3.1416/180);
            float y=r*sin(gOrbitAngle*3.1416/180);
            gDrawRocket(x,y,gOrbitAngle+90);
        }
        else if(gPhase==3){
            gDrawRocket(0,gLandY,0);
            gDrawDust(0,gLandY);
        }
        else{
            gDrawRocket(0,gLandY,0);
        }
    }
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!showPresentWorld)
        drawPrehistoricWorld();
    else if (!groupmateStarted)
        drawPresentWorld();
    else
        drawGroupmateWorld();

    glutSwapBuffers();
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1200, 700);
    glutCreateWindow("Prehistoric to Space - OpenGL Graphics Project");

    glClearColor(0, 0, 0, 1);

    srand(time(0));
    initGL();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, idle, 0);

    glutMainLoop();
    return 0;
}
