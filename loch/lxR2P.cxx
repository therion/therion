/*Standard libraries*/
#ifndef LXDEPCHECK
#ifdef LXMACOSX
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif  
/*LXDEPCHECK - standard libraries*/

#include "lxR2P.h"

struct R2PCTX {
  GLXContext ctx = {};
  Pixmap pm = {};
  XVisualInfo *visinfo = {};
  GLXPixmap glxpm = {};
  Display *dpy = {};
};
 
void R2Pmake_context(R2PContext * r2pc)
{
   int sbAttrib[] = { GLX_RGBA,
                      GLX_RED_SIZE, 1,
                      GLX_GREEN_SIZE, 1,
                      GLX_BLUE_SIZE, 1,
                      None };
   int dbAttrib[] = { GLX_RGBA,
                      GLX_RED_SIZE, 1,
                      GLX_GREEN_SIZE, 1,
                      GLX_BLUE_SIZE, 1,
                      GLX_DOUBLEBUFFER,
                      None };
   int scrnum;
   scrnum = DefaultScreen( r2pc->dpy );

   r2pc->visinfo = glXChooseVisual( r2pc->dpy, scrnum, sbAttrib );
   if (!r2pc->visinfo) {
      r2pc->visinfo = glXChooseVisual( r2pc->dpy, scrnum, dbAttrib );
      if (!r2pc->visinfo) {
         printf("Error: couldn't get an RGB visual\n");
         exit(1);
      }
   }
   
   /* need indirect context */
   r2pc->ctx = glXCreateContext( r2pc->dpy, r2pc->visinfo, NULL, False );
   if (!r2pc->ctx) {
      printf("Error: glXCreateContext failed\n");
      exit(-1);
   }
   
}


void R2Pmake_pixmap(R2PContext * r2pc, unsigned int width, unsigned int height)
{
   r2pc->pm = XCreatePixmap( r2pc->dpy, RootWindow( r2pc->dpy, DefaultScreen( r2pc->dpy )), width, height, r2pc->visinfo->depth );
   if (!r2pc->pm) {
      printf("Error: XCreatePixmap failed\n");
      exit(-1);
   }
   r2pc->glxpm = glXCreateGLXPixmap( r2pc->dpy, r2pc->visinfo, r2pc->pm );
   if (!r2pc->glxpm) {
      printf("Error: GLXCreateGLXPixmap failed\n");
      exit(-1);
   }
}

R2PContext *R2PCreate(int width, int height)
{
  R2PContext * c = new R2PContext;
  if (c) {
    c->dpy = XOpenDisplay(NULL);
    R2Pmake_context(c);
    R2Pmake_pixmap(c, (unsigned int) width, (unsigned int) height);
  }
	return c;
}


void R2PMakeCurrent(R2PContext * c)
{
  if (c) {
    glXMakeCurrent( c->dpy, c->glxpm, c->ctx );
  }
}


void R2PDestroy(R2PContext * c)
{
  if (c) {
    glXDestroyGLXPixmap(c->dpy, c->glxpm);
    XFreePixmap(c->dpy, c->pm);
    glXDestroyContext(c->dpy, c->ctx);
    XCloseDisplay(c->dpy);
    delete c;
  }
}






#ifndef LXLINUX

static GC gc;

R2PContext * global_ctx;

static Window make_rgb_window( Display *dpy,
				  unsigned int width, unsigned int height )
{
   XSetWindowAttributes attr;
   unsigned long mask;
   Window win;

   int scrnum;
   Window root;
   scrnum = DefaultScreen( dpy );
   root = RootWindow( dpy, scrnum );

   /* window attributes */
   attr.background_pixel = 0;
   attr.border_pixel = 0;
   /* TODO: share root colormap if possible */
   attr.colormap = XCreateColormap( dpy, root, global_ctx->visinfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask;
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

   win = XCreateWindow( dpy, root, 0, 0, width, height,
		        0, global_ctx->visinfo->depth, InputOutput,
		        global_ctx->visinfo->visual, mask, &attr );

   /* make an X GC so we can do XCopyArea later */
   gc = XCreateGC( global_ctx->dpy, win, 0, NULL );

   return win;
}




static void event_loop( Display *dpy, GLXPixmap pm )
{
   XEvent event;

   while (1) {
      XNextEvent( dpy, &event );

      switch (event.type) {
	 case Expose:
	    printf("Redraw\n");
	    /* copy the image from GLXPixmap to window */
	    XCopyArea( dpy, pm, event.xany.window,  /* src, dest */
		       gc, 0, 0, 300, 300,          /* gc, src pos, size */
		       0, 0 );                      /* dest pos */
	    break;
	 case ConfigureNotify:
	    /* nothing */
	    break;
      }
   }
}



int main( int argc, char *argv[] )
{

   Window win;

   global_ctx = R2PCreate(300, 300);
   win = make_rgb_window( global_ctx->dpy, 300, 300 );
   
   R2PMakeCurrent(global_ctx);

   printf("GL_RENDERER = %s\n", (char *) glGetString(GL_RENDERER));

   /* Render an image into the pixmap */
   glShadeModel( GL_FLAT );
   glClearColor( 0.5, 0.5, 0.5, 1.0 );
   glClear( GL_COLOR_BUFFER_BIT );
   glViewport( 0, 0, 300, 300 );
   glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 );
   glColor3f( 0.0, 1.0, 1.0 );
   glRectf( -0.75, -0.75, 0.75, 0.75 );
   glFlush();

   XMapWindow( global_ctx->dpy, win );

   event_loop( global_ctx->dpy, global_ctx->pm );

   R2PDestroy(global_ctx);
   
   return 0;
}

#endif


