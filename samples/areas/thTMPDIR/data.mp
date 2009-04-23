Scale:=2.00;
color HelpSymbolColor;
HelpSymbolColor := (0.8, 0.8, 0.8);
background:=(1.00000,1.00000,1.00000);
verbatimtex \input th_enc.tex etex;
%% therion source code
%%
%% therion.mp
%%
%% This file defines low-level MetaPost macros and variables required
%% for generation of map symbols
%%
%% $Date: 2003/07/01 09:06:44 $
%% $RCSfile: therion.mp,v $
%% $Revision: 1.3 $
%%
%% Copyright (C) 2000-2003 Martin Budaj
%%
%% Some macros are adapted from MPATTERN package of P. Bolek
%%
%% Some macros are used from MetaFun package of H. Hagen
%%
%% -------------------------------------------------------------------- 
%% This program is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation; either version 2 of the License, or
%% any later version.
%%
%% This program is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%% 
%% You should have received a copy of the GNU General Public License
%% along with this program; if not, write to the Free Software
%% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%% --------------------------------------------------------------------

%    This file defines low level macros used for map signatures in Therion.
%    Before loading this file set `Scale' variable to numeric value
%    representing denominator of the scale ratio. Internal variable 
%    `prologues' is set to 1 by this file. This module loads also 
%    symbol libraries.


tracingstats:=1;
prologues:=1;

if known Background: background:=Background fi;
%TrueScale:=Scale;

% @VARIABLE
% symbol_scale --
%
% <I>internal numeric</I>; in map symbol definitions used for scaling
% in <A HREF="#T">T</A> transformation; recommended
% values are 1 or 2.

%newinternal symbol_scale;
%symbol_scale := 1;

% @VARIABLE
% fill_only --
%
% <I>boolean</I>, used in <A HREF="#thdraw">thdraw</A> and
% <A HREF="#thpattfill">thpattfill</A> commands. When set to false, it has 
% no effect, when it is true, it supresses all drawing 
% commands with exception of <I>thpermanentfill</I>, so that only filled 
% areas are drawn

boolean fill_only, horiz_labels, transparency;
fill_only := false;
horiz_labels:=true;
transparency:=false;

string ATTR__id, ATTR__survey, ATTR__scrap;
boolean ATTR__scrap_centerline;
picture ATTR__text;
ATTR__scrap_centerline := false;

color label_fill_color, label_fill_color_tmp;
label_fill_color := (1.0, 1.0, 1.0);
def push_label_fill_color(expr r,g,b) =
  label_fill_color_tmp := label_fill_color;
  label_fill_color := (r,g,b);
enddef;
def pop_label_fill_color =
  label_fill_color := label_fill_color_tmp;
enddef;
def process_filledlabel(expr cent, rot) =
  begingroup
    bboxmargin:=2.0bp;
    q:=((bbox lab) smoothed 2) rotatedaround (cent, rot);
    fill q withcolor label_fill_color;
    draw lab rotatedaround (cent, rot);
    write_circ_bbox(q);  % without corners smoothing it was enough to use
                         % write_bbox(q);
  endgroup;
enddef;




numeric area_border_errors;
area_border_errors = 0;

% @VARIABLE
% last_write --
%
% <I>numeric</I>; charcode of last figure which contained 
% <A HREF="#write_bbox">write_bbox</A> macro; this value is used by
% <A HREF="#close_file">close_file</A> macro. Initially set to -1.

numeric last_write;
last_write = -1;

% @VARIABLE
% strut_string --
%
% <I>string</I> containing combination of the "highest" and "lowest" character
% in used font; it's used by <A HREF="">free_text</A> macro.

%string strut_string;
%strut_string = "(È";

% @VARIABLE
% file_name --
%
% <I>string</I>, name of file, where <I>write_bbox</I> macro writes
% text clipping path

% @VARIABLE
% s ch --
%
% <I>string</I>, temporary string/char variables

string s, ch, file_name, bg_name, clip_name, lang, diff_pos, diff_neg,
     current_scrap, current_src;

% @VARIABLE
% u v w --
%
% <I>internal numeric variables</I> used as basic length units for drawing;
% they are set by <A HREF="#initialize">initialize</A> macro.
% <UL> <LI><I>u</I> -- normal unit decreasing with increasing scale
%      <LI><I>v</I> -- like <I>u</I>, but can increase drammaticaly
%            when some limit is encountered (to get effects like logarithmic
%            scale)
%      <LI><I>w</I> -- nearly constant at all scales
% </UL>

% @VARIABLE
% legend_scale --
%
% <I>numeric</I>, length of the longer side of signatures' legend box

newinternal legend_scale, u,v,w;

% @VARIABLE
% lab Lab --
%
% <I>picture</I>, in which are saved typeset labels

picture lab, Lab;

% @VARIABLE
% T --
%
% <I>transformation</I>, defines transformation function for transforming 
% every argument of <A HREF="#thdraw>thdraw</A> and <A HREF="#thfill>thfill</A>
% macros

transform T;

% @VARIABLE
% p,q --
%
% <I>path</I>, for saving temporary paths

path p,q;

% @MACRO
% initialize --
%
% initializes basic length units <A HREF="#u">(u,v,w)</A> and pens used 
% in map symbols according to scale. Five circular pens are defined:
% <UL><LI>PenA -- thick; for outlines
%    <LI>PenB, PenC -- thinner; for pits, symbols etc.
%    <LI>PenD -- thinnest; for fine details
%    <LI>PenX -- extra thick; not recommended for use
% </UL>

def fonts_setup (expr t,s,m,l,h) =
  write "\def\updown#1#2{\vbox{" &
        "\offinterlineskip" &
        "\setbox100=\hbox{#1}" &
        "\setbox101=\hbox{#2}" &
        "\ifnum\wd100>\wd101\hsize=\wd100\else\hsize=\wd101\fi" &
        "\centerline{\box100}\vskip4pt" &
        "\centerline{\box101}}}" &
        "\def\thlabel{\thnormalsize}" &
        "\def\thremark{\thsmallsize\si}" &
        "\def\thcomment{\thsmallsize}" &
        "\def\thentrance{\thsmallsize}" &
        "\def\thaltitude{\thsmallsize}" &
        "\def\thstationname{\thsmallsize}" &
        "\def\thdate{\thsmallsize}" &
        "\def\thheight{\thsmallsize}" &
        "\def\thheightpos{+\ignorespaces}" &
        "\def\thheightneg{-\ignorespaces}" &
        "\def\thframed{\thsmallsize}" &
        "\def\thwallaltitude{\thtinysize}"
  to "mptexpre.tex";
  write "\def\thtinysize{\size[" & decimal max(optical_zoom*t,0) & "]}" & 
        "\def\thsmallsize{\size[" & decimal max(optical_zoom*s,0) & "]}" & 
        "\def\thnormalsize{\size[" & decimal max(optical_zoom*m,0) & "]}" & 
        "\def\thlargesize{\size[" & decimal max(optical_zoom*l,0) & "]}" & 
        "\def\thhugesize{\size[" & decimal max(optical_zoom*h,0) & "]}" 
  to "mptexpre.tex";
  write EOF to "mptexpre.tex";
enddef;

def initialize (expr sc) =

  if unknown BaseScale: BaseScale = sc; fi;
  optical_zoom := BaseScale/sc;

    if BaseScale <= 1:                   % 1:100
        u:=14bp; v:=14bp; w:=12bp;
        fonts_setup(8,10,12,16,24);
    elseif BaseScale <= 2:               % 1:200
        u:=12bp; v:=12bp; w:=12bp;
        fonts_setup(7,8,10,14,20);
    elseif BaseScale <= 5:               % 1:500
        u:=10bp; v:=10bp; w:=12bp;
        fonts_setup(6,7,8,10,14);
    else:
        u:=7bp; v:=14bp; w:=10bp;
        fonts_setup(5,6,7,8,10);
    fi;
    
    u := optical_zoom * u;
    v := optical_zoom * v;
    w := optical_zoom * w;

    defaultscale := 0.8 * optical_zoom;

    def PenA = pencircle scaled (u/10) enddef;
    def PenB = pencircle scaled (0.7*u/10) enddef;
    def PenC = pencircle scaled (0.5*u/10) enddef;
    def PenD = pencircle scaled (0.35*u/10) enddef;
    def PenX = pencircle scaled (1.2*u/10) enddef;
    
    legend_scale := 3.14*u;
enddef;

% macro is expanded, we have to know all dimensions and pens before reading
% mpattern macros

initialize(Scale);

vardef thTEX primary s =
  write "verbatimtex \input th_enc.tex etex" to "mptextmp.mp";
  write "btex \mainfont "&s&" etex" to "mptextmp.mp";
  write EOF to "mptextmp.mp";
  scantokens "input mptextmp"
enddef;

% @MACRO
% inscale --
%
% zooms objects to scale specified in legend_scale variable (used in legend
% typesetting)

def inscale = 
    xscaled legend_scale yscaled (0.618*legend_scale)
enddef;

% @MACRO
% draw_legend_box --
%
% draws a legend bounding box and resets drawoptions() options 

def draw_legend_box =
    clip currentpicture to unitsquare inscale;
    drawoptions();
    pickup PenB;
    draw unitsquare inscale;
enddef;

def clean_legend_box =
  unfill unitsquare inscale;
enddef;

def legend_point (expr name) =
%  if substring (2,9) of name = "station":
%    scantokens(name)((0.5,0.5) inscale);
%  else:
    scantokens(name)((0.5,0.5) inscale,0,1,(0,0));
%  fi;
  draw_legend_box;
enddef;

def legend_line (expr name) =
  scantokens(name)((((-0.3,.5)..(.3,.7)..(.5,.3)..{dir 80}(1.3,.9)) inscale) 
                randomized 3mm);
  draw_legend_box;
enddef;

% legend_label
% legend_area
% legend_random

% @MACRO
% roundone --
%
% rounds <I>numeric value</I> to one decimal point

vardef roundone(expr n) =
%    round(10*n)/10
  n
enddef;

% @MACRO
% process_label --
%
% draws a label saved in <I>lab</I> picture variable and calls 
% <A HREF="#write_bbox">write_bbox</A> macro.


def process_label (expr cent, rot) =
  begingroup
    interim bboxmargin:=0.8bp;
    q:=((bbox lab) smoothed 2) rotatedaround (cent, rot);
    draw lab rotatedaround (cent, rot);
    write_circ_bbox(q);  % without corners smoothing it was enough to use
                         % write_bbox(q);
  endgroup;
enddef;

% @MACRO
% process_uplabel --
%
% draws a label into semicircular box and writes clipping path to a file

def process_uplabel =
    q:=bbox lab;
    alef:=.8-.02*(xpart lrcorner q - xpart llcorner q);
%    show alef;
    q:=alef[llcorner q,ulcorner q]{up} .. {down}alef[lrcorner q, urcorner q] --
        lrcorner q -- llcorner q -- cycle;
    draw lab;
    draw q;
    q:=reverse q;
    write_circ_bbox(q);
enddef;

% @MACRO
% process_downlabel --
%
% draws a label into down oriented semicircular box and writes
% clipping path to a file

def process_downlabel =
    q:=bbox lab;
    alef:=1-(.8-.02*(xpart lrcorner q - xpart llcorner q));
    q:=alef[llcorner q,ulcorner q]{down} .. 
        {up}alef[lrcorner q, urcorner q] --
        urcorner q -- ulcorner q -- cycle;
    draw lab;
    draw q;
    write_circ_bbox(q);
enddef;

% @MACRO
% process_updownlabel --
%
% draws a label splitted into down and up oriented semicircular boxes and 
% writes clipping path to a file

def process_updownlabel =
    p:=bbox lab;
    leftside:=xpart llcorner p;
    rightside:=xpart lrcorner p;
    draw (xpart llcorner p,.5[ypart llcorner p,ypart ulcorner p]) -- 
         (xpart lrcorner p,.5[ypart llcorner p,ypart ulcorner p]);
    alef:=1.05-.02*(xpart lrcorner p - xpart llcorner p);
%    alef:=.8-.02*(rightside-leftside);
    p:=alef[(leftside, ypart llcorner p),(leftside, ypart ulcorner p)]{up} .. 
       {down}alef[(rightside, ypart lrcorner p), (rightside, ypart urcorner p)]
        -- alef[(rightside, ypart urcorner p), (rightside, ypart lrcorner p)]{down}
        .. {up}alef[(leftside, ypart ulcorner p),(leftside, ypart llcorner p)]
        -- cycle;
%    p:=alef[llcorner p,ulcorner p]{up} .. {down}alef[lrcorner p, urcorner p] --
%        aleff[llcorner p,ulcorner p]{down} .. 
%        {up}aleff[lrcorner p, urcorner p] -- cycle;
    draw lab;
    draw p;
    p:=reverse p;
    write_circ_bbox(p);
enddef;

def process_updownlabel_OLD =
    p:=bbox lab;
    q:=bbox Lab;
    leftside:=min(xpart llcorner p, xpart ulcorner q);
    rightside:=max(xpart lrcorner p, xpart urcorner q);
    draw .5[(leftside, ypart llcorner p),(leftside, ypart ulcorner q)] -- 
         .5[(rightside,ypart lrcorner p),(rightside,ypart urcorner q)];
    alef:=.8-.02*(rightside-leftside);
    q:=alef[(leftside, ypart llcorner p),(leftside, ypart ulcorner p)]{up} .. 
       {down}alef[(rightside, ypart lrcorner p), (rightside, ypart urcorner p)]
        -- alef[(rightside, ypart urcorner q), (rightside, ypart lrcorner q)]{down}
        .. {up}alef[(leftside, ypart ulcorner q),(leftside, ypart llcorner q)]
        -- cycle;
    draw lab;
    draw Lab;
    draw q;
    q:=reverse q;
    write_circ_bbox(q);
enddef;

% @MACRO
% process_boxedlabel --
%
% draws a label into circular box and writes
% clipping path to a file

def process_boxedlabel =
    q:=bbox lab;
    draw lab;
    draw q;
    write_bbox(q);
enddef;

% @MACRO
% process_circledlabel --
%
% draws a label into circular box and writes
% clipping path to a file

def process_circledlabel =
  begingroup;
    interim bboxmargin := 0.4 bp;
    q:=bbox lab;
  endgroup;
    q:=point 0 of q .. point 1 of q .. point 2 of q .. point 3 of q .. cycle;
    draw lab;
    draw q;
    write_circ_bbox(q);
enddef;


% @MACRO
% write_bbox --
%
% Arguments:
%   <I>path</I> variable -- rectangular bounding box of a label;
%   assumptions: path is cyclic, counterclockwise oriented, with four points,
%   composed from linear segments;
%   see general <A HREF="#write_circ_bbox">write_circ_bbox</A> macro
% Results:
%   one file per figure which uses labels with a clipping path in pseudo-pdf 
%   format

def write_bbox (expr q) =
    file_name := jobname & "." & decimal(charcode) & "bbox";
    for i:=4 downto 0:
        s  :=  decimal(roundone(xpart point i of q)) & " " 
             & decimal(roundone(ypart point i of q)) 
             & if i=4: " m " else: " l" fi;
        write s to file_name;
    endfor;
    last_write := charcode;
enddef;

% @MACRO
% write_circ_bbox --
%
% Arguments:
%   <I>path</I> variable, only assumption is that <I>path</I> is cyclic
%   and counterclockwise oriented.
% Results:
%   one file per figure which uses labels with a clipping path in pseudo-pdf 
%   format

def write_circ_bbox expr q =
    file_name := jobname & "." & decimal(charcode) & "bbox";
    tmp:=length q;
    s :=  decimal(roundone(xpart point tmp of q)) & " " 
             & decimal(roundone(ypart point tmp of q)) & " m";
    write s to file_name;
    for i:=tmp downto 1:
        s :=  decimal(roundone(xpart precontrol i of q)) & " " 
             & decimal(roundone(ypart precontrol i of q)) & " "
             & decimal(roundone(xpart postcontrol i-1 of q)) & " " 
             & decimal(roundone(ypart postcontrol i-1 of q)) & " "
             & decimal(roundone(xpart point i-1 of q)) & " " 
             & decimal(roundone(ypart point i-1 of q)) & " c";
        write s to file_name;
    endfor;
    last_write := charcode;
enddef;

% @MACRO
% close_file --
%
% closes file with a clipping path; it's invoked by <I>endchar</I> macro

def close_file =
    if last_write=charcode:
        write EOF to jobname & "." & decimal(charcode) & "bbox";
    fi;
enddef;

% endchar should run close_file macro

extra_endfig := "close_file;";

% @MACRO
% thdraw --
%
% like plain MetaPost's <I>draw</I>, but draws a <I>path</I> transformed
% (rotated, scaled, shifted) to scrap's coordinates according T variable.
% (If <I>fill_only=false</I>)

def thdrawoptions(text t) =
  def _thop_ = t enddef
enddef;
thdrawoptions();

def thdraw expr p = 
  if not fill_only:
    addto currentpicture doublepath (p transformed T) withpen currentpen _thop_ _op_
  else:
    addto currentpicture also nullpicture
  fi
enddef;

% @MACRO
% thfill --
%
% fills a <I>path</I> transformed
% (rotated, scaled, shifted) to scrap's coordinates according T variable.
% Filled areas are clipped (like most 
% other lines and points) with a clipping path around text labels.


def thfill expr c = 
    addto currentpicture contour (c transformed T) _thop_ _op_ 
enddef;

def thfilldraw expr c =
  if not fill_only:
    addto currentpicture contour (c transformed T) withpen currentpen _thop_ _op_ 
  else:
    addto currentpicture contour (c transformed T) _thop_ _op_ 
  fi
enddef;

def thunfill expr c = 
    thfill c withcolor background 
enddef;

def thundraw expr p = 
    thdraw p withcolor background 
enddef;

def thunfilldraw expr c = 
    thfilldraw c withcolor background 
enddef;

def thclean expr c =
  if transparency:
    thfill c withtransparentcolor tr_bg
  else:
    thunfill c
  fi;
enddef;

def thPatternFill (expr Path, Pattern) =
  T:=identity;
  thclean Path;
  thfill Path withpattern Pattern;
enddef;

%def thLegendPatternFill (expr Path, Pattern) =
%  T:=identity;
%  thfill Path withpattern Pattern;
%enddef;

% for drawarrow:

def _finarr text t =
  thdraw _apth t;
  thfilldraw arrowhead _apth  t
enddef;

% @MACRO
% thpermanentfill --
%
% Fills specified area with a solid color; this area is not affected by 
% text clipping path

%def thpermanentfill expr c = 
%    addto currentpicture contour (c transformed T) _thop_ _op_
%enddef;

let thpermanentfill = thfill;


primarydef p aligned al=
  p shifted (xpart al * xpart U, ypart al * ypart U)
enddef;

% macros for drawing scraps in upper and lower levels; filled lower scraps 
% require special treatment (MetaPost doesn't support non-continuous paths).
% We can't use one file for both, while PDF XObject has to be explicitely 
% filled or stroked. Second macro writes also pseudo-pdf code of a clipping 
% path for given scrap

boolean drawnext;
drawnext:=true;

def draw_upscrap (expr isout)(text t) =
  path q;
  for i=t:
    if (numeric i):
      if (i=1): drawnext:=true; else: drawnext:=false; fi;
    else:
      if drawnext: draw i withpen PenD; fi;
      if not known q: q:=i; else: q:=q -- i; fi;
    fi;
  endfor;
  if not cycle q: q:=q -- cycle; fi;
  if turningnumber q = 0: 
      thwarning("scrap outline intersects itself"); 
  fi;
  if isout=1:
    if turningnumber q > 0: q := reverse q; fi;
  else:
    if turningnumber q < 0: q := reverse q; fi;
  fi;
  addto bgfill contour q;
enddef;

% following macro writes noncontinuous PostScript path directly to EPS file
% (filled background and clipping path)

picture bgfill;
bgfill:=nullpicture;

def draw_downscrap =
  bg_name := jobname & "." & decimal(charcode) & "bg";
  clip_name := jobname & "." & decimal(charcode) & "clip";
  write "%!PS" to bg_name;
  write "%%BoundingBox: " & 
     decimal round xpart llcorner bgfill & " " &
     decimal round ypart llcorner bgfill & " " &
     decimal round xpart urcorner bgfill & " " &
     decimal round ypart urcorner bgfill 
     to bg_name;
  write "%%Page: 1 1" to bg_name;
  write "newpath" to bg_name;
  for qq within bgfill:
    q := pathpart qq;
    tmp:=length q;
    s := decimal(roundone(xpart point tmp of q)) & " " 
             & decimal(roundone(ypart point tmp of q)) & " m";
    write s & "oveto" to bg_name;
    write s to clip_name;
    for i:=tmp downto 1:
        s :=  decimal(roundone(xpart precontrol i of q)) & " " 
             & decimal(roundone(ypart precontrol i of q)) & " "
             & decimal(roundone(xpart postcontrol i-1 of q)) & " " 
             & decimal(roundone(ypart postcontrol i-1 of q)) & " "
             & decimal(roundone(xpart point i-1 of q)) & " " 
             & decimal(roundone(ypart point i-1 of q)) & " c";
        write s & "urveto" to bg_name;
        write s to clip_name;
    endfor;
  endfor;
  write "closepath fill" to bg_name;
  write "showpage" to bg_name;
  write "%%EOF" to bg_name;
  write EOF to bg_name;
  write EOF to clip_name;
  bgfill:=nullpicture;
enddef;



vardef buildcycle(text ll) =
  save ta_, tb_, k_, i_, pp_; path pp_[];
  k_=0;
  for q=ll: pp_[incr k_]=q; endfor
  i_=k_;
  for i=1 upto k_:
    (ta_[i], length pp_[i_]-tb_[i_]) =
      pp_[i] intersectiontimes reverse pp_[i_];
    if ta_[i]<0:
      message("[Error: area borders "& area_border[i] &" and "& 
         area_border[i_] &" don't intersect in scrap " & current_scrap &
         " (file " & current_src & ")]");
      area_border_errors := area_border_errors + 1;
    fi
    i_ := i;
  endfor
  for i=1 upto k_: subpath (ta_[i],tb_[i]) of pp_[i] .. endfor
    cycle
enddef;

vardef unitvector primary z = 
  if (z<>(0,0)):
    z/abs z 
  else:
    hide(thwarning("strange path"))
    (0,epsilon)
  fi
enddef;

def thwarning (expr m) = 
  message("[Warning: " & m & " in scrap " & current_scrap & "]");
enddef;

def check_area_borders = 
  if area_border_errors > 0:
  fi;
enddef;

%%%%%%%%%%%% Map symbols management %%%%%%%%%%%%%%%%%%%

def mapsymbol (expr name, set, warning) = 
  string s, stype, lname, ID;
  stype = substring (0,1) of name;
  lname = name & "_" & set;
  ID := "ID_" & lname;
  if (known scantokens ID):
    if stype = "a":
      s = "def " & name & " = scantokens(" & ditto & lname & ditto & ") enddef;";
    else:
      s = "let " & name & " = " & lname & ";";
    fi;
    scantokens s;
  elseif warning:  
    message("[Warning: undefined symbol `" & lname & "']");
  fi;
enddef;

%def hidesymbol (expr name) =
%  string s, stype;
%  stype = substring (0,1) of name;
%  if stype = "a":
%    s = "def " & name & " = scantokens(" & ditto & "a_empty" & ditto & ") enddef;";
%  else:
%    s = "vardef " & name & "@# (text t) = enddef;";
%  fi;
%  scantokens s;
%  write name to "missed.dat";
%enddef;

def initsymbol (expr name) =
  s := "ID_" & name & " = 1";
  scantokens s;
enddef;

%%%

% comment out groups in order to use 'save ATTR_*' appropriately

def beginfig(expr c) =
%  begingroup
  charcode:=c;
  clearxy; clearit; clearpen;
  pickup defaultpen;
  drawoptions();
  scantokens extra_beginfig;
  
  save smartll, smartur; 
  pair smartll[], smartur[]; 
  smart_count := 0;
enddef;

def endfig =
  scantokens extra_endfig;
  shipit;
%  endgroup
enddef;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% P. Bolek's MPATTERN package adapted and simplified for Therion
% (with the same user interface except of patterncolor)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

string Pname_, PXYsteps_;
string Pmatrix_;

numeric Pnum_; Pnum_=0;

def PmakeBB_=
   PBBox_ := "[" & decimal Plft_ & " " & decimal Plow_ & " "
   & decimal Prt_ & " " & decimal Pup_ & "]";
enddef;

def Psteps_=
   PXYsteps_ := decimal
     if unknown PXStep_: (Prt_-Plft_) else: PXStep_ fi 
   & ":" & decimal
     if unknown PYStep_: (Pup_-Plow_) else: PYStep_ fi;
enddef;
   
vardef Pfindbounds_=
   Plow_ = ypart (llcorner currentpicture);
   Plft_ = xpart (llcorner currentpicture);
   Pup_ = ypart (urcorner currentpicture);
   Prt_ = xpart (urcorner currentpicture);
   PmakeBB_;
enddef;

def PmakePattern_(expr name)=
  write decimal (Pnum_*epsilon) & ":"
    & jobname & "." & decimal charcode & ":"
    & PBBox_ & ":"
    & PXYsteps_ & ":"
    & Pmatrix_ 
  to "patterns.dat"
enddef;

% User interface macros

vardef patternbbox(expr a)(text b)=
   save Pi_, Pz_;
   numeric Pi_, Pz_[];
   if pair a:
      Plft_:=min(xpart(a),xpart(b)); Plow_:=min(ypart(a),ypart(b));
      Prt_:=max(xpart(a),xpart(b));  Pup_:=max(ypart(a),ypart(b));
   else:
      Pi_=1;
      for t=b:
	 Pz_[Pi_]=t;
	 Pi_:=Pi_+1;
      endfor;
      Plft_:=min(a,Pz_2);   Plow_:=min(Pz_1,Pz_3);
      Prt_:=max(a,Pz_2); Pup_:=max(Pz_1,Pz_3);
   fi;
   PmakeBB_;
enddef;

def beginpattern(suffix name)=
  numeric PXStep_, PYStep_;
  numeric Plow_, Plft_, Pup_, Prt_;
  string PBBox_;
  Pmatrix_:="[1 0 0 1 0 0]";
  Pname_:=str name;
  Pnum_:=Pnum_+1;
  beginfig(Pnum_+4000);
enddef;

def endpattern=
  if unknown PBBox_:
     Pfindbounds_;
  fi;
  endfig;
  Psteps_;
  PmakePattern_(Pname_);
  scantokens(Pname_ & "=Pnum_;");
enddef;

primarydef p withpattern s=
  if known s:
    p withcolor (epsilon, 10*epsilon, s*epsilon);
  else:
    p withcolor (1,0,0);
    message("Warning: undefined pattern ignored");
  fi;  
enddef;
   
def patterntransform expr t=
   Pmatrix_ := "[" & decimal xxpart t
              & " " & decimal yxpart t
              & " " & decimal xypart t
              & " " & decimal yypart t
              & " " & decimal xpart t
              & " " & decimal ypart t & "]";
enddef;

def patternxstep expr t=
   PXStep_ = t;
enddef;

def patternystep expr t=
   PYStep_ = t;
enddef;

def patternstep text t=
   if pair t:
      PXStep_ = xpart t;
      PYStep_ = ypart t;
   else:
      (PXStep_,PYStep_)=t;
   fi;
enddef;

def patterncolor expr t=
  message("Warning: patterncolor not supported in Therion");
enddef;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% end of pattern macros
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% transparent rgb colors

numeric RGBnum; RGBnum=0;
string RGBname;

def def_transparent_rgb (suffix name)(expr r, g, b) =
  RGBnum := RGBnum+1;
  RGBname := str name;
  write decimal (RGBnum*epsilon) & ":"
    & decimal r & " "
    & decimal g & " "
    & decimal b 
  to "rgbcolors.dat";
  scantokens(RGBname & ":=RGBnum;");
enddef;

primarydef p withtransparentcolor s=
   p withcolor (epsilon, 12*epsilon, s*epsilon);
enddef;

def_transparent_rgb(tr_white, 1, 1, 1);
def_transparent_rgb(tr_blue, 0, 0, 1);
def_transparent_rgb(tr_black, 0, 0, 0);
def_transparent_rgb(tr_bg, 
               redpart background, greenpart background, bluepart background);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% some useful macros from H. Hagen's MetaFun package
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

vardef paired (expr d) =
  if pair d : d else : (d,d) fi
enddef ;

primarydef p randomshifted s = 
  begingroup ; save ss ; pair ss ; ss := paired(s) ; 
  p shifted (-.5xpart ss + uniformdeviate xpart ss,
             -.5ypart ss + uniformdeviate ypart ss) 
  endgroup 
enddef ; 

primarydef p randomized s =
  (if path p : 
    for i=0 upto length(p)-1 :
      ((point       i    of p) randomshifted s) .. controls 
      ((postcontrol i    of p) randomshifted s) and 
      ((precontrol (i+1) of p) randomshifted s) .. 
    endfor 
    if cycle p : 
      cycle 
    else :
      ((point length(p) of p) randomshifted s) 
    fi
  elseif pair p :
    p randomshifted s  
  elseif color p :
    if color s : 
      (uniformdeviate redpart   s * redpart   p,
       uniformdeviate greenpart s * greenpart p,
       uniformdeviate bluepart  s * bluepart  p)  
    elseif pair s : 
      ((xpart s + uniformdeviate (ypart s - xpart s)) * p)  
    else :  
      (uniformdeviate s * p)  
    fi 
  else :
    p + uniformdeviate s  
  fi)  
enddef ; 

primarydef p llmoved d =
  ((llcorner p) shifted (-xpart paired(d),-ypart paired(d)))
enddef ;

primarydef p lrmoved d =
  ((lrcorner p) shifted (+xpart paired(d),-ypart paired(d)))
enddef ;

primarydef p urmoved d =
  ((urcorner p) shifted (+xpart paired(d),+ypart paired(d)))
enddef ;

primarydef p ulmoved d =
  ((ulcorner p) shifted (-xpart paired(d),+ypart paired(d)))
enddef ;

primarydef p smoothed d =
  (p llmoved (-xpart paired(d),0) -- p lrmoved (-xpart paired(d),0) {right} .. 
   p lrmoved (0,-ypart paired(d)) -- p urmoved (0,-ypart paired(d)) {up}    .. 
   p urmoved (-xpart paired(d),0) -- p ulmoved (-xpart paired(d),0) {left}  ..
   p ulmoved (0,-ypart paired(d)) -- p llmoved (0,-ypart paired(d)) {down}  .. cycle) 
enddef ;

vardef punked primary p = 
  (point 0 of p for i=1 upto length(p)-1 : -- point i of p endfor 
   if cycle p : -- cycle else : -- point length(p) of p fi)  
enddef ;  

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% end of MetaFun macros
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% we input map symbol definitions




%%%%% INPUT thPoint %%%%%


%% therion source code
%%
%% This file defines macros for point map symbols
%%
%% $Date: 2003/07/01 09:06:44 $
%% $RCSfile: thPoint.mp,v $
%% $Revision: 1.8 $
%%
%% Copyright (C) 2000-2004 Martin Budaj
%%
%% -------------------------------------------------------------------- 
%% This program is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation; either version 2 of the License, or
%% any later version.
%%
%% This program is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%% 
%% You should have received a copy of the GNU General Public License
%% along with this program; if not, write to the Free Software
%% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%% --------------------------------------------------------------------

pair U;

def p_stalactite_UIS(expr pos,theta,sc,al) =
    U:=(.15u, .4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,-.4u) -- (0,.15u) -- (-.15u,.4u);
    thdraw (0,.15u) -- (.15u,.4u);
enddef;

def p_stalagmite_UIS(expr pos,theta,sc,al) =
    U:=(.15u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,.4u) -- (0,-.15u) -- (-.15u,-.4u);
    thdraw (0,-.15u) -- (.15u,-.4u);
enddef;

def p_pillar_UIS(expr pos,theta,sc,al) =
    U:=(.15u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,.15u)--(0,-.15u);
    thdraw (-.15u,-.4u)--(0,-.15u)--(.15u,-.4u);
    thdraw (-.15u,.4u)--(0,.15u)--(.15u,.4u);
enddef;

def p_curtain_UIS(expr pos,theta,sc,al) =
    U:=(.15u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.15u,.4u)--(0,.15u)--(.15u,.4u);
    thdraw (0,.15u)--(0,0){dir 180}..(-.12u,-.11u)..{dir 0}(0,-.22u)--(0,-.4u);
enddef;

def p_helictite_UIS(expr pos,theta,sc,al) =
    U:=(.2u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,.4u)--(0,-.4u);
    p := (-.2u,.4u)---(-.2u,.1u){down}..{dir -10}(-.1u,0.02u)---origin;
    thdraw p;
    thdraw p reflectedabout (origin,(0,u)) reflectedabout (origin,(u,0));
enddef;

def p_sodastraw_UIS(expr pos,theta,sc,al) =
    U:=(.5u,.25u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,.2u)--(.5u,.2u);
    pickup PenD;
    thdraw (-.4u,.2u)--(-.4u,0);
    thdraw (-.12u,.2u)--(-.12u,-.25u);
    thdraw (.02u,.2u)--(.02u,-.1u);
    thdraw (.23u,.2u)--(.23u,-.19u);
    thdraw (.35u,.2u)--(.35u,-.15u);
enddef;

def p_crystal_UIS (expr pos,theta,sc,al)=
    U:=(.35u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := ((-.35*u,0)--(.35*u,0));
    thdraw p;
    thdraw p rotated (60);
    thdraw p rotated (120);
enddef;

def p_flowstone_UIS (expr pos,theta,sc,al)=
    U:=(.45u,.1u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := ((-.15*u,0)--(.15*u,0));
    thdraw p shifted (.3u,-.1u);
    thdraw p shifted (-.3u,-.1u);
    thdraw p shifted (0,.1u);
enddef;

def p_moonmilk_UIS (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,-.2u){up}..{dir -60}(-.18u,-.105u){dir 80}..
        {dir-80}(.18u,-.105u){dir 60}..{down}(.5u,-.2u);
enddef;

def p_wallcalcite_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.2u,-.2u)--(0,.1u)--(.2u,-.2u);
enddef;

def p_popcorn_UIS (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,-.2u)--(.5u,-.2u);
    pickup PenD;
    thdraw (-.3u,-.2u)--(-.3u,.1u);
    thdraw (0,-.2u)--(0,.1u);
    thdraw (.3u,-.2u)--(.3u,.1u);
    thfill fullcircle scaled .2u shifted (-.3u,.1u);
    thfill fullcircle scaled .2u shifted (0,.1u);
    thfill fullcircle scaled .2u shifted (.3u,.1u);
enddef;

def p_disk_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.2u,-.3u)--origin--(.2u,-.3u);
    p:=fullcircle scaled .3u shifted (0,.15u);
    thclean p;
    thdraw p;
enddef;

def p_gypsum_NSS (expr pos,theta,sc,al)=
    U:=(.25u,.25u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.25u,0)--(.25u,0);
    thdraw (0,-.25u)--(0,.25u);
enddef;

def p_aragonite_NSS (expr pos,theta,sc,al)=
    U:=(.3u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,-.4u)--(0,.3u);
    thdraw (0,-.1u){dir 40}..{dir 5}(.3u,.08u);
    thdraw (0,-.1u){dir 140}..{dir 175}(-.3u,.08u);
    thdraw (0,.2u){dir 30}..{dir 5}(.25u,.3u);
    thdraw (0,.2u){dir 150}..{dir 175}(-.25u,.3u);
enddef;


def p_cavepearl_SKBB (expr pos,theta,sc,al)=
    U:=(.25u,.25u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=fullcircle scaled .25u shifted (0,.16u);
    thdraw p;
    thdraw p rotated 120;
    thdraw p rotated 240;
enddef;

def p_gypsumflower_NSS (expr pos,theta,sc,al)=
    U:=(.4u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
%    thdraw (.2u,0){left}..(0,.2u)..(.25u,.35u)..(.5u,0)..
%        (.1u,-.3u)..{dir 125}(-.5u,.2u);
    thdraw (.18u,0){left}..(0,.15u)..(.2u,.3u)..(.4u,0)..
        (.15u,-.28u)..{dir 120}(-.4u,.15u);
enddef;

def p_rimstonepool_ASF (expr pos,theta,sc,al)=
    U:=(.4u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := (-.4u,.2u){dir -70}..{dir 70}(.4u,.2u);
    thfill buildcycle(p,((.5u,.1u)--(-.5u,.1u)));
    thdraw p;
enddef;

def p_rimstonedam_ASF (expr pos,theta,sc,al)=
    U:=(.4u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.4u,.2u){dir -70}..{dir 70}(.4u,.2u);
enddef;

def p_anastomosis_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=(-.4u,-.2u)--(-.3u,-.3u)--(0,.3u)--(.1u,.2u);
    thdraw p;
    thdraw p shifted (.3u,0);
enddef;

def p_karren_UIS (expr pos,theta,sc,al)=
    U:=(.5u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,-.3u){dir -85}..{dir 85}(-.32u,-.3u)..(-.28u,.3u){dir 85}..
        {dir -85}(-.12u,.3u)..(-.08u,-.3u){dir-85}..{dir 85}(.08u,-.3u)..
        {dir85}(.12u,.3u)..(.28u,.3u){dir -85}..(.32u,-.3u){dir -85}..
        {dir 85}(.5u,-.3u);
enddef;

def p_scallop_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,-.4u){dir 125}..{dir 105}(-.2u,.1u){up}..
        {down}(.2u,.1u){dir -105}...{dir -125}(0,-.4u);
enddef;

def p_flute_UIS (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,.2u)--(.5u,.2u);
    thdraw (-.3u,.2u){dir -70}..{dir 70}(.3u,.2u);
enddef;

def p_raft_NSS (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,.2u){dir -80}..(-.3u,-.2u)..origin..(.3u,.2u)..
        {dir -80}(.5u,-.2u);
enddef;

def p_raftcone_NSS (expr pos,theta,sc,al)=
    U:=(.3u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.3u,-.4u)--(0,.4u)--(.3u,-.4u);
enddef;

def p_spring_SKBB (expr pos,theta,sc,al)=
  U:=(.3u,.2u);
  T:=identity aligned al rotated theta scaled sc shifted pos;
  pickup PenC;
  thdraw (-.3u,.2u){down}..(0,-.2u)..{up}(.3u,.2u);
enddef;

def p_sink_SKBB (expr pos,theta,sc,al)=
  U:=(.3u,.2u);
  T:=identity aligned al rotated theta scaled sc shifted pos;
  pickup PenC;
  thdraw (-.3u,-.2u){up}..(0,.2u)..{down}(.3u,-.2u);
enddef;

def p_narrowend_UIS (expr pos,theta,sc,al)=
    U:=(.1u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.1u,-.4u)--(-.1u,.4u);
    thdraw (.1u,-.4u)--(.1u,.4u);
enddef;

def p_lowend_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.1u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.4u,-.1u)--(.4u,-.1u);
    thdraw (-.4u,.1u)--(.4u,.1u);
enddef;

def p_lowend_NSS (expr pos,theta,sc,al)=
    U:=(.4u,0);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.4u,0)--(.4u,0);
enddef;

def p_flowstonechoke_NSS (expr pos,theta,sc,al)=
    U:=(.4u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=(-.4u,-.4u){right}..{dir120}(-.2u,-.2u){dir -20}..{up}(0,0);
    thdraw p;
    thdraw p shifted (0,.2u);
    thdraw p reflectedabout (origin,down);
    thdraw p reflectedabout (origin,down) shifted (0,.2u);
enddef;

def p_breakdownchoke_NSS (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,-.1u)--(-.15u,.05u)--(-.48u,.2u)--cycle;
    thdraw (-.2u,-.2u)--(.15u,-.18u)--(-.08u,.14u)--cycle;
    thdraw (.1u,0)--(.4u,-.1u)--(.5u,.2u)--(.1u,.2u)--cycle;
enddef;

def p_bedrock_ASF (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,0)--(.5u,0);
    thdraw (-.3u,0)--(-.3u,.2u);
    thdraw (.3u,0)--(.3u,-.2u);
enddef;

def p_sand_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenB;
    thdraw (0,.2u);
    thdraw (0,.2u) rotated 120;
    thdraw (0,.2u) rotated 240;
enddef;

def p_clay_SKBB (expr pos,theta,sc,al) =
    U:=(.3u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.3u,0){up}..origin{down}..{up}(0.3u,0);
enddef;

def p_pebbles_UIS (expr pos,theta,sc,al)=
    U:=(.45u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=superellipse((.2u,0),(0,.1u),(-.2u,0),(0,.-.1u),.75);
    thdraw p rotated 20 shifted (0,.25u);
    thdraw p rotated -37 shifted (.25u,-.25u);
    thdraw p rotated -62 shifted (-.25u,-.25u);
enddef;

def p_debris_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.3u,-.4u)--(0,-.22u)--(-.4u,-.1u)--cycle;
    thdraw (.2u,-.4u)--(.4u,-.1u)--(.1u,-.1u)--cycle;
    thdraw (-.1u,0)--(.2u,.3u)--(-.2u,.3u)--cycle;
enddef;

def p_blocks_UIS (expr pos,theta,sc,al)=
    U:=(.5u,.5u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,-.5u)--(.3u,-.4u)--(-.17u,.2u)--cycle;
    thdraw (.25u,-.25u)--(.5u,-.15u)--(.3u,.5u)--(-.1u,.5u)--cycle;
    thdraw (-.27u,0)--(-.1u,.3u)--(-.5u,.35u)--cycle;
    pickup PenD;
    thdraw (-.5u,-.5u)--(-.1u,-.2u)--(-.17u,.2u);
    thdraw (-.1u,-.2u)--(.3u,-.4u);
    thdraw (.25u,-.25u)--(.3u,0)--(.2u,.2u)--(-.1u,.5u);
    thdraw (.3u,.5u)--(.2u,.2u);
    thdraw (.5u,-.15u)--(.3u,0);
enddef;

def p_water_UIS (expr pos,theta,sc,al)=
    U:=(.425u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenD;
    p:=fullcircle xscaled (.85u) yscaled (.6u);
    thfill p withpattern pattern_water_UIS;
    thdraw p;
enddef;

def p_ice_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := (0,.05u)--(0,.35u);
    thdraw p;
    thdraw p shifted (-.25u,-.3u);
    thdraw p shifted (.25u,-.3u);
    p := (-0.15u,.2u)--(0.15u,.2u);
    thdraw p;
    thdraw p shifted (-.25u,-.3u);
    thdraw p shifted (.25u,-.3u);
enddef;

def p_snow_SKBB (expr pos,theta,sc,al)=
    U:=(.35u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := origin--(0,.25*u);
    q := ((0,.35u)--(0,0.25u)--(0.1u,0.25u)) rotatedaround ((0,0.25u),45);
    thdraw p;
    thdraw p rotated (60);
    thdraw p rotated (120);
    thdraw p rotated (180);
    thdraw p rotated (240);
    thdraw p rotated (300);
    thdraw q;
    thdraw q rotated (60);
    thdraw q rotated (120);
    thdraw q rotated (180);
    thdraw q rotated (240);
    thdraw q rotated (300);
enddef;


def p_archeomaterial_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := fullcircle scaled .8u; 
    thdraw (-.5u,-.5u)--origin cutbefore p;
    thdraw (-.5u,.5u)--origin cutbefore p;
    thdraw origin--(.4u,0);
    thdraw p;
enddef;

def p_paleomaterial_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    thfill (-.4u,-.2u)..(-.2u,-.08u)---(-.1u,.05u)---(0,.2u)..(-.03u,.4u)..
        (.2u,.3u)..(.35u,.25u)..(.2u,.2u)---origin---(-.1u,-.17u)..
        (-.08u,-.25u){dir -120}..(-.3u,-.23u)
        ..cycle;
enddef;

def p_guano_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.4u,.2u){dir 40}..{down}(0,-.35u){up}..{dir-40}(.4u,.2u);
enddef;

def p_vegetabledebris_ASF (expr pos,theta,sc,al)=
    U:=(.38u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.3u,-.3u)--(0,.3u);
    thdraw (0,-.3u)--(.3u,.3u);
    thdraw (-.25u,.13u)--(.38u,.13u);
    thdraw (-.38u,-.13u)--(.25u,-.13u);
enddef;

def p_root_ASF (expr pos,theta,sc,al)=
    U:=(.1u,.5u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.1u,-.5u)--(-.1u,.5u);
    p:=(-.1u,-.3u)--(.1u,-.5u);
    thdraw p;
    thdraw p shifted (0,.2u);
    thdraw p shifted (0,.4u);
enddef;

def p_entrance_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.5u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    thfill (-.2u,-.5u)--(0,.5u)--(.2u,-.5u)--cycle;
enddef;

def p_waterflow_paleo_UIS (expr pos,theta,sc,al)=
    U:=(.2u,u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,-u)--(0,u);
    thdraw halfcircle scaled .4u shifted (0,-.4u);
    p:=(-.15u,.6u)--(0,u)--(.15u,.6u)--cycle;
    thfill p;
    thdraw p;
enddef;

def p_gradient_UIS (expr pos,theta,sc,al)=
    U:=(.15u,u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,-u)--(0,u);
    p:=(-.15u,.6u)--(0,u)--(.15u,.6u)--cycle;
    thfill p;
    thdraw p;
enddef;

def p_gradient_BCRA (expr pos,theta,sc,al)=
    U:=(.25u,0.7u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    thfill (-.25u,-0.7u)--(0,.7u)--(.25u,-.7u)--cycle;
enddef;

% by Philip Schuchardt
def p_gradient_NSS (expr pos,theta,sc,al) =
  U:=(.7u, .6u);
  T:=identity aligned al rotated theta scaled sc shifted pos;
  pickup PenC;

  thdraw (-.3u, -.5u) -- (-.7u, -.2u);
  thdraw (-.2u, -.4u) -- (-.4u, .3u);
  thdraw (0u, -.3u) -- (0u, .6u);
  thdraw (.3u, -.5u) -- (.7u, -.2u);
  thdraw (.2u, -.4u) -- (.4u, .3u);
enddef;

def p_waterflow_permanent_UIS (expr pos,theta,sc,al)=
    U:=(.15u,.5u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=(0,.5u){down}..(.12u,.3u)..(-.15u,.15u)..(.13u,0)..
        (-.08u,-.2u)..{down}(0,-.5u);
    p:=p rotated 180;
    thdraw p;
    thdrawoptions();
    oldahlength:=ahlength; ahlength:=2.5pt*optical_zoom;
    thdraw arrowhead p;
    thfill arrowhead p;
    ahlength:=oldahlength;
enddef;

def p_waterflow_intermittent_UIS (expr pos,theta,sc,al)=
  thdrawoptions(dashed evenly scaled (.5 * optical_zoom));
  p_waterflow_permanent_UIS (pos,theta,sc,al);
  thdrawoptions();
enddef;

def p_airdraught_UIS (expr pos,theta,sc,al)=
  U:=(.2u,u);
  T:=identity aligned al rotated theta scaled 1 shifted pos;
  pickup PenC;
  thdraw (0,u)--(0,-0.75u);
	p:=(-.2u,.65u){dir 20}..{dir 90}(0,u);
	thdraw p;
	thdraw p reflectedabout (origin,(0,u));
	for i:=1 upto round(3+2*mlog(sc)/mlog 2):
	  thdraw (0,-(1-0.2i)*u)--(.2u,-(1-0.2(i-1))*u);
	endfor;
enddef;

def p_airdraught_winter_UIS (expr pos,theta,sc,al)=
  U:=(.2u,u);
  T:=identity aligned al rotated theta scaled 1 shifted pos;
  pickup PenC;
  thdraw (0,u)--(0,-0.75u);
  p:=(-.2u,.65u){dir 20}..{dir 90}(0,u);
  thdraw p;
  thdraw p reflectedabout (origin,(0,u));
  for i:=1 upto round(3+2*mlog(sc)/mlog 2):
    thdraw (0,-(1-0.2i)*u)--(.2u,-(1-0.2(i-1))*u);
  endfor;
  thclean fullcircle scaled 0.6u shifted (0,0.05u);
  thdraw (-0.1732u,-0.05u) -- (0.1732u,0.15u);
  thdraw (0.1732u,-0.05u) -- (-0.1732u,0.15u);
  thdraw (0,-0.15u) -- (0,0.25u);
enddef;

def p_airdraught_summer_UIS (expr pos,theta,sc,al)=
  U:=(.2u,u);
  T:=identity aligned al rotated theta scaled 1 shifted pos;
  pickup PenC;
  thdraw (0,u)--(0,-0.75u);
  p:=(-.2u,.65u){dir 20}..{dir 90}(0,u);
  thdraw p;
  thdraw p reflectedabout (origin,(0,u));
  for i:=1 upto round(3+2*mlog(sc)/mlog 2):
    thdraw (0,-(1-0.2i)*u)--(.2u,-(1-0.2(i-1))*u);
  endfor;
  thclean fullcircle scaled 0.6u shifted (0,0.05u);
  thdraw (-0.1732u,-0.05u) -- (0.1732u,0.15u);
  thdraw (0.1732u,-0.05u) -- (-0.1732u,0.15u);
  thdraw (0,-0.15u) -- (0,0.25u);
  thclean fullcircle scaled 0.3u shifted (0,0.05u);
  thdraw fullcircle scaled 0.2u shifted (0,0.05u);
enddef;


def p_station_fixed_ASF (expr pos)=
    T:=identity shifted pos;
    pickup PenD;
    pair z;
    z:=(0,.2u);
    thclean z -- (z rotated 120) -- (z rotated 240) -- cycle;
    thdraw z -- (z rotated 120) -- (z rotated 240) -- cycle;
    thdraw origin;
enddef;

def p_station_painted_SKBB (expr pos)=
    T:=identity shifted pos;
    pickup PenC;
    thclean fullcircle scaled 0.25u;
    thdraw fullcircle scaled 0.25u;
enddef;

def p_station_natural_ASF (expr pos)=
    T:=identity shifted pos;
    pickup PenD;
    thclean fullcircle scaled .3u;
    thdraw fullcircle scaled .3u;
    thdraw origin;
enddef;

def p_station_temporary_ASF (expr pos)=
    T:=identity shifted pos;
    pickup PenD;
    p:=(-.2u,0)--(.2u,0);
    thdraw p;
    thdraw p rotated 90;
    thclean fullcircle scaled .16u;
    pickup PenC;
    thdraw origin;
enddef;

let p_station_temporary_SKBB = p_station_painted_SKBB;

def p_steps_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .4u);
    T:=identity aligned al shifted pos;
    thdraw unitsquare scaled .8u shifted (-0.4u,-.4u) withpen PenD;
    pickup PenC;
    thdraw (-.3u,.3u)--(-.3u,.1u)--(-.1u,.1u)--(-.1u,-.1u)--
           (.1u,-.1u)--(.1u,-.3u)--(.3u,-.3u);
enddef;

def p_fixedladder_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .4u);
    T:=identity aligned al shifted pos;
    thdraw unitsquare scaled .8u shifted (-0.4u,-.4u) withpen PenD;
    pickup PenC;
    thdraw (-.15u,-.4u)--(-.15u,.4u);
    thdraw (.15u,-.4u)--(.15u,.4u);
    thdraw (-.15u,0)--(.15u,0);
    thdraw (-.15u,.2u)--(.15u,.2u);
    thdraw (-.15u,-.2u)--(.15u,-.2u);
enddef;

def p_ropeladder_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .4u);
    T:=identity aligned al shifted pos;
    thdraw unitsquare scaled .8u shifted (-0.4u,-.4u) withpen PenD;
    pickup PenC;
    thdraw (.1u,-.4u)..(.2u,-.2u)..(.1u,.2u)..(.2u,.4u);
    thdraw (-.2u,-.4u)..(-.1u,-.2u)..(-.2u,.2u)..(-.1u,.4u);
    thdraw (-.2u,.2u)--(.1u,.2u);
    thdraw (-.15u,0)--(.15u,0);
    thdraw (-.1u,-.2u)--(.2u,-.2u);
enddef;

def p_bridge_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .4u);
    T:=identity aligned al shifted pos;
    thdraw unitsquare scaled .8u shifted (-0.4u,-.4u) withpen PenD;
    pickup PenC;
    thdraw (-.3u,.2u)--(-.2u,.1u)--(.2u,.1u)--(.3u,.2u);
    thdraw (-.3u,-.2u)--(-.2u,-.1u)--(.2u,-.1u)--(.3u,-.2u);
enddef;

def p_noequipment_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .4u);
    T:=identity aligned al shifted pos;
    thdraw unitsquare scaled .8u shifted (-0.4u,-.4u) withpen PenD;
    thfill (0,-.1u)--(-.05u,.3u)--(.05u,.3u)--cycle;
    thdraw (0,-.2u) withpen PenX;
enddef;

def p_anchor_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .4u);
    T:=identity aligned al shifted pos;
    thdraw unitsquare scaled .8u shifted (-0.4u,-.4u) withpen PenD;
    thdraw fullcircle scaled .2u shifted (0.2u,0) withpen PenC;
    begingroup;
      interim linecap:=butt;
      thdraw (-.3u,0)--(.1u,0) withpen PenA;
    endgroup;
    thdraw (.1u,-.4u)--(.1u,.4u) withpen PenD;
enddef;

def p_traverse_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .4u);
    T:=identity aligned al shifted pos;
    thdraw unitsquare scaled .8u shifted (-0.4u,-.4u) withpen PenD;
    pickup PenC;
    thdraw (-.25u,.05u)..(0,-.08u)..(.25u,.05u);
    pickup pencircle scaled .18u;
    thdraw (-.25u,.05u);
    thdraw (.25u,.05u);
enddef;

def p_rope_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .4u);
    T:=identity aligned al shifted pos;
    thdraw unitsquare scaled .8u shifted (-0.4u,-.4u) withpen PenD;
    pickup PenC;
    thdraw (-.2u,.2u)..origin..(.1u,.1u)--(.1u,-.4u);
    pickup pencircle scaled .18u;
    thdraw (-.2u,.2u);
    thdraw (.1u,.1u);
enddef;

def p_camp_SKBB (expr pos,r,s,al) = 
    U:=(.4u, .5u);
    T:=identity aligned al shifted pos;
    pickup PenC;
    thdraw (-.4u,-.4u)--(0,.5u)--(.4u,-.4u)--cycle;
    pickup PenD;
    thdraw (0,-.4u)--(0,.5u);
enddef;


def p_dig_UIS (expr pos,r,s,al) = 
    U:=(.4u, .5u);
    T:=identity aligned al shifted pos;
    thfill ((-.075u,-.5u){down} .. {up}(0.075u, -.5u) -- (0.075u, .15u) -- (0.3u, 0.15u) -- (0.3u, 0.5u) --
    (-.3u, .5u) -- (-.3u, .15u) -- (-.075u, .15u) -- cycle) rotated 45;
enddef;

def p_continuation_UIS(expr pos,theta,sc,al) =
  U:=(.15u,.3u);
  T:=identity aligned al shifted pos;
  pickup PenC;
  thdraw (-.1u,.2u){up}..{down}(.1u,.2u)..{down}(0,-.05u);
  pickup PenX;
  thdraw (0,-.2u);
enddef;

def p_station_SKBB(expr pos,mark,txt)(text flags) =
  T:=identity shifted pos;

  if mark=4: p_station_fixed(pos)
  elseif mark=3: p_station_natural(pos)
  elseif mark=2: p_station_painted(pos)
  elseif mark=1: p_station_temporary(pos)
  fi;
  
  path path_cave;
  path_cave = (-.3u,-.25u) -- (-.2u,-.25u){dir 135} .. (0, .25u) .. {dir 225}(.2u,-.25u) -- (.3u,-.25u);
  for i=flags:
    pickup PenX;
    if i="entrance":
%      thfill fullcircle scaled 1 cm withtransparentcolor tr_blue;
      thdraw path_cave;
%      p_label.urt(txt,pos+(0.4u,0),0,0);
      if not numeric txt:
        p_smartlabel(txt,pos);
      fi;
    elseif i="sink":
      thdraw path_cave cutbefore ((-u,0)--(u,0)) cutafter ((-u,0)--(u,0));
      pickup PenA;
      thdraw (0,-.45u) -- (0,.05u) withcolor blue;
      thdraw (-.15u,-.02u) -- (0,.05u) -- (.15u,-.02u) withcolor blue;
    elseif i="spring":
      thdraw path_cave cutbefore ((-u,0)--(u,0)) cutafter ((-u,0)--(u,0));
      pickup PenA;
      thdraw (0,-.45u) -- (0,.05u) withcolor blue;
      thdraw (-.15u,-.38u) -- (0,-.45u) -- (.15u,-.38u) withcolor blue;
    elseif i="doline":
      thdraw (-.5u,.05u){down}..(0,-0.6u)..{up}(.5u,.05u);
    elseif i="dig":
      thdraw (-.5u,.25u) -- (-.35u,.25u)--(-.23u,-.25u)--(.23u,-.25u)--(.35u,.25u)--(.5u,.25u);
    elseif substring(0,11) of i = "air-draught":
      pickup PenB;
      thdraw ((0,.11u){dir-120} ..{dir-70}(0,0) .. {dir-120}(0,-.11u));
      thdraw ((0,.11u){dir-120} ..{dir-70}(0,0) .. {dir-120}(0,-.11u)) shifted (.11u,0);
      thdraw ((0,.11u){dir-120} ..{dir-70}(0,0) .. {dir-120}(0,-.11u)) shifted (-.11u,0);
    elseif i="continuation":
      if picture(txt):
        picture ATTR__text;
        ATTR__text := txt;
      fi;
      begingroup;
        save T; transform T;
        p_continuation(pos+(0,.25u),0,1,(0,1));
      endgroup;
      if picture(txt):
        save ATTR__text;
      fi;
    elseif i="arch":
      thdraw (-.5u,-.25u)--(-.5u,.25u)--(.5u,.25u)--(.5u,-.25u)..(0,.2u)..cycle;
      thfill (-.5u,-.25u)--(-.5u,.25u)--(.5u,.25u)--(.5u,-.25u)..(0,.2u)..cycle;
    elseif i="overhang":
      thdraw (.2u,-.25u)--(-.2u,-.25u){up}..{right}(.2u,.25u);
    fi;
  endfor;
%  thdraw (0,0) withpen PenX withcolor red;
enddef;

def p_debug (expr col, pen, pos)=
    T:=identity shifted pos;
    thdraw origin withpen pencircle 
      scaled if pen=0: 2pt 
                 else: 4pt fi 
      withcolor if col=-2: (1,.85,0)
           elseif col=-1: black 
           elseif col=0: red 
           else: blue fi;
enddef;


def p_u(expr pos,theta,sc,al) =
  T:=identity shifted pos;
  thdraw origin withpen pencircle scaled 4pt withcolor red;
enddef;





%%%%% ENDINPUT thPoint %%%%%





%%%%% INPUT thLine %%%%%


%% therion source code
%%
%% This file defines macros for line symbols
%%
%% $Date: 2003/07/01 09:06:44 $
%% $RCSfile: thLine.mp,v $
%% $Revision: 1.4 $
%%
%% Copyright (C) 2000 Martin Budaj
%%
%% -------------------------------------------------------------------- 
%% This program is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation; either version 2 of the License, or
%% any later version.
%%
%% This program is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%% 
%% You should have received a copy of the GNU General Public License
%% along with this program; if not, write to the Free Software
%% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%% --------------------------------------------------------------------

% some definitions

vardef adjust_step (expr L, S) =     % krok is stretched a bit
  if S <= L: L / (floor(L / S))      % according to length of path
  else: L fi
enddef;

def mark_(expr p,t,l) =
  thdraw (point t of p) --
          ((point t of p) + l * unitvector(thdir(p,t) rotated 90));
enddef;

vardef thdir (expr p,t) =    % 1 * epsilon caused problems in scales < 1:1000
                             % n * epsilon is enough for scale 1:(n*1000)
%  ((direction t-100*epsilon of p) + (direction t+100*epsilon of p)) / 2
   postcontrol (t+1000*epsilon) of p - precontrol (t-1000*epsilon) of p
%  direction t of p
enddef;

% walls:

def l_wall_bedrock_UIS (expr P) = 
  T:=identity;
  pickup PenA;
  thdraw P;
enddef;

def l_wall_sand_SKBB (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.1u);
  pickup PenB;  
  forever:
    t := arctime cas of P;
    thdraw ((point t of P) + (uniformdeviate 1) * .4u 
            * unitvector(thdir(P,t) rotated -90));
    cas := cas + mojkrok;
    exitif cas > dlzka + (mojkrok / 3); % for rounding errors
  endfor;
  pickup PenA;  
  thdraw P;
enddef;

def l_wall_pebbles_SKBB (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, .35u);
  pickup PenC;
  q:=superellipse((.2u,0),(0,.1u),(-.2u,0),(0,.-.1u),.75);
  forever:
    t := arctime (cas + mojkrok/2) of P;
    thdraw q randomized (u/20) rotated (angle(thdir(P,t)) + (normaldeviate*40)) shifted point t of P;
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
enddef;

def l_wall_clay_SKBB (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, .5u);
  pickup PenC;
  q := (-0.15u,0){up}..{down}origin..{up}(0.15u,0);
  forever:
    t := arctime (cas + mojkrok/2) of P;
    thdraw q shifted (point t of P + .25u * unitvector(thdir(P,t) rotated -90));
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
  pickup PenA;  
  thdraw P;
enddef;

def l_wall_debris_SKBB (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, .4u);
  pickup PenC;
%  q := ((-.1u,-.15u)--(.2,.03u)--(-.2u,.15u)--cycle) scaled 1.5;
  q := ((-.2u,-.1u)--(.2u,-.1u)--(0,.2u)--cycle) scaled 1.1;
  forever:
    t := arctime (cas + mojkrok/2) of P;
    thdraw q randomized (u/10) rotated uniformdeviate (360) shifted point t of P;
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
enddef;

def l_wall_blocks_SKBB (expr P) = 
%  pickup PenD;
%  draw P withcolor red;

  T:=identity;
  cas := 0;
  dlzka := arclength P;
if dlzka > 0:
  mojkrok:=adjust_step(dlzka, 1.5u);
  pickup PenA;
  forever:
    t1 := arctime (cas + mojkrok*1/10) of P;
    t2 := arctime (cas + mojkrok*9/10) of P;
    q := ((point t1 of P) + .4u * unitvector(thdir(P,t1) rotated -90)) --
         (subpath (t1,t2) of P) --
         ((point t2 of P) + .4u * unitvector(thdir(P,t2) rotated -90));
    thdraw q randomized (u/6);
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
fi;
enddef;

def l_wall_ice_SKBB (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, .5u);
  pickup PenC;
  p := (-.1u,0)--(.1u,0);
  q := (0,-.1u)--(0,.1u);
  forever:
    t := arctime (cas + mojkrok/2) of P;
    thdraw p shifted (point t of P + .25u * unitvector(thdir(P,t) rotated -90));
    thdraw q shifted (point t of P + .25u * unitvector(thdir(P,t) rotated -90));
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
  pickup PenA;  
  thdraw P;
enddef;

def l_wall_underlying_UIS (expr P) = 
  T:=identity;
  pickup PenA;
  thdraw P dashed evenly scaled optical_zoom;
enddef;

def l_wall_unsurveyed_SKBB (expr P) = 
  T:=identity;
  pickup PenC;
  thdraw P;
enddef;

def l_wall_presumed_UIS (expr P) = 
  T:=identity;
  pickup PenA;
  thdraw P dashed evenly scaled (2*optical_zoom);
enddef;

% other line symbols

def l_pit_UIS (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.25u);
  pickup PenD;  
  forever:
    t := arctime cas of P;
    mark_ (P,t,0.2u);
    cas := cas + mojkrok;
    exitif cas > dlzka + (mojkrok / 3); % for rounding errors
  endfor;
  pickup PenC;  
  thdraw P;
enddef;

let l_floorstep_UIS = l_pit_UIS;

def l_overhang_SKBB (expr P) =
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.3u);
  pickup PenC;
  t1:=0;
  forever:
    t := arctime (cas + mojkrok/2) of P;
    t2 := arctime (cas + mojkrok) of P;
    thfill (subpath (t1,t2) of P) -- 
      ((point t of P) + .3u * unitvector(thdir(P,t) rotated 90)) -- 
      cycle;
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
    t1:=t2;
  endfor;
  thdraw P;
enddef;


def l_chimney_UIS (expr P) = 
  T:=identity;
  pickup PenC;
  thdraw P dashed evenly scaled optical_zoom;
enddef;

def l_ceilingstep_SKBB (expr P) =
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.8u);
  pickup PenC;
  forever:
    t1 := arctime (cas + mojkrok*1/5) of P;
    t  := arctime (cas + mojkrok/2) of P;
    t2 := arctime (cas + mojkrok*4/5) of P;
    thdraw (subpath (t1,t2) of P);
    mark_ (P,t,0.2u);
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
enddef;

def l_ceilingmeander_SKBB (expr P) =
  pair Pp;
  pair Pd;
  pair Pv;
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.8u);
  pickup PenC;
  forever:
    t  := arctime (cas + mojkrok/2) of P;
    Pp := (point t of P);
    Pd := unitvector(thdir(P,t));
    Pv := Pd rotated 90;
    thdraw (Pp + 0.1u * Pv) --
          (Pp + 0.2u * Pv);
    thdraw (Pp + 0.2u * Pv + 0.2u * Pd) --
          (Pp + 0.2u * Pv - 0.2u * Pd);
    thdraw (Pp - 0.1u * Pv) --
          (Pp - 0.2u * Pv);
    thdraw (Pp - 0.2u * Pv + 0.2u * Pd) --
          (Pp - 0.2u * Pv - 0.2u * Pd);
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
enddef;


def l_floormeander_SKBB (expr P) =
  pair Pp;
  pair Pd;
  pair Pv;
  pair PPp;
  pair PPd;
  pair PPv;

  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.25u);
  pickup PenC;
  forever:
    t := arctime cas of P;
    Pp := (point t of P);
    Pd := unitvector(thdir(P,t));
    Pv := Pd rotated 90;
    thdraw (Pp + 0.1u * Pv) --
          (Pp + 0.2u * Pv);
    thdraw (Pp - 0.1u * Pv) --
          (Pp - 0.2u * Pv);
    if cas > 0:
      thdraw (PPp + 0.2u * PPv) --
          (Pp + 0.2u * Pv);
      thdraw (PPp - 0.2u * PPv) --
          (Pp - 0.2u * Pv);
    fi;
    PPp := Pp;
    PPd := Pd;
    PPv := Pv;
    cas := cas + mojkrok;
    exitif cas > dlzka + (mojkrok / 3); % for rounding errors
  endfor;
enddef;    

boolean alw_perpendicular;

def l_slope_SKBB (expr P,S)(text Q) = 
%show Q;
  T:=identity;
  numeric dirs[];
  numeric lengths[];
  for i=Q:
    dirs[redpart i]:=greenpart i;
    lengths[redpart i]:=bluepart i;
  endfor;  


  li:=length(P); % last

  alw_perpendicular:=true;

  for i=0 upto li:
    if unknown dirs[i]: dirs[i]:=-1; 
    else: 
      if dirs[i]>-1:
        dirs[i]:=((90-dirs[i]) - angle(thdir(P,i))) mod 360; 
        alw_perpendicular:=false;
      fi;
    fi;
    if unknown lengths[i]: lengths[i]:=-1; fi;
  endfor;

%for i=0 upto li: show dirs[i]; endfor;

  ni:=0; % next
  pi:=0; % previous

  for i=0 upto li:
    d:=dirs[i];
    if d=-1:
      if (i=0) or (i=li):
        dirs[i] := angle(thdir(P,i) rotated 90) mod 360;
	pi:=i;
      else:
        if ni<=i:
	  for j=i upto li:
            ni:=j;
	    exitif dirs[j]>-1;
	  endfor;
	fi;
	w:=arclength(subpath(pi,i) of P) / 
	   arclength(subpath(pi,ni) of P);
	dirs[i]:=w[dirs[pi],dirs[ni]];
%        if (dirs[i]-angle(thdir(P,i))) mod 360>180:
%          dirs[i]:=w[dirs[ni],dirs[pi]];
%	  message("*******");
%        fi;
     fi;
    else:
      pi:=i;
    fi;
  endfor;

%for i=0 upto li: show dirs[i]; endfor;

  ni:=0; % next
  pi:=0; % previous

  for i=0 upto li:
    l:=lengths[i];
    if l=-1:
      if (i=0) or (i=li):
        lengths[i] := 1cm; % should never happen!
	thwarning("slope width at the end point not specified");
	pi:=i;
      else:
        if ni<=i:
	  for j=i+1 upto li:
            ni:=j;
	    exitif lengths[j]>-1;
	  endfor;
	fi;
	w:=arclength(subpath(pi,i) of P) / 
	   arclength(subpath(pi,ni) of P);
	lengths[i]:=w[lengths[pi],lengths[ni]];
	pi:=i;
      fi;
    else:
      pi:=i;
    fi;
  endfor;

% for i=0 upto li: show lengths[i]; endfor;

  T:=identity;
  boolean par;
  cas := 0.3u;
  krok := 0.7u;
  dlzka := (arclength P);
  if dlzka>3u: dlzka:=dlzka-0.6u fi;
  mojkrok:=adjust_step(dlzka,1.4u) / 2;
  pickup PenD;
  par := false; 
  forever:
    t := arctime cas of P;
    if t mod 1>0:  % not a key point
      w := (arclength(subpath(floor t,t) of P) / 
            arclength(subpath(floor t,ceiling t) of P));
      if alw_perpendicular:
        a := 90;
      else:
        a := w[dirs[floor t],dirs[ceiling t]];
      fi;
      l := w[lengths[floor t],lengths[ceiling t]];
    else:
      if alw_perpendicular:
        a := 90;
      else:
        a:= dirs[t];
      fi; 
      l:=lengths[t];
    fi;
    
    a := a + angle(thdir(P,t));
    
    thdraw (point t of P) -- 
      ((point t of P) + if par: 0.333 * fi l * unitvector(dir(a)));
    cas := cas + mojkrok;
    par := not par;
    exitif cas > dlzka + .3u + (krok / 3);  % for rounding errors
  endfor;
  if S = 1: pickup PenC; draw P fi;
%pickup pencircle scaled 3pt;
%for i=0 upto li: draw point i of P; endfor;
enddef;


def l_slope_BCRA (expr P,S)(text Q) = 
%show Q;
  T:=identity;
  numeric dirs[];
  numeric lengths[];
  for i=Q:
    dirs[redpart i]:=greenpart i;
    lengths[redpart i]:=bluepart i;
  endfor;  


  li:=length(P); % last

  alw_perpendicular:=true;

  for i=0 upto li:
    if unknown dirs[i]: dirs[i]:=-1; 
    else: 
      if dirs[i]>-1:
        dirs[i]:=((90-dirs[i]) - angle(thdir(P,i))) mod 360; 
        alw_perpendicular:=false;
      fi;
    fi;
    if unknown lengths[i]: lengths[i]:=-1; fi;
  endfor;

%for i=0 upto li: show dirs[i]; endfor;

  ni:=0; % next
  pi:=0; % previous

  for i=0 upto li:
    d:=dirs[i];
    if d=-1:
      if (i=0) or (i=li):
        dirs[i] := angle(thdir(P,i) rotated 90) mod 360;
	pi:=i;
      else:
        if ni<=i:
	  for j=i upto li:
            ni:=j;
	    exitif dirs[j]>-1;
	  endfor;
	fi;
	w:=arclength(subpath(pi,i) of P) / 
	   arclength(subpath(pi,ni) of P);
	dirs[i]:=w[dirs[pi],dirs[ni]];
%        if (dirs[i]-angle(thdir(P,i))) mod 360>180:
%          dirs[i]:=w[dirs[ni],dirs[pi]];
%	  message("*******");
%        fi;
     fi;
    else:
      pi:=i;
    fi;
  endfor;

%for i=0 upto li: show dirs[i]; endfor;

  ni:=0; % next
  pi:=0; % previous

  for i=0 upto li:
    l:=lengths[i];
    if l=-1:
      if (i=0) or (i=li):
        lengths[i] := 1cm; % should never happen!
	thwarning("slope width at the end point not specified");
	pi:=i;
      else:
        if ni<=i:
	  for j=i+1 upto li:
            ni:=j;
	    exitif lengths[j]>-1;
	  endfor;
	fi;
	w:=arclength(subpath(pi,i) of P) / 
	   arclength(subpath(pi,ni) of P);
	lengths[i]:=w[lengths[pi],lengths[ni]];
	pi:=i;
      fi;
    else:
      pi:=i;
    fi;
  endfor;

% for i=0 upto li: show lengths[i]; endfor;

  T:=identity;
  boolean par;
  cas := 0.3u;
  krok := 0.7u;
  dlzka := (arclength P);
  if dlzka>3u: dlzka:=dlzka-0.6u fi;
  mojkrok:=adjust_step(dlzka,1.4u) / 2;
  pickup PenD;
  par := false; 
  forever:
    t := arctime cas of P;
    if t mod 1>0:  % not a key point
      w := (arclength(subpath(floor t,t) of P) / 
            arclength(subpath(floor t,ceiling t) of P));
      if alw_perpendicular:
        a := 90;
      else:
        a := w[dirs[floor t],dirs[ceiling t]];
      fi;
      l := w[lengths[floor t],lengths[ceiling t]];
    else:
      if alw_perpendicular:
        a := 90;
      else:
        a:= dirs[t];
      fi; 
      l:=lengths[t];
    fi;
    
    a := a + angle(thdir(P,t));
    
    if par:
      thfill (point t of P) + mojkrok/2.5 * unitvector(dir(a+90))-- 
        ((point t of P) + l * unitvector(dir(a))) --
        (point t of P) + mojkrok/2.5 * unitvector(dir(a-90)) -- cycle;
    fi;
    cas := cas + mojkrok;
    par := not par;
    exitif cas > dlzka + .3u + (krok / 3);  % for rounding errors
  endfor;
enddef;


def l_contour_UIS(expr P)(text txt) =
  T:=identity;
  pickup PenD;
  thdraw P;
  for pnt=txt:
    if pnt=-2:
      mark_(P,arctime(arclength(P)/2) of P, 0.2u);
    elseif pnt>=0:
      mark_(P,pnt,0.2*u);
    fi;
    exitif pnt<0;
  endfor;
enddef;

def l_contour_SKBB(expr P)(text txt) =
  T:=identity;
  pickup PenD;
  thdraw P;
  for pnt=txt:
    if (pnt=-2) or (pnt=-1):
      mark_(P,arctime(arclength(P)/2) of P, 0.2u);
    elseif pnt>=0:
      mark_(P,pnt,0.2*u);
    fi;
    exitif pnt<0;
  endfor;
enddef;

def l_rockborder_UIS (expr P) = 
  T:=identity;
  pickup PenC;
  if cycle P: thclean P fi;
  thdraw P;
enddef;

def l_rockedge_UIS (expr P) = 
  T:=identity;
  pickup PenD;
  thdraw P;
enddef;

def l_border_visible_SKBB (expr Path) =
  T:=identity;
  pickup PenC;
  draw Path;
enddef;

def l_border_temporary_SKBB (expr Path) =
  T:=identity;
  pickup PenC;
  draw Path dashed evenly scaled optical_zoom;
enddef;

def l_flowstone_UIS (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, .7u);
  pickup PenC;  
  t1:=0;
  forever:
    t2 := arctime (cas + mojkrok) of P;
    thdraw (point t1 of P){dir (angle(thdir(P,t1)) + 60)} ..
           {dir (angle(thdir(P,t2)) - 60)}(point t2 of P);
    cas := cas + mojkrok;
    exitif cas > dlzka + (mojkrok / 3); % for rounding errors
    t1:=t2;
  endfor;
enddef;


def l_moonmilk_UIS (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, .3u);
  pickup PenC;  
  t1:=0;
  forever:
    t2 := arctime (cas + mojkrok) of P;
    thdraw (point t1 of P){dir (angle(thdir(P,t1)) + 80)} ..
           {dir (angle(thdir(P,t2)) - 80)}(point t2 of P);
    cas := cas + mojkrok;
    exitif cas > dlzka + (mojkrok / 3); % for rounding errors
    t1:=t2;
  endfor;
enddef;


def l_survey_surface_SKBB (expr P) =
  T:=identity;
  thdrawoptions(dashed withdots scaled (0.2 * optical_zoom) withpen PenC);
  thdraw P;
  thdrawoptions();
enddef;

def l_survey_cave_SKBB (expr P) =
  T:=identity;
  pickup PenC;
  if ATTR__scrap_centerline:
    thdraw P;
  else:
    PolygonLine:=.8u;
    pair zz[];
    for t = 0 upto length P - 1:
      zz1 := point t of P;
      zz2 := point t+1 of P;
      if length (zz2-zz1) > 2*PolygonLine:
        thdraw zz1 -- zz1 + PolygonLine * unitvector(zz2 - zz1);
        thdraw zz2 -- zz2 + PolygonLine * unitvector(zz1 - zz2);
      else:
        thdraw zz1 -- zz2;
      fi;
    endfor;
  fi;
enddef;

def l_survey_cave_UIS (expr P) =
  T:=identity;
  pair zz[];
  pickup PenC;
  for t = 0 upto length P - 1:
    zz1 := point t of P;
    zz2 := point t+1 of P;
    draw zz1 -- zz2;
  endfor;
enddef;

def l_waterflow_permanent_UIS (expr Path) =
  path ppp;
  T:=identity;
  cas := 0;
  dlzka := arclength Path;
  mojkrok:=adjust_step(dlzka, 0.5u);
    pickup PenD;  
    vardef azim = 50 + 15*normaldeviate enddef;
    az1 := azim;
    sgn := 1;
    ppp := point 0 of Path;
    forever:
        t1 := arctime cas of Path;
        t2 := arctime (cas+mojkrok) of Path;
        if cas+1.1*mojkrok > dlzka:
            az2 := 0;
        else:
            az2 := azim;
        fi;
        d1 := angle(thdir(Path,t1)) + sgn * az1;
        d2 := angle(thdir(Path,t2)) - sgn * az2;
        ppp := ppp & (point t1 of Path){dir d1} .. {dir d2}(point t2 of Path);
        az1 := az2;
        sgn := -1 * sgn;
        cas := cas + mojkrok;
        exitif cas > dlzka + mojkrok/3;     % for rounding errors
    endfor;
%    drawarrow ppp;
    thdraw ppp;
    thdrawoptions();
    oldahlength:=ahlength;
    ahlength:=ahlength*optical_zoom;
    thdraw arrowhead ppp;
    thfill arrowhead ppp;
    ahlength:=oldahlength;
enddef;

def l_waterflow_intermittent_SKBB (expr Path) =
  thdrawoptions(dashed evenly scaled optical_zoom);
  l_waterflow_permanent_UIS (Path);
  thdrawoptions();
enddef;

def l_waterflow_conjectural_SKBB (expr Path) =
  thdrawoptions(dashed withdots scaled (0.5 * optical_zoom) withpen PenB);
  l_waterflow_permanent_UIS (Path);
  thdrawoptions();
enddef;

def l_invisible (expr P) = 
enddef;

def l_undefined (expr P) = 
  T:=identity;
  pickup PenC;
  thdraw P withcolor red;
  thwarning("undefined line symbol used");
enddef;

% Q = 0 -- no arrows
%     1 -- end
%     2 -- begin
%     3 -- both

def l_arrow_SKBB (expr P, Q) =
  T:=identity;
  pickup PenC;
  thdraw P;
  p := (-.1u,-.25u)--(0,0)--(.1u,-.25u);
  if odd Q:
    draw p rotated (angle(thdir(P,0))+90) shifted (point 0 of P);
  fi;
  if Q>1:
    draw p rotated (angle(thdir(P,length P))-90) 
         shifted (point infinity of P);
  fi;
enddef;

def l_mapconnection_SKBB (expr P) =
  thdrawoptions(dashed withdots scaled (0.5 * optical_zoom) withpen PenB);
  l_arrow_SKBB(P,3);
  thdrawoptions();
enddef;


def l_section_SKBB (expr P)(text txt) =
  T:=identity;
  path Q; Q = punked P;
  pickup PenC;
  for t = 0 upto length P - 1:
    pair zz[];
    zz1 := point t of P;
    zz2 := point t+1 of P;
    zz3 := postcontrol t of P;
    zz4 := precontrol t+1 of P;
    if (length(zz3-1/3[zz1,zz2]) > 0.1pt) or
       (length(zz4-2/3[zz1,zz2]) > 0.1pt):
      zz5 = whatever[zz1,zz2];
      (zz3-zz5) = whatever * (zz1-zz2) rotated 90;
      draw zz1--zz5;
      zz6 = whatever[zz1,zz2];
      (zz4-zz6) = whatever * (zz1-zz2) rotated 90;
      draw zz2--zz6;
    else:
      draw zz1--zz2;
    fi;
  endfor;
  for pnt=txt:
    if pnt=-1:
    else:
      T:=identity rotated angle(thdir(Q,pnt)) shifted (point pnt of Q);
      pickup PenC;
      thdraw (0,0)--(0,.8u);
      thdraw (-.1u,.55u)--(0,.8u)--(.1u,.55u);
    fi;
    exitif pnt=-1;
  endfor;
enddef;

let l_border_invisible = l_invisible;
let l_wall_invisible = l_invisible;

def l_debug (expr col, pen, P) =
  T:=identity;
  pickup if pen=0: PenD else: PenB fi;
  thdraw P 
    withcolor if col=-2: (1,.85,0)
          elseif col=-1: black 
           elseif col=0: red 
                   else: blue fi;
enddef;


def l_u (expr P) =
  T:=identity;
  pickup PenA;
  thdraw P withcolor red;
enddef;


def l_gradient_UIS (expr P) =
  T:=identity;
  pickup PenC;
  thdraw P;
  p:=(-.15u,-.4u)--(0,0)--(.15u,-.4u)--cycle;
  thfill (p rotated (angle(thdir(P,length P))-90) 
         shifted (point infinity of P));
  thdraw (p rotated (angle(thdir(P,length P))-90) 
         shifted (point infinity of P));
enddef;

def l_gradient_BCRA (expr P) =
  T:=identity;
  pickup PenC;
  for t = 0 upto length P - 1:
    pair zz[];
    zz1 := point t of P;
    zz2 := point t+1 of P;
    zz3 := unitvector(zz2 - zz1);
    thfill zz1 + u/3 * zz3 + .25u * (zz3 rotated 90) --
           zz2 - u/3 * zz3 --
	   zz1 + u/3 * zz3 + .25u * (zz3 rotated -90) -- cycle;
  endfor;
enddef;


def l_rope_SKBB (expr P,exact) =
  T:=identity;
  pickup PenC;
  if exact:
    draw P;
  else:
    d:=0.5u;
    for i:=0 upto (length P - 2):
      x1 := xpart point i of P;
      y1 := ypart point i of P;
      x2 := xpart point i+1 of P;
      y2 := ypart point i+1 of P;
    
      dx1:=x1; dy1:=y1; x1:=0; y1:=0; x2:=x2-dx1; y2:=y2-dy1;
      if y2 > y1:
        y3 := y1 - d;
        x3 := x1 + (x2-x1)*sqrt(d)/(sqrt(d)+sqrt(y2-y1+d));
      else:
        y3 := y2 - d;
        x3 := x1 + (x2-x1)*sqrt(y1-y2+d)/(sqrt(d)+sqrt(y1-y2+d));
      fi;
      numeric a,b,c;
      a*x1/10*x1 + b/10*x1 + c/10 = y1/10;
      a*x2/10*x2 + b/10*x2 + c/10 = y2/10;
      a*x3/10*x3 + b/10*x3 + c/10 = y3/10;
      draw (x1+dx1,y1+dy1)
      for t = x1 step (x2-x1)/20 until x2+10*epsilon:
        -- (t+dx1,a*t*t+b*t+c+dy1)
      endfor;
    endfor;
    pair x;
    x = point (length P)-1 of P + whatewer * down;
    x = point (length P) of P + whatever * right;
    draw point (length P)-1 of P -- x;
%    thdraw point length P of P withpen pencircle scaled 0.3u withcolor red;
  fi;
  for i:=0 upto length P if not exact: -1 fi:
    thdraw point i of P withpen pencircle scaled 0.3u;
  endfor;
enddef;

def l_border_presumed_SKBB (expr Path) =
  T:=identity;
  pickup PenC;
  draw Path dashed evenly scaled (0.25 * optical_zoom);
enddef;



%%%%% ENDINPUT thLine %%%%%





%%%%% INPUT thArea %%%%%


%% therion source code
%%
%% This file defines tiling patterns
%%
%% $Date: 2003/07/01 09:06:44 $
%% $RCSfile: thPatterns.mp,v $
%% $Revision: 1.6 $
%%
%% Copyright (C) 2000-2004 Martin Budaj
%%
%% -------------------------------------------------------------------- 
%% This program is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation; either version 2 of the License, or
%% any later version.
%%
%% This program is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%% 
%% You should have received a copy of the GNU General Public License
%% along with this program; if not, write to the Free Software
%% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%% --------------------------------------------------------------------

if unknown a_water_UIS: else: endinput fi;

% patterns

beginpattern(pattern_water_UIS);
    draw origin--10up withpen pensquare scaled (0.02u);
    patternxstep(.18u);
    patterntransform(identity rotated 45);
endpattern;

beginpattern(pattern_sump_UIS);
    draw origin--(0,.25u) withpen pensquare scaled (0.02u);
    draw origin--(.25u,0) withpen pensquare scaled (0.02u);
    patterntransform(identity rotated 45);
endpattern;

beginpattern(pattern_sand_UIS);
    pickup PenC;
    draw (0.2u,.3u);
    draw (0.35u,.7u);
    draw (0.6u,.5u);
    draw (0.8u,.1u);    
endpattern;

beginpattern(pattern_debris_SKBB);
    pickup PenC;
    p:=(-.15u,0)--(.15u,0);
    draw p rotated -40 shifted (.2u,.3u);
    draw p rotated 70 shifted (.9u,1.5u);
    draw p rotated 20 shifted (1.5u,.8u);
%    draw p rotated 95 shifted (.9u,.8u);
endpattern;


beginpattern(pattern_moonmilk_SKBB);
    pickup PenC;
    p:=(-.5u,0){down} .. {up}(-.1666u,0){down}..
        {up}(.1666u,0){down} .. {up}(.5u,0);
    draw p;
    draw p shifted (0.5u,0.3u);
    patternxstep(1.0u);
    patternystep(0.6u);
endpattern;


beginpattern(pattern_flowstone_ASF);
    pickup PenC;
    p:=(-.35u,0){dir -60} .. {dir 60}(0.35u,0);
    draw p;
    draw p shifted (0.35u,0.4u);
    patternxstep(u);
    patternystep(0.8u);
endpattern;


beginpattern(pattern_empty);
  patternbbox(0,0,10,10);
  patternstep(10,10);
endpattern;

% symbols

def a_water_UIS (expr Path) =
  T:=identity;
  thclean Path;
  thfill Path withpattern pattern_water_UIS;
enddef;

def a_sump_UIS (expr Path) =
  T:=identity;
  thclean Path;
  thfill Path withpattern pattern_sump_UIS;
enddef;


def a_flowstone_ASF (expr Path) =
  T:=identity;
  thclean Path;
  thfill Path withpattern pattern_flowstone_ASF;
enddef;


def a_moonmilk_SKBB (expr Path) =
  T:=identity;
  thclean Path;
  thfill Path withpattern pattern_moonmilk_SKBB;
enddef;



def a_sand_UIS (expr p) =
  T:=identity;
%  thclean p;
  pickup PenC;
  path q; q = bbox p;
  picture tmp_pic; 
  tmp_pic := image(
    for i = xpart llcorner q step 1u until xpart urcorner q:
      for j = ypart llcorner q step 1u until ypart urcorner q:
        draw origin shifted ((i,j) randomized 0.7u) withpen PenC;
      endfor;  
    endfor;
  );
  clip tmp_pic to p;
  draw tmp_pic;
enddef;

def a_debris_UIS (expr Path) =
  T:=identity;
%  thclean Path;
  thfill Path withpattern pattern_debris_SKBB;
enddef;


def a_bedrock_SKBB (expr Path) =
  T:=identity;
  thclean Path;
enddef;


def a_dimensions_SKBB (expr Path) =
  T:=identity;
  thclean Path;
enddef;


def a_debris_SKBB (expr p) =
  T:=identity;
%  thclean p;
  pickup PenC;
  path q; q = bbox p;
  picture tmp_pic; 
  tmp_pic := image(
    for i = xpart llcorner q step 1u until xpart urcorner q:
      for j = ypart llcorner q step 1u until ypart urcorner q:
        draw ((-.2u,0)--(.2u,0)) 
             rotated uniformdeviate(360) 
             shifted ((i,j) randomized 0.6u) withpen PenC;
      endfor;  
    endfor;
  );
  clip tmp_pic to p;
  draw tmp_pic;
enddef;

def a_clay_SKBB (expr p) =
  T:=identity;
  pickup PenC;
  path q, qq; q = bbox p;
  picture tmp_pic; 
  tmp_pic := image(
    for i = xpart llcorner q step 1.5u until xpart urcorner q:
      for j = ypart llcorner q step 1.5u until ypart urcorner q:
        qq := ((-.25u,0){up}..origin{down}..{up}(0.25u,0)) randomized (u/15)
             shifted ((i,j) randomized 1.3u);
	if xpart (p intersectiontimes qq) < 0:
	  thdraw qq;
	fi;
      endfor;  
    endfor;
  );
  clip tmp_pic to p;
  draw tmp_pic;
enddef;

def a_ice_SKBB (expr p) =
  T:=identity;
  pickup PenC;
  path q; q = bbox p;
  picture tmp_pic, tmp_ice; 
  pair z;
  tmp_ice := image (
    draw (-.2u,0)--(.2u,0);
    draw (0,-.2u)--(0,.2u);
  );
  tmp_pic := image(
    for i = xpart llcorner q step 1.5u until xpart urcorner q:
      for j = ypart llcorner q step 1.5u until ypart urcorner q:
        z := ((i,j) randomized 1.3u);
	if xpart (p intersectiontimes ((bbox tmp_ice) shifted z)) < 0:
	  draw tmp_ice shifted z;
	fi;
      endfor;  
    endfor;
  );
  clip tmp_pic to p;
  draw tmp_pic;
enddef;


def a_snow_SKBB (expr P) =
  T:=identity;
  pickup PenC;
  path q; q = bbox P;
  picture tmp_pic, tmp_sn; 
  pair z;
  tmp_sn := image(
    pickup PenC;
    p := origin--(0,.2*u);
    draw p; draw p rotated (60); draw p rotated (120);
    draw p rotated (180); draw p rotated (240); draw p rotated (300);
    p := ((0,.3u)--(0,0.2u)--(0.1u,0.2u)) rotatedaround ((0,0.2u),45);
    draw p; draw p rotated (60); draw p rotated (120);
    draw p rotated (180); draw p rotated (240);
    draw p rotated (300);
  );
  tmp_pic := image(
    for i = xpart llcorner q step 1.5u until xpart urcorner q:
      for j = ypart llcorner q step 1.5u until ypart urcorner q:
        z:=((i,j) randomized 1.3u);
	if xpart (P intersectiontimes ((bbox tmp_sn) shifted z)) < 0:
          draw tmp_sn shifted z;
	fi;
      endfor;  
    endfor;
  );
  clip tmp_pic to P;
  draw tmp_pic;
enddef;

def a_blocks_SKBB (expr p) =
  T:=identity;
  pickup PenC;
  path q, qq; q = bbox p;
  picture tmp_pic; 
  uu := max(u, (xpart urcorner q - xpart llcorner q)/100, (ypart urcorner q - ypart llcorner q)/100);
  tmp_pic := image(
    for i = xpart llcorner q step 2uu until xpart urcorner q:
      for j = ypart llcorner q step 2uu until ypart urcorner q:
        qq := punked (((-.5uu,-.5uu)--(.5uu,-.5uu)--(.5uu,.5uu)--(-.5uu,.5uu)--cycle) 
	     randomized (uu/2))
             rotated uniformdeviate(360) 
             shifted ((i,j) randomized 1.6uu);
	if xpart (p intersectiontimes qq) < 0:
	  thclean qq;
	  thdraw qq;
	fi;
      endfor;  
    endfor;
  );
  clip tmp_pic to p;
  draw tmp_pic;
enddef;

def a_blocks_SKBB_ (expr p,o,s_beg,s_end,d_beg,d_end) =
  T:=identity;
  orient = 90-o;
  pickup PenC;
  path q[], qq; q1 = bbox p;
  draw q1; draw p;
  q2 = llcorner q1 .. lrcorner q1 .. urcorner q1 .. ulcorner q1 .. cycle;
  draw q2;
  pair zz[];
  radius := .5*length(llcorner q1 - urcorner q1);
  zz1 := .5[llcorner q1,urcorner q1];
  zz2 := zz1 - dir(orient)*radius;
  zz3 = zz1 rotatedaround (zz2,90);
  draw zz1 withpen pencircle scaled 2mm;
  draw zz2 withpen pencircle scaled 2mm;
  draw zz3 withpen pencircle scaled 2mm;
  picture tmp_pic; 
  tmp_pic := image(
    j := 0;
    forever:
      i := 0;
      forever:
        qq := punked (((-.5,-.5)--(.5,-.5)--(.5,.5)--(-.5,.5)--cycle) 
             scaled (0.7(5u-(j/(2*radius))[u*s_beg,u*s_end]))
	     randomized (u/2))
             rotated uniformdeviate(360) 
             shifted (((zz3 + (i,j)) rotatedaround (zz3,-o)) randomized 1.6u);
        draw ((zz3 + (i,j)) rotatedaround (zz3,-o)) withpen pencircle scaled 1mm withcolor red;
	if xpart (p intersectiontimes qq) < 0:
	  thclean qq;
	  thdraw qq;
	fi;
        i := i + (j/(2*radius))[u*d_beg,u*d_end];
        exitif i > 2*radius;
      endfor;  
      j := j + (j/(2*radius))[u*d_beg,u*d_end];
      exitif j > 2*radius;
    endfor;
  );
%  clip tmp_pic to p;
  draw tmp_pic;
enddef;


def a_pebbles_SKBB (expr p) =
  T:=identity;
  pickup PenC;
  path q, qq; q = bbox p;
  picture tmp_pic; 
  tmp_pic := image(
    for i = xpart llcorner q step 1.5u until xpart urcorner q:
      for j = ypart llcorner q step 1.5u until ypart urcorner q:
        qq := (superellipse((.2u,0),(0,.1u),(-.2u,0),(0,.-.1u),.75))
%             randomized (u/25)
             rotated uniformdeviate(360) 
             shifted ((i,j) randomized 0.8u);
	if xpart (p intersectiontimes qq) < 0:
	  thdraw qq;
	fi;
      endfor;  
    endfor;
  );
  clip tmp_pic to p;
  draw tmp_pic;
enddef;

def a_u (expr p) =
    T:=identity;
    thfill p withcolor red;
enddef;




%%%%% ENDINPUT thArea %%%%%





%%%%% INPUT thText %%%%%


%% therion source code
%%
%% This file defines macros for labels
%%
%% $Date: 2003/07/01 09:06:44 $
%% $RCSfile: thText.mp,v $
%% $Revision: 1.6 $
%%
%% Copyright (C) 2000-2004 Martin Budaj
%%
%% -------------------------------------------------------------------- 
%% This program is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation; either version 2 of the License, or
%% any later version.
%%
%% This program is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%% 
%% You should have received a copy of the GNU General Public License
%% along with this program; if not, write to the Free Software
%% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%% --------------------------------------------------------------------

labeloffset:=0bp;

def l_label(expr txt,path) =
  picture pict,pic;
  pair pos;
  numeric dx[],dy[],ptime;
  string ss,ch;

  pic := thelabel(txt,(0,0));
  strl := xpart(lrcorner pic - llcorner pic);
  pathl := arclength (path);
  dx0 := xpart(llcorner pic);
%  dy0 := ypart(llcorner pic);

  if strl>pathl:
    txtzoom := pathl / strl;
    thwarning("text scaled down to fit the path");
  else:
    txtzoom := 1;
  fi;
  zoom := pathl / strl;

  for p within pic:
    if textual p:                                
      ss := textpart p;
      dx1 := xpart p;
      dy1 := ypart p;
      dx2 := dx1;

      if xxpart p / yypart p <> 1: message("Error: distorted text!") fi;
      txtscale := txtzoom * xxpart p;

      l := length(ss);
      ptime := 0;
      for i=0 upto (l - 1):
        ch := substring(i,i+1) of ss;
        pict := ch infont fontpart p scaled txtscale;
        charwidth := xpart (lrcorner pict - llcorner pict) / txtzoom;
        dy2 := ypart lrcorner pict / txtzoom;
        ptime := (dx2 - dx0 + 0.5*charwidth) * zoom;
        t := arctime ptime of (path);
        pos := point t of path + 
          (dy1+dy2)*txtzoom*unitvector((direction t of path) rotated 90);
        begingroup
%          interim labeloffset:=0bp;
          lab:=thelabel.top(pict,pos);
          process_label(pos,angle direction t of path);
        endgroup;
        dx2 := dx2 + charwidth;
      endfor;

    else: message("Strange component of line label!");
    fi;
  endfor;
  % draw path;
enddef;

vardef p_label@#(expr txt,pos,rot,mode) =
  if (mode=1) or (mode=7): interim labeloffset:=(u/8) fi;
  lab:=thelabel@#(txt, pos);
  if mode>1: pickup PenD fi;
  if mode=1:
    pickup pencircle scaled (u/6);
    drawdot(pos);
    process_label(pos,0);
  elseif mode=2: process_uplabel;       
  elseif mode=3: process_downlabel;
  elseif mode=4: process_updownlabel;
  elseif mode=5: process_circledlabel;
  elseif mode=6: process_boxedlabel;
  elseif mode=7: process_label(pos,rot);  % station name
  elseif mode=8: process_filledlabel(pos, rot);
  else: process_label(pos,rot); fi;
enddef;


% at beginfig: save smartll[], smartur[]; pair smartll[], smartur[]; smart_count=0;
numeric smart_count, smart_D;
boolean smart_quit, smart_inner_quit;

def p_smartlabel (expr txt, pos) =
%  begingroup;
  interim bboxmargin:=0.2pt;
  lab:=thelabel.rt(txt,pos+(0.4u,0));
  q:=bbox lab;
  smart_D := 0;
  forever:
    smart_quit := true;
    smart_inner_quit := false;
    for i=1 upto smart_count:
      if smart_overlap(((point 0 of q) shifted (0,smart_D)), 
                       ((point 2 of q) shifted (0,smart_D)),
                       smartll[i], smartur[i]) = 1:
        smart_quit := false;
        smart_D := smart_D - 3pt;
        smart_inner_quit := true;
      fi;
      exitif smart_inner_quit;
    endfor;
    exitif smart_quit;
  endfor;
  smart_U := 0;
  forever:
    smart_quit := true;
    smart_inner_quit := false;
    for i=1 upto smart_count:
      if smart_overlap(((point 0 of q) shifted (0,smart_U)), 
                       ((point 2 of q) shifted (0,smart_U)),
                       smartll[i], smartur[i]) = 1:
        smart_quit := false;
        smart_U := smart_U + 3pt;
        smart_inner_quit := true;
      fi;
      exitif smart_inner_quit;
    endfor;
    exitif smart_quit;
  endfor;
  if (smart_U < -smart_D): smart_D := smart_U; fi;
  draw lab shifted (0,smart_D);
  if (abs(smart_D)>3pt):
    pickup PenB;
    draw pos{left}..{right}(pos shifted (.4u, smart_D)) dashed withdots scaled 0.2;
    pickup PenA;
    drawdot pos;
  fi;
  smart_count := smart_count + 1;
  smartll[smart_count] := point 0 of (q shifted (0,smart_D));
  smartur[smart_count] := point 2 of (q shifted (0,smart_D));
%  endgroup;
enddef;

def smart_overlap (expr llA, urA, llB, urB) =
  if ((xpart llA < xpart urB) and (xpart urA > xpart llB)) and
     ((ypart llA < ypart urB) and (ypart urA > ypart llB)):
    1
  else:
    0 
  fi
enddef;




def p_wallaltitude (expr pprev,pos,pnext,txt) =
  rot:=angle(unitvector(pnext-pos)+unitvector(pos-pprev)) - 90;
  pickup PenD;
  pair zz;
%  zz:=(pos + 3*unitvector(dir rot));
  zz:=(pos + (u/4)*unitvector(dir rot));
  draw (pos)--zz;
  begingroup 
%    interim labeloffset:=2pt;
    interim labeloffset:=(u/12);
    if horiz_labels:
      rot:=rot mod 360;
      if rot<=22.5: lab:=thelabel.rt(txt, zz);
      elseif rot<=67.5: lab:=thelabel.urt(txt, zz);
      elseif rot<=112.5: lab:=thelabel.top(txt, zz);
      elseif rot<=157.5: lab:=thelabel.ulft(txt, zz);
      elseif rot<=202.5: lab:=thelabel.lft(txt, zz);
      elseif rot<=247.5: lab:=thelabel.llft(txt, zz);
      elseif rot<=292.5: lab:=thelabel.bot(txt, zz);
      elseif rot<=337.5: lab:=thelabel.lrt(txt, zz);
      else: lab:=thelabel.rt(txt, zz);
      fi;
    else:
      if (abs rot>90) and (abs rot <=270):
        rot:=rot-180;
        lab:=thelabel.lft(txt, pos);
      else:
        lab:=thelabel.rt(txt, pos);
      fi;
    fi;
    process_label(pos, if horiz_labels: 0 else: rot fi);
  endgroup;
enddef;




%%%%% ENDINPUT thText %%%%%





%%%%% INPUT thSpecial %%%%%


%% therion source code
%%
%% This file defines macros for special symbols
%%
%% $Date: $
%% $RCSfile: $
%% $Revision: $
%%
%% Copyright (C) 2004 Martin Budaj
%%
%% -------------------------------------------------------------------- 
%% This program is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation; either version 2 of the License, or
%% any later version.
%%
%% This program is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%% 
%% You should have received a copy of the GNU General Public License
%% along with this program; if not, write to the Free Software
%% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%% --------------------------------------------------------------------

def s_northarrow_SKBB_old (expr rot) = 
  T:=identity;
  picture tmp_pic;
  tmp_pic = image (
    begingroup
      interim defaultscale:=3;
      label(if (lang="sk") or (lang="cz"): "S" else: "N" fi, origin);
    endgroup;
    drawarrow (0,-1cm)--(0,1cm);
  );
  draw tmp_pic rotatedaround(origin, -rot);
enddef;

def s_northarrow_SKBB (expr rot) =
  T:=identity scaled 0.7 rotated -rot;
  thdraw (-.5cm,-1cm)--(0,1.5cm)--(.5cm,-1cm)--(0,-.5cm)--cycle;
  thfill (-.5cm,-1cm)--(0,1.5cm)--(0,-.5cm)--cycle;
enddef;

def s_scalebar_SKBB (expr l, units, txt) = 
  begingroup
    interim warningcheck:=0;
    tmpl:=l / Scale * cm * units / 2;
  endgroup;
  pickup PenC;
  draw (-tmpl,0)--(tmpl,0);
  draw (-tmpl,0)--(-tmpl,2bp);
  draw (tmpl,0)--(tmpl,2bp);
  begingroup
    interim labeloffset:=2bp;
    label.top(thTEX(decimal (l) & "\thinspace" & txt),origin);
  endgroup
enddef;

def s_scalebar_SM (expr l, units, txt) = 
  begingroup
    interim warningcheck:=0;
    tmpl:=l / Scale * cm * units / 2;
  endgroup;
  pickup PenC;
  draw (-tmpl,0)--(tmpl,0);
  draw (-tmpl,0)--(-tmpl,2bp);
  draw (tmpl,0)--(tmpl,2bp);
  draw (0,0)--(0,2bp);
  begingroup
    interim labeloffset:=3.5bp;
    label.top(thTEX(decimal (0)),(-tmpl,0));
    label.top(thTEX(decimal (l/2)),origin);
    label.top(thTEX(decimal (l) & "\thinspace" & txt),(tmpl,0));
  endgroup
enddef;


def s_hgrid_SM (expr xpos, ypos, xsize, ysize) =
  pickup PenD;
  draw (
    if xpos < 0: 0 else: -5mm fi, 0
  ) -- (
    if xpos > 0: 0 else: 5mm fi, 0
  );
  draw (
    0, if ypos < 0: 0 else: -5mm fi
  ) -- (
    0, if ypos > 0: 0 else: 5mm fi
  );
enddef;


def s_vgrid_SM (expr xpos, ypos, xsize, ysize) =
  pickup PenD;
  draw (
    if xpos < 0: 0 else: -xsize/2 fi, 0
  ) -- (
    if xpos > 0: 0 else: xsize/2 fi, 0
  ) dashed evenly;
enddef;




%%%%% ENDINPUT thSpecial %%%%%






%%%%% INPUT thTrans %%%%%


% default translations

let p_station_fixed = p_station_fixed_ASF;
let p_station_painted = p_station_painted_SKBB;
let p_station_natural = p_station_natural_ASF;              
let p_station_temporary = p_station_temporary_ASF;

let p_waterflow_paleo = p_waterflow_paleo_UIS;
let p_waterflow_permanent = p_waterflow_permanent_UIS;
let p_waterflow_intermittent = p_waterflow_intermittent_UIS;

let p_stalactite = p_stalactite_UIS;
let p_stalagmite = p_stalagmite_UIS;
let p_pillar = p_pillar_UIS;
let p_curtain = p_curtain_UIS;
let p_helictite = p_helictite_UIS;
let p_sodastraw = p_sodastraw_UIS;
let p_crystal = p_crystal_UIS;
let p_flowstone = p_flowstone_UIS;
let p_moonmilk = p_moonmilk_UIS;
let p_wallcalcite = p_wallcalcite_UIS;
let p_popcorn = p_popcorn_UIS;
let p_disk = p_disk_UIS;
let p_gypsum = p_gypsum_NSS;
let p_aragonite = p_aragonite_NSS;
let p_cavepearl = p_cavepearl_SKBB;
let p_gypsumflower = p_gypsumflower_NSS;
let p_rimstonepool = p_rimstonepool_ASF;
let p_rimstonedam = p_rimstonedam_ASF;
let p_anastomosis = p_anastomosis_UIS;
let p_karren = p_karren_UIS;
let p_scallop = p_scallop_UIS;
let p_flute = p_flute_UIS;
let p_raft = p_raft_NSS;
let p_raftcone = p_raftcone_NSS;
let p_spring = p_spring_SKBB;
let p_sink = p_sink_SKBB;
let p_narrowend = p_narrowend_UIS;
let p_lowend = p_lowend_UIS;
let p_flowstonechoke = p_flowstonechoke_NSS;
let p_breakdownchoke = p_breakdownchoke_NSS;
let p_bedrock = p_bedrock_ASF;
let p_clay = p_clay_SKBB;                        
let p_sand = p_sand_UIS;
let p_pebbles = p_pebbles_UIS;
let p_debris = p_debris_UIS;
let p_blocks = p_blocks_UIS;
let p_water = p_water_UIS;
let p_ice = p_ice_UIS;
let p_snow = p_snow_SKBB;
let p_archeomaterial = p_archeomaterial_UIS;
let p_paleomaterial = p_paleomaterial_UIS;
let p_guano = p_guano_UIS;
let p_vegetabledebris = p_vegetabledebris_ASF;
let p_root = p_root_ASF;
let p_entrance = p_entrance_UIS;
let p_gradient = p_gradient_UIS;
let p_rope = p_rope_SKBB;          
let p_fixedladder = p_fixedladder_SKBB;   
let p_ropeladder = p_ropeladder_SKBB;    
let p_steps = p_steps_SKBB;         
let p_bridge = p_bridge_SKBB;        
let p_traverse = p_traverse_SKBB;
let p_anchor = p_anchor_SKBB;
let p_camp = p_camp_SKBB;
let p_dig = p_dig_UIS;
let p_noequipment = p_noequipment_SKBB;
let p_sectionarrow = p_sectionarrow_SKBB;
let p_continuation = p_continuation_UIS;
let p_airdraught = p_airdraught_UIS;
let p_airdraught_winter = p_airdraught_winter_UIS;
let p_airdraught_summer = p_airdraught_summer_UIS;
let p_station = p_station_SKBB;

let l_wall_bedrock = l_wall_bedrock_UIS;
let l_wall_sand = l_wall_sand_SKBB;
let l_wall_clay = l_wall_clay_SKBB;
let l_wall_pebbles = l_wall_pebbles_SKBB;
let l_wall_debris = l_wall_debris_SKBB;
let l_wall_blocks = l_wall_blocks_SKBB;
let l_wall_ice = l_wall_ice_SKBB;
let l_wall_underlying = l_wall_underlying_UIS;
let l_wall_unsurveyed = l_wall_unsurveyed_SKBB;
let l_wall_presumed = l_wall_presumed_UIS;
%let l_wall_invisible = l_wall_invisible;

let l_waterflow_permanent = l_waterflow_permanent_UIS;
let l_waterflow_intermittent = l_waterflow_intermittent_SKBB;
let l_waterflow_conjectural = l_waterflow_conjectural_SKBB;

let l_border_visible = l_border_visible_SKBB;
let l_border_temporary = l_border_temporary_SKBB;
let l_border_presumed = l_border_presumed_SKBB;
%let l_border_invisible = l_border_invisible;

let l_floorstep = l_floorstep_UIS;
let l_pit = l_pit_UIS;
let l_ceilingstep = l_ceilingstep_SKBB;
let l_chimney = l_chimney_UIS;
let l_overhang = l_overhang_SKBB;
let l_slope = l_slope_SKBB;
let l_ceilingmeander = l_ceilingmeander_SKBB;
let l_floormeander = l_floormeander_SKBB;
let l_contour = l_contour_SKBB;
let l_rockborder = l_rockborder_UIS;
let l_rockedge = l_rockedge_UIS;
let l_flowstone = l_flowstone_UIS;
let l_moonmilk = l_moonmilk_UIS;
let l_section = l_section_SKBB;
let l_survey_cave = l_survey_cave_SKBB;
let l_survey_surface = l_survey_surface_SKBB;
let l_arrow = l_arrow_SKBB;
let l_gradient = l_gradient_UIS;
let l_mapconnection = l_mapconnection_SKBB;

let a_water = a_water_UIS;
let a_sump = a_sump_UIS;
let a_sand = a_sand_UIS;
let a_debris = a_debris_SKBB;
let a_blocks = a_blocks_SKBB;
let a_snow = a_snow_SKBB;
let a_ice = a_ice_SKBB;
let a_pebbles = a_pebbles_SKBB;
let a_clay = a_clay_SKBB;
let a_bedrock = a_bedrock_SKBB;
let a_flowstone = a_flowstone_ASF;
let a_moonmilk = a_moonmilk_SKBB;
let a_dimensions = a_dimensions_SKBB;

let s_northarrow = s_northarrow_SKBB;
let s_scalebar = s_scalebar_SKBB;
let s_hgrid = s_hgrid_SM;
let s_vgrid = s_vgrid_SM;



%%%%% ENDINPUT thTrans %%%%%



nonstopmode;
initsymbol("s_hgrid_SM");
initsymbol("s_northarrow_SKBB");
initsymbol("s_scalebar_SKBB");
initsymbol("s_scalebar_SM");
initsymbol("s_vgrid_SM");
initsymbol("p_airdraught_UIS");
initsymbol("p_airdraught_summer_UIS");
initsymbol("p_airdraught_winter_UIS");
initsymbol("p_anastomosis_UIS");
initsymbol("p_anchor_SKBB");
initsymbol("p_aragonite_NSS");
initsymbol("p_archeomaterial_UIS");
initsymbol("p_bedrock_ASF");
initsymbol("p_blocks_UIS");
initsymbol("p_breakdownchoke_NSS");
initsymbol("p_bridge_SKBB");
initsymbol("p_camp_SKBB");
initsymbol("p_cavepearl_SKBB");
initsymbol("p_clay_SKBB");
initsymbol("p_continuation_UIS");
initsymbol("p_crystal_UIS");
initsymbol("p_curtain_UIS");
initsymbol("p_debris_UIS");
initsymbol("p_dig_UIS");
initsymbol("p_disk_UIS");
initsymbol("p_entrance_UIS");
initsymbol("p_fixedladder_SKBB");
initsymbol("p_flowstone_UIS");
initsymbol("p_flowstonechoke_NSS");
initsymbol("p_flute_UIS");
initsymbol("p_gradient_BCRA");
initsymbol("p_gradient_NSS");
initsymbol("p_gradient_UIS");
initsymbol("p_guano_UIS");
initsymbol("p_gypsum_NSS");
initsymbol("p_gypsumflower_NSS");
initsymbol("p_helictite_UIS");
initsymbol("p_ice_UIS");
initsymbol("p_karren_UIS");
initsymbol("p_lowend_NSS");
initsymbol("p_lowend_UIS");
initsymbol("p_moonmilk_UIS");
initsymbol("p_narrowend_UIS");
initsymbol("p_noequipment_SKBB");
initsymbol("p_paleomaterial_UIS");
initsymbol("p_pebbles_UIS");
initsymbol("p_pillar_UIS");
initsymbol("p_popcorn_UIS");
initsymbol("p_raft_NSS");
initsymbol("p_raftcone_NSS");
initsymbol("p_rimstonedam_ASF");
initsymbol("p_rimstonepool_ASF");
initsymbol("p_root_ASF");
initsymbol("p_rope_SKBB");
initsymbol("p_ropeladder_SKBB");
initsymbol("p_sand_UIS");
initsymbol("p_scallop_UIS");
initsymbol("p_sink_SKBB");
initsymbol("p_snow_SKBB");
initsymbol("p_sodastraw_UIS");
initsymbol("p_spring_SKBB");
initsymbol("p_stalactite_UIS");
initsymbol("p_stalagmite_UIS");
initsymbol("p_station_SKBB");
initsymbol("p_station_fixed_ASF");
initsymbol("p_station_natural_ASF");
initsymbol("p_station_painted_SKBB");
initsymbol("p_station_temporary_ASF");
initsymbol("p_station_temporary_SKBB");
initsymbol("p_steps_SKBB");
initsymbol("p_traverse_SKBB");
initsymbol("p_vegetabledebris_ASF");
initsymbol("p_wallcalcite_UIS");
initsymbol("p_water_UIS");
initsymbol("p_waterflow_intermittent_UIS");
initsymbol("p_waterflow_paleo_UIS");
initsymbol("p_waterflow_permanent_UIS");
initsymbol("l_arrow_SKBB");
initsymbol("l_border_presumed_SKBB");
initsymbol("l_border_temporary_SKBB");
initsymbol("l_border_visible_SKBB");
initsymbol("l_ceilingmeander_SKBB");
initsymbol("l_ceilingstep_SKBB");
initsymbol("l_chimney_UIS");
initsymbol("l_contour_SKBB");
initsymbol("l_contour_UIS");
initsymbol("l_floormeander_SKBB");
initsymbol("l_floorstep_UIS");
initsymbol("l_flowstone_UIS");
initsymbol("l_gradient_BCRA");
initsymbol("l_gradient_UIS");
initsymbol("l_mapconnection_SKBB");
initsymbol("l_moonmilk_UIS");
initsymbol("l_overhang_SKBB");
initsymbol("l_pit_UIS");
initsymbol("l_rockborder_UIS");
initsymbol("l_rockedge_UIS");
initsymbol("l_rope_SKBB");
initsymbol("l_section_SKBB");
initsymbol("l_slope_BCRA");
initsymbol("l_slope_SKBB");
initsymbol("l_survey_cave_SKBB");
initsymbol("l_survey_cave_UIS");
initsymbol("l_survey_surface_SKBB");
initsymbol("l_wall_bedrock_UIS");
initsymbol("l_wall_blocks_SKBB");
initsymbol("l_wall_clay_SKBB");
initsymbol("l_wall_debris_SKBB");
initsymbol("l_wall_ice_SKBB");
initsymbol("l_wall_pebbles_SKBB");
initsymbol("l_wall_presumed_UIS");
initsymbol("l_wall_sand_SKBB");
initsymbol("l_wall_underlying_UIS");
initsymbol("l_wall_unsurveyed_SKBB");
initsymbol("l_waterflow_conjectural_SKBB");
initsymbol("l_waterflow_intermittent_SKBB");
initsymbol("l_waterflow_permanent_UIS");
initsymbol("a_bedrock_SKBB");
initsymbol("a_blocks_SKBB");
initsymbol("a_clay_SKBB");
initsymbol("a_debris_SKBB");
initsymbol("a_debris_UIS");
initsymbol("a_dimensions_SKBB");
initsymbol("a_flowstone_ASF");
initsymbol("a_ice_SKBB");
initsymbol("a_moonmilk_SKBB");
initsymbol("a_pebbles_SKBB");
initsymbol("a_sand_UIS");
initsymbol("a_snow_SKBB");
initsymbol("a_sump_UIS");
initsymbol("a_water_UIS");

lang:="en";

% user defined symbols defaults
% end of user defined symbols

defaultfont:="cmss10";
transparency:=true;
background:=(1.00000,1.00000,1.00000);
def_transparent_rgb(tr_bg, 1.00000, 1.00000, 1.00000);
warningcheck := 1;
current_scrap := "scrap.types@";
ATTR__scrap := "scrap.types";
beginfig(1);
current_src := "types.th2 [190]";
string area_border[];
area_border[1] := "l18-413-242";
area_border[2] := "l6-342-1086";
area_border[3] := "l19-409-159";
area_border[4] := "l7-497-1096";
a_ice(buildcycle(((-35.27,-148.11)
 .. controls (-18.21,-149.57) and (28.60,-149.62) .. (38.11,-148.36)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-36.61,-181.04)
 .. controls (-23.52,-182.13) and (31.21,-181.33) .. (39.14,-179.68)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [183]";
string area_border[];
area_border[1] := "l19-409-159";
area_border[2] := "l6-342-1086";
area_border[3] := "l46-411-69";
area_border[4] := "l7-497-1096";
a_snow(buildcycle(((-36.61,-181.04)
 .. controls (-23.52,-182.13) and (31.21,-181.33) .. (39.14,-179.68)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-36.75,-216.34)
 .. controls (-19.29,-217.01) and (29.90,-217.83) .. (40.20,-215.77)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [168]";
string area_border[];
area_border[1] := "l15-428-512";
area_border[2] := "l6-342-1086";
area_border[3] := "l16-411-424";
area_border[4] := "l7-497-1096";
a_bedrock(buildcycle(((-35.27,-41.41)
 .. controls (-15.83,-42.46) and (30.16,-40.53) .. (39.66,-37.68)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-32.24,-76.30)
 .. controls (-15.96,-78.16) and (35.18,-75.40) .. (41.91,-73.36)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [161]";
string area_border[];
area_border[1] := "l16-411-424";
area_border[2] := "l6-342-1086";
area_border[3] := "l17-420-327";
area_border[4] := "l7-497-1096";
a_flowstone(buildcycle(((-32.24,-76.30)
 .. controls (-15.96,-78.16) and (35.18,-75.40) .. (41.91,-73.36)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-33.15,-113.59)
 .. controls (-6.56,-115.77) and (30.32,-114.70) .. (40.22,-112.64)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [152]";
string area_border[];
area_border[1] := "l17-420-327";
area_border[2] := "l6-342-1086";
area_border[3] := "l18-413-242";
area_border[4] := "l7-497-1096";
area_border[5] := "l7-497-1096";
area_border[6] := "l7-497-1096";
a_moonmilk(buildcycle(((-33.15,-113.59)
 .. controls (-6.56,-115.77) and (30.32,-114.70) .. (40.22,-112.64)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-35.27,-148.11)
 .. controls (-18.21,-149.57) and (28.60,-149.62) .. (38.11,-148.36)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [82]";
string area_border[];
area_border[1] := "l8-427-1151";
area_border[2] := "l6-342-1086";
area_border[3] := "l9-429-1051";
area_border[4] := "l7-497-1096";
a_water(buildcycle(((-35.94,206.88)
 .. controls (-19.72,212.15) and (23.09,216.44) .. (40.20,209.03)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-35.25,167.61)
 .. controls (-17.04,172.11) and (23.40,174.80) .. (38.11,170.54)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [75]";
string area_border[];
area_border[1] := "l9-429-1051";
area_border[2] := "l6-342-1086";
area_border[3] := "l10-429-957";
area_border[4] := "l7-497-1096";
a_sump(buildcycle(((-35.25,167.61)
 .. controls (-17.04,172.11) and (23.40,174.80) .. (38.11,170.54)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-36.21,137.07)
 .. controls (-18.73,133.23) and (30.04,135.18) .. (37.55,138.41)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [68]";
string area_border[];
area_border[1] := "l10-429-957";
area_border[2] := "l6-342-1086";
area_border[3] := "l11-426-861";
area_border[4] := "l7-497-1096";
a_clay(buildcycle(((-36.21,137.07)
 .. controls (-18.73,133.23) and (30.04,135.18) .. (37.55,138.41)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-39.90,100.15)
 .. controls (-23.22,97.10) and (27.56,95.50) .. (35.48,97.54)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [61]";
string area_border[];
area_border[1] := "l11-426-861";
area_border[2] := "l6-342-1086";
area_border[3] := "l12-412-775";
area_border[4] := "l7-497-1096";
a_sand(buildcycle(((-39.90,100.15)
 .. controls (-23.22,97.10) and (27.56,95.50) .. (35.48,97.54)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-39.24,64.46)
 .. controls (-18.60,62.23) and (23.05,62.54) .. (36.13,63.44)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [54]";
string area_border[];
area_border[1] := "l12-412-775";
area_border[2] := "l6-342-1086";
area_border[3] := "l13-424-689";
area_border[4] := "l7-497-1096";
a_pebbles(buildcycle(((-39.24,64.46)
 .. controls (-18.60,62.23) and (23.05,62.54) .. (36.13,63.44)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-42.16,30.33)
 .. controls (-11.20,27.78) and (30.84,28.49) .. (35.99,29.72)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [47]";
string area_border[];
area_border[1] := "l13-424-689";
area_border[2] := "l6-342-1086";
area_border[3] := "l14-414-607";
area_border[4] := "l7-497-1096";
a_debris(buildcycle(((-42.16,30.33)
 .. controls (-11.20,27.78) and (30.84,28.49) .. (35.99,29.72)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-39.12,-4.56)
 .. controls (-22.85,-5.23) and (29.49,-4.04) .. (37.41,-2.00)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
current_src := "types.th2 [40]";
string area_border[];
area_border[1] := "l14-414-607";
area_border[2] := "l6-342-1086";
area_border[3] := "l15-428-512";
area_border[4] := "l7-497-1096";
a_blocks(buildcycle(((-39.12,-4.56)
 .. controls (-22.85,-5.23) and (29.49,-4.04) .. (37.41,-2.00)
),
((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
),
((-35.27,-41.41)
 .. controls (-15.83,-42.46) and (30.16,-40.53) .. (39.66,-37.68)
),
((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
)));
ATTR__id := "l8-427-1151";
l_border_visible(((-35.94,206.88)
 .. controls (-19.72,212.15) and (23.09,216.44) .. (40.20,209.03)
));
ATTR__id := "l9-429-1051";
l_border_visible(((-35.25,167.61)
 .. controls (-17.04,172.11) and (23.40,174.80) .. (38.11,170.54)
));
ATTR__id := "l10-429-957";
l_border_visible(((-36.21,137.07)
 .. controls (-18.73,133.23) and (30.04,135.18) .. (37.55,138.41)
));
ATTR__id := "l11-426-861";
l_border_visible(((-39.90,100.15)
 .. controls (-23.22,97.10) and (27.56,95.50) .. (35.48,97.54)
));
ATTR__id := "l12-412-775";
l_border_visible(((-39.24,64.46)
 .. controls (-18.60,62.23) and (23.05,62.54) .. (36.13,63.44)
));
ATTR__id := "l13-424-689";
l_border_visible(((-42.16,30.33)
 .. controls (-11.20,27.78) and (30.84,28.49) .. (35.99,29.72)
));
ATTR__id := "l14-414-607";
l_border_visible(((-39.12,-4.56)
 .. controls (-22.85,-5.23) and (29.49,-4.04) .. (37.41,-2.00)
));
ATTR__id := "l15-428-512";
l_border_visible(((-35.27,-41.41)
 .. controls (-15.83,-42.46) and (30.16,-40.53) .. (39.66,-37.68)
));
ATTR__id := "l16-411-424";
l_border_visible(((-32.24,-76.30)
 .. controls (-15.96,-78.16) and (35.18,-75.40) .. (41.91,-73.36)
));
ATTR__id := "l17-420-327";
l_border_visible(((-33.15,-113.59)
 .. controls (-6.56,-115.77) and (30.32,-114.70) .. (40.22,-112.64)
));
ATTR__id := "l18-413-242";
l_border_visible(((-35.27,-148.11)
 .. controls (-18.21,-149.57) and (28.60,-149.62) .. (38.11,-148.36)
));
ATTR__id := "l19-409-159";
l_border_visible(((-36.61,-181.04)
 .. controls (-23.52,-182.13) and (31.21,-181.33) .. (39.14,-179.68)
));
ATTR__id := "l46-411-69";
l_border_visible(((-36.75,-216.34)
 .. controls (-19.29,-217.01) and (29.90,-217.83) .. (40.20,-215.77)
));
endfig;
beginfig(2);
draw_upscrap(1)(1,((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
),
1,reverse ((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
));
draw_downscrap;
endfig;
beginfig(3);
ATTR__id := "l6-342-1086";
l_wall_bedrock(((-28.35,-225.80)
 .. controls (-35.33,-193.72) and (-19.55,-75.41) .. (-30.56,-35.43)
 .. controls (-46.29,21.70) and (-23.71,163.34) .. (-34.84,219.58)
));
ATTR__id := "l7-497-1096";
l_wall_bedrock(((36.29,-223.73)
 .. controls (25.24,-177.41) and (46.29,-75.31) .. (32.89,-33.77)
 .. controls (19.49,7.78) and (38.59,158.65) .. (31.77,222.85)
));
endfig;
beginfig(4);
ATTR__id := "";
p_label.top(btex \thlabel\thhugesize \thfb\char84 \char104 \char101 \char114 \char105 \char111 \char110 \ \char97 \char114 \char101 \char97 \ \char116 \char121 \char112 \char101 \char115 \mainfont{} etex,(21.79,231.50),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char98 \char101 \char100 \char114 \char111 \char99 \char107 \mainfont{} etex,(43.37,-56.69),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char102 \char108 \char111 \char119 \char115 \char116 \char111 \char110 \char101 \mainfont{} etex,(40.89,-95.58),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char109 \char111 \char111 \char110 \char109 \char105 \char108 \char107 \mainfont{} etex,(38.40,-133.68),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char105 \char99 \char101 \mainfont{} etex,(37.05,-165.42),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char115 \char110 \char111 \char119 \mainfont{} etex,(37.71,-201.11),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char119 \char97 \char116 \char101 \char114 \mainfont{} etex,(39.55,189.59),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char115 \char117 \char109 \char112 \mainfont{} etex,(39.03,153.49),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char99 \char108 \char97 \char121 \mainfont{} etex,(36.92,116.99),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char115 \char97 \char110 \char100 \mainfont{} etex,(35.61,80.09),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char112 \char101 \char98 \char98 \char108 \char101 \char115 \mainfont{} etex,(34.29,43.99),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char100 \char101 \char98 \char114 \char105 \char115 \mainfont{} etex,(34.14,11.46),0.0,0);
p_label.rt(btex \thlabel\thnormalsize \thfb\char98 \char108 \char111 \char99 \char107 \char115 \mainfont{} etex,(37.95,-21.43),0.0,0);
endfig;
beginfig(5);
s_northarrow(0);
endfig;
beginfig(6);
s_scalebar(10, 1, "\thfb\char109 \mainfont{}");
endfig;
background:=(1.00000,1.00000,1.00000);
transparency:=false;
end;
