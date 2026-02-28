#include <Windows.h>
#include <gl/GL.h>

#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "Practical 07"

//Step 1: Variable declaration 
BITMAP BMP;				//bitmap structure
HBITMAP hBMP = NULL;	//bitmap handle

LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
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

void drawCube(float size) {

	glBegin(GL_QUADS);
	// Face 1  

	//1f for 1D texture, 2f for 2D texture
	glTexCoord2f(0.0, 1.0);  //this texture map to the top left of cube face (0.0,0.0,1.0)
	glVertex3f(0.0f, 0.0f, size);

	glTexCoord2f(1.0, 1.0); //this texture map to the top right of cube face (1.0,0.0,1.0)
	glVertex3f(size, 0.0f, size);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(size, 0.0f, 0.0f); //in the practical sheet, the sequence is wrong, swap between 3 and 4 

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.0f, 0.0f, 0.0f); // swap with this

	// Face 2  (must connected with face 1, meaning back face should not be considered for instance) (red)
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0f, 0.0f, 0.0f); // last point of face 1 is the starting of face 2

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.0f, size, 0.0f);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.0f, size, size);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.0f, 0.0f, size);

	// Face 3
	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0f, 0.0f, size);  // last point of face 2 is the starting of face 3 (green)

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.0f, size, size);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(size, size, size);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(size, 0.0f, size);

	// Face 4  
	glTexCoord2f(0.0, 1.0);
	glVertex3f(size, 0.0f, size);   // last point of face 3 is the starting of face 4 (purple)

	glTexCoord2f(1.0, 1.0);
	glVertex3f(size, size, size);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(size, size, 0.0f);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(size, 0.0f, 0.0f);

	// Face 5 
	glTexCoord2f(0.0, 1.0);
	glVertex3f(size, 0.0f, 0.0f);   // last point of face 4 is the starting of face 5  (bottom)

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.0f, size, 0.0f);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(size, size, 0.0f);

	// Face 6
	glTexCoord2f(0.0, 1.0);
	glVertex3f(size, size, 0.0f);   // last point of face 5 is the starting of face 6 (middle back)

	glTexCoord2f(1.0, 1.0);
	glVertex3f(0.0f, size, 0.0f);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(0.0f, size, size);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(size, size, size);
	glEnd();

}

GLuint loadTexture(LPCSTR filename) {

	//Take from step 1
	GLuint texture = 0;		//texture name

	//Step 3: Initialize texture information
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	HBITMAP hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL),
		filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

	GetObject(hBMP, sizeof(BMP), &BMP);

	//Step 4: Assign texture to polygon.
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);

	DeleteObject(hBMP); //take from step 5

	return texture;
}


void display()
{

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glRotatef(0.1, 1.0, 1.0, 1.0);  //rotate all axis

	GLuint textureArr[2];
	textureArr[0] = loadTexture("Box.bmp");
	drawCube(0.5); //cube 1
	glDeleteTextures(1, &textureArr[0]);

	textureArr[1] = loadTexture("Box.bmp");
	drawCube(-0.5);  //cube 2
	glDeleteTextures(1, &textureArr[1]);

	//Step5: Remove texture info.
	glDisable(GL_TEXTURE_2D);

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