#include <GL/glut.h>
#include <math.h>
#include "RGBpixmap.h" 
#include "SolarSystemGlobals.h"
#include "ParticleSystem.h"

GLfloat viewerAzimuth = INITIAL_VIEWER_AZIMUTH;
GLfloat viewerZenith = INITIAL_VIEWER_ZENITH;

GLuint ParticleTextureName = -1;
GLuint EarthTextureName = 3;
GLuint MoonTextureName = 4;
GLuint SunTextureName = 0;
GLuint MercuryTextureName = 1;
GLuint VenusTextureName = 2;
GLuint MarsTextureName = 5;
GLuint JupiterTextureName = 6;
GLuint SaturnTextureName = 7;
GLuint UranusTextureName = 8;
GLuint NeptuneTextureName = 9;
GLuint PlutoTextureName = 10;
GLuint RingTextureName = 11;

GLfloat CurrentEarthRotation = 0.00;
GLfloat EarthDaysTranspired = 0.00;
GLfloat EarthDayIncrement = 0.01;

GLint currWindowSize[2] = { 750, 750 / ASPECT_RATIO };
GLint currViewportSize[2] = { 750, 750 / ASPECT_RATIO };

GLfloat ViewerDistance = INITIAL_VIEWER_DISTANCE;

ParticleSystem particles;
bool particleFlag = true;
bool blendFlag = true;

void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition);
void NonASCIIKeyboardPress(int pressedKey, int mouseXPosition, int mouseYPosition);
void TimerFunction(int value);
void Display();
void MyDisplay();
void MakeAllImages();
void MakeImage(const char bitmapFilename[], GLuint& textureName, bool hasAlpha);

void SetLights();
void UpdateLight();

void ResizeWindow(GLsizei w, GLsizei h);

void drawEarthAndMoon();
void drawSun();
void drawSaturnRing();
void drawAllPlanets();
void drawGenericPlanet(GLfloat inclination, GLfloat orbitDuration,
	GLfloat orbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius);
void drawParticle(Particle currParticle);
void drawAllParticles();
void SortDisplay();
void drawSortPlanets();
void drawGenericPlanet1(GLfloat inclination, GLfloat xpos, GLfloat ypos, GLuint texturename, GLfloat radius);

void sortedRing(GLfloat xpos, GLfloat ypos, GLfloat radius);
void Mercury();
void Venus();
void Earth();
void Mars();
void Jupiter();
void Saturn();
void Uranus();
void Neptune();
void Pluto();

int main(int argc, char** argv)
{
	char ch;
	printf("\tWelcome to Solar System\n");
	printf("Press Enter to Continue...\n");
	scanf_s("%c", &ch);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL | GLUT_DEPTH);
	glutInitWindowPosition(INIT_WINDOW_POSITION[0], INIT_WINDOW_POSITION[1]);
	glutInitWindowSize(currWindowSize[0], currWindowSize[1]);
	glutCreateWindow("Solar System");
	glutReshapeFunc(ResizeWindow);
	glutKeyboardFunc(KeyboardPress);
	glutSpecialFunc(NonASCIIKeyboardPress);
	glutDisplayFunc(MyDisplay);
	glViewport(0, 0, currWindowSize[0], currWindowSize[1]);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);							
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_NORMALIZE);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	SetLights();
	MakeAllImages();
	glutMainLoop();
	return 0;
}

void MyDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 640, 0, 480, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	{
		glColor3f(1.0, 1.0, 1.0);
		glPointSize(2);
		glBegin(GL_POINTS);
		for (int i = 1; i < 100; i++)
		{
			int x = rand() % 640;
			int y = rand() % 480;
			glVertex2i(x, y);
		}
		glEnd();
	}
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(220.0, 360.0, 0.0);
	char st1[] = "WELCOME TO THE SOLAR SYSTEM!";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);

	glRasterPos3f(50.0, 310.0, 0.0);
	char st2[] = "-> Press ENTER to view the complete Solar System";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(70.0, 290.0, 0.0);
	char st3[] = "- Use + and - keys to increase / decrease the speed of movement";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(70.0, 270.0, 0.0);
	char st4[] = "- Use ARROW keys for top, bottom, left and right views";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(70.0, 250.0, 0.0);
	char st5[] = "- Use z to zoom in and Z to zoom out";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);

	glRasterPos3f(50.0, 210.0, 0.0);
	char st6[] = "-> Press S to view the planets based on size";
	for (int i = 0; i < strlen(st6); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st6[i]);


	glRasterPos3f(50.0, 170.0, 0.0);
	char st8[] = "-> Press the first letter(lowercase) of the respective planet to get a detailed view";
	for (int i = 0; i < strlen(st8); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st8[i]);

	glRasterPos3f(70.0, 150.0, 0.0);
	char st10[] = "- m for Mercury and M for Mars";
	for (int i = 0; i < strlen(st10); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st10[i]);

	glRasterPos3f(500.0, 100.0, 0.0);
	char st9[] = "- BY RAMYA AND SANJANA B.S";
	for (int i = 0; i < strlen(st9); i++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, st9[i]);
	glutSwapBuffers();
}

void KeyboardPress(unsigned char pressedKey, int mouseXPosition, int mouseYPosition)
{
	char pressedChar = char(pressedKey);
	switch (pressedKey)
	{
	case '\r': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Display);
		glutPostRedisplay();
		break;
	}
	case 'S': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(SortDisplay);
		glutPostRedisplay();
		break;
	}
	case 'm': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Mercury);
		glutPostRedisplay();
		break;
	}
	case 'v': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Venus);
		glutPostRedisplay();
		break;
	}
	case 'e': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Earth);
		glutPostRedisplay();
		break;
	}
	case 'M': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Mars);
		glutPostRedisplay();
		break;
	}
	case 'j': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Jupiter);
		glutPostRedisplay();
		break;
	}
	case 's': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Saturn);
		glutPostRedisplay();
		break;
	}
	case 'u': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Uranus);
		glutPostRedisplay();
		break;
	}
	case 'n': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Neptune);
		glutPostRedisplay();
		break;
	}
	case 'p': {
		glutTimerFunc(20, TimerFunction, 1);
		glutDisplayFunc(Pluto);
		glutPostRedisplay();
		break;
	}


	case '+': {
		EarthDayIncrement *= 2.0;
		if (EarthDayIncrement > 10.0)
			EarthDayIncrement = 10.0;
		glutTimerFunc(20, TimerFunction, 1);
		break;
	}
	case '-': {
		EarthDayIncrement *= 0.5;
		if (EarthDayIncrement < 0.01)
			EarthDayIncrement = 0.01;
		glutTimerFunc(20, TimerFunction, 1);
		break;
	}
	case 'z': {
		ViewerDistance -= VIEWER_DISTANCE_INCREMENT;
		if (ViewerDistance < MINIMUM_VIEWER_DISTANCE)
			ViewerDistance = MINIMUM_VIEWER_DISTANCE;
		break;
	}
	case 'Z': {
		ViewerDistance += VIEWER_DISTANCE_INCREMENT;
		if (ViewerDistance > MAXIMUM_VIEWER_DISTANCE)
			ViewerDistance = MAXIMUM_VIEWER_DISTANCE;
		break;
	}


	}
}

void NonASCIIKeyboardPress(int pressedKey, int mouseXPosition, int mouseYPosition)
{
	glutIgnoreKeyRepeat(false);
	switch (pressedKey)
	{
	case GLUT_KEY_RIGHT: {
		viewerAzimuth += VIEWER_ANGLE_INCREMENT;
		if (viewerAzimuth > 2 * PI)
			viewerAzimuth -= 2 * PI;
		break;
	}
	case GLUT_KEY_LEFT: {
		viewerAzimuth -= VIEWER_ANGLE_INCREMENT;
		if (viewerAzimuth < 0.0)
			viewerAzimuth += 2 * PI;
		break;
	}
	case GLUT_KEY_UP: {
		viewerZenith -= VIEWER_ANGLE_INCREMENT;
		if (viewerZenith < VIEWER_ANGLE_INCREMENT)
			viewerZenith = VIEWER_ANGLE_INCREMENT;
		break;
	}
	case GLUT_KEY_DOWN: {
		viewerZenith += VIEWER_ANGLE_INCREMENT;
		if (viewerZenith > PI - VIEWER_ANGLE_INCREMENT)
			viewerZenith = PI - VIEWER_ANGLE_INCREMENT;
		break;
	}
	}
}
void TimerFunction(int value)
{
	CurrentEarthRotation += EarthDayIncrement;
	EarthDaysTranspired += EarthDayIncrement;
	if (EarthDaysTranspired == EARTH_ORBIT_DUR)
		EarthDaysTranspired = 0;
	glutPostRedisplay();
	glutTimerFunc(20, TimerFunction, 1);
}
void Display()
{

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LIGHT_MODEL_AMBIENT);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	UpdateLight();
	drawEarthAndMoon();
	drawSun();
	drawAllPlanets();
	drawSaturnRing();

	if (blendFlag) { 
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
	}
	if (particleFlag)
		drawAllParticles();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 640, 0, 480, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	{
		glColor3f(1.0, 1.0, 1.0);
		glPointSize(2);
		glBegin(GL_POINTS);
		for (int i = 1; i < 20; i++)
		{
			int x = rand() % 640;
			int y = rand() % 480;
			glVertex2i(x, y);
		}
		glEnd();
	}
	glPopMatrix();
	glutSwapBuffers();
	glFlush();
}

void SortDisplay() {
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LIGHT_MODEL_AMBIENT);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);

	// Render scene.
	UpdateLight();
	drawSortPlanets();
	sortedRing(3.0, 0.0, SATURN_RADIUS);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	// 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 640, 0, 480, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	{
		glColor3f(1.0, 1.0, 1.0);
		glPointSize(2);
		glBegin(GL_POINTS);
		for (int i = 1; i < 20; i++)
		{
			int x = rand() % 640;
			int y = rand() % 480;
			glVertex2i(x, y);
		}
		glEnd();
	}
	glPopMatrix();
	glutSwapBuffers();
	glFlush();
}

void drawSortPlanets()
{
	drawGenericPlanet1(PLUTO_INCLINATION, -6.5, -0.07, PlutoTextureName, PLUTO_RADIUS);
	drawGenericPlanet1(MERCURY_INCLINATION, -5.5, 0.5, MercuryTextureName, MERCURY_RADIUS);
	drawGenericPlanet1(MARS_INCLINATION, -4.5, 0.0, MarsTextureName, MARS_RADIUS);
	drawGenericPlanet1(VENUS_INCLINATION, -3.5, 0.0, VenusTextureName, VENUS_RADIUS);
	drawGenericPlanet1(EARTH_INCLINATION, -2.0, 0.15, EarthTextureName, EARTH_RADIUS);
	drawGenericPlanet1(NEPTUNE_INCLINATION, -0.5, 0.0, NeptuneTextureName, NEPTUNE_RADIUS);
	drawGenericPlanet1(URANUS_INCLINATION, 1.3, 0.0, UranusTextureName, URANUS_RADIUS);
	drawGenericPlanet1(SATURN_INCLINATION, 3.0, 0.0, SaturnTextureName, SATURN_RADIUS);
	drawGenericPlanet1(JUPITER_INCLINATION, 5.0, 0.0, JupiterTextureName, JUPITER_RADIUS);

}
void sortedRing(GLfloat xpos, GLfloat ypos, GLfloat radius)
{
	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH);
	gluQuadricTexture(quadro, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	glRotatef(SATURN_INCLINATION, 0.0, 0.0, 1.0);
	glTranslatef(xpos, ypos, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, RingTextureName);
	glScalef(1, 1, .02);
	gluSphere(quadro, radius * 2, 48, 48);
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);

}
void Mercury()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(MERCURY_INCLINATION, 0.0, 0.0, MercuryTextureName, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(60.0, 900.0, 0.0);
	char st1[] = "MERCURY";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 1407.5 hrs (58.646 Earth days)";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 88 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 3.285 x 10^23 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 200.0, 0.0);
	char st5[] = "Distance from Sun = 53.015 million km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();
}
void Venus()
{
	// Set up the properties of the viewing camera.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(VENUS_INCLINATION, 0.0, 0.0, VenusTextureName, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(160.0, 900.0, 0.0);
	char st1[] = "VENUS";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 5832 hrs (243 Earth days)";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 224.7 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 4.867 × 10^24 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 200.0, 0.0);
	char st5[] = "Distance from Sun = 108.48 million km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();

}

void Earth()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(EARTH_INCLINATION, 0.0, 0.0, EarthTextureName, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(160.0, 900.0, 0.0);
	char st1[] = "EARTH";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 24 hrs";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 365.26 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 5.972 x 10^24 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 200.0, 0.0);
	char st5[] = "Distance from Sun = 152.07 million km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();

}
void Mars()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(MARS_INCLINATION, 0.0, 0.0, MarsTextureName, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(160.0, 900.0, 0.0);
	char st1[] = "MARS";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 24.6 (1 Earth day)";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 687 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 6.39 x 10^23 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 200.0, 0.0);
	char st5[] = "Distance from Sun = 207.91 million km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();
}

void Jupiter()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(JUPITER_INCLINATION, 0.0, 0.0, JupiterTextureName, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(90.0, 900.0, 0.0);
	char st1[] = "JUPITER";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 9.8 hrs (0.41 Earth days)";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 4333 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 1.898 x 10^27 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 200.0, 0.0);
	char st5[] = "Distance from Sun = 742.73 million km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();
}

void Saturn()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(SATURN_INCLINATION, 0.0, 0.0, SaturnTextureName, 2.0);
	sortedRing(0.0, 0.0, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(90.0, 900.0, 0.0);
	char st1[] = "SATURN";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 10.7 hrs (0.45 Earth days)";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 10756 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 5.683 x 10^26 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 150.0, 0.0);
	char st5[] = "Distance from Sun = 1.4759 billion km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();
}

void Uranus()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(URANUS_INCLINATION, 0.0, 0.0, UranusTextureName, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(90.0, 900.0, 0.0);
	char st1[] = "URANUS";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 17 hrs (0.72 Earth days)";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 30687 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 8.681 x 10^25 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 200.0, 0.0);
	char st5[] = "Distance from Sun = 2.9459 billion km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();
}

void Neptune()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(NEPTUNE_INCLINATION, 0.0, 0.0, NeptuneTextureName, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(90.0, 900.0, 0.0);
	char st1[] = "NEPTUNE";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 16 hrs (0.67 Earth days)";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 60190 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 1.024 x 10^26 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 200.0, 0.0);
	char st5[] = "Distance from Sun = 4.4742 billion km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();
}
void Pluto()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ASPECT_RATIO, 0.2, 100.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Position and orient viewer.
	gluLookAt(LOOK_AT_POSITION[0] + ViewerDistance * sin(viewerZenith) * sin(viewerAzimuth),
		LOOK_AT_POSITION[1] + ViewerDistance * cos(viewerZenith),
		LOOK_AT_POSITION[2] + ViewerDistance * sin(viewerZenith) * cos(viewerAzimuth),
		LOOK_AT_POSITION[0], LOOK_AT_POSITION[1], LOOK_AT_POSITION[2],
		0.0, 1.0, 0.020);
	drawGenericPlanet1(PLUTO_INCLINATION, 0.0, 0.0, PlutoTextureName, 2.0);
	glOrtho(0, 640, 0, 480, -1, 1);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(130.0, 900.0, 0.0);
	char st1[] = "PLUTO";
	for (int i = 0; i < strlen(st1); i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st1[i]);


	glRasterPos3f(1100.0, 350.0, 0.0);
	char st2[] = "1 Rotation = 153 hrs (6.39 Earth days)";
	for (int i = 0; i < strlen(st2); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st2[i]);

	glRasterPos3f(1100.0, 200.0, 0.0);
	char st3[] = "1 Revolution = 90560 days";
	for (int i = 0; i < strlen(st3); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st3[i]);

	glRasterPos3f(-1800.0, 350.0, 0.0);
	char st4[] = "Mass = 1.309 x 10^22 kg";
	for (int i = 0; i < strlen(st4); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st4[i]);

	glRasterPos3f(-1800.0, 200.0, 0.0);
	char st5[] = "Distance from Sun = 5.925 billion km";
	for (int i = 0; i < strlen(st5); i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, st5[i]);
	glutSwapBuffers();
	glFlush();
}
void drawGenericPlanet1(GLfloat inclination, GLfloat xpos, GLfloat ypos, GLuint texturename, GLfloat radius)
{
	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH);
	gluQuadricTexture(quadro, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	glRotatef(inclination, 0.0, 0.0, 1.0);
	glTranslatef(xpos, ypos, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, texturename);
	gluSphere(quadro, radius, 48, 48);
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);
}
// Create the textures associated with all texture-mapped objects being displayed. //
void MakeAllImages()
{
	MakeImage(EARTH_BMP_FILENAME, EarthTextureName, false);
	MakeImage(MOON_BMP_FILENAME, MoonTextureName, false);
	MakeImage(SUN_BMP_FILENAME, SunTextureName, false);
	MakeImage(MERCURY_BMP_FILENAME, MercuryTextureName, false);
	MakeImage(VENUS_BMP_FILENAME, VenusTextureName, false);
	MakeImage(PARTICLE_BMP_FILENAME, ParticleTextureName, false);
	MakeImage(MARS_BMP_FILENAME, MarsTextureName, false);
	MakeImage(JUPITER_BMP_FILENAME, JupiterTextureName, false);
	MakeImage(SATURN_BMP_FILENAME, SaturnTextureName, false);
	MakeImage(URANUS_BMP_FILENAME, UranusTextureName, false);
	MakeImage(NEPTUNE_BMP_FILENAME, NeptuneTextureName, false);
	MakeImage(PLUTO_BMP_FILENAME, PlutoTextureName, false);
	MakeImage(RING_BMP_FILENAME, RingTextureName, false);
	return;
}

void drawAllPlanets() {
	drawGenericPlanet(MERCURY_INCLINATION, MERCURY_ORBIT_DUR, MERCURY_ORBIT_RADIUS,
		MERCURY_ROTATION_DUR, MercuryTextureName, MERCURY_RADIUS);
	drawGenericPlanet(VENUS_INCLINATION, VENUS_ORBIT_DUR, VENUS_ORBIT_RADIUS,
		VENUS_ROTATION_DUR, VenusTextureName, VENUS_RADIUS);
	drawGenericPlanet(MARS_INCLINATION, MARS_ORBIT_DUR, MARS_ORBIT_RADIUS,
		MARS_ROTATION_DUR, MarsTextureName, MARS_RADIUS);
	drawGenericPlanet(JUPITER_INCLINATION, JUPITER_ORBIT_DUR, JUPITER_ORBIT_RADIUS,
		JUPITER_ROTATION_DUR, JupiterTextureName, JUPITER_RADIUS);
	drawGenericPlanet(SATURN_INCLINATION, SATURN_ORBIT_DUR, SATURN_ORBIT_RADIUS,
		SATURN_ROTATION_DUR, SaturnTextureName, SATURN_RADIUS);
	drawGenericPlanet(URANUS_INCLINATION, URANUS_ORBIT_DUR, URANUS_ORBIT_RADIUS,
		URANUS_ROTATION_DUR, UranusTextureName, URANUS_RADIUS);
	drawGenericPlanet(NEPTUNE_INCLINATION, NEPTUNE_ORBIT_DUR, NEPTUNE_ORBIT_RADIUS,
		NEPTUNE_ROTATION_DUR, NeptuneTextureName, NEPTUNE_RADIUS);
	drawGenericPlanet(PLUTO_INCLINATION, PLUTO_ORBIT_DUR, PLUTO_ORBIT_RADIUS,
		PLUTO_ROTATION_DUR, PlutoTextureName, PLUTO_RADIUS);

}

void MakeImage(const char bitmapFilename[], GLuint& textureName, bool hasAlpha)
{
	RGBpixmap pix;
	pix.readBMPFile(bitmapFilename, hasAlpha);
	pix.setTexture(textureName);
	return;
}

void SetLights()
{
	glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT_SPECULAR);
	glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_0_POSITION);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT1, GL_SPECULAR, LIGHT_SPECULAR);
	glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_1_POSITION);

	glLightfv(GL_LIGHT2, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT2, GL_SPECULAR, LIGHT_SPECULAR);
	glLightfv(GL_LIGHT2, GL_POSITION, LIGHT_2_POSITION);

	glLightfv(GL_LIGHT3, GL_AMBIENT, LIGHT_AMBIENT);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, LIGHT_DIFFUSE);
	glLightfv(GL_LIGHT3, GL_SPECULAR, LIGHT_SPECULAR);
	glLightfv(GL_LIGHT3, GL_POSITION, LIGHT_3_POSITION);
}


// Enable the scene's lighting. //
void UpdateLight()
{
	glPushMatrix();
	glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_0_POSITION);
	glLightfv(GL_LIGHT1, GL_POSITION, LIGHT_1_POSITION);
	glLightfv(GL_LIGHT2, GL_POSITION, LIGHT_2_POSITION);
	glLightfv(GL_LIGHT3, GL_POSITION, LIGHT_3_POSITION);
	glPopMatrix();

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void ResizeWindow(GLsizei w, GLsizei h)
{
	currWindowSize[0] = w;
	currWindowSize[1] = h;
	if (ASPECT_RATIO > w / h)
	{
		currViewportSize[0] = w;
		currViewportSize[1] = w / ASPECT_RATIO;
	}
	else
	{
		currViewportSize[1] = h;
		currViewportSize[0] = h * ASPECT_RATIO;
	}

	glViewport(0.5 * (w - currViewportSize[0]), 0.5 * (h - currViewportSize[1]), currViewportSize[0], currViewportSize[1]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//Draws the texture-mapped earth and moon. //
void drawEarthAndMoon()
{
	GLfloat MoonRevolution = EarthDaysTranspired / LUNAR_CYCLE;
	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH);
	gluQuadricTexture(quadro, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	glRotatef(EARTH_INCLINATION, 0.0, 0.0, 1.0);
	glRotatef(360.0 * (EarthDaysTranspired / EARTH_ORBIT_DUR), 0.0, 1.0, 0.0);
	glTranslatef(EARTH_ORBIT_RADIUS, 0.0, 0.0);
	glRotatef(360.0 * CurrentEarthRotation, 0.0, 1.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, EarthTextureName);
	gluSphere(quadro, EARTH_RADIUS, 48, 48);
	glPopMatrix();
	glRotatef(EARTH_INCLINATION, 0.0, 0.0, 1.0);
	glRotatef(360.0 * (EarthDaysTranspired / EARTH_ORBIT_DUR), 0.0, 1.0, 0.0);
	glTranslatef(EARTH_ORBIT_RADIUS, 0.0, 0.0);
	glRotatef(360.0 * MoonRevolution, 0.0, 1.0, 0.0);
	glTranslatef(MOON_ORBIT_RADIUS, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, MoonTextureName);
	gluSphere(quadro, MOON_RADIUS, 48, 48);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);
}

//Function to draw and texture map the sun at the origin
void drawSun()
{
	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH);
	gluQuadricTexture(quadro, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, SunTextureName);
	gluSphere(quadro, SUN_RADIUS, 48, 48);
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);
}


//Draw Rings

void drawSaturnRing()
{
	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH);
	gluQuadricTexture(quadro, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	glRotatef(SATURN_INCLINATION, 0.0, 0.0, 1.0);
	glRotatef(360.0 * (EarthDaysTranspired / SATURN_ORBIT_DUR), 0.0, 1.0, 0.0);
	glTranslatef(SATURN_ORBIT_RADIUS, 0.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, RingTextureName);
	glScalef(1, 1, .02);
	gluSphere(quadro, SATURN_RADIUS * 2, 48, 48);
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);
}

void drawGenericPlanet(GLfloat inclination, GLfloat orbitDuration,
	GLfloat orbitRadius, GLfloat rotationDuration, GLuint texturename, GLfloat radius)
{
	GLUquadricObj* quadro = gluNewQuadric();
	gluQuadricNormals(quadro, GLU_SMOOTH);
	gluQuadricTexture(quadro, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	glRotatef(inclination, 0.0, 0.0, 1.0);
	glRotatef(360.0 * (EarthDaysTranspired / orbitDuration), 0.0, 1.0, 0.0);
	glTranslatef(orbitRadius, 0.0, 0.0);
	glRotatef(360.0 * (CurrentEarthRotation / rotationDuration), 0.0, 1.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, texturename);
	gluSphere(quadro, radius, 48, 48);
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(quadro);
}

void drawAllParticles() {
	particles.updateAll();
	for (int i = 0; i < particles.getNumberOfParticles(); i++)
		drawParticle(particles.getNextParticle());

}
void drawParticle(Particle currParticle)
{
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glRotatef(currParticle.azimuthRoation, 0, 1, 0);
	glRotatef(currParticle.zenithRotation, 0, 0, 1);
	glTranslatef(SUN_RADIUS + currParticle.surfaceTranslationFactor, 0, 0);
	glRotatef(90, 0, 1, 0);
	glScalef(.5, .5, 1.0);
	glBindTexture(GL_TEXTURE_2D, ParticleTextureName);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1, 1);
	glVertex3f(0.5f, 0.5f, 0.0f); 
	glTexCoord2d(0, 1);
	glVertex3f(-0.5f, 0.5f, 0.0f); 
	glTexCoord2d(1, 0);
	glVertex3f(0.5f, -0.5f, 0.0f); 
	glTexCoord2d(0, 0);
	glVertex3f(-0.5f, -0.5f, 0.0f); 
	glEnd();

	// Logo Facing Away From Earth
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1, 1);
	glVertex3f(-0.5f, 0.5f, 0.0f); // Top Right
	glTexCoord2d(0, 1);
	glVertex3f(0.5f, 0.5f, 0.0f); // Top Left
	glTexCoord2d(1, 0);
	glVertex3f(-0.5f, -0.5f, 0.0f); // Bottom Right
	glTexCoord2d(0, 0);
	glVertex3f(0.5f, -0.5f, 0.0f); // Bottom Left
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
