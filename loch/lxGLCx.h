// Standard libraries
#ifndef LXDEPCHECK
#endif  
//LXDEPCHECK - standart libraries

#ifdef LXCONTEXT
#undef LXCONTEXT
#endif

#ifdef LXOFFSCREENHEAD
#define LXCONTEXT(x) x##O
#else
#define LXCONTEXT(x) x##S
#endif

void LXCONTEXT(RenderAll) ();

void LXCONTEXT(SetFontColors) ();

void LXCONTEXT(RenderSurface) ();

void LXCONTEXT(GeomOutline) ();

void LXCONTEXT(RenderCenterline) ();

void LXCONTEXT(RenderScrapWalls) ();

void LXCONTEXT(SetCamera) ();

void LXCONTEXT(OpenGLInit) ();

void LXCONTEXT(RenderInds) ();

void LXCONTEXT(RenderICompass) (double size);

void LXCONTEXT(RenderIClino) (double size);

void LXCONTEXT(RenderIScalebar) (double size);
 
void LXCONTEXT(RenderIDepthbar) (double size);

void LXCONTEXT(RenderILine) (double fx, double fy, double tx, double ty, bool rounded = 
#ifdef LXOFFSCREENHEAD
                             true
#else
                             false
#endif
);


