/* therion source code
 *
 * theps2pdf.c -- therion project file of type executable
 *
 * Converts PostScript commands to PDF ones
 *
 * $Date: 2000/07/29 07:40:36 $
 * $RCSfile: thsvx.cxx,v $
 * $Revision: 1.1 $
 *
 * Copyright (C) 2000 Martin Budaj
 *
 * -------------------------------------------------------------------- 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * --------------------------------------------------------------------
 */
 
/* @FILE theps2pdf /bin
 *
 * @NAME
 *      theps2pdf - converts PostScript to PDF commands
 *
 * @SYNOPSIS
 *      <B>theps2pdf</B> <I>dx dy mode</I>
 *
 * @DESCRIPTION
 *  This program converts PostScript commands produced by MetaPost
 *  and preprocessed by thpdf to PDF. Do not run this program 
 *  manually; it's called by thpdf. 
 *  <BR>Arguments:
 *  <DL>
 *    <DT>dx</DT><DD>shift in x-axis for all coordinates</DD>
 *    <DT>dy</DT><DD>shift in y-axis for all coordinates</DD>
 *    <DT>mode</DT><DD>0 for forms, 1 for patterns</DD>
 *  </DL>
 *  <BR>Input: <I>stdin</I>
 *  <BR>Output: if converting forms, output is added to <I>th_formdef.tex</I>
 *      else <I>pat.tmp</I> is rewritten.
 *
 * @RESTRICTIONS
 *  Program doesn't convert arbitrary Postscript file, only MetaPost
 *  output (optionaly postprocessed by mpattern program for 
 *  pattern addition). 
 * Conversion of two following special cases is not yet supported:
 *  <UL><LI>postscript <I>concat</I> operator when applied to other 
 *        objects than texts
 *      <LI>long postscript strings splitted in more lines
 *  </UL>
 *  For long labels uses Therion <I>btex...etex</I> construction,
 *  so second case never happens; occurence of the first one
 *  depends on definitions of map symbols; if you don't transform
 *  picture variables, it should never happen.
 *
 * @AUTHOR
 *  Martin Budaj
 */

/*
 * This program is inspired by Hans Hagen's Tex macros for MP->PDF conversion
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "theps2pdf.h"



void error(char *s) {
    printf("%s\n", s);
    exit(1);
}

char eps_line[MAX_LINE], *pos=eps_line;
char pdf_line[MAX_PDF_LINE], 
     prev_pdf_line[MAX_PDF_LINE] = "", 
     tmp_line[MAX_LINE],
     prev_font[MAX_LINE],
     fnt_matr[MAX_LINE],
     fsize[MAX_TOKEN], fname[MAX_TOKEN], fid[MAX_LINE];
     
char token[MAX_TOKEN];
int sp=0, EXIT=0, intext=0;
FILE *file_out;


void gtok() {            /* prof. Knuth's function */
  register char *p;
  if (*pos==0||*pos=='\n') {
    if (!fgets(eps_line,MAX_LINE,stdin)) EXIT=1; /* normal exit at end of file */
    pos=eps_line;
  }
  for (;*pos==' ';pos++) ; /* move to next nonspace */
  for (p=token;*pos!=' ' && *pos && *pos!='\n';p++,pos++) *p=*pos;
  *p=0;
  for (;*pos==' ';pos++) ;
}

void process_lines(void) {
    print_line;
    strcpy(prev_pdf_line, pdf_line);
    if (intext) {
        strcpy(pdf_line, "ET ");
        intext = 0;
    } 
    else strcpy(pdf_line, "");
    sp=0;
}

int main(int argc, char *argv[]) {
    float dx, dy;
    int i,                          /* position in stack */
        concat=0;
    char *stack[MAX_STACK], lastmovex[MAX_TOKEN], lastmovey[MAX_TOKEN];
    
    if (argc!=4) error("Specify shift and mode as three params"); 
                            /* 1=dx, 2=dy, 3==0 for form ==1 for pattern */
    dx = atof(argv[1]);
    dy = atof(argv[2]);

/*    if((file_in = fopen("eps.tmp", "rt")) == NULL) 
 *        error("Can't open input file");
 * we read from stdin now
 */
    if (strcmp(argv[3],"0")==0) {
        if((file_out = fopen("th_formdef.tex", "at")) == NULL) 
            error("Can't open output file");
    }
    else {
        if((file_out = fopen("pat.tmp", "wt")) == NULL) 
            error("Can't open output file for patterns");
    };
            
        
    for (i=0; i<MAX_STACK; i++) {
        stack[i] = (char *) malloc(MAX_TOKEN);
    }
    
    while(1) {
        gtok(); 
        if (EXIT) break;
        

        if (strcmp(token, "moveto")==0) {
            add_coord(stack[0],stack[1]);
            add_str("m");
            strcpy(lastmovex,stack[0]);
            strcpy(lastmovey,stack[1]);
            process_lines();
        }
        else if (strcmp(token, "curveto")==0) {
            add_coord(stack[0],stack[1]);
            add_coord(stack[2],stack[3]);
            add_coord(stack[4],stack[5]);
            add_str("c");
            process_lines();
        }
        else if (strcmp(token, "lineto")==0) {
            add_coord(stack[0],stack[1]);
            add_str("l");
            process_lines();
        }
        else if (strcmp(token, "rlineto")==0) {
            add_coord(lastmovex,lastmovey);
            add_str("l");
            process_lines();
        }
        else if (strcmp(token, "newpath")==0) {
/*          process_lines(); */
        }
        else if (strcmp(token, "closepath")==0) {
            add_str("h");
            process_lines();
        }
        else if (strcmp(token, "fill")==0) {
            add_str("f");
            process_lines();
        }
        else if (strcmp(token, "stroke")==0) {
            add_str("S");
            process_lines();
        }
        else if (strcmp(token, "clip")==0) {
            add_str("W n");
            process_lines();
        }
        else if (strcmp(token, "setlinejoin")==0) {
            add_str(stack[0]);
            add_str(" j");
            process_lines();
        }
        else if (strcmp(token, "setlinecap")==0) {
            add_str(stack[0]);
            add_str(" J");
            process_lines();
        }
        else if (strcmp(token, "setmiterlimit")==0) {
            add_str(stack[0]);
            add_str(" M");
            process_lines();
        }
        else if (strcmp(token, "setgray")==0) {
            if (strcmp(stack[0], "%")) {        /*  patterns def */
                add_str(stack[0]);
                add_str(" g ");
                add_str(stack[0]);
                add_str(" G");
            }
            process_lines();
        }
        else if (strcmp(token, "setrgbcolor")==0) {
            add_str(stack[0]);
            add_str(" ");
            add_str(stack[1]);
            add_str(" ");
            add_str(stack[2]);
            add_str(" rg ");
            add_str(stack[0]);
            add_str(" ");
            add_str(stack[1]);
            add_str(" ");
            add_str(stack[2]);
            add_str(" RG");
            process_lines();
        }
        else if (strcmp(token, "setdash")==0) {
            for (i=0; i<sp; i++) {
                add_str(stack[i]);
                add_str(" ");
            }
            add_str("d");
            process_lines();
        }
        else if (strcmp(token, "setlinewidth")==0) {
            if (strcmp(stack[0], "0")!=0) 
                add_str(stack[0]); 
            else {
                add_str(stack[1]);
                gtok();
            }
            add_str(" w");
            process_lines();
        }
        else if (strcmp(token, "setpattern")==0) {
            add_str("/CS1 cs /");
            add_str(stack[0]);
            add_str(" scn");
            process_lines();
        }
        else if (strcmp(token, "gstate")==0) {
            add_str(stack[0]);
            add_str(" gs");
            process_lines();
        }
        else if (strcmp(token, "gsave")==0) {
            add_str("q");
            process_lines();
        }
        else if (strcmp(token, "grestore")==0) {
            add_str("Q");
            process_lines();
        }
        else if (strcmp(token, "translate")==0) {
            add_str("1 0 0 1 ");
            add_num(stack[0]);
            add_str(" ");
            add_num(stack[1]);
            add_str("cm");
            process_lines();
        }
        else if (strcmp(token, "scale")==0) {
            add_num(stack[0]);
            add_str(" 0 0 ");
            add_num(stack[1]);
            add_str(" 0 0 cm");
            process_lines();
        }
        else if (strcmp(token, "fshow")==0) {
            if (strstr(prev_pdf_line, "ET") == NULL) {
                add_str("BT ");
                strcpy(prev_font,"");
            }
            
            strcpy(fsize,stack[sp-1]);
            strcpy(fname,stack[sp-2]);
            strcpy(fid, fname);
            strcat(fid, " ");
            strcat(fid, fsize);
            
            if (strcmp(fid, prev_font)!=0) {
                add_str("/F\\pdffontname\\");
                add_str(fname);
                add_str("\\space ");
                add_str(fsize);
                add_str(" Tf ");
            }
            if (concat) {
                add_str(fnt_matr);
                add_str(" Tm\n");
            }
            else {
                add_str("1 0 0 1 ");
                add_coord(lastmovex,lastmovey);
                add_str("Tm\n");
            }
            for (i=0; i<(sp-2); i++) {
                add_str(stack[i]);
                add_str(" ");
            }
            add_str("Tj");
            intext = 1;
                /* if (concat) gtok(); */ /* grestore removed by Perl */
            concat = 0;
            strcpy(prev_font, fid);
            strcpy(prev_pdf_line, "");
            process_lines();
        }
        else if (strcmp(token, "concat")==0) { 
            strcpy(fnt_matr,stack[0]);
            strcat(fnt_matr," ");
            strcat(fnt_matr,stack[1]);
            strcat(fnt_matr," ");
            strcat(fnt_matr,stack[2]);
            strcat(fnt_matr," ");
            strcat(fnt_matr,stack[3]);
            sprintf(tmp_line, " %.1f %.1f", 
                    atof(stack[4])-dx, atof(stack[5])-dy);
            strcat(fnt_matr, tmp_line);

            concat = 1;
            if (strstr(prev_pdf_line, "ET") != NULL) {  /* line with gsave */
                add_str("ET ");
            }
            strcpy(prev_pdf_line, "");          /* redundant gsave */
            gtok(); gtok(); gtok();             /* redundant 0 0 moveto */
            process_lines();
        }
        else {
            strcpy(stack[sp++], token);
            if (sp==MAX_STACK) error("Invalid eps file? (stack overflow)");
        }
        
    }
    
    print_line;
    if (strcmp(pdf_line,"")!=0) {
        fprintf(file_out, "\\PL{ET}\n");
    }
    if (fclose(file_out) == EOF) error ("Can't close file!");
    return(0);
}

