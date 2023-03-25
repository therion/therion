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
      label(if (lang="sk") or (lang="cs"): "S" else: "N" fi, origin);
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

def s_altitudebar_SKBB (expr zmin, zmax, units)(text t)(text labels) =
  begingroup;
    ncol := 0;  % number of colors
    for i=t:
      ncol := ncol+1;
    endfor;
    ncol := ncol / 2;  % t contains pairs
    j := 0;
    string s;
    hs := 36 bp;
    vs := ncol*25*bp + (ncol-1)*pt - 1bp;   % simulate the height of the discrete legend
    prevpos := -1;
    for i = t:
      if not odd j:
        k := (1-i)*vs;
      else:
        if prevpos >= 0:
          p := (0,prevpos)--(hs,prevpos)--(hs,k)--(0,k)--cycle;
          fill p withalpha -2 withlinearshading(p, 2.5, tmpcol, i);
        fi;
        tmpcol := i;
        prevpos := k;
      fi;
      j := j+1;
    endfor;
    % check if the labels parameter contains anything useful
    ii := 0;
    for i=labels:
      ii := ii+1;
    endfor;
    interim labeloffset := 15 bp;
    interim linecap := butt;
    if ii>1: % custom labeling
      j := 0;
      for i=labels:
        if not odd j:
          k := (1-i)*vs;
        else:
          draw (hs, k) -- (hs+5bp, k) withpen PenD;
          label.rt(thTEX(fixminus(i)), (hs, k));
        fi;
        j := j+1;
      endfor;
    else:   % calculated labeling; breakpoints for the (zmax-zmin) difference: 1*10^n, 2*10^n, 5*10^n
      d := zmax - zmin;
      dlog := log(d);
      if dlog mod 1 < 0.301: dv := 2;
      elseif dlog mod 1 < 0.7: dv := 5;
      else: dv := 10;
      fi;
      dv := floor(dv*(10**(floor(dlog-1))));
      if (dv > 0) or (floor zmax - floor zmin >= 2):
        if dv > 0:    % zmax-zmin >= 10
          i := dv * floor(zmin/dv);
          if i < zmin: i := i + dv; fi;
        else:         % at least two whole numbers between zmin and zmax
          i := floor(zmin);
          if i < zmin: i := i + 1; fi;
          dv := 1;
        fi;
        forever:
          exitif i > zmax;
          j := (i-zmin)/d*vs;
          draw (hs, j) -- (hs+5bp, j) withpen PenD;
          label.rt(thTEX(fixminus(decimal round i) & "\thinspace " & units), (hs, j));
          i := i + dv;
        endfor;
      else:
        draw (0, 0) -- (hs+5bp, 0) withpen PenD;
        draw (0, vs) -- (hs+5bp, vs) withpen PenD;
        label.rt(thTEX(fixminus(decimal(round(10*zmin)/10)) & "\thinspace " & units),(hs,0));
        label.rt(thTEX(fixminus(decimal(round(10*zmax)/10)) & "\thinspace " & units),(hs,vs));
      fi;
    fi;
  endgroup;
enddef;
