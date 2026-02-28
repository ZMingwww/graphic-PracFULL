#include <Windows.h>
#include <gl/GL.h>
#include <math.h>

#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "Combined Exercises - Press 1 for Pyramid, 2 for Cube"

// Global variables
float rotateX = 0.0f;
float rotateY = 0.0f;
float rotateZ = 0.0f;

// Exercise switching
int currentExercise = 1;  // 1 = Pyramid, 2 = Cube

// Cube texture switching variables (for Exercise 2)
int currentTexture = 0;  // 0=wood, 1=metal, 2=brick
GLuint textures[3];      // Array to hold texture IDs for cube
bool texturesLoaded = false;

// Pyramid texture (for Exercise 1)
GLuint pyramidTexture = 0;

// Bitmap structure
BITMAP BMP;
HBITMAP hBMP = NULL;

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

			// Exercise switching
		case '1':
			currentExercise = 1; // Switch to Pyramid (Exercise 1)
			break;
		case '2':
			currentExercise = 2; // Switch to Cube (Exercise 2)
			break;

			// Texture switching for cube (Exercise 2 only)
		case '3':
			if (currentExercise == 2) currentTexture = 0; // Wood texture
			break;
		case '4':
			if (currentExercise == 2) currentTexture = 1; // Metal texture
			break;
		case '5':
			if (currentExercise == 2) currentTexture = 2; // Brick texture
			break;

			// Rotation controls (work for both exercises)
		case 'Q': // Rotate around X-axis positive
			rotateX += 5.0f;
			break;
		case 'A': // Rotate around X-axis negative
			rotateX -= 5.0f;
			break;
		case 'W': // Rotate around Y-axis positive
			rotateY += 5.0f;
			break;
		case 'S': // Rotate around Y-axis negative
			rotateY -= 5.0f;
			break;
		case 'E': // Rotate around Z-axis positive
			rotateZ += 5.0f;
			break;
		case 'D': // Rotate around Z-axis negative
			rotateZ -= 5.0f;
			break;
		case 'R': // Reset rotation
			rotateX = rotateY = rotateZ = 0.0f;
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

	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}

// EXERCISE 1: Pyramid functions
void drawPyramid(float size) {
	float halfSize = size / 2.0f;
	float height = size;

	glBegin(GL_TRIANGLES);

	// Front face (triangle)
	glTexCoord2f(0.5f, 1.0f);  // Top of pyramid
	glVertex3f(0.0f, height, 0.0f);
	glTexCoord2f(0.0f, 0.0f);  // Bottom left
	glVertex3f(-halfSize, 0.0f, halfSize);
	glTexCoord2f(1.0f, 0.0f);  // Bottom right
	glVertex3f(halfSize, 0.0f, halfSize);

	// Right face (triangle)
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, height, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(halfSize, 0.0f, halfSize);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(halfSize, 0.0f, -halfSize);

	// Back face (triangle)
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, height, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(halfSize, 0.0f, -halfSize);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-halfSize, 0.0f, -halfSize);

	// Left face (triangle)
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, height, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-halfSize, 0.0f, -halfSize);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-halfSize, 0.0f, halfSize);

	glEnd();

	// Base of pyramid (square)
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-halfSize, 0.0f, halfSize);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(halfSize, 0.0f, halfSize);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(halfSize, 0.0f, -halfSize);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-halfSize, 0.0f, -halfSize);
	glEnd();
}

// EXERCISE 2: Cube functions
void drawCube(float size) {
	float halfSize = size / 2.0f;

	glBegin(GL_QUADS);

	// Front face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);

	// Back face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);

	// Top face
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, halfSize, halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);

	// Bottom face
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, -halfSize, -halfSize);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);

	// Right face
	glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);

	// Left face
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);

	glEnd();
}

GLuint loadSingleTexture(LPCSTR filename) {
	GLuint texture = 0;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	HBITMAP hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	if (hBMP == NULL) {
		return 0; // Return 0 if texture loading failed
	}

	GetObject(hBMP, sizeof(BMP), &BMP);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	DeleteObject(hBMP);
	return texture;
}

void loadAllTextures() {
	if (texturesLoaded) return;

	// Load pyramid texture (Exercise 1)
	pyramidTexture = loadSingleTexture("Brick.bmp");

	// Load cube textures (Exercise 2)
	textures[0] = loadSingleTexture("Wood.bmp");    // Wood texture
	textures[1] = loadSingleTexture("Metal.bmp");   // Metal texture  
	textures[2] = loadSingleTexture("Brick.bmp");   // Brick texture

	texturesLoaded = true;
}

void display()
{
	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);  // Dark blue background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Reset transformations
	glLoadIdentity();

	// Move object back so it's visible
	if (currentExercise == 1) {
		glTranslatef(0.0f, -0.3f, -3.0f);  // Position for pyramid
	}
	else {
		glTranslatef(0.0f, 0.0f, -5.0f);   // Position for cube
	}

	// Apply rotations based on user input
	glRotatef(rotateX, 1.0f, 0.0f, 0.0f);  // X-axis rotation
	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);  // Y-axis rotation
	glRotatef(rotateZ, 0.0f, 0.0f, 1.0f);  // Z-axis rotation

	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	// Load textures if not already loaded
	loadAllTextures();

	// Draw based on current exercise
	if (currentExercise == 1) {
		// EXERCISE 1: Draw Pyramid with brick texture
		if (pyramidTexture != 0) {
			glBindTexture(GL_TEXTURE_2D, pyramidTexture);
		}
		drawPyramid(1.0f);
	}
	else {
		// EXERCISE 2: Draw Cube with switchable textures
		if (textures[currentTexture] != 0) {
			glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
		}
		drawCube(2.0f);
	}

	glDisable(GL_TEXTURE_2D);
}

void displayInstructions(HWND hWnd) {
	MessageBox(hWnd,
		"COMBINED EXERCISES\n\n"
		"Exercise Selection:\n"
		"1 - Exercise 1: Pyramid (brick texture)\n"
		"2 - Exercise 2: Cube (switchable textures)\n\n"
		"Cube Texture Controls (Exercise 2 only):\n"
		"3 - Wood texture\n"
		"4 - Metal texture\n"
		"5 - Brick texture\n\n"
		"Rotation Controls (Both exercises):\n"
		"Q/A - Rotate around X-axis\n"
		"W/S - Rotate around Y-axis\n"
		"E/D - Rotate around Z-axis\n"
		"R - Reset rotation\n\n"
		"ESC - Exit\n\n"
		"Required Files:\n"
		"- Brick.bmp (for pyramid)\n"
		"- Wood.bmp, Metal.bmp, Brick.bmp (for cube)",
		"Combined Exercise Controls", MB_OK);
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
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize window for OpenGL
	HDC hdc = GetDC(hWnd);
	initPixelFormat(hdc);
	HGLRC hglrc = wglCreateContext(hdc);

	if (!wglMakeCurrent(hdc, hglrc)) return false;

	// Set up perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -0.75, 0.75, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);

	ShowWindow(hWnd, nCmdShow);

	// Display instructions
	displayInstructions(hWnd);

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
	}

	// Clean up textures
	if (texturesLoaded) {
		glDeleteTextures(3, textures);
		glDeleteTextures(1, &pyramidTexture);
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);
	return true;
}