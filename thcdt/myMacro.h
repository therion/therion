/* myMacro.h*/
/* THERION
#define	NULL 0
THERION */

#define SWAP(t,x,y)     { t = x; x = y; y = t; }

/*
#define NEW(p,type)     if ((p=(type *) malloc (sizeof(type))) == NIL) {\
                                printf ("Out of Memory!\n");\
                                exit(0);\
                        }

#define FREE(p)         if (p) { free ((char *) p); p = NIL; }
*/

#define ADD( head, p )  if ( head )  { \
                                p->prev = head->prev; \
                                p->next = head; \
                                head->prev = p; \
                                p->prev->next = p; \
                        } \
                        else { \
                                head = p; \
                                head->next = head->prev = p; \
                        }

#define DELETE( head, p )   if ( head )  { \
                                if ( head == head->next ) \
                                        head = NIL;  \
                                else if ( p == head ) \
                                        head = head->next; \
                                p->next->prev = p->prev;  \
                                p->prev->next = p->next;  \
                                delete( p ); \
                        } 

#define	MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))

