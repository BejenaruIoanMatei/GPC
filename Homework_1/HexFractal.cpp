#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <string>

class Turtle
{
public:
    float x, y;
    float angle;

    Turtle(float startX, float startY, float startAngle)
        : x(startX), y(startY), angle(startAngle) {}

    void moveForward(float length, bool draw)
    {
        float rad = angle * 3.14159265359 / 180.0f;
        float newX = x + length * cos(rad);
        float newY = y + length * sin(rad);

        if (draw)
        {
            glColor3f(0.9, 0.1, 0.1);
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(newX, newY);
            glEnd();
        }

        x = newX;
        y = newY;
    }

    void turnLeft(float degrees)
    {
        angle += degrees;
    }

    void turnRight(float degrees)
    {
        angle -= degrees;
    }
};

int currentRecursionLevel = 7;
int defaultW = 800, defaultH = 800;

std::string generateLSystem(int level)
{
    std::string axiom = "A";
    for (int i = 0; i < level; ++i)
    {
        std::string next = "";
        for (char c : axiom)
        {
            if (c == 'A')
            {
                next += "B-A-B";
            }
            else if (c == 'B')
            {
                next += "A+B+A";
            }
            else
            {
                next += c;
            }
        }
        axiom = next;
    }
    return axiom;
}

void drawSierpinskiArrowhead(Turtle &turtle, const std::string &instructions, float length)
{
    for (char c : instructions)
    {
        if (c == 'A' || c == 'B')
        {
            turtle.moveForward(length, true);
        }
        else if (c == '+')
        {
            turtle.turnLeft(60);
        }
        else if (c == '-')
        {
            turtle.turnRight(60);
        }
    }
}

void Display5()
{
    glPushMatrix();
    glTranslatef(-0.4, -0.3, 0.0);

    float startX = 0.0;
    float startY = 0.0;
    float startAngle = 0.0;
    if (currentRecursionLevel % 2 != 0)
    {
        startAngle = 60.f;
    }

    Turtle turtle(startX, startY, startAngle);

    std::string instructions = generateLSystem(currentRecursionLevel);
    float length = 0.8 / pow(2, currentRecursionLevel);
    drawSierpinskiArrowhead(turtle, instructions, length);

    glPopMatrix();

    glColor3f(0.9, 0.1, 0.1);
    glRasterPos2f(-0.2, -0.9);
    std::string text = "Recursion Level: " + std::to_string(currentRecursionLevel);
    for (char c : text)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void init()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glLineWidth(1);
    glPolygonMode(GL_FRONT, GL_LINE);
}

void Display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    Display5();
    glFlush();
}

void Reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        exit(0);
    }
    else if (key >= '1' && key <= '9')
    {
        currentRecursionLevel = key - '0';
        std::cout << "Recursion level set to: " << currentRecursionLevel << std::endl;
        glutPostRedisplay();
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(defaultW, defaultH);
    glutInitWindowPosition(-1, -1);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow("Sierpinski Arrowhead Curve");

    init();
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(KeyboardFunc);
    glutDisplayFunc(Display);

    glutMainLoop();

    return 0;
}
