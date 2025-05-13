/*
** Simple example of rendering to a Windows DIB (Device Independent Bitmap)
*/

#include "lxR2D.h"
#include <math.h>
#include <stdlib.h>
#include <GL/gl.h>

#if !defined(M_PI)
#define M_PI 3.14159265F
#endif

#define colorIndexMode FALSE
#define doubleBuffered FALSE
#define renderToDIB TRUE

struct _R2DCTX {
	int winWidth, winHeight;
	HDC hDC;
	HGLRC hGLRC;
	HPALETTE hPalette;
	HBITMAP hBitmap;
};




/* Struct used to manage color ramps */
struct R2DcolorIndexState {
	GLfloat amb[3];	/* ambient color / bottom of ramp */
	GLfloat diff[3];	/* diffuse color / middle of ramp */
	GLfloat spec[3];	/* specular color / top of ramp */
	GLfloat ratio;	/* ratio of diffuse to specular in ramp */
	GLint indexes[3];	/* where ramp was placed in palette */
};

/*
** Each entry in this array corresponds to a color ramp in the
** palette.  The indexes member of each struct is updated to
** reflect the placement of the color ramp in the palette.
*/
#define NUM_COLORS (sizeof(R2Dcolors) / sizeof(R2Dcolors[0]))
struct R2DcolorIndexState R2Dcolors[] = {
	{
		{ 0.0F, 0.0F, 0.0F },
		{ 0.1F, 0.6F, 0.3F },
		{ 1.0F, 1.0F, 1.0F },
		0.75F, { 0, 0, 0 },
	},
	{
		{ 0.0F, 0.0F, 0.0F },
		{ 0.0F, 0.2F, 0.5F },
		{ 1.0F, 1.0F, 1.0F },

		0.75F, { 0, 0, 0 },
	},
		{
			{ 0.0F, 0.05F, 0.05F },
			{ 0.6F, 0.0F, 0.8F },
			{ 1.0F, 1.0F, 1.0F },
			0.75F, { 0, 0, 0 },
		},
};


/*****************************************************************/

void
R2DsetupDIB(R2DContext * ctx)
{
	HDC hDC = ctx->hDC;
	BITMAPINFO *bmInfo;
	BITMAPINFOHEADER *bmHeader;
	UINT usage;
	VOID *base;
	int bmiSize;
	int bitsPerPixel;

	bmiSize = sizeof(*bmInfo);
	bitsPerPixel = GetDeviceCaps(hDC, BITSPIXEL);

	switch (bitsPerPixel) {
case 8:
	/* bmiColors is 256 WORD palette indices */
	bmiSize += (256 * sizeof(WORD)) - sizeof(RGBQUAD);
	break;
case 16:
	/* bmiColors is 3 WORD component masks */
	bmiSize += (3 * sizeof(DWORD)) - sizeof(RGBQUAD);
	break;
case 24:
case 32:
default:
	/* bmiColors not used */
	break;
	}

	bmInfo = (BITMAPINFO *) calloc(1, bmiSize);
	bmHeader = &bmInfo->bmiHeader;

	bmHeader->biSize = sizeof(*bmHeader);
	bmHeader->biWidth = ctx->winWidth;
	bmHeader->biHeight = ctx->winHeight;
	bmHeader->biPlanes = 1;			/* must be 1 */
	bmHeader->biBitCount = (WORD) bitsPerPixel;
	bmHeader->biXPelsPerMeter = 0;
	bmHeader->biYPelsPerMeter = 0;
	bmHeader->biClrUsed = 0;			/* all are used */
	bmHeader->biClrImportant = 0;		/* all are important */

	switch (bitsPerPixel) {
case 8:
	bmHeader->biCompression = BI_RGB;
	bmHeader->biSizeImage = 0;
	usage = DIB_PAL_COLORS;
	/* bmiColors is 256 WORD palette indices */
	{
		WORD *palIndex = (WORD *) &bmInfo->bmiColors[0];
		int i;

		for (i=0; i<256; i++) {
			palIndex[i] = (WORD)i;
		}
	}
	break;
case 16:
	bmHeader->biCompression = BI_RGB;
	bmHeader->biSizeImage = 0;
	usage = DIB_RGB_COLORS;
	/* bmiColors is 3 WORD component masks */
	{
		DWORD *compMask = (DWORD *) &bmInfo->bmiColors[0];

		compMask[0] = 0xF800;
		compMask[1] = 0x07E0;
		compMask[2] = 0x001F;
	}
	break;
case 24:
case 32:
default:
	bmHeader->biCompression = BI_RGB;
	bmHeader->biSizeImage = 0;
	usage = DIB_RGB_COLORS;
	/* bmiColors not used */
	break;
	}

	ctx->hBitmap = CreateDIBSection(hDC, bmInfo, usage, &base, NULL, 0);
	if (ctx->hBitmap == NULL) {
		(void) MessageBox(WindowFromDC(hDC),
			"Failed to create DIBSection.",
			"OpenGL application error",
			MB_ICONERROR | MB_OK);
		exit(1);
	}

	SelectObject(hDC, ctx->hBitmap);

	free(bmInfo);
}


void
R2DsetupPalette(R2DContext * ctx)
{
	HDC hDC = ctx->hDC;
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE* pPal;
	int pixelFormat = GetPixelFormat(hDC);
	int paletteSize;

	DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	/*
	** Determine if a palette is needed and if so what size.
	*/
	if (pfd.dwFlags & PFD_NEED_PALETTE) {
		paletteSize = 1 << pfd.cColorBits;
	} else if (pfd.iPixelType == PFD_TYPE_COLORINDEX) {
		paletteSize = 4096;
	} else {
		return;
	}

	pPal = (LOGPALETTE*)
		malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
	pPal->palVersion = 0x300;
	pPal->palNumEntries = (WORD) paletteSize;

	if (pfd.iPixelType == PFD_TYPE_RGBA) {
		/*
		** Fill the logical paletee with RGB color ramps
		*/
		int redMask = (1 << pfd.cRedBits) - 1;
		int greenMask = (1 << pfd.cGreenBits) - 1;
		int blueMask = (1 << pfd.cBlueBits) - 1;
		int i;

		for (i=0; i<paletteSize; ++i) {
			pPal->palPalEntry[i].peRed = (BYTE)(
				(((i >> pfd.cRedShift) & redMask) * 255) / redMask);
			pPal->palPalEntry[i].peGreen = (BYTE)(
				(((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask);
			pPal->palPalEntry[i].peBlue = (BYTE)(
				(((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask);
			pPal->palPalEntry[i].peFlags = 0;
		}
	} else {
		/*
		** Fill the logical palette with color ramps.
		**
		** Set up the logical palette so that it can be realized
		** into the system palette as an identity palette.
		**
		** 1) The default static entries should be present and at the right
		**    location.  The easiest way to do this is to grab them from
		**    the current system palette.
		**
		** 2) All non-static entries should be initialized to unique values.
		**    The easiest way to do this is to ensure that all of the non-static
		**    entries have the PC_NOCOLLAPSE flag bit set.
		*/
		int numRamps = NUM_COLORS;
		int rampSize = (paletteSize - 20) / numRamps;
		int extra = (paletteSize - 20) - (numRamps * rampSize);
		int i, r;

		/*
		** Initialize static entries by copying them from the
		** current system palette.
		*/
		GetSystemPaletteEntries(hDC, 0, paletteSize, &pPal->palPalEntry[0]);

		/*
		** Fill in non-static entries with desired colors.
		*/
		for (r=0; r<numRamps; ++r) {
			int rampBase = r * rampSize + 10;
			PALETTEENTRY *pe = &pPal->palPalEntry[rampBase];
			int diffSize = (int) (rampSize * R2Dcolors[r].ratio);
			int specSize = rampSize - diffSize;

			for (i=0; i<rampSize; ++i) {
				GLfloat *c0, *c1;
				GLint a;

				if (i < diffSize) {
					c0 = R2Dcolors[r].amb;
					c1 = R2Dcolors[r].diff;
					a = (i * 255) / (diffSize - 1);
				} else {
					c0 = R2Dcolors[r].diff;
					c1 = R2Dcolors[r].spec;
					a = ((i - diffSize) * 255) / (specSize - 1);
				}

				pe[i].peRed = (BYTE) (a * (c1[0] - c0[0]) + 255 * c0[0]);
				pe[i].peGreen = (BYTE) (a * (c1[1] - c0[1]) + 255 * c0[1]);
				pe[i].peBlue = (BYTE) (a * (c1[2] - c0[2]) + 255 * c0[2]);
				pe[i].peFlags = PC_NOCOLLAPSE;
			}

			R2Dcolors[r].indexes[0] = rampBase;
			R2Dcolors[r].indexes[1] = rampBase + (diffSize-1);
			R2Dcolors[r].indexes[2] = rampBase + (rampSize-1);
		}

		/*
		** Initialize any remaining non-static entries.
		*/
		for (i=0; i<extra; ++i) {
			int index = numRamps*rampSize+10+i;
			PALETTEENTRY *pe = &pPal->palPalEntry[index];

			pe->peRed = (BYTE) 0;
			pe->peGreen = (BYTE) 0;
			pe->peBlue = (BYTE) 0;
			pe->peFlags = PC_NOCOLLAPSE;
		}
	}

	ctx->hPalette = CreatePalette(pPal);
	free(pPal);

	if (ctx->hPalette) {
		SelectPalette(hDC, ctx->hPalette, FALSE);
		RealizePalette(hDC);
	}
}

void
R2DsetupPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	/* size of this pfd */
			1,				/* version num */
			PFD_SUPPORT_OPENGL,		/* support OpenGL */
			0,				/* pixel type */
			0,				/* 8-bit color depth */
			0, 0, 0, 0, 0, 0,		/* color bits (ignored) */
			0,				/* no alpha buffer */
			0,				/* alpha bits (ignored) */
			0,				/* no accumulation buffer */
			0, 0, 0, 0,			/* accum bits (ignored) */
			16,				/* depth buffer */
			0,				/* no stencil buffer */
			0,				/* no auxiliary buffers */
			PFD_MAIN_PLANE,			/* main layer */
			0,				/* reserved */
			0, 0, 0,			/* no layer, visible, damage masks */
	};
	int SelectedPixelFormat;
	BOOL retVal;

	pfd.cColorBits = (BYTE) GetDeviceCaps(hDC, BITSPIXEL);

	/* if (colorIndexMode) { */
	/*	pfd.iPixelType = PFD_TYPE_COLORINDEX; */
	/* } else { */
		pfd.iPixelType = PFD_TYPE_RGBA;
	/* } */

	/* if (doubleBuffered) { */
	/*	pfd.dwFlags |= PFD_DOUBLEBUFFER; */
	/* } */

	/* if (renderToDIB) { */
		pfd.dwFlags |= PFD_DRAW_TO_BITMAP;
	/* } else { */
	/* 	pfd.dwFlags |= PFD_DRAW_TO_WINDOW; */
	/* } */

	SelectedPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (SelectedPixelFormat == 0) {
		(void) MessageBox(WindowFromDC(hDC),
			"Failed to find acceptable pixel format.",
			"OpenGL application error",
			MB_ICONERROR | MB_OK);
		exit(1);
	}

	retVal = SetPixelFormat(hDC, SelectedPixelFormat, &pfd);
	if (retVal != TRUE) {
		(void) MessageBox(WindowFromDC(hDC),
			"Failed to set pixel format.",
			"OpenGL application error",
			MB_ICONERROR | MB_OK);
		exit(1);
	}
}


R2DContext * R2DCreate(int width, int height)
{
	R2DContext * ctx = (R2DContext *) calloc(1, sizeof(R2DContext));
	if (ctx) {
		ctx->winWidth = width;
		ctx->winHeight = height;
		ctx->hDC = CreateCompatibleDC(NULL);
		R2DsetupDIB(ctx);
		R2DsetupPixelFormat(ctx->hDC);
		R2DsetupPalette(ctx);
		ctx->hGLRC = wglCreateContext(ctx->hDC);
	}
	return ctx;
}

void R2DMakeCurrent(R2DContext * ctx)
{
	if (ctx) {
		wglMakeCurrent(ctx->hDC, ctx->hGLRC);
	}
}


void R2DDestroy(R2DContext * ctx)
{
	if (ctx) {
		if (ctx->hGLRC) {
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(ctx->hGLRC);
		}
		DeleteDC(ctx->hDC);
		free(ctx);
	}
}


/* Stand alone stuff */
#ifndef LXWIN32

HDC hDCFrontBuffer;
void (*idleFunc)(void);
R2DContext * global_ctx;

void
drawTorus(void)
{
	int numMajor = 32;
	int numMinor = 24;
	float majorRadius = 0.6F;
	float minorRadius = 0.2F;
	double majorStep = 2.0F*M_PI / numMajor;
	double minorStep = 2.0F*M_PI / numMinor;
	int i, j;

	for (i=0; i<numMajor; ++i) {
		double a0 = i * majorStep;
		double a1 = a0 + majorStep;
		GLfloat x0 = (GLfloat) cos(a0);
		GLfloat y0 = (GLfloat) sin(a0);
		GLfloat x1 = (GLfloat) cos(a1);
		GLfloat y1 = (GLfloat) sin(a1);

		if (i & 1) {
			glColor3fv(R2Dcolors[0].diff);
			glMaterialiv(GL_FRONT, GL_COLOR_INDEXES, R2Dcolors[0].indexes);
		} else {
			glColor3fv(R2Dcolors[1].diff);
			glMaterialiv(GL_FRONT, GL_COLOR_INDEXES, R2Dcolors[1].indexes);
		}

		glBegin(GL_TRIANGLE_STRIP);
		for (j=0; j<=numMinor; ++j) {
			double b = j * minorStep;
			GLfloat c = (GLfloat) cos(b);
			GLfloat r = minorRadius * c + majorRadius;
			GLfloat z = minorRadius * (GLfloat) sin(b);

			glNormal3f(x0*c, y0*c, z/minorRadius);
			glVertex3f(x0*r, y0*r, z);

			glNormal3f(x1*c, y1*c, z/minorRadius);
			glVertex3f(x1*r, y1*r, z);
		}
		glEnd();
	}
}

/*****************************************************************/

void
setProjection(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/*
	** Preserve the aspect ratio of objects in the scene.
	*/
	if (global_ctx->winWidth > global_ctx->winHeight) {
		GLfloat aspect = (GLfloat) global_ctx->winWidth / (GLfloat) global_ctx->winHeight;
		glFrustum(-0.5F*aspect, 0.5F*aspect, -0.5F, 0.5F, 1.0F, 3.0F);
	} else {
		GLfloat aspect = (GLfloat) global_ctx->winHeight / (GLfloat) global_ctx->winWidth;
		glFrustum(-0.5F, 0.5F, -0.5F*aspect, 0.5F*aspect, 1.0F, 3.0F);
	}
	glMatrixMode(GL_MODELVIEW);
}

void
init(void)
{
	GLfloat matShine = 20.00F;
	GLfloat light0Pos[4] = { 0.70F, 0.70F, 1.25F, 0.00F };

	glClearColor(R2Dcolors[2].diff[0], R2Dcolors[2].diff[1], R2Dcolors[2].diff[2], 1.0F);
	glClearIndex((GLfloat) R2Dcolors[2].indexes[1]);

	setProjection();
	glTranslatef(0.0F, 0.0F, -2.0F);

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glEnable(GL_LIGHT0);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	if (!colorIndexMode) {
		glEnable(GL_COLOR_MATERIAL);
	}
}

void
doRedraw(void)
{
	static GLfloat x, y, z;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(x, 1.0F, 0.0F, 0.0F);
	glRotatef(y, 0.0F, 1.0F, 0.0F);
	glRotatef(z, 0.0F, 0.0F, 1.0F);

	drawTorus();

	glPopMatrix();

	if (renderToDIB) {
		glFinish();
		BitBlt(hDCFrontBuffer, 0, 0, global_ctx->winWidth, global_ctx->winHeight, global_ctx->hDC, 0, 0, SRCCOPY);
		GdiFlush();
	} else {
		SwapBuffers(hDCFrontBuffer);
	}

	x += 5.0F;
	if (x > 360.0F) x -= 360.0F;
	y += 7.0F;
	if (y > 360.0F) y -= 360.0F;
	z += 9.0F;
	if (z > 360.0F) z -= 360.0F;
}

void
redraw(void)
{
	idleFunc = doRedraw;
}

void
resize(void)
{
	setProjection();
	glViewport(0, 0, global_ctx->winWidth, global_ctx->winHeight);
}

LRESULT APIENTRY
WndProc(
				HWND hWnd,
				UINT message,
				WPARAM wParam,
				LPARAM lParam)
{
	switch (message) {
case WM_CREATE:
	return 0;
case WM_DESTROY:
	PostQuitMessage(0);
	return 0;
case WM_SIZE:
	if (global_ctx->hGLRC) {
		//global_ctx->winWidth = (int) LOWORD(lParam);
		//global_ctx->winHeight = (int) HIWORD(lParam);
		resize();
		return 0;
	}
	break;
case WM_PALETTECHANGED:
	/*
	** Update palette mapping if this *is not* the active window.
	*/
	if (global_ctx->hGLRC && global_ctx->hPalette && (HWND) wParam != hWnd) {
		UnrealizeObject(global_ctx->hPalette);
		SelectPalette(global_ctx->hDC, global_ctx->hPalette, FALSE);
		RealizePalette(global_ctx->hDC);
		redraw();
		return 0;
	}
	break;
case WM_QUERYNEWPALETTE:
	/*
	** Update palette mapping if this *is* the active window.
	*/
	if (global_ctx->hGLRC && global_ctx->hPalette) {
		UnrealizeObject(global_ctx->hPalette);
		SelectPalette(global_ctx->hDC, global_ctx->hPalette, FALSE);
		RealizePalette(global_ctx->hDC);
		redraw();
		return TRUE;
	}
	break;
case WM_PAINT:
	/*
	** Update the window.  Don't use the device context returned by
	** BeginPaint as it won't have the right palette selected into it.
	*/
	if (global_ctx->hGLRC) {
		PAINTSTRUCT ps;

		BeginPaint(hWnd, &ps);
		redraw();
		EndPaint(hWnd, &ps);
		return 0;
	}
	break;
case WM_CHAR:
	switch ((int)wParam) {
case VK_ESCAPE:
	DestroyWindow(hWnd);
	return 0;
case VK_SPACE:
	if (idleFunc) {
		idleFunc = NULL;
	} else {
		idleFunc = doRedraw;
	}
	break;
default:
	break;
	}
	break;
default:
	break;
	}

	/* Deal with any unprocessed messages */
	return DefWindowProc(hWnd, message, wParam, lParam);
}


int APIENTRY
WinMain(
				HINSTANCE hCurrentInst,
				HINSTANCE hPreviousInst,
				LPSTR lpszCmdLine,
				int nCmdShow)
{
	WNDCLASS wndClass;
	HWND hWnd;
	MSG msg;

	char *className = "OpenGL";
	char *windowName = "Simple DIB Example";
	int winX = 0, winY = 0;

	R2DContext * ctx = R2DCreate(512, 512);
	global_ctx = ctx;


	/* Define and register a window class */
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hCurrentInst;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className;
	RegisterClass(&wndClass);

	/* Create a window of the previously defined class */
	hWnd = CreateWindow(
		className,		/* Window class's name */
		windowName,		/* Title bar text */
		WS_OVERLAPPEDWINDOW |	/* The window's style */
		WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS,
		winX, winY,		/* Position */
		global_ctx->winWidth, global_ctx->winHeight,	/* Size */
		NULL,			/* Parent window's handle */
		NULL,			/* Menu handle */
		hCurrentInst,		/* Instance handle */
		NULL);			/* No additional data */

	/* Map the window to the screen */
	ShowWindow(hWnd, nCmdShow);

	/* Force the window to repaint itself */
	UpdateWindow(hWnd);

	/*
	** Set up for OpenGL rendering.  Bind the rendering context to
	** the same device context that the palette will be selected into.
	*/
	hDCFrontBuffer = GetDC(hWnd);
	R2DMakeCurrent(ctx);
	init();
	idleFunc = doRedraw;

	/* Process Messages */
	while (1) {
		/* execute the idle function while there are no messages to process */
		while (idleFunc &&
			PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == FALSE)
		{
			(*idleFunc)();
		}
		if (GetMessage(&msg, NULL, 0, 0) != TRUE) {
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	/*
	** Finish OpenGL rendering.
	*/
	idleFunc = NULL;
	R2DDestroy(ctx);
	ReleaseDC(hWnd, hDCFrontBuffer);
	return (int) msg.wParam;
}


#endif
/* Stand alone stuff */


