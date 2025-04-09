#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <iostream>

// Dimensiuni fereastră
int windowWidth = 800, windowHeight = 600;

// Parametri grid
int gridWidth = 30, gridHeight = 30;
float cellSize = 20.0f;

// Pixelii selectați și primitivele virtuale
std::vector<std::pair<int, int>> pixels;
std::vector<std::pair<float, float>> virtualLines;
std::vector<std::pair<float, float>> virtualCircles;

// Grosimea liniei
int lineThickness = 3;

// Moduri de operare
enum Mode
{
    LINE,
    CIRCLE
};
Mode currentMode = CIRCLE;

// Parametri linii
int startX = -1, startY = -1, endX = -1, endY = -1;
bool firstPointSelected = false;

// Parametri cercuri
int centerX = -1, centerY = -1, radiusX = -1, radiusY = -1;
bool centerSelected = false;

#pragma region Helper Functions

// Conversii între coordonate grid <-> ecran
void gridToScreen(int gx, int gy, float &sx, float &sy)
{
    float ar = (float)windowWidth / windowHeight;
    float gW = gridWidth * cellSize, gH = gridHeight * cellSize;
    float scale = (gW / gH > ar) ? windowWidth / gW : windowHeight / gH;
    float effW = gW * scale, effH = gH * scale;
    float offsetX = (windowWidth - effW) / 2.0f, offsetY = (windowHeight - effH) / 2.0f;
    sx = offsetX + gx * cellSize * scale;
    sy = offsetY + gy * cellSize * scale;
}

void screenToGrid(float sx, float sy, int &gx, int &gy)
{
    float ar = (float)windowWidth / windowHeight;
    float gW = gridWidth * cellSize, gH = gridHeight * cellSize;
    float scale = (gW / gH > ar) ? windowWidth / gW : windowHeight / gH;
    float effW = gW * scale, effH = gH * scale;
    float offsetX = (windowWidth - effW) / 2.0f, offsetY = (windowHeight - effH) / 2.0f;
    gx = (int)((sx - offsetX) / (cellSize * scale));
    gy = (int)((sy - offsetY) / (cellSize * scale));
}

// Adaugă un pixel dacă nu există deja
void addPixelUnique(int x, int y)
{
    for (const auto &p : pixels)
        if (p.first == x && p.second == y)
            return;
    pixels.emplace_back(x, y);
}

// Grosime pentru cerc și linie
void plotCirclePoints(int cx, int cy, int x, int y)
{
    for (int i = -lineThickness / 2; i <= lineThickness / 2; ++i)
        for (int j = -lineThickness / 2; j <= lineThickness / 2; ++j)
        {
            addPixelUnique(cx + x + i, cy + y + j);
            addPixelUnique(cx - x + i, cy + y + j);
            addPixelUnique(cx + x + i, cy - y + j);
            addPixelUnique(cx - x + i, cy - y + j);
            addPixelUnique(cx + y + i, cy + x + j);
            addPixelUnique(cx - y + i, cy + x + j);
            addPixelUnique(cx + y + i, cy - x + j);
            addPixelUnique(cx - y + i, cy - x + j);
        }
}

void thickenLine(int x, int y, int thickness)
{
    int half = thickness / 2;
    for (int i = -half; i <= half; ++i)
        for (int j = -half; j <= half; ++j)
            if (i * i + j * j <= half * half + 1)
                addPixelUnique(x + i, y + j);
}
#pragma endregion

#pragma region Algoritmi Rasterizare

void midPointLine(int x0, int y0, int x1, int y1)
{
    virtualLines = {{x0, y0}, {x1, y1}};
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1 ? 1 : -1), sy = (y0 < y1 ? 1 : -1), err = dx - dy;

    while (true)
    {
        thickenLine(x0, y0, lineThickness);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
            err -= dy, x0 += sx;
        if (e2 < dx)
            err += dx, y0 += sy;
    }
}

void midPointCircle(int cx, int cy, int r)
{
    virtualCircles = {{cx, cy}, {(float)r, 0}};
    int x = 0, y = r, d = 1 - r;
    plotCirclePoints(cx, cy, x, y);

    while (y > x)
    {
        if (d < 0)
            d += 2 * x + 3;
        else
        {
            d += 2 * (x - y) + 5;
            --y;
        }
        ++x;
        plotCirclePoints(cx, cy, x, y);
    }
}
#pragma endregion

#pragma region Drawing

void drawGrid()
{
    float ar = (float)windowWidth / windowHeight;
    float gW = gridWidth * cellSize, gH = gridHeight * cellSize;
    float scale = (gW / gH > ar) ? windowWidth / gW : windowHeight / gH;
    float effW = gW * scale, effH = gH * scale;
    float offsetX = (windowWidth - effW) / 2.0f, offsetY = (windowHeight - effH) / 2.0f;

    glColor3f(0.9f, 0.9f, 0.9f);
    for (int i = 0; i <= gridHeight; ++i)
    {
        float y = offsetY + i * cellSize * scale;
        glBegin(GL_LINES);
        glVertex2f(offsetX, y);
        glVertex2f(offsetX + effW, y);
        glEnd();
    }
    for (int i = 0; i <= gridWidth; ++i)
    {
        float x = offsetX + i * cellSize * scale;
        glBegin(GL_LINES);
        glVertex2f(x, offsetY);
        glVertex2f(x, offsetY + effH);
        glEnd();
    }
}

void drawPixel(int x, int y)
{
    if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight)
        return;
    float sx, sy;
    gridToScreen(x, y, sx, sy);
    float scale = std::min(windowWidth / (gridWidth * cellSize), windowHeight / (gridHeight * cellSize));
    float radius = 0.3f * cellSize * scale;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(sx, sy);
    for (int i = 0; i <= 32; ++i)
    {
        float theta = 2 * M_PI * i / 32;
        glVertex2f(sx + radius * cosf(theta), sy + radius * sinf(theta));
    }
    glEnd();
}

void drawVirtualPrimitives()
{
    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);
    if (virtualLines.size() == 2)
    {
        float x0, y0, x1, y1;
        gridToScreen(virtualLines[0].first, virtualLines[0].second, x0, y0);
        gridToScreen(virtualLines[1].first, virtualLines[1].second, x1, y1);
        glBegin(GL_LINES);
        glVertex2f(x0, y0);
        glVertex2f(x1, y1);
        glEnd();
    }

    if (virtualCircles.size() == 2)
    {
        float cx, cy;
        gridToScreen(virtualCircles[0].first, virtualCircles[0].second, cx, cy);
        float scale = std::min(windowWidth / (gridWidth * cellSize), windowHeight / (gridHeight * cellSize));
        float r = virtualCircles[1].first * cellSize * scale;

        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 64; ++j)
        {
            float theta = 2 * M_PI * j / 64;
            glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
        }
        glEnd();
    }
    glLineWidth(1.0f);
}
#pragma endregion

#pragma region GLUT Callbacks

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    drawVirtualPrimitives();
    glColor3f(0.0f, 0.0f, 0.0f);
    for (auto &p : pixels)
        drawPixel(p.first, p.second);
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;
    y = windowHeight - y;

    int gx, gy;
    screenToGrid(x, y, gx, gy);
    if (gx < 0 || gx >= gridWidth || gy < 0 || gy >= gridHeight)
        return;

    if (currentMode == LINE)
    {
        if (!firstPointSelected)
        {
            startX = gx;
            startY = gy;
            firstPointSelected = true;
        }
        else
        {
            endX = gx;
            endY = gy;
            firstPointSelected = false;
            pixels.clear();
            virtualLines.clear();
            virtualCircles.clear();
            midPointLine(startX, startY, endX, endY);
            glutPostRedisplay();
        }
    }
    else if (currentMode == CIRCLE)
    {
        if (!centerSelected)
        {
            centerX = gx;
            centerY = gy;
            centerSelected = true;
        }
        else
        {
            radiusX = gx;
            radiusY = gy;
            centerSelected = false;
            int dx = radiusX - centerX, dy = radiusY - centerY;
            int radius = (int)round(sqrt(dx * dx + dy * dy));
            pixels.clear();
            virtualLines.clear();
            virtualCircles.clear();
            midPointCircle(centerX, centerY, radius);
            glutPostRedisplay();
        }
    }
}

void keyboard(unsigned char key, int, int)
{
    switch (key)
    {
    case 'l':
    case 'L':
        currentMode = LINE;
        firstPointSelected = centerSelected = false;
        break;
    case 'c':
    case 'C':
        currentMode = CIRCLE;
        firstPointSelected = centerSelected = false;
        break;
    case '+':
        ++lineThickness;
        break;
    case '-':
        if (lineThickness > 1)
            --lineThickness;
        break;
    case 'r':
    case 'R':
        pixels.clear();
        virtualLines.clear();
        virtualCircles.clear();
        firstPointSelected = centerSelected = false;
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}
#pragma endregion

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Rasterizare: Algoritmul Mid-point");

    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);

    // Cercul inițial (exemplu)
    centerX = gridWidth / 2;
    centerY = gridHeight / 2;
    int radius = gridWidth / 3;
    midPointCircle(centerX, centerY, radius);

    std::cout << "Controale:\nL - linie\nC - cerc\n+/- - grosime\nR - reset\nESC - ieșire\nClick stânga - selectează puncte\n";
    glutMainLoop();
    return 0;
}
