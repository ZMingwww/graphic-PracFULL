#include <Windows.h>
#include <gl/GL.h>
#include <cstdlib>

//#pragma comment (lib, "OpenGL32.lib")//linker -  Input - Additional Dependencies - OpenGL32.lib

#define WINDOW_TITLE "Practical 4"

int qNo = 1;	//question no

float tx1 = 0, ty1 = 0, tx2 = 0, ty2 = 0, tSpeed = 0.1;//translate x and y for quad 1&2 with tSpeed

float angle = 0.0;
float speed = 0.5;
int spinDirection = 0; // 0 = Stop, 1 = Clockwise, -1 = AntiClockwise

float rotX = 0.0, rotY = 0.0, rotZ = 0.0; // pyramid rotation angles
bool rotateX = false, rotateY = false, rotateZ = false;

float lowerArmAngle = 0.0f;
float fullArmRotation = 0.0f;

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		else if (wParam == '1')	//when press key '1'
		{
			qNo = 1;
		}
		else if (wParam == '2') //when press key '2'
		{
			qNo = 2;
			rotateX = rotateY = rotateZ = false;
			rotX = rotY = rotZ = 0.0f;
		}
		else if (wParam == '3') //when press key '3'
		{
			qNo = 3;
			lowerArmAngle = 0.0f;
			fullArmRotation = 0.0f;
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();  // Reset transformations
		}
		else if (wParam == 'X') {
			rotateX = true;
			rotateY = false;
			rotateZ = false;
		}
		else if (wParam == 'Y') {
			rotateX = false;
			rotateY = true;
			rotateZ = false;
		}
		else if (wParam == 'Z') {
			rotateX = false;
			rotateY = false;
			rotateZ = true;
		}
		else if (qNo == 3) {
			if (wParam == VK_UP) lowerArmAngle += 5.0f;
			else if (wParam == VK_DOWN) lowerArmAngle -= 5.0f;
			else if (wParam == VK_LEFT) fullArmRotation -= 0.01;
			else if (wParam == VK_RIGHT) fullArmRotation += 0.01;
			else if (wParam == VK_SPACE) {
				lowerArmAngle = 0.0f;
				fullArmRotation = 0.0f;
			}
		}
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

void drawCube(float size)// f - float crtl+h
{
	glBegin(GL_QUADS);
	// Face 1 BOTTOM
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0f, 0.0f, size);
	glVertex3f(size, 0.0f, size);
	glVertex3f(size, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	// Face 2 LEFT
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, size, 0.0f);
	glVertex3f(0.0f, size, size);
	glVertex3f(0.0f, 0.0f, size);
	// Face 3 FRONT
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0f, 0.0f, size);
	glVertex3f(0.0f, size, size);
	glVertex3f(size, size, size);
	glVertex3f(size, 0.0f, size);

	// Face 4 RIGHT
	glColor3f(1.0, 1.0, 0.0);
	glVertex3f(size, 0.0f, size);
	glVertex3f(size, size, size);
	glVertex3f(size, size, 0.0f);
	glVertex3f(size, 0.0f, 0.0f);

	// Face 5 BACK
	glColor3f(0.0, 1.0, 1.0);
	glVertex3f(size, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, size, 0.0f);
	glVertex3f(size, size, 0.0f);

	// Face 6 UP
	glColor3f(1.0, 0.0, 1.0);
	glVertex3f(size, size, 0.0f);
	glVertex3f(0.0f, size, 0.0f);
	glVertex3f(0.0f, size, size);
	glVertex3f(size, size, size);
	glEnd();
}

void drawPyramid(float size)
{
	glLineWidth(5.0);
	glBegin(GL_LINE_LOOP);
	// Face 1 BOTTOM
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0f, 0.0f, size);
	glVertex3f(size, 0.0f, size);
	glVertex3f(size, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(size / 2, size, size / 2);
	glVertex3f(0.0f, 0.0f, size);
	glVertex3f(size / 2, size, size / 2);
	glVertex3f(size, 0.0f, size);
	glVertex3f(size / 2, size, size / 2);
	glVertex3f(size, 0.0f, 0.0f);
	glVertex3f(size / 2, size, size / 2);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

}

void q3(float w, float h, float d)
{
	glBegin(GL_LINES);
		// bottom square
		glVertex3f(0, 0, 0);     glVertex3f(w, 0, 0);
		glVertex3f(w, 0, 0);     glVertex3f(w, 0, d);
		glVertex3f(w, 0, d);     glVertex3f(0, 0, d);
		glVertex3f(0, 0, d);     glVertex3f(0, 0, 0);

		// top square
		glVertex3f(0, h, 0);     glVertex3f(w, h, 0);
		glVertex3f(w, h, 0);     glVertex3f(w, h, d);
		glVertex3f(w, h, d);     glVertex3f(0, h, d);
		glVertex3f(0, h, d);     glVertex3f(0, h, 0);

		// verticals
		glVertex3f(0, 0, 0);     glVertex3f(0, h, 0);
		glVertex3f(w, 0, 0);     glVertex3f(w, h, 0);
		glVertex3f(w, 0, d);     glVertex3f(w, h, d);
		glVertex3f(0, 0, d);     glVertex3f(0, h, d);
	glEnd();
}

void display()
{
	//--------------------------------
	//	OpenGL drawing
	//--------------------------------
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//glRotatef(0.01, 1.0, 1.0, 1.0);	//rotate in all axis
	//drawCube(0.5);
	//drawCube(-0.5);

	//drawPyramid(0.5);
	switch (qNo)
	{
	case 1:
		glRotatef(0.01, 1.0, 1.0, 1.0);	//rotate in all axis
		drawCube(0.5);
		break;

	case 2:
		if (rotateX) rotX += 0.000001;
		if (rotateY) rotY += 0.000001;
		if (rotateZ) rotZ += 0.000001;

		glRotatef(rotX, 1, 0, 0);
		glRotatef(rotY, 0, 1, 0);
		glRotatef(rotZ, 0, 0, 1);
		drawPyramid(0.5);
		break;
	case 3:
		glRotatef(fullArmRotation, 0, 1, 0); // Rotate whole arm

		// Draw upper arm
		glPushMatrix();
			glColor3f(1, 1, 1);
			q3(1, 0.3, 0.3);
		glPopMatrix();

		// Draw lower arm
		glPushMatrix();
			glTranslatef(1.0, 0, 0);              // Move to end of upper arm
			glRotatef(lowerArmAngle, 0, 0, 1);     // Rotate around connection point
			glTranslatef(0.0, 0, 0);              // No second translate
			glColor3f(1, 1, 1);
			q3(1, 0.3f, 0.3);
		glPopMatrix();
		break;

	default:
		break;
	}

	//--------------------------------
	//	End of OpenGL drawing
	//--------------------------------
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
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2, 2, -1.5, 1.5, -10, 10); // adjust if needed
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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