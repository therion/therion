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

vardef thdir (expr p,t) = 
  ((direction t-epsilon of p) + (direction t+epsilon of p)) / 2
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

def l_wall_clay_SKBB (expr P) = 
  T:=identity;
  thUndefinedPath(P);
enddef;

def l_wall_pebbles_SKBB (expr P) = 
  T:=identity;
  thUndefinedPath(P);
enddef;

def l_wall_debris_SKBB (expr P) = 
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, .35u);
  pickup PenC;
  q := ((-.1u,-.15u)--(.2,.03u)--(-.2u,.15u)--cycle) scaled 1.5;
  forever:
    t := arctime (cas + mojkrok/2) of P;
    thdraw q randomized .2mm rotated uniformdeviate (360) shifted point t of P;
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
    thdraw q randomized 1mm;
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
fi;
enddef;

def l_wall_ice_SKBB (expr P) = 
  T:=identity;
  thUndefinedPath(P);
enddef;

def l_wall_underlying_UIS (expr P) = 
  T:=identity;
  pickup PenA;
  thdraw P dashed evenly;
enddef;

def l_wall_unsurveyed_SKBB (expr P) = 
  T:=identity;
  pickup PenC;
  thdraw P;
enddef;

def l_wall_presumed_UIS (expr P) = 
  T:=identity;
  pickup PenA;
  thdraw P dashed evenly scaled 2;
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
  thdraw P dashed evenly;
enddef;

def l_chimney_SKBB (expr P) =
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

def l_slope_UIS (expr P, showline, widthA, widthB) = 
  T:=identity;
  boolean par;
  cas := 0;
  krok := 0.7u;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka,1.4u) / 2;
  pickup PenD;
  par := false; 
  forever:
    t := arctime cas of P;
    width:=(cas/dlzka)[widthA,widthB];
%   show width;
    if par: mark_(P,t,.33*width) 
    else: mark_(P,t,width) fi;
    cas := cas + mojkrok;
    par := not par;
    exitif cas > dlzka + (krok / 3);  % for rounding errors
  endfor;
  if showline = 1: pickup PenC; draw P fi;
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
        dirs[i]:=(90-dirs[i]) mod 360; 
        alw_perpendicular:=false;
      fi;
    fi;
    if unknown lengths[i]: lengths[i]:=-1; fi;
  endfor;

% for i=0 upto li: show dirs[i]; endfor;

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

% for i=0 upto li: show dirs[i]; endfor;

  ni:=0; % next
  pi:=0; % previous

  for i=0 upto li:
    l:=lengths[i];
    if l=-1:
      if (i=0) or (i=li):
        lengths[i] := 1cm; % should never happen!
	message("Warning: slope width at the end point not specified!");
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
        a := angle(thdir(P,t))+90;
      else:
        a := w[dirs[floor t],dirs[ceiling t]];
      fi;
      l := w[lengths[floor t],lengths[ceiling t]];
    else:
      if alw_perpendicular:
        a := angle(thdir(P,t))+90;
      else:
        a:= dirs[t];
      fi; 
      l:=lengths[t];
    fi;
%    if (a-angle(thdir(P,t))) mod 360>180:
%      a:=a-180;
%    fi;
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


def l_contour_UIS(expr P)(text txt) =      % osetrit Reverse!
  T:=identity;
  pickup PenD;
  thdraw P;
  for pnt=txt:
    if pnt=-1:
      mark_(P,arctime(arclength(P)/2) of P, 0.2u);
    else:
      mark_(P,pnt,0.2*u);
    fi;
    exitif pnt=-1;
  endfor;
enddef;


def l_rockborder_UIS (expr P) = 
  T:=identity;
  pickup PenC;
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
  draw Path dashed evenly;
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


PolygonLine:=.8u;
%PolygonPoint:=0.25u;

def thPolygon_SKBB (expr P) =
  T:=identity;
  pair zz[];
  pickup PenC;
  for t = 0 upto length P - 1:
    zz1 := point t of P;
    zz2 := point t+1 of P;
    if length (zz2-zz1) > 2*PolygonLine:
      thdraw zz1 -- zz1 + PolygonLine * unitvector(zz2 - zz1);
      thdraw zz2 -- zz2 + PolygonLine * unitvector(zz1 - zz2);
    else:
      thdraw zz1 -- zz2;
    fi;
%    p := fullcircle scaled PolygonPoint shifted (zz1);
%    q := fullcircle scaled PolygonPoint shifted (zz2);
%    thunfill p;
%    thunfill q;
%    thdraw p;
%    thdraw q;
  endfor;
enddef;

def thPolygon_JSSJ (expr P) =
  T:=identity;
  pair zz[];
  pickup PenC;
  for t = 0 upto length P - 1:
    zz1 := point t of P;
    zz2 := point t+1 of P;
    draw zz1 -- zz2;
%    p := fullcircle scaled PolygonPoint shifted (zz1);
%    q := fullcircle scaled PolygonPoint shifted (zz2);
%    thunfill p;
%    thunfill q;
%    thdraw p;
%    thdraw q;
  endfor;
enddef;


def ShiftedPath_A (expr Path, Dist, Az) =
    path ppp,pp[];
    pair Dir[];
    for t = 0 upto length Path - 1:
        pair zz[];
        Dir1 := dir((angle(direction t-epsilon of Path) + 
                     angle(direction t+epsilon of Path)) / 2);
        Dir2 := dir((angle(direction t+1-epsilon of Path) + 
                     angle(direction t+1+epsilon of Path)) / 2);                 
        zooma := 1 / (cosd(angle(direction t+epsilon of Path) - angle(Dir1)));
        zoomb := 1 / (cosd(angle(direction t+1-epsilon of Path) - angle(Dir2)));
        zz1 := point t of Path;    zz2 := point t+1 of Path;
        zz3 := postcontrol t of Path;    zz4 := precontrol t+1 of Path;
        if abs(angle(Dir1) - angle(Dir2)) > 10*epsilon:

            zz0 = (zz1 + whatever * (Dir1 rotated 90)) 
                = (zz2 + whatever * (Dir2 rotated 90));
            zz5 := zz1 + (Dist * zooma * unitvector(Dir1 rotated Az));
            zz6 := zz2 + (Dist * zoomb * unitvector(Dir2 rotated Az));
            zz7 = zz5 + whatever * (zz3-zz1) = zz0 + whatever * (zz3-zz0);
            zz8 = zz6 + whatever * (zz4-zz2) = zz0 + whatever * (zz4-zz0);
%            for i = 0: drawdot zz[i]; endfor;
%            draw zz1--zz0--zz2;
        else:
            def Shift = (Dist * unitvector(Dir1 rotated Az)) enddef;
            zz5 := zz1 + Shift;
            zz6 := zz2 + Shift;
            zz7 := zz3 + Shift;
            zz8 := zz4 + Shift;
        fi;
        pp[t] := zz5 .. controls zz7 and zz8 .. zz6;
    endfor;
    ppp := pp[0];
    for t = 1 upto length Path - 1:
        ppp := ppp -- pp[t];
    endfor;
enddef;

def DoublePit_A (expr Path, Dist) =
    ShiftedPath (Path, Dist/2, -90);
    Pit(ppp);
    ShiftedPath (Path, Dist/2, 90);
    Pit(reverse ppp);
enddef;

def l_water_permanent_UIS (expr Path,Rnd) =
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
    drawarrow ppp;
enddef;

def l_water_intermittent_JSSJ (expr Path,Rnd) =
  drawoptions(dashed evenly);
  thPermanentwater_UIS (Path,Rnd);
  drawoptions();
enddef;

def l_water_conjectural_SKBB (expr Path,Rnd) =
  drawoptions(dashed withdots scaled .5 withpen PenB);
  thPermanentwater_UIS (Path,Rnd);
  drawoptions();
enddef;

def l_invisible (expr P) = 
enddef;

def l_undefined (expr P) = 
  T:=identity;
  pickup PenC;
  thdraw P withcolor red;
  message("[Warning: undefined line symbol used]");
enddef;

def NoJoinPath expr P =
  T:=identity;
  pickup PenC;
  thdraw P withcolor blue;
enddef;

def NoMorphPath expr P =
  T:=identity;
  pickup PenC;
  thdraw P withcolor blue dashed evenly;
enddef;

% Q = 0 -- no arrows
%     1 -- end
%     2 -- begin
%     3 -- both

def ArrowPath (expr P, Q) =
  T:=identity;
  pickup PenC;
  thdraw P;
  p := (-.1u,-.25u)--(0,0)--(.1u,-.25u);
  if odd Q:
    draw p rotated (angle(direction 0 of P)+90) shifted (point 0 of P);
  fi;
  if Q>1:
    draw p rotated (angle(direction infinity of P)-90) 
         shifted (point infinity of P);
  fi;
enddef;

boolean drawsection;

%% len demo, treba prerobit!

def thSectionPath_SKBB (expr P) =
  T:=identity;
  pair zz[];
  drawsection:=true;
  pickup PenC;
  for t = 0 upto length P - 1:
    zz1 = point t of P;
    zz2 = point t+1 of P;
    zz3 = postcontrol t of P;
    zz4 = precontrol t+1 of P;
    if length (zz1-zz3) > epsilon:
      zz5 = whatever[zz1,zz2];
      (zz3-zz5) dotprod (zz1-zz2) = 0;
      draw zz1--zz5;
      drawsection:=false;
    fi;
    if length (zz2-zz4) > epsilon:
      zz6 = whatever[zz1,zz2];
      (zz4-zz6) dotprod (zz1-zz2) = 0;
      draw zz2--zz6;
      drawsection:=false;
    fi;
    if drawsection:
      draw zz1--zz2;
    fi;
  endfor;
enddef;

let l_border_invisible = l_invisible;
let l_wall_invisible = l_invisible;
