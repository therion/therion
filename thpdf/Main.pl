#!/usr/bin/perl -w
##
## thimp -- therion project file of type executable
##
## Makes a map in Pdf format
##
## $Date: 2002/10/29 21:55:22 $
## $RCSfile: Main.pl,v $
## $Revision: 1.4 $
##
## Copyright (C) 2000-2002 Martin Budaj
##
## -------------------------------------------------------------------- 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
## -------------------------------------------------------------------- 

# @FILE thpdf /bin
#
# @NAME
#      thpdf - makes a map/atlas in Pdf format
#
# @SYNOPSIS
#      <B>thpdf</B> <I>[-options]</I> <I>input-file</I>
#
# @DESCRIPTION
#    This program converts output files from Metapost (in EPS format)
#    to PDF XObjects and writes PdfTeX objects for an atlas of the cave. 
#    Four files are written:
#    <I>th_fontdef.tex</I>, <I>th_formdef.tex</I>, <I>th_pagedef.tex</I>
#    and <I>th_pages.tex</I>. To make an atlas, edit TeX file (e.g. 
#    <I>main.tex</I>, see EXAMPLES) in which you define appropriate
#    page dimensions 
#    and include four files mentioned (in order).
#    Then process this file with PdfTeX (PdfeTeX if you have a lot of
#    scraps). 
#    
# @OPTIONS
#    There are two command line options; for others see description of 
#    the file <I>Config</I>
#    <DL><DT>-q</DT>
#        <DD> quick mode, <B>thpdf</B> uses already converted
#            data (files <I>th_fontdef.tex</I> and <I>th_formdef.tex</I>). 
#            The file th_hashdef from the previous run must be present, too.
#            Cannot be used when Therion produces new Metapost files.
#        </DD>
#        <DT>-m</DT>
#        <DD> draws one page map instead of atlas
#        </DD>
#    </DL>
#
# @EXAMPLES
#    %example main.tex file:
#    <BR>
#        \pdfpagewidth=21cm<BR>
#        \pdfpageheight=20cm<BR>
#        \hsize=20cm<BR>
#        \vsize=19.5cm<BR>
#        \hoffset=-2.04cm<BR>
#        \voffset=-2.04cm<BR>
#    <BR>
#        \pdfinfo {%<BR>
#            /Title (Closani Cave)<BR>
#            /Author (Fred Flinstone)<BR>
#        }<BR>
#    <BR>
#        \input th_fontdef<BR>
#        \input th_formdef<BR>
#        \input th_pagedef<BR>
#        \input th_pages<BR>
#    <BR>
#        \end<BR>
#
# @FILES
# 
# <I>Config</I> -- specifies additional options for <I>thpdf</I>.
# <BR>
# Options for <I>dimensions</I> (dimension can be in points (pt), postscript 
# points (bp), milimeters (mm), centimeters (cm), picas (pc), inches (in),
# Didot's points (dd) and ciceros (cc)): 
# <DL>
#    <DT>Hsize:, Vsize:</DT><DD>horizontal and vertical size of the map sheet</DD> 
#    <DT>Hoffset:, Voffset:</DT><DD>amount for shifting paper</DD> 
#    <DT>Overlap:</DT><DD>width of an overlap to adjoining sheets</DD> 
#    <DT>Hgrid:, Vgrid:</DT><DD>distances between grid lines</DD> 
#    <DT>HgridOffset:, VgridOffset:</DT><DD>shifts the whole grid</DD> 
# </DL>
# <I>Boolean</I> options (true is 1, false 0 or empty string):
# <DL>
#    <DT>Background:</DT><DD>if true, background of each page is filled gray</DD> 
#    <DT>ExclPages:</DT><DD>if true, pages defined in <I>ExclList</I>
#        are excluded from atlas</DD> 
#    <DT>TitlePages:</DT><DD>if true, title pages are printed before
#        new layer in atlas</DD> 
#    <DT>PageNumbering:</DT><DD>if true, page number of the page
#        and page numbers of adjoining pages are printed</DD> 
# </DL>
# <I>Integer</I> options:
# <DL>
#    <DT>NavRight:</DT><DD>number of pages to right and left
#        shown in the Navigator window</DD>
#    <DT>NavUp:</DT><DD>the same for up and down directions</DD>
#    <DT>NavFactor:</DT><DD>scale factor main window : one navigator
#        window</DD>
#    <DT>OwnPages:</DT><DD>number of your own pages added to main tex file
#        before the first page of automatically generated maps 
#        (necessary for correct page numbering)</DD>
# </DL>
# <I>Integer or string</I> options:
# <DL>
#    <DT>StartX:, StartY</DT><DD>name of the most upper-left sheet</DD>
# </DL>
# <I>List</I> options:
# <DL>
#    <DT>ExclList:</DT><DD>pages which are omited; string may contain
#        numbers separated by comma or dash (for intervals) -- e.g.
#        2,4-7,9,23 means, that pages 2, 4, 5, 6, 7, 9 and 23 are omited.
#        Please don't change <I>TitlePages</I> and <I>OwnPages</I>
#        options without redefining <I>ExclList</I>. Hint: to find
#        correct numbers for pages that should be ommited,
#        set <I>ExclPages</I> false and specify correct <I>OwnPages</I>
#        and process your file with thpdf and pdftex. Correct page
#        numbers can be read in Acrobat Reader status line.
#       </DD>
# </DL>
#
# @AUTHOR
#      Martin Budaj

use Getopt::Std;
getopts('m');            # -q quick mode
                          # -m one page map

$basename = shift;
if (!$basename) {error("Specify filename to process!")};

$|=1;                    # output buffer

read_scraps($basename);        


read_scraps("th_hashdef");

build();

sub read_scraps {
    my $filename = shift;
    do $filename or error ("Can't read scraps file");
}
