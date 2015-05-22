//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2013-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni az onInitialization függvényt kivéve, a lefoglalt adat korrekt felszabadítása nélkül
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D,
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi,
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Bálint Ferenc
// Neptun : EGUGHN
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

struct Vector {
    float x, y, z;
    
    Vector( ) {
        x = y = z = 0;
    }
    Vector(float x0, float y0, float z0 = 0) {
        x = x0; y = y0; z = z0;
    }
    Vector operator*(float a) {
        return Vector(x * a, y * a, z * a);
    }
    Vector operator+(const Vector& v) {
        return Vector(x + v.x, y + v.y, z + v.z);
    }
    Vector operator-(const Vector& v) {
        return Vector(x - v.x, y - v.y, z - v.z);
    }
    float operator*(const Vector& v) {
        return (x * v.x + y * v.y + z * v.z);
    }
    Vector operator%(const Vector& v) {
        return Vector(y*v.z-z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    float Length() { return sqrt(x * x + y * y + z * z); }
};

struct Color {
    float r, g, b;
    
    Color( ) {
        r = g = b = 0;
    }
    Color(float r0, float g0, float b0) {
        r = r0; g = g0; b = b0;
    }
    Color operator*(float a) {
        return Color(r * a, g * a, b * a);
    }
    Color operator*(const Color& c) {
        return Color(r * c.r, g * c.g, b * c.b);
    }
    Color operator+(const Color& c) {
        return Color(r + c.r, g + c.g, b + c.b);
    }
};

Color greenColor;
Color redColor;
Color yellowColor;

const int screenWidth = 600;
const int screenHeight = 600;

const float lambda = 1.25;

const float PI = 3.14;

const int birdPoints = 375;

Color image[screenWidth*screenHeight];

float sTime, oTime, dTime = 0;

float greenBirdGlobalX = 0;

Vector oldMousePosition = Vector(0, 0, 0);

int mouseButtonPressed = 0;

float redBirdT = 0;

int isGameOn = 1;

class Bird {
    float centerX, centerY;
    
    Vector skinPoints[birdPoints];
    Vector leftEye[720];
    Vector rightEye[720];
    
    public:
    int isFlying;
    Vector v0;
    Vector dropPoint;
    Vector originalPosition;
    int isFlipped;
    
    Bird() {
        centerX = centerY = 0;
        
        dropPoint = Vector(centerX, centerY, 0);
        
        isFlying = 0;
        
        isFlipped = 0;
        
        for (int i = 0; i < 360; i++) {
            skinPoints[i].x = 0.15 / lambda * cos((float)i);
            skinPoints[i].y = 0.15 * sin((float)i);
            
            skinPoints[i].z = 0;
        }
        
        for (int i = 0; i < 360; i++) {
            leftEye[i].x = 0.3 * skinPoints[i].x + 0.055;
            leftEye[i].y = 0.3 * skinPoints[i].y + 0.04;
        }
        
        for (int i = 360; i < 720; i++) {
            leftEye[i].x = 0.1 * skinPoints[i-360].x + 0.06;
            leftEye[i].y = 0.1 * skinPoints[i-360].y + 0.02;
        }
        
        for (int i = 0; i < 360; i++) {
            rightEye[i].x = 0.3 * skinPoints[i].x - 0.01;
            rightEye[i].y = 0.3 * skinPoints[i].y + 0.04;
        }
        
        for (int i = 360; i < 720; i++) {
            rightEye[i].x = 0.1 * skinPoints[i-360].x;
            rightEye[i].y = 0.1 * skinPoints[i-360].y + 0.02;
        }
        
        skinPoints[360] = Vector(centerX, centerY, 0);
        skinPoints[361] = Vector(centerX - 0.15, centerY + 0.02, 0);
        skinPoints[362] = Vector(centerX - 0.13, centerY + 0.07, 0);
        
        skinPoints[363] = Vector(centerX, centerY, 0);
        skinPoints[364] = Vector(centerX - 0.15, centerY - 0.02, 0);
        skinPoints[365] = Vector(centerX - 0.13, centerY - 0.07, 0);
        
        skinPoints[366] = Vector(centerX + 0.01, centerY - 0.09, 0);
        skinPoints[367] = Vector(centerX + 0.025, centerY - 0.05, 0);
        skinPoints[368] = Vector(centerX + 0.09, centerY - 0.08, 0);
        
        skinPoints[369] = Vector(centerX + 0.02, centerY + 0.11, 0);
        skinPoints[370] = Vector(centerX - 0.04, centerY + 0.12, 0);
        skinPoints[371] = Vector(centerX - 0.04, centerY + 0.1, 0);
        skinPoints[372] = Vector(centerX + 0.02, centerY + 0.09, 0);
        skinPoints[373] = Vector(centerX + 0.08, centerY + 0.1, 0);
        skinPoints[374] = Vector(centerX + 0.08, centerY + 0.12, 0);
    }
    
    float getCenterX() {
        return centerX;
    }
    
    float getCenterY() {
        return centerY;
    }
    
    void drawBird(Color color) {
        glColor3f(0, 0, 0);
        glBegin(GL_TRIANGLES);
        
        glVertex2f(skinPoints[360].x, skinPoints[360].y);
        glVertex2f(skinPoints[361].x, skinPoints[361].y);
        glVertex2f(skinPoints[362].x, skinPoints[362].y);
        
        glVertex2f(skinPoints[363].x, skinPoints[363].y);
        glVertex2f(skinPoints[364].x, skinPoints[364].y);
        glVertex2f(skinPoints[365].x, skinPoints[365].y);
        
        glEnd();
        
        glColor3f(color.r, color.g, color.b);
        glBegin(GL_TRIANGLE_FAN);
        
        glVertex2f(centerX, centerY);
        for (int i = 0; i < 360; i++) {
            glVertex2f(skinPoints[i].x, skinPoints[i].y);
        }
        
        glEnd();
        
        glColor3f(0.87, 0.93, 0.38);
        glBegin(GL_TRIANGLES);
        
        glVertex2f(skinPoints[366].x, skinPoints[366].y);
        glVertex2f(skinPoints[367].x, skinPoints[367].y);
        glVertex2f(skinPoints[368].x, skinPoints[368].y);
        
        glEnd();
        
        glColor3f(1, 1, 1);
        glBegin(GL_TRIANGLE_FAN);
        
        for (int i = 0; i < 360; i++) {
            glVertex2f(leftEye[i].x, leftEye[i].y);
        }
        
        glEnd();
        
        glBegin(GL_TRIANGLE_FAN);
        
        for (int i = 0; i < 360; i++) {
            glVertex2f(rightEye[i].x, rightEye[i].y);
        }
        
        glEnd();
        
        glColor3f(0, 0, 0);
        glBegin(GL_TRIANGLE_FAN);
        
        for (int i = 360; i < 720; i++) {
            glVertex2f(leftEye[i].x, leftEye[i].y);
        }
        
        glEnd();
        
        glBegin(GL_TRIANGLE_FAN);
        
        for (int i = 360; i < 720; i++) {
            glVertex2f(rightEye[i].x, rightEye[i].y);
        }
        
        glEnd();
        
        glBegin(GL_TRIANGLE_FAN);
        
        glVertex2f(skinPoints[369].x, skinPoints[369].y);
        glVertex2f(skinPoints[370].x, skinPoints[370].y);
        glVertex2f(skinPoints[371].x, skinPoints[371].y);
        glVertex2f(skinPoints[372].x, skinPoints[372].y);
        glVertex2f(skinPoints[373].x, skinPoints[373].y);
        glVertex2f(skinPoints[374].x, skinPoints[374].y);
        
        glEnd();
        
    }
    
    void updateBirdPosition(Vector newPosition) {
        float xCoord = newPosition.x - centerX;
        float yCoord = newPosition.y - centerY;
        
        centerX += xCoord;
        centerY += yCoord;
        
        for (int i = 0; i < birdPoints; i++) {
            skinPoints[i].x += xCoord;
            skinPoints[i].y += yCoord;
        }
        
        for (int i = 0; i < 720; i++) {
            leftEye[i].x += xCoord;
            leftEye[i].y += yCoord;
            rightEye[i].x += xCoord;
            rightEye[i].y += yCoord;
        }
    }
    
    void flipBirdVertical () {
        for (int i = 0; i < birdPoints; i++) {
            skinPoints[i].x *= -1;
        }
        for (int i = 0; i < 720; i++) {
            leftEye[i].x *= -1;
            rightEye[i].x *= -1;
        }
    }
    
    void flipBirdHorizontal () {
        if (isFlipped == 0) {
            isFlipped = 1;
        } else {
            isFlipped = 0;
        }
        
        for (int i = 0; i < birdPoints; i++) {
            skinPoints[i].y *= -1;
        }
        for (int i = 0; i < 720; i++) {
            leftEye[i].y *= -1;
            rightEye[i].y *= -1;
        }
    }
    
    void setOriginalPosition(Vector origPos) {
        originalPosition.x = origPos.x;
        originalPosition.y = origPos.y;
    }
    
    void resetBirdsParameters() {
        dropPoint = Vector(0, 0, 0);
        v0 = Vector(0, 0, 0);
        isFlying = 0;
        updateBirdPosition(Vector(0, 0, 0));
    }
};

Bird greenBird;
Bird redBird;

Vector transformCoordinates(float x, float y) {
    return Vector((x - 300) / 300, (y - 300) / 300, 0);
}

float transformOneCoordinate(float a) {
    return (a - 300) / 300;
}

int checkCollision() {
    float xCoordinate = powf((redBird.getCenterX() - greenBird.getCenterX()) * lambda, 2);
    float yCoordinate = powf((redBird.getCenterY() - greenBird.getCenterY()), 2);
    
    if (sqrtf(xCoordinate + yCoordinate) <= 0.15 + 0.15) {
        return 1;
    } else {
        return 0;
    }
}

void onInitialization( ) {
	glViewport(0, 0, screenWidth, screenHeight);
    
    greenColor = Color(0.22, 0.64, 0.12);
    redColor = Color(0.83, 0.31, 0.36);
    yellowColor = Color(0.91, 1, 0);
    
    for(int Y = 0; Y < screenHeight; Y++)
		for(int X = 0; X < screenWidth; X++)
            if (Y < 50) {
                image[Y*screenWidth + X] = Color(0.34, 0.92, 0.38);
            } else {
                image[Y*screenWidth + X] = Color(1, 1, 1);
            }
    
    greenBird.setOriginalPosition(transformCoordinates(500, 325));
    redBird.setOriginalPosition(transformCoordinates(200, 200));
    
    greenBird.flipBirdVertical();
    greenBird.updateBirdPosition(greenBird.originalPosition);
    
    redBird.updateBirdPosition(redBird.originalPosition);
    
}

void onDisplay( ) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawPixels(screenWidth, screenHeight, GL_RGB, GL_FLOAT, image);
    
	glColor3f(0, 0, 1);
    
    greenBird.drawBird(greenColor);
    
    glColor3f(0.35, 0.35, 0.35);
    glBegin(GL_TRIANGLE_STRIP);
    
    glVertex2f(redBird.originalPosition.x + 0.05, transformOneCoordinate(50));
    glVertex2f(redBird.originalPosition.x - 0.05, transformOneCoordinate(50));
    glVertex2f(redBird.originalPosition.x + 0.05, redBird.originalPosition.y - 0.27);
    glVertex2f(redBird.originalPosition.x - 0.05, redBird.originalPosition.y - 0.27);
    glVertex2f(redBird.originalPosition.x + 0.2, redBird.originalPosition.y + 0.05);
    glVertex2f(redBird.originalPosition.x + 0.1, redBird.originalPosition.y + 0.05);
    
    glEnd();
    
    glColor3f(0, 0, 0);
    glBegin(GL_TRIANGLE_STRIP);
    if (redBird.isFlying == 0) {
        glVertex2f(redBird.originalPosition.x + 0.15, redBird.originalPosition.y + 0.05);
        glVertex2f(redBird.originalPosition.x + 0.12, redBird.originalPosition.y - 0.05);
        glVertex2f(redBird.getCenterX() - 0.11, redBird.getCenterY() );
        glVertex2f(redBird.getCenterX() - 0.08, redBird.getCenterY() - (0.15 / lambda));
    } else {
        glVertex2f(redBird.originalPosition.x + 0.15, redBird.originalPosition.y + 0.05);
        glVertex2f(redBird.originalPosition.x + 0.12
                   , redBird.originalPosition.y - 0.05);
        glVertex2f(redBird.originalPosition.x - 0.15, redBird.originalPosition.y + 0.05);
        glVertex2f(redBird.originalPosition.x - 0.15, redBird.originalPosition.y - 0.05);
    }
    
    glEnd();
    
    if (isGameOn == 0) {
        redBird.drawBird(yellowColor);
    } else {
        redBird.drawBird(redColor);
    }
    
    glColor3f(0, 0, 0);
    glBegin(GL_TRIANGLE_STRIP);
    if (redBird.isFlying == 0) {
        glVertex2f(redBird.originalPosition.x - 0.15, redBird.originalPosition.y + 0.05);
        glVertex2f(redBird.originalPosition.x - 0.15, redBird.originalPosition.y - 0.05);
        glVertex2f(redBird.getCenterX() - 0.13, redBird.getCenterY() );
        glVertex2f(redBird.getCenterX() - 0.08, redBird.getCenterY() - (0.15 / lambda));
    }
    
    glEnd();
    
    glColor3f(0.35, 0.35, 0.35);
    glBegin(GL_TRIANGLE_STRIP);
    
    glVertex2f(redBird.originalPosition.x - 0.05, redBird.originalPosition.y - 0.27);
    glVertex2f(redBird.originalPosition.x + 0.05, redBird.originalPosition.y - 0.27);
    glVertex2f(redBird.originalPosition.x - 0.2, redBird.originalPosition.y + 0.05);
    glVertex2f(redBird.originalPosition.x - 0.1, redBird.originalPosition.y + 0.05);
    
    glEnd();
    
    glutSwapBuffers();
    
}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'r') {
        greenBirdGlobalX = 0;
        greenBird.updateBirdPosition(greenBird.originalPosition);
        if (redBird.isFlipped == 1) {
            redBird.updateBirdPosition(Vector(0, 0, 0));
            redBird.flipBirdHorizontal();
            redBird.isFlipped = 0;
        }
        redBird.resetBirdsParameters();
        redBird.updateBirdPosition(redBird.originalPosition);
        redBirdT = 0;
        isGameOn = 1;
    }
    
    glutPostRedisplay( );
    
}

void onKeyboardUp(unsigned char key, int x, int y) {
    
}

void onMouse(int button, int state, int x, int y) {
    if (isGameOn && redBird.isFlying == 0) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            float xDistance = powf((transformOneCoordinate(x) - redBird.getCenterX()), 2) / powf(0.15 / lambda, 2);
            float yDistance = powf((transformOneCoordinate(600 - y) - redBird.getCenterY()), 2) / powf(0.15, 2);
        
        
            if ((xDistance + yDistance) <= 1) {
                oldMousePosition = transformCoordinates(x, y);
                mouseButtonPressed = 1;
            }
        }
    
        if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && mouseButtonPressed == 1) {
            oldMousePosition = Vector(0, 0, 0);
            mouseButtonPressed = 0;
            redBird.dropPoint = Vector(redBird.getCenterX(), redBird.getCenterY(), 0);
            if (redBird.dropPoint.x == redBird.originalPosition.x && redBird.dropPoint.y == redBird.originalPosition.y) {
                redBird.isFlying = 0;
            } else {
                redBird.isFlying = 1;
            }
        }
        
        glutPostRedisplay( );
    }

	
}

void onMouseMotion(int x, int y)
{
    if (isGameOn) {
        if (mouseButtonPressed == 1) {
            redBird.updateBirdPosition(Vector(redBird.getCenterX() + (transformOneCoordinate(x) - oldMousePosition.x), redBird.getCenterY() - (transformOneCoordinate(y) - oldMousePosition.y), 0));
            
            oldMousePosition = transformCoordinates(x, y);
            
            if ((redBird.getCenterX() + 0.15 / lambda) < -1 || (redBird.getCenterX() - 0.1 / lambda) > 1 || (redBird.getCenterY() + 0.15) < -1 || (redBird.getCenterY() - 0.15) > 1) {
                redBird.resetBirdsParameters();
                redBirdT = 0;
                redBird.updateBirdPosition(redBird.originalPosition);
                mouseButtonPressed = 0;
            }
            
            glutPostRedisplay( );
        }
    }
    
}
void onIdle( ) {
    sTime = glutGet(GLUT_ELAPSED_TIME);
    dTime = sTime - oTime;
    oTime = sTime;
    if (isGameOn) {
        if (dTime > 100) {
            for (int i = 100; i < dTime; i += 100) {
                float newPosition = Vector(transformCoordinates(500, 325)).y + 0.6 * sin(greenBirdGlobalX * (PI / 180));
                greenBird.updateBirdPosition(Vector(greenBird.getCenterX(), newPosition, 0));
                
                if (greenBirdGlobalX == 360)
                    greenBirdGlobalX = 0;
                else
                    greenBirdGlobalX += 7;
                
                if (redBird.isFlying) {
                    if (redBird.v0.x == 0 && redBird.v0.y == 0 && redBirdT == 0) {
                        redBird.v0.x = redBird.originalPosition.x - redBird.getCenterX();
                        redBird.v0.y = redBird.originalPosition.y - redBird.getCenterY();
                    }
                    
                    float xNewPos = redBird.v0.Length() * redBirdT * (redBird.v0.x / redBird.v0.Length());
                    float yNewPos = redBird.v0.Length() * redBirdT * (redBird.v0.y / redBird.v0.Length()) - (0.15 / lambda / 2 * redBirdT * redBirdT);
                    
                    redBird.updateBirdPosition(Vector(redBird.dropPoint.x + xNewPos, redBird.dropPoint.y + yNewPos, 0));
                    
                    redBirdT += 0.5;
                }
                
                if (checkCollision() && redBird.isFlying == 1) {
                    isGameOn = 0;
                }
                
                if ((redBird.getCenterX() + 0.15 / lambda) < -1 || (redBird.getCenterX() - 0.15 / lambda) > 1 || (redBird.getCenterY() + 0.15) < -1 || (redBird.getCenterY() - 0.15) > 1) {
                    redBird.resetBirdsParameters();
                    redBirdT = 0;
                    redBird.flipBirdHorizontal();
                    redBird.updateBirdPosition(redBird.originalPosition);
                }
                
                glutPostRedisplay();
                
            }
            
        }
        
        if(dTime < 100) {
            float newPosition = Vector(transformCoordinates(500, 325)).y * dTime / 100 + 0.6 * sin(greenBirdGlobalX * (PI / 180));
            greenBird.updateBirdPosition(Vector(greenBird.getCenterX(), newPosition, 0));
        
            if (greenBirdGlobalX >= 360)
                greenBirdGlobalX = 0;
            else
                greenBirdGlobalX += 7 * dTime / 100;
                
            if (redBird.isFlying) {
                if (redBird.v0.x == 0 && redBird.v0.y == 0 && redBirdT == 0) {
                    redBird.v0.x = redBird.originalPosition.x - redBird.getCenterX();
                    redBird.v0.y = redBird.originalPosition.y - redBird.getCenterY();
                }
                
                float xNewPos = redBird.v0.Length() * redBirdT * (redBird.v0.x / redBird.v0.Length());
                float yNewPos = redBird.v0.Length() * redBirdT * (redBird.v0.y / redBird.v0.Length()) - (0.15 / lambda / 2 * redBirdT * redBirdT);
                    
                redBird.updateBirdPosition(Vector(redBird.dropPoint.x + xNewPos, redBird.dropPoint.y + yNewPos, 0));
                
                redBirdT += 0.5 * dTime / 100;
            }
                
            if (checkCollision() && redBird.isFlying == 1) {
                isGameOn = 0;
            }
            
            if ((redBird.getCenterX() + 0.15 / lambda) < -1 || (redBird.getCenterX() - 0.15 / lambda) > 1 || (redBird.getCenterY() + 0.15) < -1 || (redBird.getCenterY() - 0.15) > 1) {
                redBird.resetBirdsParameters();
                redBirdT = 0;
                redBird.flipBirdHorizontal();
                redBird.updateBirdPosition(redBird.originalPosition);
            }
                
            glutPostRedisplay();

        }
    }
    
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani
int main(int argc, char **argv) {
    glutInit(&argc, argv); 				// GLUT inicializalasa
    glutInitWindowSize(600, 600);			// Alkalmazas ablak kezdeti merete 600x600 pixel
    glutInitWindowPosition(100, 100);			// Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bites R,G,B,A + dupla buffer + melyseg buffer
    
    glutCreateWindow("Grafika hazi feladat");		// Alkalmazas ablak megszuletik es megjelenik a kepernyon
    
    glMatrixMode(GL_MODELVIEW);				// A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);			// A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    
    onInitialization();					// Az altalad irt inicializalast lefuttatjuk
    
    glutDisplayFunc(onDisplay);				// Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutMotionFunc(onMouseMotion);
    
    glutMainLoop();					// Esemenykezelo hurok
    
    return 0;
}
