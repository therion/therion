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

