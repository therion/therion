#pragma once

typedef struct R2PCTX R2PContext;

extern R2PContext *R2PCreate(int width, int height);

extern void R2PMakeCurrent(R2PContext * ctx);

extern void R2PDestroy(R2PContext * ctx);
