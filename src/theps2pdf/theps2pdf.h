/* therion source code
 *
 * thdeps2pdf.h -- therion project file of type general header file
 *
 * Constants and macros for theps2pdf
 *
 * $Date: 2000/07/29 07:40:36 $
 * $RCSfile: thdefs.h,v $
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

#define MAX_LINE 100        /* length of input line */
#define MAX_PDF_LINE 200    /* length of converted (output) line */
#define MAX_TOKEN 40        /* length of token */
#define MAX_STACK 20        /* no. of tokens in stack */


#define add_coord(c1,c2)  \
        sprintf(tmp_line, "%.1f %.1f ", atof(c1)-dx, atof(c2)-dy); \
        strcat(pdf_line, tmp_line)
#define add_num(n) sprintf(tmp_line, "%.1f ", atof(n)); \
        strcat(pdf_line, tmp_line)
#define add_str(s) strcat(pdf_line, s)

#define print_line  if (strcmp(prev_pdf_line,"")) \
        fprintf(file_out, "\\PL{%s}\n", prev_pdf_line)
