/* The GLUT library (GL/glut.h) already includes
   GL/glu.h and GL/gl.h .
   Functions from each library are prefixed by: gl, glu, or glut.

   In Visual Studio: create an empty CPP project, then
   extract the glut library package (containing
   glut.h, glut.lib, glut.dll - or their 32 bit versions).
   Copy them so that the .h and .lib files are in the same directory
   as your .cpp file, and the .dll is together with your .exe .
   If you choose the 32-bit version of files, make sure you're
   also building a 32-bit version of the project.

   On most GNU/Linux distro, MacOS, Codeblocks, Visual Studio Code + mingw:
   GLUT is implelmented by freeglut.
   In the case of Debian / Ubuntu and variants, the packages are
   'freeglut3' and 'freeglut3-dev'.
   Manually building the binary can be done like so:
   g++ -o binary -lGL -lGLU -lglut source.cpp
   Nota bene: GL and GLU are ALLCAPS, glut is not.
   You can also use the enclosed Makefile.
*/

#include <cstdlib>
#include <cmath>
#include <iostream>
// #include "glut.h" //MSVC local library install
#include <GL/glut.h> //system-wide install (or compiler default path)

float circle = atan(1) * 8;     // 2 * PI = TAU
float halfCircle = atan(1) * 4; // PI

int defaultW = 1000, defaultH = 1000;

unsigned char prevKey;

void Display1()
{
    glColor3f(0.2, 0.15, 0.88); // blue
    glBegin(GL_LINES);
    {                     // we'll draw lines; don't forget the ';'
        glVertex2i(1, 1); // the coordinates of a vertex (2d, integer coordinates)
        glVertex2i(-1, -1);
    }
    glEnd();

    glColor3f(1, 0.1, 0.1); // red
    glBegin(GL_LINES);
    {
        glVertex2i(-1, 1);
        glVertex2i(1, -1);
    }
    glEnd();

    glBegin(GL_LINES);
    {
        glVertex2d(-0.5, 0);
        glVertex2d(0.5, 0);
    }
    glEnd();
}

void Display2()
{
    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINES);
    {                         // We don't need a special Begin/End block for each line segment
        glVertex2f(1.0, 1.0); // OpenGL knows that a line segment is determined by two vertices.
        glVertex2f(0.9, 0.9);
        glVertex2f(0.8, 0.8);
        glVertex2f(0.7, 0.7);
        glVertex2f(0.6, 0.6);
        glVertex2f(0.5, 0.5);
        glVertex2f(-0.5, -0.5);
        glVertex2f(-1.0, -1.0);
    }
    glEnd();
}

void Display3()
{
    glColor3f(0.9, 0.1, 0.1);
    // Search for glPointSize(4) in this file;
    // try to change it to 12.
    glBegin(GL_POINTS);
    {
        glVertex2f(0.9, 0.9);
        glVertex2f(0.1, 0.9);
        glVertex2f(0.1, 0.1);
        glVertex2f(0.9, 0.1);
    }
    glEnd();

    glColor3f(0.1, 0.9, 0.1);
    // an open loop:
    //   (v0,v1), (v1,v2), ..., (v_{n-2},v_{n-1})
    glBegin(GL_LINE_STRIP);
    {
        glVertex2f(-0.9, 0.9);
        glVertex2f(-0.1, 0.9);
        glVertex2f(-0.1, 0.1);
        glVertex2f(-0.9, 0.1);
    }
    glEnd();

    glColor3f(0.1, 0.1, 0.9);
    // a self-closing loop:
    //   (v0,v1), (v1,v2), ..., (v_{n-2},v_{n-1}), (v_{n-1},v0)
    glBegin(GL_LINE_LOOP);
    {
        glVertex2f(-0.9, -0.9);
        glVertex2f(-0.1, -0.9);
        glVertex2f(-0.1, -0.1);
        glVertex2f(-0.9, -0.1);
    }
    glEnd();

    // one or more rectangles, which can be filled:
    glBegin(GL_QUADS);
    {
        // we can change colours inside glBegin
        glColor3f(0.9, 0.5, 0.1);
        glVertex2f(0.9, -0.9);
        glVertex2f(0.1, -0.9);
        glVertex2f(0.1, -0.1);
        glVertex2f(0.9, -0.1);

        // Try to fill this!
        //(And understand why it wasn't filling in the first place!)
        glColor3f(0.9, 0.1, 0.1);
        glVertex2f(0.7, -0.3);
        glColor3f(0.1, 0.9, 0.1);
        glVertex2f(0.3, -0.3);
        glColor3f(0.1, 0.1, 0.9);
        glVertex2f(0.3, -0.7);
        glColor3f(0.9, 0.9, 0.9);
        glVertex2f(0.7, -0.7);
    }
    glEnd();
}

void Display4()
{
    // Multiple triangles. Explain why one is filled, the other empty!
    glBegin(GL_TRIANGLES);
    {
        glColor3f(0.9, 0.1, 0.1);
        glVertex2f(0.9, 0.9);
        glVertex2f(0.2, 0.9);
        glVertex2f(0.9, 0.2);

        glColor3f(0.1, 0.1, 0.9);
        glVertex2f(0.1, 0.1);
        glVertex2f(0.1, 0.8);
        glVertex2f(0.8, 0.1);
    }
    glEnd();

    /*
      Draw a fan of triangles. We start from a root point,
      and need another to make the first line.
      Then, each point extends from that first point, and
      connects to the previous point in the line.
    */
    float rootX = -0.9, rootY = 0.9, radius = 0.9;
    int maxSteps = 30;
    glBegin(GL_TRIANGLE_FAN);
    {
        glColor3f(0.9, 0.1, 0.1);
        glVertex2f(rootX, rootX); // Root point.
        for (int ii = 0; ii < maxSteps; ++ii)
        {
            glColor3f(0.9 * ii / maxSteps, 0.1, 0.9);
            float angle = -ii / float(maxSteps) * circle / 4.0;
            glVertex2f(rootX + radius * cos(angle), rootY + radius * sin(angle));
        }
    }
    glEnd();

    rootX = 0.45;
    rootY = -0.45;
    radius = 0.45;
    maxSteps = 360;
    // A polygon, taking as many points as we give it.
    // In this case, glBegin draws a single geometric figure.
    glBegin(GL_POLYGON);
    {
        for (int ii = 0; ii < maxSteps; ++ii)
        {
            float normalisedPosition = ii / float(maxSteps);
            // Try removing the minus. And fixing the drawing.
            float angle = -normalisedPosition * circle;
            glColor3f(normalisedPosition, 0, 0);
            glVertex2f(rootX + radius * cos(angle), rootY + radius * sin(angle));
        }
    }
    glEnd();
}

void Display5()
{
}

void Display6()
{
}

void Display7()
{
}

void Display8()
{
}

void init(void)
{
    // The screen is cleared using the function glClear()
    // This sets the screen to the RGBA colour specified below.
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glLineWidth(3);
    glPointSize(4);
    /*
      void glPolygonMode (GLenum face, GLenum mode)
      face: the face of the polygon (according to the right-hand rule)
      GL_FRONT
      GL_BACK
      GL_FRONT_AND_BACK
      mode: GL_POINT (only draw the vertices as points)
      GL_LINE (only draw the edges)
      GL_FILL (also fill the inside pixels)
    */
    glPolygonMode(GL_FRONT, GL_LINE);
}

void Display(void)
{
    std::cout << ("Call Display") << std::endl;
    // Clear the buffer. See init();
    glClear(GL_COLOR_BUFFER_BIT);

    switch (prevKey)
    {
    case '1':
        Display1();
        break;
    case '2':
        Display2();
        break;
    case '3':
        Display3();
        break;
    case '4':
        Display4();
        break;
    case '5':
        Display5();
        break;
    case '6':
        Display6();
        break;
    case '7':
        Display7();
        break;
    case '8':
        Display8();
        break;
    default:
        break;
    }

    // Flushes all buffers, and forces the image to be sent to the screen.
    glFlush();
}

/*
  Callback upon window reshape.
*/
void Reshape(int w, int h)
{
    std::cout << "Window reshape. Width: " << w << " height: " << h << std::endl;
    /* void glViewport (GLint x, GLint y,
                        GLsizei width, GLsizei height)
       reshapes the surface onto which we draw so that it matches the inside
       of the on-desktop window. (x, y) define the lower-left pixel.
       Use (0, 0, w, h) for a full-window drawing surface.
     */
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

/*
  Callback upon key press. We receive the keycode, and the
  mouse cursor position.
*/
void KeyboardFunc(unsigned char key, int x, int y)
{
    std::cout << "Key: " << key << " has been pressed at coordinates: ";
    std::cout << x << " x " << y << std::endl;
    // Save the last-pressed key in the global variable, so we can switch
    // display functions.
    prevKey = key;
    if (key == 27) // escape
        exit(0);
    // The proper way to ask glut to redraw the window.
    glutPostRedisplay();
}

/*
  Callback upon mouse press or release.
  The button can be:
   GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
   (and further for mousewheel and other mouse buttons)
  The state can be either GLUT_DOWN or  GLUT_UP, for
  a pressed or released button.
  (x, y) are the coordinates of the mouse.
*/
void MouseFunc(int button, int state, int x, int y)
{
    std::cout << "Mouse button ";
    std::cout << ((button == GLUT_LEFT_BUTTON) ? "left" : ((button == GLUT_RIGHT_BUTTON) ? "right" : "middle")) << " ";
    std::cout << ((state == GLUT_DOWN) ? "pressed" : "released");
    std::cout << " at coordinates: " << x << " x " << y << std::endl;
}

int main(int argc, char **argv)
{
    /*Initialise the GLUT library.
      void glutInit(int *argcp, char **argv)
      GLUT may read and change/remove arguments from the command line.
      You should not have any glut* function call before this one.
    */
    glutInit(&argc, argv);

    // Set window size, in pixels.
    glutInitWindowSize(defaultW, defaultH);

    /* Set window position, in pixels from the top-left corner of the deskop
       to the top-left corner of the window.
       (-1, -1) means let the window manager decide.
     */
    glutInitWindowPosition(-1, -1);

    /*
      void glutInitDisplayMode (unsigned int mode)
      Request a specific display mode, with specific
      capabilities. Capability flags are bitmasks.
      GLUT_SINGLE or  GLUT_DOUBLE : single buffering or
        double-buffering (for animations).
      GLUT_RGB or GLUT_RGBA : Alpha: enable transparency
      Multisampling and other advanced modes can be enabled here.
     */
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    /*
    int glutCreateWindow (char *name)
    Attempt to create a window with the provided name.
    */
    glutCreateWindow(argv[0]);

    // Our custom on-initialisation operations.
    init();

    /*Callback functions: we can write our own behaviour in
      functions, then register those function pointers in GLUT,
      so that the GLUT event loop calls our code when appropriate.
     */

    // void glutReshapeFunc (void (*Reshape)(int width, int height))
    glutReshapeFunc(Reshape);

    // void glutKeyboardFunc (void (*KeyboardFunc)(unsigned char,int,int))
    glutKeyboardFunc(KeyboardFunc);

    // void glutMouseFunc (void (*MouseFunc)(int,int,int,int))
    glutMouseFunc(MouseFunc);

    /* void glutDisplayFunc (void (*Display)(void))
     Display is also a callback. GLUT calls it whenever it needs to
     redraw pixels - window resize, window dragged from off-screen,
     unminimised, or if another window partially covered it,
     and now it (partially) uncovered it.
    */
    glutDisplayFunc(Display);

    /*
      void glutIdleFunc (void (*IdleFunc) (void))
      Whenever GLUT has some spare time, it calls this function.
      This is an easy way to display animations at a fluid framerate.
     */
    // glutIdleFunc(Display);

    /* After the previous initialisation, the glut main loop starts. This
      event loop will process external events, and call our assigned callback
      functions as appropriate. We surrender control of the program flow to it.
    */
    glutMainLoop();

    return 0;
}
