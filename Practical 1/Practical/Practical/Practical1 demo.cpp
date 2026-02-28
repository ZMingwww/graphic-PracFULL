
#include <Windows.h>
#include <gl/GL.h>
#include <math.h>

#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "Practical Exercise 1"

int qNo = 1; //question no

float x = 0, y = 0;		//origin of the circle
float radius = 0.2;		//radius
float angle = 0;		//angle
float x2 = 0, y2 = 0;	//point on circle
float PI = 3.14159;		//PI
int noOfTri = 30;		//no of triangles


LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//WM  = window message
	//click 'go to difinition'
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		//VK = visual key
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		else if (wParam == '1') //when press key '1'
			qNo = 1;
		else if (wParam == '2') //when press key '2'
			qNo = 2;
		else if (wParam == '3') //when press key '3'
			qNo = 3;
		else if (wParam == '4') //when press key '4'
			qNo = 4;
		else if (wParam == '5') //when press key '5'
			qNo = 5;
		else if (wParam == '6')	//when enter key "6"
			qNo = 6;
		else if (wParam == '7')	//when enter key "7"
			qNo = 7;
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

	pfd.cAlphaBits = 8;		//transparent
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

void pahangFlag()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);		//clear background with BLACK
	glClear(GL_COLOR_BUFFER_BIT);			//clear the color buffer

	glBegin(GL_QUADS);
		glColor3f(1.0, 1.0, 1.0); //WHITE
		glVertex2f(-0.8, 0.0);//left		v1
		glVertex2f(-0.8, 0.5);//left up		v2
		glVertex2f(0.8, 0.5);//right up	v3
		glVertex2f(0.8, 0.0);//right		v4
	glEnd();

	glBegin(GL_QUADS);
		glColor3f(0.0, 0.0, 0.0); //BLACK
		glVertex2f(-0.8, -0.5);//left		v1
		glVertex2f(-0.8, 0.0);//left up		v2
		glVertex2f(0.8, 0.0);//right up	v3
		glVertex2f(0.8, -0.5);//right		v4
	glEnd();

	glLineWidth(1.0);
	glBegin(GL_LINE_LOOP);
		glColor3f(0.0, 0.0, 1.0); //BLUE
		glVertex2f(-0.8, -0.5);//left		v1
		glVertex2f(-0.8, 0.5);//left up		v2
		glVertex2f(0.8, 0.5);//right up	v3
		glVertex2f(0.8, -0.5);//right		v4
	glEnd();

}

void n9Flag()
{
	glClearColor(0.0, 0.0, 0.0, 0.0); // Clear background with BLACK
	glClear(GL_COLOR_BUFFER_BIT);     // Clear the color buffer

	// Yellow background (entire flag)
	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 0.0);	// Yellow
		glVertex2f(-0.8, -0.5);
		glVertex2f(-0.8, 0.5);
		glVertex2f(0.8, 0.5);
		glVertex2f(0.8, -0.5);
	glEnd();

	// Red rectangle (top-left corner)
	glBegin(GL_QUADS);
	glColor3f(1.0, 0.0, 0.0);	// Red
		glVertex2f(-0.8, 0.5);
		glVertex2f(-0.8, 0.0);
		glVertex2f(0.0, 0.0);
		glVertex2f(0.0, 0.5);
	glEnd();

	// Black triangle
	glBegin(GL_TRIANGLES);
	glColor3f(0.0, 0.0, 0.0);	// Black
		glVertex2f(-0.8, 0.5);  // Top-left
		glVertex2f(-0.0, 0.0);  // Bottom-right of red box
		glVertex2f(-0.8, 0.0);  // Bottom-left of red box
	glEnd();

}

void englandFlag()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0); // white
	glBegin(GL_QUADS);
	glVertex2d(-0.8, -0.5);// top-left
	glVertex2d(-0.8, 0.5);  // top-right
	glVertex2d(0.8, 0.5);  // bottom-right
	glVertex2d(0.8, -0.5);  // bottom-left
	glEnd();

	// Draw red horizontal bar -
	glColor3f(0.5, 0.0, 0.0); // dark red
	glBegin(GL_QUADS);
	glVertex2d(-0.8, 0.08);
	glVertex2d(0.8, 0.08);
	glVertex2d(0.8, -0.08);
	glVertex2d(-0.8, -0.08);
	glEnd();

	// Draw red vertical bar |
	glBegin(GL_QUADS);
	glVertex2d(-0.06, 0.5);
	glVertex2d(0.06, 0.5);
	glVertex2d(0.06, -0.5);
	glVertex2d(-0.06, -0.5);
	glEnd();

}

void scotlandFlag()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// Blue background
	glColor3d(0.0, 0.3, 0.8);
	glBegin(GL_QUADS);
	glVertex2d(-0.8, 0.5);
	glVertex2d(0.8, 0.5);
	glVertex2d(0.8, -0.5);
	glVertex2d(-0.8, -0.5);
	glEnd();

	// First white diagonal
	glColor3d(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(-0.8, 0.4);	//v1
	glVertex2f(-0.8, 0.5);	//v2
	glVertex2f(-0.7, 0.5);	//v3
	glVertex2f(0.8, -0.4);	//v4
	glVertex2f(0.8, -0.5);	//v5
	glVertex2f(0.7, -0.5);	//v6
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2f(0.7, 0.5);	//v1
	glVertex2f(0.8, 0.5);	//v2
	glVertex2f(0.8, 0.4);	//v3
	glVertex2f(-0.7, -0.5);	//v4
	glVertex2f(-0.8, -0.5);	//v5
	glVertex2f(-0.8, -0.4);	//v6
	glEnd();

	glLineWidth(1.0);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0); //BLUE
	glVertex2f(-0.8, -0.5);//left		v1
	glVertex2f(-0.8, 0.5);//left up		v2
	glVertex2f(0.8, 0.5);//right up	    v3
	glVertex2f(0.8, -0.5);//right		v4
	glEnd();
}

void japanFlag()
{
	glClearColor(1.0, 1.0, 1.0, 1.0); // Clear background with black
	glClear(GL_COLOR_BUFFER_BIT);     // Clear the color buffer

	// Red circle (sun)
	glBegin(GL_TRIANGLE_FAN);//GL_LINE_LOOP
	glColor3f(1.0, 0.0, 0.0); // Red
		glVertex2f(x, y); // Origin of circle
		for (angle = 0; angle <= ( 2 * PI ); angle += ( 2 * PI )/ noOfTri) {
			x2 = x + radius * cos(angle); //quadric equation
			y2 = y + radius * sin(angle);
		glVertex2f(x2, y2);
	}
	glEnd();
	//angle <= (PI ); angle += (PI )/ noOfTri) -> half circle
	//angle = PI ->bottom PI

	// Border
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 1.0); // Black border
		glVertex2f(-0.8, -0.5);
		glVertex2f(-0.8, 0.5);
		glVertex2f(0.8, 0.5);
		glVertex2f(0.8, -0.5);
	glEnd();
	
}

void drawCircle(double cx, double cy, double r, int segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(cx, cy);
	for (int i = 0; i <= segments; i++) {
		double angle = 2.0 * PI * i / segments;
		glVertex2d(cx + r * cos(angle), cy + r * sin(angle));
	}
	glEnd();
}

void drawEllipse(double cx, double cy, double rx, double ry, int segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2d(cx, cy);
	for (int i = 0; i <= segments; i++) {
		double angle = 2.0 * PI * i / segments;
		glVertex2d(cx + rx * cos(angle), cy + ry * sin(angle));
	}
	glEnd();
}

void drawRectangle(double x, double y, double w, double h) {
	glBegin(GL_QUADS);
	glVertex2d(x, y);
	glVertex2d(x + w, y);
	glVertex2d(x + w, y + h);
	glVertex2d(x, y + h);
	glEnd();
}

void emoji() {
	glClearColor(0.4, 0.6, 0.4, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// === HAIR (drawn first, behind face) ===
	glColor3d(0.3, 0.15, 0.05); // Dark brown hair

	// Main hair mass (top and sides)
	drawEllipse(0.0, 0.25, 0.55, 0.35, 100); // Top hair
	drawEllipse(-0.35, 0.1, 0.25, 0.3, 50);  // Left side hair
	drawEllipse(0.35, 0.1, 0.25, 0.3, 50);   // Right side hair

	// Hair texture details
	for (int i = 0; i < 25; i++) {
		double angle = PI * 0.1 + PI * 0.8 * i / 25.0;
		double r = 0.5 + 0.15 * sin(angle * 2);
		double x = r * cos(angle);
		double y = r * sin(angle);
		drawCircle(x, y, 0.02, 10);
	}

	// Hair strands for more detail
	glColor3d(0.25, 0.12, 0.04); // Slightly darker hair
	for (int i = 0; i < 15; i++) {
		double angle = PI * 0.2 + PI * 0.6 * i / 15.0;
		double r = 0.55 + 0.1 * sin(angle * 3);
		double x = r * cos(angle);
		double y = r * sin(angle);
		drawCircle(x, y, 0.015, 8);
	}

	// === EARS (drawn before face for layering) ===
	glColor3d(1.0, 0.9, 0.8); // Same skin tone as face

	// Left ear
	drawEllipse(-0.45, 0.0, 0.08, 0.12, 50);
	// Left ear inner detail
	glColor3d(0.9, 0.8, 0.7); // Slightly darker for inner ear
	drawEllipse(-0.45, 0.0, 0.04, 0.06, 30);

	// Right ear
	glColor3d(1.0, 0.9, 0.8); // Back to skin tone
	drawEllipse(0.45, 0.0, 0.08, 0.12, 50);
	// Right ear inner detail
	glColor3d(0.9, 0.8, 0.7);
	drawEllipse(0.45, 0.0, 0.04, 0.06, 30);

	// === FACE ===
	glColor3d(1.0, 0.9, 0.8);
	drawCircle(0.0, 0.0, 0.45, 100);

	// === HIPSTER GLASSES ===
	glColor3d(0.6, 0.3, 0.1); // Brown frame
	glLineWidth(3.0);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 30; i++) {
		double angle = 2.0 * PI * i / 30.0;
		glVertex2d(-0.18 + 0.08 * cos(angle), 0.15 + 0.08 * sin(angle));
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 30; i++) {
		double angle = 2.0 * PI * i / 30.0;
		glVertex2d(0.18 + 0.08 * cos(angle), 0.15 + 0.08 * sin(angle));
	}
	glEnd();

	// Glasses bridge
	glBegin(GL_LINES);
	glVertex2d(-0.1, 0.15);
	glVertex2d(0.1, 0.15);
	glEnd();

	// === EYES BEHIND GLASSES ===
	glColor3d(0.0, 0.0, 0.0);
	drawCircle(-0.18, 0.15, 0.03, 50);
	drawCircle(0.18, 0.15, 0.03, 50);

	// Eye reflections
	glColor3d(1.0, 1.0, 1.0);
	drawCircle(-0.16, 0.17, 0.01, 20);
	drawCircle(0.20, 0.17, 0.01, 20);

	// === FULL BEARD ===
	glColor3d(0.4, 0.2, 0.1);
	for (int i = 0; i < 80; i++) {
		double angle = PI * 0.2 + PI * 0.6 * i / 80.0;
		double r = 0.35 + 0.1 * sin(angle * 3);
		double x = r * cos(angle);
		double y = r * sin(angle);
		drawCircle(x, y, 0.01, 8);
	}

	// === MUSTACHE ===
	glColor3d(0.3, 0.15, 0.05);
	drawEllipse(0.0, 0.0, 0.12, 0.03, 50);

	// === HIPSTER SMILE ===
	glColor3d(0.8, 0.2, 0.2);
	glLineWidth(3.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= 20; i++) {
		double t = (double)i / 20.0;
		double x = -0.1 + 0.2 * t;
		double y = -0.1 + 0.05 * (2 * t - 1) * (2 * t - 1);
		glVertex2d(x, y);
	}
	glEnd();

	// === NOSE ===
	glColor3d(0.95, 0.85, 0.75); // Slightly darker than face
	drawEllipse(0.0, 0.05, 0.015, 0.025, 30);

	// === ADDITIONAL HAIR DETAILS (front strands) ===
	glColor3d(0.35, 0.18, 0.06); // Hair color
	// Some hair strands falling on forehead
	for (int i = 0; i < 8; i++) {
		double x = -0.2 + 0.05 * i;
		double y = 0.4 - 0.02 * i;
		drawEllipse(x, y, 0.01, 0.04, 20);
	}

	glFlush();
}

void p1demo()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);		//clear background with BLACK
	glClear(GL_COLOR_BUFFER_BIT);			//clear the color buffer

	//glShadeModel(GL_SMOOTH);				//smooth shading
	glLineWidth(5.0);						//set the line width
	//glPointSize(10.0);					//set the point size
	glBegin(GL_LINE_LOOP);					//draw line LOOP
	//glBegin(GL_POLYGON);
	//glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);			//RED		
		glVertex2f(-0.5, 0.0);			//v1
	glColor3f(0.0, 1.0, 0.0);			//GREEN
		glVertex2f(0.0, 0.5);			//v2
	glColor3f(0.0, 0.0, 1.0);			//BLUE
		glVertex2f(0.5, 0.0);			//v3
	glEnd();							//Compulsory
}

void display()
{
	switch (qNo) {
	case 1:
		pahangFlag();
		break;
	case 2:
		n9Flag();
		break;
	case 3:
		englandFlag();
		break;
	case 4:
		scotlandFlag();
		break;
	case 5:
		japanFlag();
		break;
	case 6:
		emoji();
		break;
	case 7:
		p1demo();
		break;
	default:
		break;
	}

	
	//--------------------------------
	//	OpenGL drawing
	//--------------------------------
	
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
	//WS = window style
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