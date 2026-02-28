#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>
#include <random> // For random sprinkles generation

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

#define WINDOW_TITLE "Practical 4B"

// ------------------------ Globals ------------------------
float rotY = 0.0f;             // rotation angle around Y axis
bool autoRotate = true;        // auto rotation flag
float camZ = 3.2f;             // distance from camera

// Lighting and material properties
GLfloat light_position[] = { 5.0f, 5.0f, 5.0f, 0.0f };
GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Background gradient colors
GLfloat bgColorTop[] = { 0.5f, 0.7f, 1.0f, 1.0f };     // Light blue
GLfloat bgColorBottom[] = { 0.9f, 0.6f, 0.8f, 1.0f };  // Pink-purple

// Sprinkles data
struct Sprinkle {
    float x, y, z;
    float r, g, b;
    float size;
    float angle;
};

const int MAX_SPRINKLES = 50;
Sprinkle sprinkles[MAX_SPRINKLES];

// ------------------------ Win32 --------------------------
LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY: PostQuitMessage(0); break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
        else if (wParam == 'R') autoRotate = !autoRotate;
        else if (wParam == VK_LEFT)  rotY -= 4.0f;
        else if (wParam == VK_RIGHT) rotY += 4.0f;
        else if (wParam == 'Z') camZ -= 0.1f;
        else if (wParam == 'X') camZ += 0.1f;
        break;
    default: break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool initPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cAlphaBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int pf = ChoosePixelFormat(hdc, &pfd);
    return SetPixelFormat(hdc, pf, &pfd) == TRUE;
}

// ---------- GLU helpers (solid & wire versions where needed) ----------
static GLUquadric* newQuad(GLenum style)
{
    GLUquadric* q = gluNewQuadric();
    gluQuadricDrawStyle(q, style);
    return q;
}

void drawSphereSolid(float r)
{
    GLUquadric* q = newQuad(GLU_FILL);
    gluSphere(q, r, 30, 24);
    gluDeleteQuadric(q);
}

void drawCylinderSolid(float br, float tr, float h)
{
    GLUquadric* c = newQuad(GLU_FILL);
    gluCylinder(c, br, tr, h, 30, 1);
    gluDeleteQuadric(c);
}

void drawDiskSolid(float inr, float outr)
{
    GLUquadric* d = newQuad(GLU_FILL);
    gluDisk(d, inr, outr, 30, 1);
    gluDeleteQuadric(d);
}

void drawDiskWire(float inr, float outr)
{
    GLUquadric* d = newQuad(GLU_LINE);
    gluDisk(d, inr, outr, 30, 1);
    gluDeleteQuadric(d);
}

// ---------- Ice-cream parts ---------------
void drawScoop(float radius, float r, float g, float b)
{
    // Set material properties for the scoop
    GLfloat mat_ambient[] = { r * 0.3f, g * 0.3f, b * 0.3f, 1.0f };
    GLfloat mat_diffuse[] = { r, g, b, 1.0f };
    GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess[] = { 30.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    // A slightly squashed sphere (so it looks like a scoop)
    glPushMatrix();
    glScalef(1.0f, 0.80f, 1.0f);
    glColor3f(r, g, b);       // custom color for each scoop
    drawSphereSolid(radius);
    glPopMatrix();
}

void drawWaferCone(float rTop, float h)
{
    const float rBase = 0.0f;
    const int segments = 24; // Increased segments for smoother cone

    // Set material properties for the cone
    GLfloat mat_ambient[] = { 0.3f, 0.2f, 0.1f, 1.0f };
    GLfloat mat_diffuse[] = { 0.62f, 0.45f, 0.27f, 1.0f };
    GLfloat mat_specular[] = { 0.3f, 0.2f, 0.1f, 1.0f };
    GLfloat mat_shininess[] = { 10.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    // Solid cone (brown) - pointing up
    glColor3f(0.62, 0.45, 0.27);
    glPushMatrix();
    glRotatef(90, 1, 0, 0); // point upwards in Y

    // Draw cone with straight lines using triangles
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 2.0f * 3.14159f;
        float angle2 = (float)(i + 1) / segments * 2.0f * 3.14159f;

        float x1 = rTop * cos(angle1);
        float z1 = rTop * sin(angle1);
        float x2 = rTop * cos(angle2);
        float z2 = rTop * sin(angle2);

        // Triangle from top point to base edge
        glVertex3f(0.0f, 0.0f, h);      // Apex of the cone
        glVertex3f(x1, z1, 0.0f);       // First point on base
        glVertex3f(x2, z2, 0.0f);       // Second point on base
    }
    glEnd();

    // Close the bottom of cone
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = (float)i / segments * 2.0f * 3.14159f;
        float x = rTop * cos(angle);
        float z = rTop * sin(angle);
        glVertex3f(x, z, 0.0f);
    }
    glEnd();
    glPopMatrix();

    // White lattice overlay (wire)
    glColor3f(1, 0.95f, 0.85f);
    glPushMatrix();
    glRotatef(90, 1, 0, 0);

    // Store points for vertical lines
    float points[6][segments][3]; // [ring][segment][x,y,z]

    // a few horizontal "rings"
    for (int i = 0; i < 6; ++i) {
        float z = (h / 6.0f) * i;
        float r = rTop * (1.0f - z / h);
        glPushMatrix();
        glTranslatef(0, 0, z);

        // Draw straight line rings
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < segments; j++) {
            float angle = (float)j / segments * 2.0f * 3.14159f;
            float x = r * cos(angle);
            float y = r * sin(angle);
            glVertex3f(x, y, 0.0f);

            // Store points for vertical lines
            points[i][j][0] = x;
            points[i][j][1] = y;
            points[i][j][2] = z;
        }
        glEnd();

        glPopMatrix();
    }

    // Draw vertical lines connecting the rings
    glBegin(GL_LINES);
    for (int j = 0; j < segments; j++) {
        for (int i = 0; i < 5; i++) { // Connect adjacent rings
            glVertex3f(points[i][j][0], points[i][j][1], points[i][j][2]);
            glVertex3f(points[i + 1][j][0], points[i + 1][j][1], points[i + 1][j][2]);
        }
    }
    glEnd();

    glPopMatrix();
}

void drawChocolateStick()
{
    glPushMatrix();
    glTranslatef(0.10f, 0.86f, -0.02f);
    glRotatef(20, 0, 0, 1);
    glRotatef(25, 1, 0, 0);
    glColor3f(0.30f, 0.15f, 0.05f);
    glRotatef(-90, 1, 0, 0);
    drawCylinderSolid(0.045f, 0.045f, 0.5f);
    glPopMatrix();
}

void drawCherry()
{
    // cherry
    glPushMatrix();
    glTranslatef(-0.15f, 0.92f, 0.07f);
    glColor3f(1.0f, 0.1f, 0.1f);
    drawSphereSolid(0.08f);

    // stem
    glColor3f(0.8f, 0.1f, 0.1f);
    glRotatef(-35, 0, 0, 1);
    glTranslatef(0.0, 0.09, 0.0);
    glRotatef(90, 1, 0, 0);
    drawCylinderSolid(0.008f, 0.008f, 0.18f);
    glPopMatrix();
}

void drawGoldLoop()
{
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(90, 1, 0, 0);
    
    // Set material properties for gold
    GLfloat mat_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
    GLfloat mat_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
    GLfloat mat_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
    GLfloat mat_shininess[] = { 51.2f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
    glColor3f(0.85f, 0.65f, 0.13f);  // Gold color

    // Draw a thin torus-like ring using a series of thin cylinders
    const int segments = 36;
    const float radius = 0.25f;
    const float thickness = 0.01f;

    for (int i = 0; i < segments; i++) {
        float angle1 = (float)i / segments * 2.0f * 3.14159f;
        float angle2 = (float)(i + 1) / segments * 2.0f * 3.14159f;

        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);

        glPushMatrix();
        glTranslatef(x1, z1, 0.0f);
        glRotatef(angle1 * 180.0f / 3.14159f, 0, 0, 1);
        drawCylinderSolid(thickness, thickness, sqrt((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1)));
        glPopMatrix();
    }

    glPopMatrix();
}

// Function to initialize random sprinkles
void initSprinkles() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistrib(-0.3f, 0.3f);
    std::uniform_real_distribution<float> heightDistrib(0.0f, 0.15f);
    std::uniform_real_distribution<float> colorDistrib(0.0f, 1.0f);
    std::uniform_real_distribution<float> sizeDistrib(0.01f, 0.03f);
    std::uniform_real_distribution<float> angleDistrib(0.0f, 360.0f);
    
    for (int i = 0; i < MAX_SPRINKLES; i++) {
        sprinkles[i].x = posDistrib(gen);
        sprinkles[i].y = heightDistrib(gen);
        sprinkles[i].z = posDistrib(gen);
        
        // Bright colors for sprinkles
        sprinkles[i].r = colorDistrib(gen);
        sprinkles[i].g = colorDistrib(gen);
        sprinkles[i].b = colorDistrib(gen);
        
        sprinkles[i].size = sizeDistrib(gen);
        sprinkles[i].angle = angleDistrib(gen);
    }
}

// Function to draw sprinkles
void drawSprinkles(float yOffset) {
    for (int i = 0; i < MAX_SPRINKLES; i++) {
        glPushMatrix();
        glTranslatef(sprinkles[i].x, yOffset + sprinkles[i].y, sprinkles[i].z);
        glRotatef(sprinkles[i].angle, 0, 1, 0);
        glColor3f(sprinkles[i].r, sprinkles[i].g, sprinkles[i].b);
        
        // Draw elongated cube for sprinkle
        glScalef(sprinkles[i].size, sprinkles[i].size / 3.0f, sprinkles[i].size);
        glBegin(GL_QUADS);
        // Front face
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        // Back face
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        // Top face
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        // Bottom face
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        // Right face
        glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        // Left face
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glEnd();
        
        glPopMatrix();
    }
}

// Function to draw background gradient
void drawBackground() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glBegin(GL_QUADS);
    // Top color
    glColor4fv(bgColorTop);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    // Bottom color
    glColor4fv(bgColorBottom);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ---------- Ice-cream assembly ----------
void drawIceCream()
{
    // auto-rotate around Y axis to show 3D effect
    if (autoRotate) rotY = fmodf(rotY + 0.10, 360.0f);

    glTranslatef(0.0f, -0.25f, -camZ);  // camera back & slight down
    glRotatef(rotY, 0, 1, 0);           // rotate around Y axis

    // Wafer Cone (pointing up)
    glPushMatrix();
    glTranslatef(0.0, 0.2, 0.0);  // Position cone at the bottom
    drawWaferCone(0.38, 1.10);
    glPopMatrix();

    // Mint scoop on top of the cone
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);   // Position on top of the cone
    drawScoop(0.35f, 0.20f, 0.85f, 0.45f);  // mint green color
    // Add sprinkles to the mint scoop
    drawSprinkles(0.25f);
    glPopMatrix();

    // Strawberry scoop on top of the mint scoop
    glPushMatrix();
    glTranslatef(0.0f, 0.75f, 0.0f);  // Position on top of mint scoop
    drawScoop(0.35f, 0.95f, 0.55f, 0.65f);  // strawberry pink color
    // Add sprinkles to the strawberry scoop
    drawSprinkles(0.75f);
    glPopMatrix();

    // Add gold loop around the ice cream
    glPushMatrix();
    glTranslatef(0.0f, 0.50f, 0.0f);  // Position between scoops
    drawGoldLoop();
    glPopMatrix();

    // Add chocolate stick
    glPushMatrix();
    glTranslatef(0.10f, 0.86f, -0.02f);
    glRotatef(20, 0, 0, 1);
    glRotatef(25, 1, 0, 0);
    glColor3f(0.30f, 0.15f, 0.05f);
    glRotatef(-90, 1, 0, 0);
    drawCylinderSolid(0.045f, 0.045f, 0.35f);
    glPopMatrix();

    // Add cherry on top
    glPushMatrix();
    glTranslatef(-0.15, 0.92, 0.25);
    glColor3f(1.0f, 0.1f, 0.1f);
    drawSphereSolid(0.08f);

    // stem
    glColor3f(0.8f, 0.1f, 0.1f);
    glRotatef(-35, 0, 0, 1);
    glTranslatef(0.0f, 0.09f, 0.0f);
    glRotatef(90, -1, 0, 0);
    drawCylinderSolid(0.008f, 0.008f, 0.18f);
    glPopMatrix();
}

// ------------------------ Render Loop -------------------
void display()
{
    // Draw gradient background
    drawBackground();
    
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);    // enable depth testing
    
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    // Enable color material mode
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 30.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    drawIceCream();
}

// ------------------------ WinMain -----------------------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WindowProcedure;
    wc.lpszClassName = WINDOW_TITLE;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClassEx(&wc)) return FALSE;

    HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, wc.hInstance, NULL);

    HDC hdc = GetDC(hWnd);
    if (!initPixelFormat(hdc)) return FALSE;
    HGLRC hglrc = wglCreateContext(hdc);
    if (!wglMakeCurrent(hdc, hglrc)) return FALSE;

    ShowWindow(hWnd, nCmdShow);

    // nice lines for lattice rings
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    // Initialize sprinkles
    initSprinkles();
    
    // Set window title to a more appealing name
    SetWindowText(hWnd, "Deluxe Ice Cream Cone");

    MSG msg{};
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        display();
        SwapBuffers(hdc);
    }

    UnregisterClass(WINDOW_TITLE, wc.hInstance);
    return TRUE;
}