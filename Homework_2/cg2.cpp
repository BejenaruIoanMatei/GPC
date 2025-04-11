#include "glut.h"
#include <cmath>
const int gridSize = 30;

int windowWidth = 600;
int windowHeight = 600;


void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;

    int size = (w < h) ? w : h;
    glViewport((w - size) / 2, (h - size) / 2, size, size);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, gridSize, 0, gridSize);
    glMatrixMode(GL_MODELVIEW);
}
void drawGrid() {
    glColor3f(0.7f, 0.7f, 0.7f);
    for (int i = 0; i <= gridSize; i++) {
        glBegin(GL_LINES);
        glVertex2f(i, 0);
        glVertex2f(i, gridSize);
        glEnd();
    }

    for (int i = 0; i <= gridSize; i++) {
        glBegin(GL_LINES);
        glVertex2f(0, i);
        glVertex2f(gridSize, i);
        glEnd();
    }
}

void drawVirtualLine(int x1, int y1, int x2, int y2) {
    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(1.f);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1.0f);
}

void drawVirtualCircle(int cx, int cy, float radius = 5.0f, int segments = 100) {
    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(1.f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawDiscAt(int x, int y, float radius = 0.3f, int segments = 20) {
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float dx = radius * cosf(theta);
        float dy = radius * sinf(theta);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void drawThickDisc(int x, int y, int thickness = 1) {
    for (int dx = -thickness; dx <= thickness; dx++) {
        for (int dy = -thickness; dy <= thickness; dy++) {
            if (dx * dx + dy * dy <= thickness * thickness)
                drawDiscAt(x + dx, y + dy);
        }
    }
}

void rasterizeLine(int x0, int y0, int x1, int y1, int thickness = 1) {

    /*
        Thickness table

        0 - light
        1 - normal
        2 - thick
        3 - thicc
        above 3 : kim kardashian
    */

    drawVirtualLine(x0, y0, x1, y1);

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        drawThickDisc(x0, y0, thickness);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void rasterizeLineBresenham(int x0, int y0, int x1, int y1, int thickness = 1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    drawVirtualLine(x0, y0, x1, y1);
    //Octants 1 and 2
    if (dx > dy) {
        int err = dx / 2;

        while (x0 != x1) {
            drawThickDisc(x0, y0, thickness);

            err -= dy;
            if (err < 0) {
                y0 += sy;
                err += dx;
            }
            x0 += sx;
        }
    }
    //Octants 3 and 4
    else {
        int err = dy / 2;

        while (y0 != y1) {
            drawThickDisc(x0, y0, thickness);

            err -= dx;
            if (err < 0) {
                x0 += sx;
                err += dy;
            }
            y0 += sy;
        }
    }
}

void drawCircleMidPoint(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 1 - r;

    drawVirtualCircle(xc, yc, r);
    // 8 octants
    auto drawSymmetry = [&](int x, int y) {
        // One call for each octant
        drawDiscAt(xc + x, yc + y);
        drawDiscAt(xc - x, yc + y);
        drawDiscAt(xc + x, yc - y);
        drawDiscAt(xc - x, yc - y);
        drawDiscAt(xc + y, yc + x);
        drawDiscAt(xc - y, yc + x);
        drawDiscAt(xc + y, yc - x);
        drawDiscAt(xc - y, yc - x);

        rasterizeLineBresenham(xc - x, yc + y, xc + x, yc + y, 0);
        rasterizeLineBresenham(xc - x, yc - y, xc + x, yc - y, 0);
        rasterizeLineBresenham(xc - y, yc + x, xc + y, yc + x, 0);
        rasterizeLineBresenham(xc - y, yc - x, xc + y, yc - x, 0);
        };

    drawSymmetry(x, y);

    for (int i = 0; i <= x; i++) {
        drawSymmetry(i, y);
    }

    // Incremental approach for the circle's edge calculation
    while (x < y) {
        if (d < 0) {
            d += 2 * x + 3; // East
        }
        else {
            d += 2 * (x - y) + 5; // South-East
            y--;
        }
        x++;

        drawSymmetry(x, y);
    }
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    drawGrid();

    /*rasterizeLine(7, 3, 17, 1);
    rasterizeLine(17, 1, 27, 7);
    rasterizeLine(27, 7, 29, 17);
    rasterizeLine(29, 17, 23, 27);
    rasterizeLine(23, 27, 13, 29);
    rasterizeLine(13, 29, 3, 23);
    rasterizeLine(3, 23, 1, 12);
    rasterizeLine(1, 12, 7 ,3);*/

    /*rasterizeLineBresenham(7, 3, 17, 1);
    rasterizeLineBresenham(17, 1, 27, 7);
    rasterizeLineBresenham(27, 7, 29, 17);
    rasterizeLineBresenham(29, 17, 23, 27);
    rasterizeLineBresenham(23, 27, 13, 29);
    rasterizeLineBresenham(13, 29, 3, 23);
    rasterizeLineBresenham(3, 23, 1, 12);
    rasterizeLineBresenham(1, 12, 7, 3);*/

    drawCircleMidPoint(16, 16, 13);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow("30x30 Raster Grid");

    glClearColor(1.0, 1.0, 1.0, 1.0); // white background

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
