#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "GLU32.lib")


#define WINDOW_TITLE "Practical 6"

// Light properties
float lightPos[4] = { 0.0, 0.0, 2.0, 1.0 }; // Light position (x, y, z, w)
float redDiffuse[4] = { 1.0, 0.0, 0.0, 1.0 }; // Red diffuse light
float ambient[4] = { 0.2, 0.2, 0.2, 1.0 }; // Ambient light

// Object properties
bool isLightOn = true; // Light is on by default
bool showSphere = true; // Show sphere by default
float rotationX = 0.0; // Rotation around X-axis
float rotationY = 0.0; // Rotation around Y-axis
float rotationZ = 0.0; // Rotation around Z-axis

// Material properties
float materialDiffuse[4] = { 0.8, 0.8, 0.8, 1.0 }; // White material
float materialSpecular[4] = { 1.0, 1.0, 1.0, 1.0 }; // Specular reflection
float materialShininess = 50.0; // Shininess factor

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		else if (wParam == VK_SPACE) // Toggle light on/off
			isLightOn = !isLightOn;
		// Light position controls
		else if (wParam == 'W') // Move light up
			lightPos[1] += 0.5;
		else if (wParam == 'S') // Move light down
			lightPos[1] -= 0.5;
		else if (wParam == 'A') // Move light left
			lightPos[0] -= 0.5;
		else if (wParam == 'D') // Move light right
			lightPos[0] += 0.5;
		else if (wParam == 'E') // Move light near
			lightPos[2] -= 0.5;
		else if (wParam == 'Q') // Move light far
			lightPos[2] += 0.5;
		// Object rotation controls
		else if (wParam == VK_UP) // Rotate clockwise at all-axis
		{
			rotationX += 0.5;
			rotationY += 0.5;
			rotationZ += 0.5;
		}
		else if (wParam == VK_DOWN) // Rotate anti clockwise at all-axis
		{
			rotationX -= 0.5;
			rotationY -= 0.5;
			rotationZ -= 0.5;
		}
		// Object switching controls
		else if (wParam == 'O') // Switch to Sphere
			showSphere = true;
		else if (wParam == 'P') // Switch to Pyramid
			showSphere = false;
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//--------------------------------------------------------------------

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

	// choose pixel format returns the number most similar pixel format available
	int n = ChoosePixelFormat(hdc, &pfd);

	// set pixel format returns whether it sucessfully set the pixel format
	if (SetPixelFormat(hdc, n, &pfd))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------

void drawSphere(double r) {
	GLUquadricObj* sphere = NULL;
	sphere = gluNewQuadric();
	gluSphere(sphere, r, 30, 30);
	gluDeleteQuadric(sphere);
}

void drawPyramid(double size) {
	glBegin(GL_TRIANGLES);
	// Front face
	glNormal3f(0.0, 0.447, 0.894);
	glVertex3f(0.0, size, 0.0);     // Top vertex
	glVertex3f(-size, -size, size); // Bottom left
	glVertex3f(size, -size, size);  // Bottom right

	// Right face
	glNormal3f(0.894, 0.447, 0.0);
	glVertex3f(0.0, size, 0.0);      // Top vertex
	glVertex3f(size, -size, size);   // Bottom front
	glVertex3f(size, -size, -size);  // Bottom back

	// Back face
	glNormal3f(0.0, 0.447, -0.894);
	glVertex3f(0.0, size, 0.0);       // Top vertex
	glVertex3f(size, -size, -size);   // Bottom right
	glVertex3f(-size, -size, -size);  // Bottom left

	// Left face
	glNormal3f(-0.894, 0.447, 0.0);
	glVertex3f(0.0, size, 0.0);       // Top vertex
	glVertex3f(-size, -size, -size);  // Bottom back
	glVertex3f(-size, -size, size);   // Bottom front
	glEnd();

	// Bottom face (square base)
	glBegin(GL_QUADS);
	glNormal3f(0.0, -1.0, 0.0);
	glVertex3f(-size, -size, size);   // Front left
	glVertex3f(size, -size, size);    // Front right
	glVertex3f(size, -size, -size);   // Back right
	glVertex3f(-size, -size, -size);  // Back left
	glEnd();
}

void setupLighting() {
	if (isLightOn) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		// Set up red diffuse light
		glLightfv(GL_LIGHT0, GL_DIFFUSE, redDiffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

		// Enable color material so glColor3f affects material properties
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
}
void display()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Setup lighting
	setupLighting();

	// Setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 0.1, 100.0);

	// Setup modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0,  // Eye position
		0.0, 0.0, 0.0,  // Look at point
		0.0, 1.0, 0.0); // Up vector

	// Apply rotations
	glRotatef(rotationX, 1.0, 0.0, 0.0);
	glRotatef(rotationY, 0.0, 1.0, 0.0);
	glRotatef(rotationZ, 0.0, 0.0, 1.0);

	// Set material properties
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

	// Set object color (white for proper lighting)
	glColor3f(1.0, 1.0, 1.0);

	// Draw the selected object
	if (showSphere) {
		drawSphere(0.8);
	}
	else {
		drawPyramid(0.8);
	}
}
//--------------------------------------------------------------------

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
		900, 10, 600, 600,
		NULL, NULL, wc.hInstance, NULL);

	//--------------------------------
	//	Initialize window for OpenGL
	//--------------------------------

	HDC hdc = GetDC(hWnd);

	//	initialize pixel format for the window
	initPixelFormat(hdc);

	//	get an openGL context
	HGLRC hglrc = wglCreateContext(hdc);

	//	make context current
	if (!wglMakeCurrent(hdc, hglrc)) return false;

	//--------------------------------
	//	End initialization
	//--------------------------------

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
	}

	UnregisterClass(WINDOW_TITLE, wc.hInstance);

	return true;
}
//--------------------------------------------------------------------