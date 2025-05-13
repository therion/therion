#pragma once

/*Standard libraries*/
#ifndef LXDEPCHECK
#include <windows.h>
#endif  
/*LXDEPCHECK - standard libraries*/

typedef struct _R2DCTX R2DContext;

extern R2DContext *R2DCreate(int width, int height);

extern void R2DMakeCurrent(R2DContext * ctx);

extern void R2DDestroy(R2DContext * ctx);
