#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <cmath>
#include <random>
#include <string>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

#define WINDOW_TITLE "Practical 4B (Textured)"

// ------------------------ Globals ------------------------
float rotY = 0.0f;             // rotation angle around Y axis
bool  autoRotate = true;       // auto rotation flag
float camZ = 3.2f;             // distance from camera

// Lighting and material properties
GLfloat light_position[] = { 5.0f, 5.0f, 5.0f, 0.0f };
GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// ---- Texture IDs (>=4 + background)
GLuint texSky = 0, texCone = 0, texMint = 0, texStraw = 0, texChoco = 0;

// Some GL headers don’t define these:
#ifndef GL_BGR_EXT
#define GL_BGR_EXT  0x80E0
#endif
#ifndef GL_BGRA_EXT
#define GL_BGRA_EXT 0x80E1
#endif

// Sprinkles data
struct Sprinkle { float x, y, z, r, g, b, size, angle; };
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

// ---------- BMP ? OpenGL texture (matches your guide) ----------
GLuint LoadTextureBMP_Win32(const char* filename, bool repeatWrap = true)
{
    // a) Pixel storage alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // b) Load BMP into HBITMAP (DIBSECTION so bmBits is valid)
    HBITMAP hBMP = (HBITMAP)LoadImageA(
        NULL, filename, IMAGE_BITMAP, 0, 0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION
    );
    if (!hBMP) {
        MessageBoxA(NULL, (std::string("Failed to load BMP: ") + filename).c_str(), "Error", MB_ICONERROR);
        return 0;
    }

    // c) GetObject to BITMAP
    BITMAP bmp{};
    GetObject(hBMP, sizeof(BITMAP), &bmp);
    const int width = (int)bmp.bmWidth;
    const int height = (int)bmp.bmHeight;
    const int bpp = bmp.bmBitsPixel;     // 24 or 32
    const void* data = bmp.bmBits;          // BGR(A) bottom-up already handled by DIBSECTION

    if (!data || (bpp != 24 && bpp != 32)) {
        DeleteObject(hBMP);
        MessageBoxA(NULL, "Only 24/32-bit uncompressed BMP supported.", "Error", MB_ICONERROR);
        return 0;
    }

    // d) Create GL texture
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeatWrap ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeatWrap ? GL_REPEAT : GL_CLAMP);

    GLenum format = (bpp == 32) ? GL_BGRA_EXT : GL_BGR_EXT;

    // e) Upload + mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, (bpp == 32) ? GL_RGBA : GL_RGB, width, height, format, GL_UNSIGNED_BYTE, data);

    // f) Cleanup GDI
    DeleteObject(hBMP);
    return tex;
}

// ---------- GLU helpers (with texture option) ----------
static GLUquadric* newQuad(GLenum style, bool wantTex = false)
{
    GLUquadric* q = gluNewQuadric();
    gluQuadricDrawStyle(q, style);
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, wantTex ? GL_TRUE : GL_FALSE);
    return q;
}

void drawSphereTextured(float r)
{
    GLUquadric* q = newQuad(GLU_FILL, true);
    gluSphere(q, r, 30, 24);
    gluDeleteQuadric(q);
}

void drawCylinderTextured(float br, float tr, float h)
{
    GLUquadric* c = newQuad(GLU_FILL, true);
    gluCylinder(c, br, tr, h, 30, 1);
    gluDeleteQuadric(c);
}

void drawDiskSolid(float inr, float outr)
{
    GLUquadric* d = newQuad(GLU_FILL, false);
    gluDisk(d, inr, outr, 30, 1);
    gluDeleteQuadric(d);
}

// ---------- Textured parts ---------------
void drawScoop(float radius, GLuint tex)
{
    // neutral material (texture provides color)
    GLfloat mat_ambient[] = { 0.25f,0.25f,0.25f,1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 0.15f,0.15f,0.15f,1.0f };
    GLfloat mat_shine[] = { 12.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();
    glScalef(1.0f, 0.80f, 1.0f); // scoop squashed
    drawSphereTextured(radius);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// UV-mapped triangle cone (cylindrical unwrap)
void drawWaferCone(float rTop, float h, GLuint tex)
{
    const int segments = 48;

    GLfloat mat_ambient[] = { 0.3f,0.2f,0.1f,1.0f };
    GLfloat mat_diffuse[] = { 0.9f,0.8f,0.6f,1.0f };
    GLfloat mat_specular[] = { 0.08f,0.08f,0.08f,1.0f };
    GLfloat mat_shine[] = { 6.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shine);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();
    glRotatef(90, 1, 0, 0); // point up along +Y

    glBegin(GL_TRIANGLES);
    for (int i = 0;i < segments;i++) {
        float t0 = (float)i / segments;
        float t1 = (float)(i + 1) / segments;
        float a0 = t0 * 2.0f * 3.1415926f;
        float a1 = t1 * 2.0f * 3.1415926f;

        float x0 = rTop * cosf(a0), y0 = rTop * sinf(a0);
        float x1 = rTop * cosf(a1), y1 = rTop * sinf(a1);

        // apex
        glTexCoord2f((t0 + t1) * 0.5f, 1.0f); glVertex3f(0, 0, h);
        glTexCoord2f(t0, 0.0f);          glVertex3f(x0, y0, 0);
        glTexCoord2f(t1, 0.0f);          glVertex3f(x1, y1, 0);
    }
    glEnd();

    // base cap (fan with radial UV)
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f); glVertex3f(0, 0, 0);
    for (int i = 0;i <= segments;i++) {
        float t = (float)i / segments;
        float a = t * 2.0f * 3.1415926f;
        float x = rTop * cosf(a), y = rTop * sinf(a);
        float u = 0.5f + x / (2.0f * rTop);
        float v = 0.5f + y / (2.0f * rTop);
        glTexCoord2f(u, v); glVertex3f(x, y, 0);
    }
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void drawChocolateStick(GLuint tex)
{
    GLfloat ma[] = { 0.25f,0.20f,0.15f,1.0f };
    GLfloat md[] = { 0.7f,0.6f,0.5f,1.0f };
    GLfloat ms[] = { 0.05f,0.05f,0.05f,1.0f };
    GLfloat sh[] = { 6.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ma);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, md);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ms);
    glMaterialfv(GL_FRONT, GL_SHININESS, sh);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();
    glTranslatef(0.10f, 0.86f, -0.02f);
    glRotatef(20, 0, 0, 1);
    glRotatef(25, 1, 0, 0);
    glRotatef(-90, 1, 0, 0);
    drawCylinderTextured(0.045f, 0.045f, 0.5f);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void drawCherry()
{
    glPushMatrix();
    glTranslatef(-0.15f, 0.92f, 0.07f);
    glColor3f(1.0f, 0.1f, 0.1f);
    GLUquadric* q = gluNewQuadric();
    gluSphere(q, 0.08f, 24, 18);
    gluDeleteQuadric(q);

    glColor3f(0.8f, 0.1f, 0.1f);
    glRotatef(-35, 0, 0, 1);
    glTranslatef(0.0f, 0.09f, 0.0f);
    glRotatef(90, 1, 0, 0);
    drawCylinderTextured(0.008f, 0.008f, 0.18f);
    glPopMatrix();
}

void drawGoldLoop()
{
    glPushMatrix();
    glRotatef(90, 1, 0, 0);

    GLfloat ma[] = { 0.24725f,0.1995f,0.0745f,1.0f };
    GLfloat md[] = { 0.75164f,0.60648f,0.22648f,1.0f };
    GLfloat ms[] = { 0.628281f,0.555802f,0.366065f,1.0f };
    GLfloat sh[] = { 51.2f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ma);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, md);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ms);
    glMaterialfv(GL_FRONT, GL_SHININESS, sh);

    glColor3f(0.85f, 0.65f, 0.13f);

    const int   segments = 36;
    const float radius = 0.25f;
    const float thickness = 0.01f;

    for (int i = 0;i < segments;i++) {
        float a0 = (float)i / segments * 2.0f * 3.14159f;
        float a1 = (float)(i + 1) / segments * 2.0f * 3.14159f;
        float x0 = radius * cosf(a0), z0 = radius * sinf(a0);
        float x1 = radius * cosf(a1), z1 = radius * sinf(a1);

        glPushMatrix();
        glTranslatef(x0, z0, 0.0f);
        glRotatef(a0 * 180.0f / 3.14159f, 0, 0, 1);
        drawCylinderTextured(thickness, thickness, sqrtf((x1 - x0) * (x1 - x0) + (z1 - z0) * (z1 - z0)));
        glPopMatrix();
    }
    glPopMatrix();
}

// Sprinkles
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
        sprinkles[i].r = colorDistrib(gen);
        sprinkles[i].g = colorDistrib(gen);
        sprinkles[i].b = colorDistrib(gen);
        sprinkles[i].size = sizeDistrib(gen);
        sprinkles[i].angle = angleDistrib(gen);
    }
}

void drawSprinkles(float yOffset) {
    glDisable(GL_TEXTURE_2D);
    for (int i = 0;i < MAX_SPRINKLES;i++) {
        glPushMatrix();
        glTranslatef(sprinkles[i].x, yOffset + sprinkles[i].y, sprinkles[i].z);
        glRotatef(sprinkles[i].angle, 0, 1, 0);
        glColor3f(sprinkles[i].r, sprinkles[i].g, sprinkles[i].b);

        glScalef(sprinkles[i].size, sprinkles[i].size / 3.0f, sprinkles[i].size);
        glBegin(GL_QUADS);
        // Front
        glVertex3f(-0.5f, -0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        // Back
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
        // Top
        glVertex3f(-0.5f, 0.5f, -0.5f); glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
        // Bottom
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
        // Right
        glVertex3f(0.5f, -0.5f, -0.5f); glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f); glVertex3f(0.5f, -0.5f, 0.5f);
        // Left
        glVertex3f(-0.5f, -0.5f, -0.5f); glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f); glVertex3f(-0.5f, 0.5f, -0.5f);
        glEnd();
        glPopMatrix();
    }
}

// Textured fullscreen background
void drawBackgroundTextured()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texSky);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

// ---------- Ice-cream assembly ----------
void drawIceCream()
{
    if (autoRotate) rotY = fmodf(rotY + 0.10f, 360.0f);

    glTranslatef(0.0f, -0.25f, -camZ);
    glRotatef(rotY, 0, 1, 0);

    // Wafer Cone (pointing up)
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.0f);
    drawWaferCone(0.38f, 1.10f, texCone);
    glPopMatrix();

    // Mint scoop
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);
    drawScoop(0.35f, texMint);
    drawSprinkles(0.25f);
    glPopMatrix();

    // Strawberry scoop
    glPushMatrix();
    glTranslatef(0.0f, 0.75f, 0.0f);
    drawScoop(0.35f, texStraw);
    drawSprinkles(0.75f);
    glPopMatrix();

    // Gold loop
    glPushMatrix();
    glTranslatef(0.0f, 0.50f, 0.0f);
    drawGoldLoop();
    glPopMatrix();

    // Chocolate stick
    drawChocolateStick(texChoco);

    // Cherry
    drawCherry();
}

// ------------------------ Render Loop -------------------
void display()
{
    // Background texture
    drawBackgroundTextured();

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

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
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = WindowProcedure;
    wc.lpszClassName = WINDOW_TITLE;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClassEx(&wc)) return FALSE;

    HWND hWnd = CreateWindow(WINDOW_TITLE, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, wc.hInstance, NULL);
    if (!hWnd) return FALSE;

    HDC hdc = GetDC(hWnd);
    if (!initPixelFormat(hdc)) return FALSE;
    HGLRC hglrc = wglCreateContext(hdc);
    if (!wglMakeCurrent(hdc, hglrc)) return FALSE;

    ShowWindow(hWnd, nCmdShow);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // ---- Load BMP textures (at least 4 + background)
    texSky = LoadTextureBMP_Win32("sky.bmp", false);           // clamp so no seams
    texCone = LoadTextureBMP_Win32("cone_waffle.bmp", true);
    texMint = LoadTextureBMP_Win32("scoop_mint.bmp", true);
    texStraw = LoadTextureBMP_Win32("scoop_strawberry.bmp", true);
    texChoco = LoadTextureBMP_Win32("choco_stick.bmp", true);

    glEnable(GL_TEXTURE_2D);
    initSprinkles();

    SetWindowText(hWnd, "Deluxe Ice Cream Cone (Textured) - R: auto-rotate, Arrows: rotate, Z/X: zoom");

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

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hWnd, hdc);
    UnregisterClass(WINDOW_TITLE, wc.hInstance);
    return TRUE;
}
