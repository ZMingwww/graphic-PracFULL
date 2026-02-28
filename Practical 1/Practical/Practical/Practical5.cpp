#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>
#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define WINDOW_TITLE "Enhanced London Tower Bridge - SPACE: open/close, P: projection, B: bridge only, C: camera presets, R: reset, Arrows: rotate, +/-: zoom, WASD: ortho pan, Z/X: ortho zoom, O: ortho reset, 1-4: ortho views, T: time, L: lighting"

// Global variables for bridge animation and camera
float bridgeAngle = 0.0f;
bool bridgeOpening = false;
bool bridgeClosing = false;
float maxBridgeAngle = 85.0f;
float bridgeSpeed = 1.5f;

// Enhanced camera controls with multiple preset viewing angles
float cameraAngleX = 5.0f;
float cameraAngleY = 15.0f;
float cameraDistance = 80.0f;

// Camera presets for different bridge viewing angles
struct CameraPreset {
    float angleX, angleY, distance;
    const char* name;
};

CameraPreset cameraPresets[] = {
    {5.0f, 15.0f, 80.0f, "Front View"},
    {-10.0f, 45.0f, 50.0f, "Side View"},
    {-30.0f, 0.0f, 40.0f, "Top View"},
    {-5.0f, 90.0f, 35.0f, "End View"},
    {-20.0f, 135.0f, 45.0f, "Diagonal View"}
};

int currentPreset = 0;

// Enhanced projection control
bool usePerspective = true; // true = perspective, false = orthographic
float orthoSize = 80.0f; // Size for orthographic projection
float orthoZoom = 1.0f; // Zoom factor for orthographic view
float orthoOffsetX = 0.0f; // X offset for orthographic view
float orthoOffsetY = 0.0f; // Y offset for orthographic view

// Water animation variables
float waterTime = 0.0f;
float waveAmplitude = 0.3f;
float waveFrequency = 0.8f;

// Dynamic lighting variables
float timeOfDay = 0.0f;
float lightingSpeed = 0.005f;
bool autoTimeProgression = true;

// Display mode control
bool showOnlyBridge = false; // Toggle to show only bridge components

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            break;
        case VK_SPACE:
            if (!bridgeOpening && !bridgeClosing)
            {
                if (bridgeAngle == 0.0f)
                    bridgeOpening = true;
                else if (bridgeAngle >= maxBridgeAngle)
                    bridgeClosing = true;
            }
            break;
        case 'P': // Toggle projection mode
        case 'p':
            usePerspective = !usePerspective;
            break;
        case VK_LEFT:
            cameraAngleY -= 3.0f;
            break;
        case VK_RIGHT:
            cameraAngleY += 3.0f;
            break;
        case VK_UP:
            cameraAngleX -= 3.0f;
            break;
        case VK_DOWN:
            cameraAngleX += 3.0f;
            break;
        case VK_ADD:
        case 187: // + key
            if (usePerspective)
            {
                cameraDistance -= 2.0f;
                if (cameraDistance < 10.0f) cameraDistance = 10.0f;
            }
            else
            {
                orthoSize -= 5.0f;
                if (orthoSize < 20.0f) orthoSize = 20.0f;
            }
            break;
        case VK_SUBTRACT:
        case 189: // - key
            if (usePerspective)
            {
                cameraDistance += 2.0f;
                if (cameraDistance > 150.0f) cameraDistance = 150.0f;
            }
            else
            {
                orthoSize += 5.0f;
                if (orthoSize > 300.0f) orthoSize = 300.0f;
            }
            break;
        case 'T': // Toggle time progression
        case 't':
            autoTimeProgression = !autoTimeProgression;
            break;
        case 'L': // Manual time control
        case 'l':
            timeOfDay += 0.1f;
            if (timeOfDay > 1.0f) timeOfDay = 0.0f;
            break;
        case 'C': // Cycle through camera presets (only in perspective mode)
        case 'c':
            if (usePerspective)
            {
                currentPreset = (currentPreset + 1) % 5;
                cameraAngleX = cameraPresets[currentPreset].angleX;
                cameraAngleY = cameraPresets[currentPreset].angleY;
                cameraDistance = cameraPresets[currentPreset].distance;
            }
            break;
        case 'R': // Reset camera to default
        case 'r':
            cameraAngleX = 5.0f;
            cameraAngleY = 15.0f;
            cameraDistance = 80.0f;
            currentPreset = 0;
            break;
        case 'Z': // Zoom in orthographic view
        case 'z':
            if (!usePerspective)
            {
                orthoZoom *= 0.9f;
                if (orthoZoom < 0.1f) orthoZoom = 0.1f;
            }
            break;
        case 'X': // Zoom out orthographic view
        case 'x':
            if (!usePerspective)
            {
                orthoZoom *= 1.1f;
                if (orthoZoom > 5.0f) orthoZoom = 5.0f;
            }
            break;
        case 'W': // Move orthographic view up
        case 'w':
            if (!usePerspective)
            {
                orthoOffsetY += 2.0f;
            }
            break;
        case 'S': // Move orthographic view down
        case 's':
            if (!usePerspective)
            {
                orthoOffsetY -= 2.0f;
            }
            break;
        case 'A': // Move orthographic view left
        case 'a':
            if (!usePerspective)
            {
                orthoOffsetX -= 2.0f;
            }
            break;
        case 'D': // Move orthographic view right
        case 'd':
            if (!usePerspective)
            {
                orthoOffsetX += 2.0f;
            }
            break;
        case 'O': // Reset orthographic view
        case 'o':
            if (!usePerspective)
            {
                orthoZoom = 1.0f;
                orthoOffsetX = 0.0f;
                orthoOffsetY = 0.0f;
                orthoSize = 80.0f;
            }
            break;
        case 'B': // Toggle bridge-only view
        case 'b':
            showOnlyBridge = !showOnlyBridge;
            break;
        case '1': // Orthographic Front View
            if (!usePerspective)
            {
                cameraAngleX = 0.0f;
                cameraAngleY = 0.0f;
            }
            break;
        case '2': // Orthographic Side View
            if (!usePerspective)
            {
                cameraAngleX = 0.0f;
                cameraAngleY = 90.0f;
            }
            break;
        case '3': // Orthographic Top View
            if (!usePerspective)
            {
                cameraAngleX = 90.0f;
                cameraAngleY = 0.0f;
            }
            break;
        case '4': // Orthographic Isometric View
            if (!usePerspective)
            {
                cameraAngleX = 30.0f;
                cameraAngleY = 45.0f;
            }
            break;
        }
        break;
    default:
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool initPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.cAlphaBits = 8;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 0;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;

    int n = ChoosePixelFormat(hdc, &pfd);
    return SetPixelFormat(hdc, n, &pfd);
}

// Enhanced geometry functions
void drawCylinder(float radius, float height, int segments)
{
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * (float)i / (float)segments;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);
        glVertex3f(x, 0, z);
        glVertex3f(x, height, z);
    }
    glEnd();

    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, height, 0);
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * (float)i / (float)segments;
        glVertex3f(radius * cosf(angle), height, radius * sinf(angle));
    }
    glEnd();

    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for (int i = segments; i >= 0; i--)
    {
        float angle = 2.0f * M_PI * (float)i / (float)segments;
        glVertex3f(radius * cosf(angle), 0, radius * sinf(angle));
    }
    glEnd();
}

void drawCube(float width, float height, float depth)
{
    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;

    glBegin(GL_QUADS);
    // All faces with proper normals
    glNormal3f(0, 0, 1); // Front
    glVertex3f(-w, -h, d); glVertex3f(w, -h, d); glVertex3f(w, h, d); glVertex3f(-w, h, d);

    glNormal3f(0, 0, -1); // Back
    glVertex3f(-w, -h, -d); glVertex3f(-w, h, -d); glVertex3f(w, h, -d); glVertex3f(w, -h, -d);

    glNormal3f(0, 1, 0); // Top
    glVertex3f(-w, h, -d); glVertex3f(-w, h, d); glVertex3f(w, h, d); glVertex3f(w, h, -d);

    glNormal3f(0, -1, 0); // Bottom
    glVertex3f(-w, -h, -d); glVertex3f(w, -h, -d); glVertex3f(w, -h, d); glVertex3f(-w, -h, d);

    glNormal3f(1, 0, 0); // Right
    glVertex3f(w, -h, -d); glVertex3f(w, h, -d); glVertex3f(w, h, d); glVertex3f(w, -h, d);

    glNormal3f(-1, 0, 0); // Left
    glVertex3f(-w, -h, -d); glVertex3f(-w, -h, d); glVertex3f(-w, h, d); glVertex3f(-w, h, -d);
    glEnd();
}

void drawPyramid(float base, float height)
{
    float b = base / 2.0f;

    glBegin(GL_TRIANGLES);
    // Front face
    glNormal3f(0, 0.707f, 0.707f);
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(-b, 0.0f, b);
    glVertex3f(b, 0.0f, b);

    // Right face
    glNormal3f(0.707f, 0.707f, 0);
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(b, 0.0f, b);
    glVertex3f(b, 0.0f, -b);

    // Back face
    glNormal3f(0, 0.707f, -0.707f);
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(b, 0.0f, -b);
    glVertex3f(-b, 0.0f, -b);

    // Left face
    glNormal3f(-0.707f, 0.707f, 0);
    glVertex3f(0.0f, height, 0.0f);
    glVertex3f(-b, 0.0f, -b);
    glVertex3f(-b, 0.0f, b);
    glEnd();

    // Base
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(-b, 0.0f, b);
    glVertex3f(-b, 0.0f, -b);
    glVertex3f(b, 0.0f, -b);
    glVertex3f(b, 0.0f, b);
    glEnd();
}

void drawSphere(float radius, int segments)
{
    for (int i = 0; i < segments; i++)
    {
        float lat0 = M_PI * (-0.5f + (float)i / (float)segments);
        float lat1 = M_PI * (-0.5f + (float)(i + 1) / (float)segments);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= segments; j++)
        {
            float lng = 2.0f * M_PI * (float)j / (float)segments;

            float x0 = cosf(lat0) * cosf(lng);
            float y0 = sinf(lat0);
            float z0 = cosf(lat0) * sinf(lng);

            float x1 = cosf(lat1) * cosf(lng);
            float y1 = sinf(lat1);
            float z1 = cosf(lat1) * sinf(lng);

            glNormal3f(x0, y0, z0);
            glVertex3f(radius * x0, radius * y0, radius * z0);
            glNormal3f(x1, y1, z1);
            glVertex3f(radius * x1, radius * y1, radius * z1);
        }
        glEnd();
    }
}

// Enhanced Tower Bridge tower with Victorian Gothic architecture
void drawDetailedTower(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 0, z);

    // Main tower base (weathered Portland stone color)
    glColor3f(0.85f, 0.82f, 0.75f);

    // Foundation/base with ornate details
    glPushMatrix();
    glTranslatef(0, 2, 0);
    drawCube(10, 4, 10);
    glPopMatrix();

    // Multiple decorative foundation moldings
    glColor3f(0.75f, 0.71f, 0.65f);
    for (int i = 0; i < 3; i++)
    {
        float moldingY = 4.2f + i * 0.3f;
        float moldingSize = 10.5f + i * 0.2f;
        glPushMatrix();
        glTranslatef(0, moldingY, 0);
        drawCube(moldingSize, 0.2f, moldingSize);
        glPopMatrix();
    }

    // Lower tower section with architectural details
    glColor3f(0.85f, 0.82f, 0.75f);
    glPushMatrix();
    glTranslatef(0, 8, 0);
    drawCube(8, 12, 8);
    
    // Decorative pilasters (vertical columns)
    glColor3f(0.88f, 0.85f, 0.78f);
    for (int i = 0; i < 4; i++)
    {
        float pilasterX = (i % 2) * 6 - 3;
        float pilasterZ = (i / 2) * 6 - 3;
        glPushMatrix();
        glTranslatef(pilasterX, 0, pilasterZ);
        drawCube(0.8f, 12, 0.8f);
        glPopMatrix();
    }
    glPopMatrix();

    // Middle tower section with Gothic arches
    glColor3f(0.87f, 0.84f, 0.78f);
    glPushMatrix();
    glTranslatef(0, 18, 0);
    drawCube(7, 8, 7);
    
    // Gothic arched windows
    glColor3f(0.3f, 0.3f, 0.4f);
    for (int i = 0; i < 4; i++)
    {
        float windowX = (i % 2) * 4 - 2;
        float windowZ = (i / 2) * 4 - 2;
        
        // Arched window opening
        glPushMatrix();
        glTranslatef(windowX, 2, windowZ + 3.6f);
        drawCube(1.5f, 4, 0.2f);
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(windowX, 2, windowZ - 3.6f);
        drawCube(1.5f, 4, 0.2f);
        glPopMatrix();
    }
    glPopMatrix();

    // Upper tower section with clock faces
    glColor3f(0.90f, 0.87f, 0.80f);
    glPushMatrix();
    glTranslatef(0, 25, 0);
    drawCube(6, 6, 6);
    
    // Clock faces on each side
    glColor3f(0.95f, 0.95f, 0.9f);
    for (int i = 0; i < 4; i++)
    {
        float clockX = (i % 2) * 3 - 1.5f;
        float clockZ = (i / 2) * 3 - 1.5f;
        
        glPushMatrix();
        glTranslatef(clockX, 0, clockZ + 3.1f);
        drawCube(2.5f, 2.5f, 0.1f);
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(clockX, 0, clockZ - 3.1f);
        drawCube(2.5f, 2.5f, 0.1f);
        glPopMatrix();
    }
    glPopMatrix();

    // Corner turrets with Victorian details
    glColor3f(0.75f, 0.71f, 0.65f);
    for (int i = 0; i < 4; i++)
    {
        float turretX = (i % 2) * 6 - 3;
        float turretZ = (i / 2) * 6 - 3;

        // Turret cylinder
        glPushMatrix();
        glTranslatef(turretX, 28, turretZ);
        drawCylinder(1.2f, 12, 12);
        glPopMatrix();

        // Turret conical roof
        glColor3f(0.35f, 0.5f, 0.35f);
        glPushMatrix();
        glTranslatef(turretX, 40, turretZ);
        drawPyramid(3, 5);
        glPopMatrix();

        // Decorative finial
        glColor3f(0.8f, 0.7f, 0.3f);
        glPushMatrix();
        glTranslatef(turretX, 45.5f, turretZ);
        drawSphere(0.3f, 8);
        glPopMatrix();

        glColor3f(0.75f, 0.71f, 0.65f);
    }

    // Gothic arched openings in lower section
    glColor3f(0.2f, 0.2f, 0.3f);
    glPushMatrix();
    glTranslatef(0, 8, 4.1f);
    drawCube(3, 8, 0.2f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 8, -4.1f);
    drawCube(3, 8, 0.2f);
    glPopMatrix();

    // Suspension chains and cables connecting towers to walkways
    glColor3f(0.4f, 0.4f, 0.4f); // Dark iron color
    
    // Main suspension cables
    for (int i = 0; i < 8; i++)
    {
        float cableX = (i - 3.5f) * 4.0f;
        
        // Cable from tower to walkway
        glBegin(GL_LINES);
        glVertex3f(cableX, 25, 0);  // Tower connection
        glVertex3f(cableX, 28, 0);  // Walkway connection
        glEnd();
        
        // Vertical suspension hangers
        for (int j = 0; j < 3; j++)
        {
            float hangerX = cableX + (j - 1) * 2.0f;
            glBegin(GL_LINES);
            glVertex3f(hangerX, 28, 0);
            glVertex3f(hangerX, 26, 0);
            glEnd();
        }
    }

    glPopMatrix();
}

// Detailed high-level walkways
void drawDetailedWalkways()
{
    // Main walkway structure - Tower Bridge blue
    glColor3f(0.25f, 0.45f, 0.75f);
    glPushMatrix();
    glTranslatef(0, 28, 0);
    drawCube(35, 3, 6);
    glPopMatrix();

    // White decorative trim on walkway
    glColor3f(0.95f, 0.95f, 0.9f);
    glPushMatrix();
    glTranslatef(0, 29.2f, 0);
    drawCube(35.2f, 0.4f, 6.2f);
    glPopMatrix();

    // Glass windows
    glColor3f(0.6f, 0.8f, 0.9f);
    for (int i = 0; i < 14; i++)
    {
        float windowX = (i - 6.5f) * 2.2f;

        // Side windows
        glPushMatrix();
        glTranslatef(windowX, 28, 3.1f);
        drawCube(1.8f, 2, 0.1f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(windowX, 28, -3.1f);
        drawCube(1.8f, 2, 0.1f);
        glPopMatrix();
    }

    // Walkway support structure
    glColor3f(0.2f, 0.4f, 0.7f);
    for (int i = 0; i < 10; i++)
    {
        float supportX = (i - 4.5f) * 3.5f;
        glPushMatrix();
        glTranslatef(supportX, 28, 0);
        drawCube(0.5f, 3, 5);
        glPopMatrix();
    }
}

void drawRealisticBascule(float x, float width, bool isMovable, float angle)
{
    glPushMatrix();
    glTranslatef(x, 0, 0);

    if (isMovable && angle > 0)
    {
        if (x < 0) // Left bascule
        {
            glTranslatef(-width / 2, 12, 0);
            glRotatef(angle, 0, 0, 1);
            glTranslatef(width / 2, -12, 0);
        }
        else // Right bascule  
        {
            glTranslatef(width / 2, 12, 0);
            glRotatef(-angle, 0, 0, 1);
            glTranslatef(-width / 2, -12, 0);
        }
    }

    // Main deck surface
    glColor3f(0.45f, 0.45f, 0.4f);
    glPushMatrix();
    glTranslatef(0, 12, 0);
    drawCube(width, 1.5f, 12);
    glPopMatrix();
    
    // Road markings and lane lines
    glColor3f(0.95f, 0.95f, 0.95f); // White road markings
    for (int i = 0; i < 3; i++)
    {
        float lineX = (i - 1) * width / 3.0f;
        glPushMatrix();
        glTranslatef(lineX, 12.8f, 0);
        drawCube(0.2f, 0.1f, 11.5f);
        glPopMatrix();
    }
    
    // Center dashed line
    glColor3f(0.95f, 0.95f, 0.95f);
    for (int i = 0; i < 6; i++)
    {
        float dashZ = (i - 2.5f) * 2.0f;
        glPushMatrix();
        glTranslatef(0, 12.8f, dashZ);
        drawCube(0.3f, 0.1f, 1.5f);
        glPopMatrix();
    }

    // Side barriers - Tower Bridge blue
    glColor3f(0.25f, 0.45f, 0.75f);
    glPushMatrix();
    glTranslatef(0, 15, 5);
    drawCube(width, 5, 1.5f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 15, -5);
    drawCube(width, 5, 1.5f);
    glPopMatrix();

    // Decorative ironwork pattern
    glColor3f(0.9f, 0.9f, 0.85f);
    for (int i = 0; i < (int)(width / 2.0f); i++)
    {
        float ironX = ((float)i - width / 4.0f) * 2.0f;

        // Vertical supports
        glPushMatrix();
        glTranslatef(ironX, 15, 4.5f);
        drawCube(0.3f, 4, 0.3f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(ironX, 15, -4.5f);
        drawCube(0.3f, 4, 0.3f);
        glPopMatrix();
    }

    // Underside support structure
    glColor3f(0.5f, 0.5f, 0.45f);
    for (int i = 0; i < 3; i++)
    {
        float beamX = ((float)i - 1.0f) * width / 3.0f;
        glPushMatrix();
        glTranslatef(beamX, 9, 0);
        drawCube(1, 4, 10);
        glPopMatrix();
    }
    
    // Counterweight mechanism (only for movable bascules)
    if (isMovable)
    {
        // Counterweight box
        glColor3f(0.3f, 0.3f, 0.35f);
        float counterweightX = (x < 0) ? -width * 0.8f : width * 0.8f;
        glPushMatrix();
        glTranslatef(counterweightX, 6, 0);
        drawCube(3, 4, 8);
        glPopMatrix();
        
        // Counterweight support arms
        glColor3f(0.4f, 0.4f, 0.45f);
        for (int i = 0; i < 2; i++)
        {
            float armZ = (i - 0.5f) * 6.0f;
            glPushMatrix();
            glTranslatef(counterweightX, 8, armZ);
            drawCube(0.3f, 2, 0.3f);
            glPopMatrix();
        }
        
        // Pivot mechanism
        glColor3f(0.2f, 0.2f, 0.25f);
        glPushMatrix();
        glTranslatef(0, 12, 0);
        drawCylinder(1.5f, 2, 12);
        glPopMatrix();
    }

    glPopMatrix();
}

// Enhanced river with realistic water effects
void drawRealisticWater()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Multi-layered water with different wave patterns
    // Base water layer
    glColor4f(0.12f, 0.32f, 0.48f, 0.9f);

    glBegin(GL_QUADS);
    for (int x = -50; x < 50; x++)
    {
        for (int z = -20; z < 20; z++)
        {
            float x1 = (float)x * 2.0f;
            float z1 = (float)z * 2.0f;
            float x2 = (float)(x + 1) * 2.0f;
            float z2 = (float)(z + 1) * 2.0f;

            // Multiple wave patterns for realistic water
            float wave1 = sinf((x1 * 0.08f + waterTime * 1.8f)) * waveAmplitude;
            float wave2 = cosf((z1 * 0.12f + waterTime * 1.3f)) * waveAmplitude * 0.6f;
            float wave3 = sinf((x1 * 0.15f + z1 * 0.1f + waterTime * 2.2f)) * waveAmplitude * 0.4f;
            float y1 = wave1 + wave2 + wave3;

            float y2 = sinf(((x2) * 0.08f + waterTime * 1.8f)) * waveAmplitude +
                cosf(((z1) * 0.12f + waterTime * 1.3f)) * waveAmplitude * 0.6f +
                sinf(((x2) * 0.15f + z1 * 0.1f + waterTime * 2.2f)) * waveAmplitude * 0.4f;
            float y3 = sinf(((x2) * 0.08f + waterTime * 1.8f)) * waveAmplitude +
                cosf(((z2) * 0.12f + waterTime * 1.3f)) * waveAmplitude * 0.6f +
                sinf(((x2) * 0.15f + z2 * 0.1f + waterTime * 2.2f)) * waveAmplitude * 0.4f;
            float y4 = sinf(((x1) * 0.08f + waterTime * 1.8f)) * waveAmplitude +
                cosf(((z2) * 0.12f + waterTime * 1.3f)) * waveAmplitude * 0.6f +
                sinf(((x1) * 0.15f + z2 * 0.1f + waterTime * 2.2f)) * waveAmplitude * 0.4f;

            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z1);
            glVertex3f(x2, y3, z2);
            glVertex3f(x1, y4, z2);
        }
    }
    glEnd();
    
    // Foam and highlights layer
    glColor4f(0.8f, 0.85f, 0.9f, 0.3f);
    glBegin(GL_QUADS);
    for (int x = -50; x < 50; x += 2)
    {
        for (int z = -20; z < 20; z += 2)
        {
            float x1 = (float)x * 2.0f;
            float z1 = (float)z * 2.0f;
            float x2 = (float)(x + 2) * 2.0f;
            float z2 = (float)(z + 2) * 2.0f;

            // Foam on wave crests
            float foam1 = sinf((x1 * 0.05f + waterTime * 1.5f)) * 0.1f;
            float foam2 = cosf((z1 * 0.08f + waterTime * 1.2f)) * 0.1f;
            float y1 = foam1 + foam2 + 0.05f;

            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y1, z1);
            glVertex3f(x2, y1, z2);
            glVertex3f(x1, y1, z2);
        }
    }
    glEnd();

    glDisable(GL_BLEND);

    // River banks
    glColor3f(0.35f, 0.5f, 0.3f);
    glPushMatrix();
    glTranslatef(0, -1, 45);
    drawCube(200, 2, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -1, -45);
    drawCube(200, 2, 10);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

// River traffic and boats
void drawRiverTraffic()
{
    // River boats
    for (int i = 0; i < 3; i++)
    {
        float boatX = -40.0f + i * 40.0f;
        float boatZ = -15.0f + (i % 2) * 30.0f;
        float boatY = sinf(waterTime * 0.5f + i) * 0.2f + 0.5f;
        
        // Boat hull
        glColor3f(0.6f, 0.4f, 0.2f); // Brown hull
        glPushMatrix();
        glTranslatef(boatX, boatY, boatZ);
        drawCube(8, 1.5f, 3);
        glPopMatrix();
        
        // Boat cabin
        glColor3f(0.8f, 0.7f, 0.5f); // Light brown cabin
        glPushMatrix();
        glTranslatef(boatX, boatY + 1.2f, boatZ);
        drawCube(4, 2, 2.5f);
        glPopMatrix();
        
        // Boat wake
        glColor4f(0.9f, 0.9f, 0.95f, 0.4f);
        glEnable(GL_BLEND);
        glBegin(GL_QUADS);
        for (int j = 0; j < 5; j++)
        {
            float wakeX = boatX - j * 2.0f;
            float wakeY = boatY + 0.1f;
            glVertex3f(wakeX - 1, wakeY, boatZ - 2);
            glVertex3f(wakeX + 1, wakeY, boatZ - 2);
            glVertex3f(wakeX + 1, wakeY, boatZ + 2);
            glVertex3f(wakeX - 1, wakeY, boatZ + 2);
        }
        glEnd();
        glDisable(GL_BLEND);
    }
}

// Seagulls flying around
void drawSeagulls()
{
    glColor3f(0.95f, 0.95f, 0.95f); // White seagulls
    
    for (int i = 0; i < 5; i++)
    {
        float birdX = -30.0f + i * 15.0f;
        float birdY = 20.0f + sinf(waterTime * 0.8f + i) * 3.0f;
        float birdZ = -10.0f + cosf(waterTime * 0.6f + i) * 20.0f;
        
        // Simple seagull representation (small sphere)
        glPushMatrix();
        glTranslatef(birdX, birdY, birdZ);
        drawSphere(0.3f, 6);
        glPopMatrix();
    }
}

// Street lighting fixtures
void drawStreetLamps()
{
    glColor3f(0.2f, 0.2f, 0.25f); // Dark metal
    
    // Street lamps along the bridge
    for (int i = 0; i < 8; i++)
    {
        float lampX = (i - 3.5f) * 8.0f;
        
        // Lamp post
        glPushMatrix();
        glTranslatef(lampX, 6, 6.5f);
        drawCylinder(0.2f, 12, 8);
        glPopMatrix();
        
        // Lamp head
        glColor3f(0.9f, 0.9f, 0.7f); // Warm light color
        glPushMatrix();
        glTranslatef(lampX, 18, 6.5f);
        drawSphere(0.8f, 8);
        glPopMatrix();
        
        glColor3f(0.2f, 0.2f, 0.25f);
    }
    
    // Tower lighting
    glColor3f(0.9f, 0.9f, 0.7f);
    for (int tower = 0; tower < 2; tower++)
    {
        float towerX = (tower - 0.5f) * 30.0f;
        
        // Tower top lighting
        glPushMatrix();
        glTranslatef(towerX, 31, 0);
        drawSphere(1.2f, 8);
        glPopMatrix();
        
        // Walkway lighting
        for (int i = 0; i < 3; i++)
        {
            float lightX = towerX + (i - 1) * 8.0f;
            glPushMatrix();
            glTranslatef(lightX, 30, 0);
            drawSphere(0.6f, 6);
            glPopMatrix();
        }
    }
}

// London cityscape background
void drawCityBackground()
{
    glColor3f(0.6f, 0.6f, 0.55f);

    // Buildings on north bank
    for (int i = 0; i < 15; i++)
    {
        float buildX = (i - 7) * 12;
        float buildHeight = 8 + (i % 4) * 6;

        if (fabs(buildX) > 25.0f)
        {
            glPushMatrix();
            glTranslatef(buildX, buildHeight / 2, 60);
            drawCube(8, buildHeight, 8);
            glPopMatrix();
            
            // Building windows (lights)
            glColor3f(0.9f, 0.8f, 0.6f);
            for (int floor = 0; floor < (int)(buildHeight / 3); floor++)
            {
                for (int window = 0; window < 2; window++)
                {
                    float windowX = buildX + (window - 0.5f) * 3.0f;
                    float windowY = floor * 3.0f + buildHeight / 2 - 1.5f;
                    glPushMatrix();
                    glTranslatef(windowX, windowY, 64.1f);
                    drawCube(1.5f, 2, 0.1f);
                    glPopMatrix();
                }
            }
            glColor3f(0.6f, 0.6f, 0.55f);
        }
    }

    // Buildings on south bank  
    for (int i = 0; i < 12; i++)
    {
        float buildX = (i - 6) * 15;
        float buildHeight = 6 + (i % 3) * 4;

        if (fabs(buildX) > 30.0f)
        {
            glPushMatrix();
            glTranslatef(buildX, buildHeight / 2, -65);
            drawCube(10, buildHeight, 10);
            glPopMatrix();
            
            // Building windows (lights)
            glColor3f(0.9f, 0.8f, 0.6f);
            for (int floor = 0; floor < (int)(buildHeight / 3); floor++)
            {
                for (int window = 0; window < 3; window++)
                {
                    float windowX = buildX + (window - 1) * 3.0f;
                    float windowY = floor * 3.0f + buildHeight / 2 - 1.5f;
                    glPushMatrix();
                    glTranslatef(windowX, windowY, -69.1f);
                    drawCube(1.5f, 2, 0.1f);
                    glPopMatrix();
                }
            }
            glColor3f(0.6f, 0.6f, 0.55f);
        }
    }
}

void setupProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (usePerspective)
    {
        // Enhanced perspective projection for better bridge viewing
        gluPerspective(60.0f, 800.0f / 600.0f, 0.5f, 500.0f);
    }
    else
    {
        // Enhanced orthographic projection with zoom and pan controls
        float aspect = 800.0f / 600.0f;
        float scaledSize = orthoSize * orthoZoom;
        
        glOrtho(-scaledSize * aspect + orthoOffsetX, scaledSize * aspect + orthoOffsetX,
            -scaledSize + orthoOffsetY, scaledSize + orthoOffsetY,
            -500.0f, 500.0f);
    }

    glMatrixMode(GL_MODELVIEW);
}

void updateBridgeAnimation()
{
    if (bridgeOpening)
    {
        bridgeAngle += bridgeSpeed;
        if (bridgeAngle >= maxBridgeAngle)
        {
            bridgeAngle = maxBridgeAngle;
            bridgeOpening = false;
        }
    }
    else if (bridgeClosing)
    {
        bridgeAngle -= bridgeSpeed;
        if (bridgeAngle <= 0.0f)
        {
            bridgeAngle = 0.0f;
            bridgeClosing = false;
        }
    }

    // Update water animation
    waterTime += 0.02f;
    if (waterTime > 2.0f * M_PI * 10.0f) waterTime = 0.0f;

    // Update time of day
    if (autoTimeProgression)
    {
        timeOfDay += lightingSpeed;
        if (timeOfDay > 1.0f) timeOfDay = 0.0f;
    }
}

void display()
{
    updateBridgeAnimation();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup projection matrix
    setupProjection();

    glLoadIdentity();

    // Enhanced camera positioning for better bridge viewing
    if (usePerspective)
    {
        // Perspective: Use distance-based positioning
        glTranslatef(0, -5, -cameraDistance);
        glRotatef(cameraAngleX, 1, 0, 0);
        glRotatef(cameraAngleY, 0, 1, 0);
        glTranslatef(0, 2, 0);
    }
    else
    {
        // Orthographic: Focused on Tower Bridge structure
        glTranslatef(0, -25, -orthoSize * 0.25f);
        glRotatef(cameraAngleX, 1, 0, 0);
        glRotatef(cameraAngleY, 0, 1, 0);
        glTranslatef(0, 20, 0);
    }

    // Dynamic lighting setup
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    // Simple lighting
    float lightPos[] = { 50.0f, 80.0f, 50.0f, 1.0f };
    float lightColor[] = { 1.0f, 1.0f, 0.9f, 1.0f };
    float ambientLight[] = { 0.3f, 0.3f, 0.4f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Draw bridge components based on display mode
    if (!showOnlyBridge)
    {
        // Draw all environmental elements
        drawRealisticWater();
        drawRiverTraffic();
        drawSeagulls();
        drawCityBackground();
        drawStreetLamps();
    }

    // Always draw bridge components
    // Simple ground plane for bridge-only view
    if (showOnlyBridge)
    {
        glColor3f(0.15f, 0.15f, 0.15f); // Very dark ground for technical drawing
        glPushMatrix();
        glTranslatef(0, -1, 0);
        drawCube(150, 2, 150);
        glPopMatrix();
    }

    // Main bridge towers
    drawDetailedTower(-15, 0);
    drawDetailedTower(15, 0);

    // High-level walkways
    drawDetailedWalkways();

    // Approach spans (fixed sections)
    drawRealisticBascule(-30, 24, false, 0);
    drawRealisticBascule(30, 24, false, 0);

    // Bascule bridges (opening sections)
    drawRealisticBascule(-7.5f, 15, true, bridgeAngle);
    drawRealisticBascule(7.5f, 15, true, bridgeAngle);

    glDisable(GL_LIGHTING);

    // Display projection mode text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, 580);

    const char* projectionText = usePerspective ? "Projection: PERSPECTIVE (Press P to toggle)" : "Projection: ORTHOGRAPHIC (Press P to toggle)";
    for (int i = 0; projectionText[i] != '\0'; i++)
    {
        // Simple character display - in a real implementation you'd use a font rendering library
    }
    
    // Display current camera preset
    glRasterPos2f(10, 560);
    const char* cameraText = cameraPresets[currentPreset].name;
    for (int i = 0; cameraText[i] != '\0'; i++)
    {
        // Simple character display
    }
    
    // Display orthographic controls when in ortho mode
    if (!usePerspective)
    {
        glRasterPos2f(10, 540);
        const char* orthoText = "Ortho Controls: WASD=pan, Z/X=zoom, O=reset, 1-4=views";
        for (int i = 0; orthoText[i] != '\0'; i++)
        {
            // Simple character display
        }
        
        glRasterPos2f(10, 520);
        const char* viewText = "Views: 1=Front, 2=Side, 3=Top, 4=Isometric";
        for (int i = 0; viewText[i] != '\0'; i++)
        {
            // Simple character display
        }
    }
    
    // Display bridge-only mode status
    if (showOnlyBridge)
    {
        glRasterPos2f(10, 520);
        const char* bridgeText = "Bridge Only Mode (Press B to toggle)";
        for (int i = 0; bridgeText[i] != '\0'; i++)
        {
            // Simple character display
        }
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WindowProcedure;
    wc.lpszClassName = WINDOW_TITLE;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClassEx(&wc)) return false;

    HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768,
        NULL, NULL, wc.hInstance, NULL);

    HDC hdc = GetDC(hWnd);
    initPixelFormat(hdc);
    HGLRC hglrc = wglCreateContext(hdc);

    if (!wglMakeCurrent(hdc, hglrc)) return false;

    // Enhanced OpenGL initialization with better rendering settings
    glClearColor(0.65f, 0.8f, 0.95f, 1.0f); // Slightly cooler sky color
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    
    // Enable fog for atmospheric effect
    glEnable(GL_FOG);
    float fogColor[] = {0.65f, 0.8f, 0.95f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 80.0f);
    glFogf(GL_FOG_END, 200.0f);

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        display();
        SwapBuffers(hdc);
        Sleep(16); // ~60 FPS
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
    UnregisterClass(WINDOW_TITLE, wc.hInstance);
    return true;
}