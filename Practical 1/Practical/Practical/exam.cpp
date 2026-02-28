#include <Windows.h>
#include <gl/GL.h>

#pragma comment (lib, "OpenGL32.lib")

#define WINDOW_TITLE "OpenGL Window"

// Global variables for animation control
int currentStep = 0;
const int maxSteps = 4;  // 0: original, 1: translate, 2: rotate, 3: both
float rotationAngle = 0.0f;
bool isRotating = false;
const float rotationSpeed = 0.1f;  // degrees per frame (very slow)
int transformationOrder = 1;  // 1 = Translate then Rotate, 2 = Rotate then Translate

// Global variables for answer display
bool showAnswers = false;
int currentQuestion = 0;  // 0-3: Four different questions
int currentAnswerType = 0;  // 0: GL_LINES, 1: GL_LINE_STRIP, 2: GL_TRIANGLES, 3: GL_TRIANGLE_STRIP

// Global variable for bow-tie geometry display
bool showBowTie = false;

// Global variable for triangle demo display
bool showTriangleDemo = false;


LRESULT WINAPI WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		if (wParam == VK_SPACE) {
			if (currentStep == 0) {
				currentStep = 1;  // Move to first transformation step
			}
			else if (currentStep == 1) {
				currentStep = 2;  // Start second transformation animation
				isRotating = true;
				rotationAngle = 0.0f;
			}
			else if (currentStep == 2 && !isRotating) {
				currentStep = 3;  // Show final result
			}
			else if (currentStep == 3) {
				currentStep = 0;  // Reset to beginning
				rotationAngle = 0.0f;
				isRotating = false;
			}
		}
		if (wParam == '2') {
			// Switch transformation order
			transformationOrder = (transformationOrder == 1) ? 2 : 1;
			// Reset animation
			currentStep = 0;
			rotationAngle = 0.0f;
			isRotating = false;
		}
		if (wParam == '3') {
			// Key 3 functionality: Apply problem-specific transformations
			// 1. Translate 0.2 units down and 0.5 units to the right
			// 2. Rotate 90 degrees clockwise along z-axis
			currentStep = 3;
			rotationAngle = -90.0f;  // Negative for clockwise rotation
			isRotating = false;      // Stop any ongoing animation
		}
		if (wParam == '4') {
			// Key 4 functionality: Cycle through 4 different questions
			if (!showAnswers) {
				showAnswers = true;
				currentQuestion = 0;
				currentAnswerType = 0;  // Start with GL_LINES
			} else {
				// Cycle through questions
				currentQuestion = (currentQuestion + 1) % 4;
				currentAnswerType = 0;  // Reset to GL_LINES for each question
			}
		}
		if (wParam == VK_LEFT && showAnswers) {
			// Navigate through answer types
			currentAnswerType = (currentAnswerType - 1 + 4) % 4;
		}
		if (wParam == VK_RIGHT && showAnswers) {
			// Navigate through answer types
			currentAnswerType = (currentAnswerType + 1) % 4;
		}
		if (wParam == VK_ESCAPE && showAnswers) {
			// Exit answer mode and return to transformation demo
			showAnswers = false;
		}
		if (wParam == '5') {
			// Key 5: Toggle bow-tie geometry display
			showBowTie = !showBowTie;
			// Exit other modes when showing bow-tie
			if (showBowTie) {
				showAnswers = false;
				showTriangleDemo = false;
				currentStep = 0;
				rotationAngle = 0.0f;
				isRotating = false;
			}
		}
		if (wParam == '6') {
			// Key 6: Toggle triangle demo display
			showTriangleDemo = !showTriangleDemo;
			// Exit other modes when showing triangle demo
			if (showTriangleDemo) {
				showAnswers = false;
				showBowTie = false;
				currentStep = 0;
				rotationAngle = 0.0f;
				isRotating = false;
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

void drawBowTieGeometry() {
	// Bow-tie geometry primitive from Figure 3
	// Define the six vertices with exact coordinates from the diagram
	float vertices[6][2] = {
		{-0.8f, 0.8f},   // Top-left vertex
		{-0.8f, -0.8f},  // Bottom-left vertex
		{0.8f, 0.8f},    // Top-right vertex
		{0.8f, -0.8f},   // Bottom-right vertex
		{0.0f, 0.2f},    // Top-center vertex (on y-axis)
		{0.0f, -0.2f}    // Bottom-center vertex (on y-axis)
	};
	
	// Set color for the bow-tie (light tan/brown as shown in diagram)
	glColor3f(0.8f, 0.6f, 0.4f);  // Light brown/tan color
	
	// Draw the bow-tie as two quadrilaterals (trapezoids)
	// Left quadrilateral: (-0.8, 0.8), (0.0, 0.2), (0.0, -0.2), (-0.8, -0.8)
	glBegin(GL_QUADS);
		glVertex2f(vertices[0][0], vertices[0][1]);  // Top-left
		glVertex2f(vertices[4][0], vertices[4][1]);  // Top-center
		glVertex2f(vertices[5][0], vertices[5][1]);  // Bottom-center
		glVertex2f(vertices[1][0], vertices[1][1]);  // Bottom-left
	glEnd();
	
	// Right quadrilateral: (0.8, 0.8), (0.0, 0.2), (0.0, -0.2), (0.8, -0.8)
	glBegin(GL_QUADS);
		glVertex2f(vertices[2][0], vertices[2][1]);  // Top-right
		glVertex2f(vertices[4][0], vertices[4][1]);  // Top-center
		glVertex2f(vertices[5][0], vertices[5][1]);  // Bottom-center
		glVertex2f(vertices[3][0], vertices[3][1]);  // Bottom-right
	glEnd();
	
	// Draw vertex points for reference (white)
	glColor3f(1.0f, 1.0f, 1.0f);
	glPointSize(8.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < 6; i++) {
		glVertex2f(vertices[i][0], vertices[i][1]);
	}
	glEnd();
	
	// Draw vertex numbers as small squares
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glVertex2f(vertices[i][0] - 0.05f, vertices[i][1] - 0.05f);
		glVertex2f(vertices[i][0] + 0.05f, vertices[i][1] - 0.05f);
		glVertex2f(vertices[i][0] + 0.05f, vertices[i][1] + 0.05f);
		glVertex2f(vertices[i][0] - 0.05f, vertices[i][1] + 0.05f);
		glEnd();
	}
}

void drawDetailedGridBackground() {
	// Draw detailed coordinate grid background matching the image
	// X-axis range: -14 to 14, Y-axis range: -6 to 6
	
	// Draw minor grid lines (lighter color)
	glColor3f(0.95f, 0.95f, 0.95f);  // Very light gray for minor grid
	
	// Draw vertical minor grid lines (every 1 unit)
	for (int x = -14; x <= 14; x++) {
		glBegin(GL_LINES);
			glVertex2f(x / 14.0f, -6.0f / 6.0f);  // Convert to OpenGL coordinates
			glVertex2f(x / 14.0f, 6.0f / 6.0f);
		glEnd();
	}
	
	// Draw horizontal minor grid lines (every 1 unit)
	for (int y = -6; y <= 6; y++) {
		glBegin(GL_LINES);
			glVertex2f(-14.0f / 14.0f, y / 6.0f);  // Convert to OpenGL coordinates
			glVertex2f(14.0f / 14.0f, y / 6.0f);
		glEnd();
	}
	
	// Draw major grid lines (every 2 units) - slightly darker
	glColor3f(0.9f, 0.9f, 0.9f);  // Light gray for major grid
	
	// Draw vertical major grid lines
	for (int x = -14; x <= 14; x += 2) {
		glBegin(GL_LINES);
			glVertex2f(x / 14.0f, -6.0f / 6.0f);
			glVertex2f(x / 14.0f, 6.0f / 6.0f);
		glEnd();
	}
	
	// Draw horizontal major grid lines
	for (int y = -6; y <= 6; y += 2) {
		glBegin(GL_LINES);
			glVertex2f(-14.0f / 14.0f, y / 6.0f);
			glVertex2f(14.0f / 14.0f, y / 6.0f);
		glEnd();
	}
	
	// Draw main axes in darker color
	glColor3f(0.7f, 0.7f, 0.7f);  // Dark gray for axes
	glLineWidth(2.0f);
	
	// X-axis
	glBegin(GL_LINES);
		glVertex2f(-1.0f, 0.0f);  // Left end
		glVertex2f(1.0f, 0.0f);   // Right end
	glEnd();
	
	// Y-axis
	glBegin(GL_LINES);
		glVertex2f(0.0f, -1.0f);  // Bottom end
		glVertex2f(0.0f, 1.0f);   // Top end
	glEnd();
	
	// Draw axis labels as small squares (simulating numbers)
	glColor3f(0.5f, 0.5f, 0.5f);  // Medium gray for labels
	
	// X-axis labels (every 2 units)
	for (int x = -14; x <= 14; x += 2) {
		glBegin(GL_QUADS);
			float xPos = x / 14.0f;
			glVertex2f(xPos - 0.02f, -0.03f);
			glVertex2f(xPos + 0.02f, -0.03f);
			glVertex2f(xPos + 0.02f, 0.03f);
			glVertex2f(xPos - 0.02f, 0.03f);
		glEnd();
	}
	
	// Y-axis labels (every 2 units)
	for (int y = -6; y <= 6; y += 2) {
		glBegin(GL_QUADS);
			float yPos = y / 6.0f;
			glVertex2f(-0.03f, yPos - 0.02f);
			glVertex2f(0.03f, yPos - 0.02f);
			glVertex2f(0.03f, yPos + 0.02f);
			glVertex2f(-0.03f, yPos + 0.02f);
		glEnd();
	}
	
	glLineWidth(1.0f);  // Reset line width
}

void drawTriangleDemo() {
	// Triangle demo from pic 1 - exact implementation of the code shown
	// First triangle block - translated up by 0.5
	glLoadIdentity();
	glTranslatef(0.0f, 0.5f, 0.0f);
	
	// Draw first triangle (default color - white)
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, 0.0f);  // Left vertex
		glVertex2f(0.0f, 0.5f);   // Top vertex
		glVertex2f(0.5f, 0.0f);   // Right vertex
	glEnd();
	
	// Second triangle block - translated down by 0.5 (cumulative with first translation)
	glTranslatef(0.0f, -0.5f, 0.0f);
	
	// Draw second triangle (red color)
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, 0.0f);  // Left vertex
		glVertex2f(0.0f, 0.5f);   // Top vertex
		glVertex2f(0.5f, 0.0f);   // Right vertex
	glEnd();
}

void drawPrimitiveAnswer(int questionNum, int primitiveType) {
	// Define different vertex sets for each question
	float vertices[4][6][2] = {
		// Question 1: Original vertices
		{
			{0.0f, -0.8f},   // Vertex 0
			{-0.5f, -0.5f},  // Vertex 1
			{-0.5f, 0.5f},   // Vertex 2
			{0.0f, 0.0f},    // Vertex 3
			{0.5f, 0.5f},    // Vertex 4
			{0.5f, -0.5f}    // Vertex 5
		},
		// Question 2: Different arrangement
		{
			{-0.6f, -0.6f},  // Vertex 0
			{-0.2f, -0.8f},  // Vertex 1
			{0.2f, -0.4f},   // Vertex 2
			{0.6f, 0.0f},    // Vertex 3
			{0.4f, 0.6f},    // Vertex 4
			{-0.4f, 0.8f}    // Vertex 5
		},
		// Question 3: Square-like pattern
		{
			{-0.7f, -0.7f},  // Vertex 0
			{0.0f, -0.7f},   // Vertex 1
			{0.7f, -0.7f},   // Vertex 2
			{0.7f, 0.0f},    // Vertex 3
			{0.7f, 0.7f},    // Vertex 4
			{0.0f, 0.7f}     // Vertex 5
		},
		// Question 4: Triangle pattern
		{
			{0.0f, -0.8f},   // Vertex 0
			{-0.6f, 0.2f},   // Vertex 1
			{0.6f, 0.2f},    // Vertex 2
			{0.0f, 0.8f},    // Vertex 3
			{-0.3f, 0.0f},   // Vertex 4
			{0.3f, 0.0f}     // Vertex 5
		}
	};
	
	// Set color based on primitive type
	switch (primitiveType) {
		case 0: glColor3f(1.0f, 0.0f, 0.0f); break;  // Red for GL_LINES
		case 1: glColor3f(0.0f, 1.0f, 0.0f); break;  // Green for GL_LINE_STRIP
		case 2: glColor3f(0.0f, 0.0f, 1.0f); break;  // Blue for GL_TRIANGLES
		case 3: glColor3f(1.0f, 0.0f, 1.0f); break;  // Magenta for GL_TRIANGLE_STRIP
	}
	
	// Draw vertices as points for reference
	glColor3f(1.0f, 1.0f, 1.0f);  // White for vertices
	glPointSize(8.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < 6; i++) {
		glVertex2f(vertices[questionNum][i][0], vertices[questionNum][i][1]);
	}
	glEnd();
	
	// Draw vertex numbers
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 6; i++) {
		// Draw small squares to represent vertex numbers
		glBegin(GL_QUADS);
		glVertex2f(vertices[questionNum][i][0] - 0.05f, vertices[questionNum][i][1] - 0.05f);
		glVertex2f(vertices[questionNum][i][0] + 0.05f, vertices[questionNum][i][1] - 0.05f);
		glVertex2f(vertices[questionNum][i][0] + 0.05f, vertices[questionNum][i][1] + 0.05f);
		glVertex2f(vertices[questionNum][i][0] - 0.05f, vertices[questionNum][i][1] + 0.05f);
		glEnd();
	}
	
	// Draw the primitive based on type
	switch (primitiveType) {
		case 0: // GL_LINES - draws lines between pairs of vertices
			glColor3f(1.0f, 0.0f, 0.0f);  // Red
			glBegin(GL_LINES);
			// GL_LINES draws lines between pairs: (0,1), (2,3), (4,5)
			glVertex2f(vertices[questionNum][0][0], vertices[questionNum][0][1]);  // Vertex 0
			glVertex2f(vertices[questionNum][1][0], vertices[questionNum][1][1]);  // Vertex 1
			glVertex2f(vertices[questionNum][2][0], vertices[questionNum][2][1]);  // Vertex 2
			glVertex2f(vertices[questionNum][3][0], vertices[questionNum][3][1]);  // Vertex 3
			glVertex2f(vertices[questionNum][4][0], vertices[questionNum][4][1]);  // Vertex 4
			glVertex2f(vertices[questionNum][5][0], vertices[questionNum][5][1]);  // Vertex 5
			glEnd();
			break;
			
		case 1: // GL_LINE_STRIP - draws connected line segments
			glColor3f(0.0f, 1.0f, 0.0f);  // Green
			glBegin(GL_LINE_STRIP);
			// GL_LINE_STRIP connects all vertices in sequence
			for (int i = 0; i < 6; i++) {
				glVertex2f(vertices[questionNum][i][0], vertices[questionNum][i][1]);
			}
			glEnd();
			break;
			
		case 2: // GL_TRIANGLES - draws triangles from triplets of vertices
			glColor3f(0.0f, 0.0f, 1.0f);  // Blue
			glBegin(GL_TRIANGLES);
			// GL_TRIANGLES draws triangles: (0,1,2), (3,4,5)
			glVertex2f(vertices[questionNum][0][0], vertices[questionNum][0][1]);  // Vertex 0
			glVertex2f(vertices[questionNum][1][0], vertices[questionNum][1][1]);  // Vertex 1
			glVertex2f(vertices[questionNum][2][0], vertices[questionNum][2][1]);  // Vertex 2
			glVertex2f(vertices[questionNum][3][0], vertices[questionNum][3][1]);  // Vertex 3
			glVertex2f(vertices[questionNum][4][0], vertices[questionNum][4][1]);  // Vertex 4
			glVertex2f(vertices[questionNum][5][0], vertices[questionNum][5][1]);  // Vertex 5
			glEnd();
			break;
			
		case 3: // GL_TRIANGLE_STRIP - draws connected triangles
			glColor3f(1.0f, 0.0f, 1.0f);  // Magenta
			glBegin(GL_TRIANGLE_STRIP);
			// GL_TRIANGLE_STRIP draws triangles: (0,1,2), (1,2,3), (2,3,4), (3,4,5)
			for (int i = 0; i < 6; i++) {
				glVertex2f(vertices[questionNum][i][0], vertices[questionNum][i][1]);
			}
			glEnd();
			break;
	}
}

void display()
{
	//--------------------------------
	//	OpenGL drawing
	//--------------------------------
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	
	// If showing triangle demo, display it instead of other demos
	if (showTriangleDemo) {
		// Draw detailed grid background
		drawDetailedGridBackground();
		
		// Draw the triangle demo
		drawTriangleDemo();
		
		// Draw instruction text (simulated with colored rectangles)
		glColor3f(0.0f, 1.0f, 0.0f);  // Green for instructions
		glBegin(GL_QUADS);
			glVertex2f(-0.9f, 0.8f);
			glVertex2f(0.9f, 0.8f);
			glVertex2f(0.9f, 0.9f);
			glVertex2f(-0.9f, 0.9f);
		glEnd();
		
		return;  // Exit early, don't draw other demos
	}
	
	// If showing bow-tie geometry, display it instead of other demos
	if (showBowTie) {
		// Draw detailed grid background
		drawDetailedGridBackground();
		
		// Draw the bow-tie geometry
		drawBowTieGeometry();
		
		// Draw instruction text (simulated with colored rectangles)
		glColor3f(0.0f, 1.0f, 1.0f);  // Cyan for instructions
		glBegin(GL_QUADS);
			glVertex2f(-0.9f, 0.8f);
			glVertex2f(0.9f, 0.8f);
			glVertex2f(0.9f, 0.9f);
			glVertex2f(-0.9f, 0.9f);
		glEnd();
		
		return;  // Exit early, don't draw other demos
	}
	
	// If showing answers, display the primitive tracing instead of the transformation demo
	if (showAnswers) {
		// Draw detailed grid background
		drawDetailedGridBackground();
		
		// Draw the primitive answer
		drawPrimitiveAnswer(currentQuestion, currentAnswerType);
		
		// Draw instruction text (simulated with colored rectangles)
		glColor3f(1.0f, 1.0f, 0.0f);  // Yellow for instructions
		glBegin(GL_QUADS);
			glVertex2f(-0.9f, 0.8f);
			glVertex2f(0.9f, 0.8f);
			glVertex2f(0.9f, 0.9f);
			glVertex2f(-0.9f, 0.9f);
		glEnd();
		
		// Draw question number indicator
		glColor3f(1.0f, 0.5f, 0.0f);  // Orange for question number
		glBegin(GL_QUADS);
			glVertex2f(-0.9f, 0.6f);
			glVertex2f(0.9f, 0.6f);
			glVertex2f(0.9f, 0.7f);
			glVertex2f(-0.9f, 0.7f);
		glEnd();
		
		// Draw primitive type indicator
		glColor3f(0.0f, 1.0f, 1.0f);  // Cyan for primitive type
		glBegin(GL_QUADS);
			glVertex2f(-0.9f, 0.5f);
			glVertex2f(0.9f, 0.5f);
			glVertex2f(0.9f, 0.6f);
			glVertex2f(-0.9f, 0.6f);
		glEnd();
		
		return;  // Exit early, don't draw the transformation demo
	}

	// Draw detailed grid background
	drawDetailedGridBackground();

	// Update rotation animation
	if (isRotating) {
		rotationAngle += rotationSpeed;
		if (rotationAngle >= 90.0f) {
			rotationAngle = 90.0f;
			isRotating = false;
		}
	}
	
	// For Order 2, we need to animate translation in step 2
	// We'll use rotationAngle as a general animation variable
	float translationProgress = 0.0f;
	if (transformationOrder == 2 && currentStep == 2 && isRotating) {
		// Use rotationAngle to animate translation (0 to 0.5)
		translationProgress = (rotationAngle / 90.0f) * 0.5f;
	}
	
	// For Order 2, we need to animate rotation in step 1
	float rotationProgress = 0.0f;
	if (transformationOrder == 2 && currentStep == 1 && isRotating) {
		rotationProgress = rotationAngle;
	}

	// Draw triangle based on current step and transformation order
	glPushMatrix();
	
	// IMPORTANT: OpenGL applies transformations in REVERSE ORDER!
	// The LAST transformation you write is applied FIRST!
	
	if (transformationOrder == 1) {
		// ORDER 1: Translate first, then Rotate
		// Step 1: Translation
		if (currentStep >= 1) {
			if (currentStep == 3) {
				// Key 3: Translate 0.2 down and 0.5 right
				glTranslatef(0.5f, -0.2f, 0.0f);
			} else {
				// Original animation: Move triangle left by 0.5 units
				glTranslatef(-0.5f, 0.0f, 0.0f);
			}
		}
		// Step 2: Rotation  
		if (currentStep >= 2) {
			glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);  // Rotate around Z-axis
		}
	}
	else {
		// ORDER 2: Rotate first, then Translate
		// Step 1: Rotation (animated)
		if (currentStep >= 1) {
			if (currentStep == 1 && isRotating) {
				// Animate rotation from 0 to 90 degrees
				glRotatef(rotationProgress, 0.0f, 0.0f, 1.0f);
			}
			else {
				// Final rotation (90 degrees or -90 for key 3)
				glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
			}
		}
		// Step 2: Translation (animated)
		if (currentStep >= 2) {
			if (currentStep == 2 && isRotating) {
				// Animate translation from 0 to -0.5
				glTranslatef(-translationProgress, 0.0f, 0.0f);
			}
			else {
				// Final translation
				if (currentStep == 3) {
					// Key 3: Translate 0.2 down and 0.5 right
					glTranslatef(0.5f, -0.2f, 0.0f);
				} else {
					// Original animation: Move left by 0.5
					glTranslatef(-0.5f, 0.0f, 0.0f);
				}
			}
		}
	}
	
	// Choose color based on step and order
	if (transformationOrder == 1) {
		switch (currentStep) {
			case 0: glColor3f(1.0f, 1.0f, 1.0f); break;  // White - original position
			case 1: glColor3f(0.0f, 1.0f, 0.0f); break;  // Green - after translation only
			case 2: glColor3f(1.0f, 0.0f, 0.0f); break;  // Red - during rotation animation
			case 3: glColor3f(1.0f, 1.0f, 0.0f); break;  // Yellow - final result (both applied)
		}
	}
	else {
		switch (currentStep) {
			case 0: glColor3f(1.0f, 1.0f, 1.0f); break;  // White - original position
			case 1: glColor3f(0.0f, 0.0f, 1.0f); break;  // Blue - after rotation only
			case 2: glColor3f(1.0f, 0.0f, 1.0f); break;  // Magenta - during translation animation
			case 3: glColor3f(0.0f, 1.0f, 1.0f); break;  // Cyan - final result (both applied)
		}
	}
	
	// Draw the triangle with specified vertices
	glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, 0.0f);  // Left vertex
		glVertex2f(0.0f, 0.5f);   // Top vertex  
		glVertex2f(0.5f, 0.0f);   // Right vertex
	glEnd();
	glPopMatrix();
	
	// Draw explanation text (if you want to add text rendering later)
	// For now, watch the colors and positions to understand the order!

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