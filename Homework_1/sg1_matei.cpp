/*
  This program plots different 2D functions.
*/

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <iostream>
// #include "glut.h" //MSVC local library install
#include <GL/glut.h> //system-wide install (or compiler default path)

double circle = atan(1) * 8;
double halfCircle = atan(1) * 4;
double tau = circle;    // 2 * PI = TAU
double pi = halfCircle; // TAU / 2 = PI

// How often should the drawing algorithm sample the function.
double step = 0.05;

int defaultW = 1000, defaultH = 1000;

unsigned char prevKey;

/*
   Nicomedes' Conchoid
   $x = a + b \cdot cos(t), y = a \cdot tg(t) + b \cdot sin(t)$. or
   $x = a - b \cdot cos(t), y = a \cdot tg(t) - b \cdot sin(t)$. where
   $t \in (-\pi / 2, \pi / 2)$
*/
void Display1()
{
    double xmax, ymax, xmin, ymin;
    /*
      Nicomedes' Conchoid is a family of functions. These two parameters,
      a and b, choose a specific 2D function from that family.
      It has two branches, so, for each y, we have two x values.
      Therefore, we're actually drawing two lines, not one.
     */
    double a = 1, b = 2;

    /*
      We shadow the global variable with a different step size,
      as values different from 0.05
      would yield a significantly different plot.
    */
    double step = 0.05;

    /*
      First, we compute the points of the function, so we can determine
      the maximal extend of the drawing.
     */
    xmax = a - b - 1;
    xmin = a + b + 1;
    ymax = ymin = 0;
    for (double t = -pi / 2 + step; t < pi / 2; t += step)
    {
        double x1, y1, x2, y2;
        x1 = a + b * cos(t);
        xmax = (xmax < x1) ? x1 : xmax;
        xmin = (xmin > x1) ? x1 : xmin;

        x2 = a - b * cos(t);
        xmax = (xmax < x2) ? x2 : xmax;
        xmin = (xmin > x2) ? x2 : xmin;

        y1 = a * tan(t) + b * sin(t);
        ymax = (ymax < y1) ? y1 : ymax;
        ymin = (ymin > y1) ? y1 : ymin;

        y2 = a * tan(t) - b * sin(t);
        ymax = (ymax < y2) ? y2 : ymax;
        ymin = (ymin > y2) ? y2 : ymin;
    }
    // We care about the maximal extent on each axis (from the origin).
    xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
    ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

    /*
      Since we have, for x and for y, the maximal absolute values,
      dividing the coordinates of the points by these values will
      ensure we draw the whole function inside [-1, 1]^2, the default
      OpenGL screen.
     */

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);
    for (double t = -pi / 2 + step; t < pi / 2; t += step)
    {
        double x1, y1, x2, y2; // You might get some warnings in your IDE. Why?
        x1 = (a + b * cos(t)) / xmax;
        x2 = (a - b * cos(t)) / xmax;
        y1 = (a * tan(t) + b * sin(t)) / ymax;
        y2 = (a * tan(t) - b * sin(t)) / ymax;

        glVertex2d(x1, y1);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (double t = -pi / 2 + step; t < pi / 2; t += step)
    {
        double x1, y1, x2, y2;
        x1 = (a + b * cos(t)) / xmax;
        x2 = (a - b * cos(t)) / xmax;
        y1 = (a * tan(t) + b * sin(t)) / ymax;
        y2 = (a * tan(t) - b * sin(t)) / ymax;

        glVertex2d(x2, y2);
    }
    glEnd();
}

// $f(x) = \left| sin(x) \right| \cdot e^{-sin(x)}, x \in \left[ 0, 8 \cdot \pi \right]$,
void Display2()
{
    /*
      We can determine how far the function extends
      (and thus the needed scaling factors)
      by looking at the function and doing a bit of Calculus.
    */
    double xmax = 8 * pi;
    double ymax = exp(1.1); // Why 1.1?

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);
    for (double x = 0; x < xmax; x += step)
    {
        double x1, y1;
        x1 = x / xmax;
        y1 = (fabs(sin(x)) * exp(-sin(x))) / ymax;
        glVertex2d(x1, y1);
    }
    glEnd();
}

/* 1)
   \( f(x) =
     \left\{
       \begin{array}{cl}
         1              & x = 0   \\
         \frac{d(x)}{x} & x \gt 0 \\
       \end{array}
     \right.
   \)
 */

// Function to calculate d(x) - distance to nearest integer
double d(double x)
{
    int closest_int = round(x);
    return fabs(x - closest_int);
}

// Function to calculate f(x)
double f(double x)
{
    if (!x)
        return 1.0;
    return d(x) / x;
}

void Display3()
{
    double xmin = 0.0;
    double xmax = 20.0;

    double step = 0.05;
    double ymax = 1.1;

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);

    // Plot the rest of the function
    for (double x = xmin + step; x <= xmax; x += step)
    {
        double x_scaled = x / xmax;
        double y_scaled = f(x) / ymax;
        glVertex2d(x_scaled, y_scaled);
    }

    glEnd();
}

// 3) function arguments e.g.: f(a, b, t), where a and b are function family parameters, and the is the driving variables.
void plot(double (*x)(double, double, double), double (*y)(double, double, double),
          double a, double b, double intervalStart, double intervalEnd,
          double step = 0.01, double scaleX = 1, double scaleY = 1,
          GLint primitive = GL_LINE_STRIP)
{

    double xmax = 0, ymax = 0;

    // First, compute the maximum extent of the function to properly scale it
    for (double t = intervalStart; t <= intervalEnd; t += step)
    {
        double xVal = x(a, b, t);
        double yVal = y(a, b, t);

        // Track maximum absolute values for scaling
        xmax = std::max(xmax, std::abs(xVal));
        ymax = std::max(ymax, std::abs(yVal));
    }

    // Add a small margin to ensure the curve fits within the display area
    xmax *= 1.1;
    ymax *= 1.1;

    // Apply additional scaling if needed
    xmax /= scaleX;
    ymax /= scaleY;

    // Plot the function with color gradient
    glBegin(primitive);

    // Total number of points to draw
    int numPoints = (int)((intervalEnd - intervalStart) / step);
    int pointIndex = 0;

    for (double t = intervalStart; t <= intervalEnd; t += step)
    {
        double xVal = x(a, b, t);
        double yVal = y(a, b, t);

        // Calculate gradient factor (0 to 1) based on current position
        double gradientFactor = (double)pointIndex / numPoints;

        // Red component transitions based on gradient
        glColor3f(1.0 - gradientFactor, 0.1, 0.1);

        // Scale to fit within the [-1, 1] OpenGL viewport
        glVertex2d(xVal / xmax, yVal / ymax);

        pointIndex++;
    }

    glEnd();
}
/*
  2) Circle Concoid (Limaçon, Pascal's Snail):
  \(x = 2 \cdot (a \cdot cos(t) + b) \cdot cos(t), \; y = 2 \cdot (a \cdot cos(t) + b) \cdot sin(t), \; t \in (-\pi, \pi)\) .
  For this plot, \(a = 0.3, \; b = 0.2\) .
*/

void Display4()
{
    double a = 0.3, b = 0.2;
    double xmax = 0, ymax = 0;
    double tmin = -pi, tmax = pi;
    double step = 0.01;

    // First, compute the maximum extent of the function to properly scale it
    for (double t = tmin; t <= tmax; t += step)
    {
        double x = 2 * (a * cos(t) + b) * cos(t);
        double y = 2 * (a * cos(t) + b) * sin(t);

        // Track maximum absolute values for scaling
        xmax = std::max(xmax, std::abs(x));
        ymax = std::max(ymax, std::abs(y));
    }

    // Add a small margin to ensure the curve fits within the display area
    xmax *= 1.1;
    ymax *= 1.1;

    // Plot the function with color gradient
    glBegin(GL_LINE_STRIP);

    // Total number of points to draw
    int numPoints = (int)((tmax - tmin) / step);
    int pointIndex = 0;

    for (double t = tmin; t <= tmax; t += step)
    {
        double x = 2 * (a * cos(t) + b) * cos(t);
        double y = 2 * (a * cos(t) + b) * sin(t);

        // Calculate gradient factor (0 to 1) based on current position
        double gradientFactor = (double)pointIndex / numPoints;

        // Red component decreases from 1.0 to 0.0
        // Green component stays low
        // Blue component stays low
        glColor3f(1.0 - gradientFactor, 0.1, 0.1);

        // Scale to fit within the [-1, 1] OpenGL viewport
        glVertex2d(x / xmax, y / ymax);

        pointIndex++;
    }

    glEnd();
}
/////////
// Limacon of Pascal x function
double limaconX(double a, double b, double t)
{
    return 2 * (a * cos(t) + b) * cos(t);
}

// Limacon of Pascal y function
double limaconY(double a, double b, double t)
{
    return 2 * (a * cos(t) + b) * sin(t);
}

void Display8()
{
    double a = 0.3, b = 0.2;
    double tmin = -pi, tmax = pi;

    plot(limaconX, limaconY, a, b, tmin, tmax);
}
/////////

// Cycloid x function
double cycloidX(double a, double b, double t)
{
    return a * t - b * sin(t);
}

// Cycloid y function
double cycloidY(double a, double b, double t)
{
    return a - b * cos(t);
}

void Display9()
{
    double a = 0.1, b = 0.2;
    double tmin = 0, tmax = 6 * pi; // Using 6π to show 3 complete cycles

    plot(cycloidX, cycloidY, a, b, tmin, tmax);
}

///////////

// Epicycloid x function
double epicycloidX(double a, double b, double t)
{
    double ratio = b / a;
    return (a + b) * cos(ratio * t) - b * cos(t + ratio * t);
}

// Epicycloid y function
double epicycloidY(double a, double b, double t)
{
    double ratio = b / a;
    return (a + b) * sin(ratio * t) - b * sin(t + ratio * t);
}

void Display10()
{
    double a = 0.1, b = 0.3;
    double tmin = 0, tmax = 2 * pi;

    plot(epicycloidX, epicycloidY, a, b, tmin, tmax);
}

///////////

// Hypocycloid x function
double hypocycloidX(double a, double b, double t)
{
    double ratio = b / a;
    return (a - b) * cos(ratio * t) - b * cos(t - ratio * t);
}

// Hypocycloid y function
double hypocycloidY(double a, double b, double t)
{
    double ratio = b / a;
    return (a - b) * sin(ratio * t) - b * sin(t - ratio * t);
}

void Display11()
{
    double a = 0.1, b = 0.3;
    double tmin = 0, tmax = 2 * pi;

    plot(hypocycloidX, hypocycloidY, a, b, tmin, tmax);
}

////////

/*
  2) Cicloid:
  \(x = a \cdot t - b \cdot sin(t), \; y = a - b \cdot cos(t), \; t \in \mathbb{R} \) .
  For this plot, \(a = 0.1, \; b = 0.2\) .
*/
void Display5()
{
    double a = 0.1, b = 0.2;

    // For a cycloid, we need a specific range to match the image
    double tmin = 0, tmax = 6 * pi; // Using 6π to show 3 complete cycles
    double step = 0.01;

    double xmax = 0, ymax = 0;
    double ratio = b / a;

    for (double t = tmin; t <= tmax; t += step)
    {
        double x = a * t - b * sin(t);
        double y = a - b * cos(t);

        xmax = std::max(xmax, std::abs(x));
        ymax = std::max(ymax, std::abs(y));
    }

    // Add a small margin
    xmax *= 1.1;
    ymax *= 1.1;

    glBegin(GL_LINE_STRIP);

    int numPoints = (int)((tmax - tmin) / step);
    int pointIndex = 0;

    for (double t = tmin; t <= tmax; t += step)
    {
        double x = a * t - b * sin(t);
        double y = a - b * cos(t);

        // Calculate gradient factor
        double gradientFactor = (double)pointIndex / numPoints;

        // Transition from red to black
        // 1 -> Red
        // 0 -> Black
        glColor3f(gradientFactor, 0.1, 0.1);

        // Scale to fit within the [-1, 1] OpenGL viewport
        // The 0.7 factor creates more horizontal space between cycles
        glVertex2d((x / xmax), y / ymax);

        pointIndex++;
    }

    glEnd();
}

/*
  2) Epicicloid:
  \(x = (a + b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t + \frac{b}{a}\cdot t \right) \)
  \(y = (a + b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t + \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
*/
void Display6()
{
    double a = 0.1, b = 0.3;
    double xmax = 0, ymax = 0;

    // Parameter range for epicycloid
    double tmin = 0, tmax = 2 * pi;
    double step = 0.01;

    // Calculate the ratio b/a (used multiple times in the formula)
    double ratio = b / a;

    // First, compute the maximum extent
    for (double t = tmin; t <= tmax; t += step)
    {
        double x = (a + b) * cos(ratio * t) - b * cos(t + ratio * t);
        double y = (a + b) * sin(ratio * t) - b * sin(t + ratio * t);

        xmax = std::max(xmax, std::abs(x));
        ymax = std::max(ymax, std::abs(y));
    }

    // Add a small margin
    xmax *= 1.1;
    ymax *= 1.1;

    // Plot the function with color gradient
    glBegin(GL_LINE_STRIP);

    int numPoints = (int)((tmax - tmin) / step);
    int pointIndex = 0;

    for (double t = tmin; t <= tmax; t += step)
    {
        double x = (a + b) * cos(ratio * t) - b * cos(t + ratio * t);
        double y = (a + b) * sin(ratio * t) - b * sin(t + ratio * t);

        // Calculate gradient factor
        double gradientFactor = (double)pointIndex / numPoints;

        // Transition from red to black
        glColor3f(gradientFactor, 0.1, 0.1);

        glVertex2d(x / xmax, y / ymax);

        pointIndex++;
    }

    glEnd();
}

/*
  2) Hipocicloid:
  \(x = (a - b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t - \frac{b}{a}\cdot t \right) \)
  \(y = (a - b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t - \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
 */
void Display7()
{
    double a = 0.1, b = 0.3;
    double xmax = 0;
    double ymax = 0;

    double tmin = 0;
    double tmax = 2 * pi;
    double step = 0.01;

    // Calculate the ratio b/a (used multiple times in the formula)
    double ratio = b / a;

    // First, compute the maximum extent
    for (double t = tmin; t <= tmax; t += step)
    {
        double x = (a - b) * cos(ratio * t) - b * cos(t - ratio * t);
        double y = (a - b) * sin(ratio * t) - b * sin(t - ratio * t);

        xmax = std::max(xmax, std::abs(x));
        ymax = std::max(ymax, std::abs(y));
    }

    // Add a small margin
    xmax *= 1.1;
    ymax *= 1.1;

    // Plot the function with color gradient
    glBegin(GL_LINE_STRIP);

    int numPoints = (int)((tmax - tmin) / step);
    int pointIndex = 0;

    for (double t = tmin; t <= tmax; t += step)
    {
        double x = (a - b) * cos(ratio * t) - b * cos(t - ratio * t);
        double y = (a - b) * sin(ratio * t) - b * sin(t - ratio * t);

        // Calculate gradient factor
        double gradientFactor = (double)pointIndex / numPoints;

        // Transition from red to black
        glColor3f(gradientFactor, 0.1, 0.1);

        glVertex2d(x / xmax, y / ymax);

        pointIndex++;
    }

    glEnd();
}

/*
 4) Logarithmic spiral (in polar coordinates):
 \( r = a \cdot e^{1+t}, \; t \in (0, \infty) \) .
 For this plot, \(a = 0.02\) .
*/

/*
  4) Sine polar plot flower:
  \( r = sin(a \cdot t), \; t \in (0, \infty)  \) .
  For this plot, \(a = 10\), and the number 'petals' is \( 2 \cdot a \). Think about why.
*/

/*
5) Longchamps' Trisectrix:
\(
x = \frac{a}{4 \cdot cos^2(t) - 3}, \;
y = \frac{a \cdot tg(t)}{4 \cdot cos^2(t) - 3}, \;
t \in (-\pi/2, \pi/2) \setminus \{ -\pi/6, \pi/6 \} \) .
For this plot, \(a = 0.2\) .
 */

void init(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glLineWidth(2);
    glPointSize(1);
    // glPolygonMode(GL_FRONT, GL_LINE);
    // Enabling blending and smoothing
    glEnable(GL_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_NICEST, GL_POINT_SMOOTH_HINT);
    glHint(GL_NICEST, GL_LINE_SMOOTH_HINT);
    glHint(GL_NICEST, GL_POLYGON_SMOOTH_HINT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
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
    case '9':
        Display9();
        break;
    case '0':
        Display10();
        break;
    default:
        break;
    }
    glFlush();
}

void Reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}
void KeyboardFunc(unsigned char key, int x, int y)
{
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
    glutInit(&argc, argv);
    glutInitWindowSize(defaultW, defaultH);
    glutInitWindowPosition(-1, -1);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow(argv[0]);
    init();
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(KeyboardFunc);
    glutMouseFunc(MouseFunc);
    glutDisplayFunc(Display);
    // glutIdleFunc(Display);
    glutMainLoop();

    return 0;
}
