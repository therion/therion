%% therion source code
%%
%% thText -- therion project file of type metapost symbol library
%%
%% This file defines macros for writing labels
%%
%% $Date: 2002/07/30 15:10:22 $
%% $RCSfile: thText.mp,v $
%% $Revision: 1.3 $
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

% @LIBRARY
% thText --
%
% This file provides two kinds of high-level macros. In both are
% labels typeset by MetaPost, not by TeX. This implies some
% restrictions. Macro for writing texts along a path accepts
% as text argument only simple string, so you can't use superscripts etc.
% Second type of macros are for simple -- optionally boxed or circled --
% labels (used e.g. as passage height symbol). Text argument 
% should be a string containing no spaces and at most 5 characters long.
% For other types of labels, it's better, when
% Therion constructs them from more primitive commands. (They are
% typeset by TeX and in MetaPost's
% <I>btex ... etex</I> construction cannot be inserted string
% variable; one tricky solution of this problem in TEX.mp file from 
% standard MetaPost distribution works rather slow.)
% <P>
% General syntax for label typesetting (done by TeX) is:
% <PRE>
%   lab:=btex ... etex;    
%   lab:=thelabel.suffix(lab, position);
%   process_label(position,rot);
% </PRE>
%
% where <I>...</I> means text of your label (you can use all Plain TeX commands
% in it), <I>lab</I> is predefined
% picture variable, <I>suffix</I> is one of standard
% MetaPost's suffixes for label centering, <I>position</I> is
% pair of coordinates and <I>rot</I> is rotation angle.<BR><BR>
% Result is always rectangular label.


def thFreetext(expr Path,Style,String) =
  picture pict[];
  pair zz[];
  string Font;
  Font := defaultfont;
  if Style = 1:      % normal
    txtScale:=2; 
  elseif Style = 2:  % big
    txtScale:=3;
  else:              % small
    txtScale:=1;
  fi;
  l := length (String);				% No. of characters
  pict[0] := (String) infont (Font) scaled (txtScale);
  zz[0] := (lrcorner pict[0]) - (llcorner pict[0]);
  strl := xpart zz[0];				% real length 
  pathl := arclength (Path);
  if strl>pathl:
    txtScale := (pathl/strl) * txtScale;
    delta := 0;
    message("[Warning: text scaled down to fit the leading path]");
  else:
    delta := (pathl - strl) / (l - 1);			% distance between chars
  fi;
  cas := 0;
  for i=0 upto (l - 1):
    ch := substring(i,i+1) of (String);
    pict[0] := ch infont (Font) scaled (txtScale);
    zz[0] := (lrcorner pict[0]) - (llcorner pict[0]);	    
    charwidth := xpart zz[0];			% character width
    if ASCII (ch) <> 32: 
      zz[1] := lrcorner pict[0];
      addto pict[0] also (strut_string infont Font scaled txtScale);	% like TeX's $\backslash$ strut
      zz[2] := llcorner pict[0];
      zz[3] := ulcorner pict[0];
      pict[1] := ch infont Font scaled txtScale;
      setbounds pict[1] to (zz[2]--(xpart zz[1],ypart zz[2])--
        (xpart zz[1],ypart zz[3])--zz[3]--cycle);
      cas := cas + (charwidth / 2);
      t := arctime cas of (Path);			% moving along path
      lab:=thelabel(pict[1],point t of (Path));
      process_label(point t of (Path),angle direction t of (Path));
    else:
      cas := cas + (charwidth / 2);
    fi;
    cas := cas + (charwidth / 2) + delta; 
  endfor;
enddef;

% text argumen must be encolsed in btex ... etex or "..."

vardef thLabel@# (expr txt,pos,rot) =
  lab:=thelabel@#(txt, pos);
  process_label(pos,rot);
enddef;

vardef thSmalllabel@#(expr txt,pos) =
  begingroup
    interim defaultscale:=.8;
    thLabel@#(txt,pos,0);
  endgroup;
enddef;


vardef thUplabel@# (expr txt, pos) =
  if length txt <= 5:
    interim defaultscale:=.8;
    pickup PenD;
    lab:=thelabel@#(txt, pos);
    process_uplabel;
  fi
enddef;


vardef thDownlabel@# (expr txt, pos) =
  if length txt <= 5:
    interim defaultscale:=.8;
    pickup PenD;
    lab:=thelabel@#(txt, pos);
    process_downlabel;
  fi
enddef;

% alignment is ignored, now

vardef thUpDownlabel@# (expr num, den, pos) =
  if (length num <= 5) and (length den <=5):
    pickup PenD;
    begingroup interim labeloffset:=2bp;
      interim defaultscale:=.8;
      lab:=thelabel.top(num, pos);
      Lab:=thelabel.bot(den, pos);
      process_updownlabel;
    endgroup;
  fi
enddef;


vardef thCircledlabel@# (expr txt, pos) =
  if length txt <= 5:
    begingroup interim bboxmargin := 1bp;
      interim defaultscale:=.8;
      pickup PenD;
      lab:=thelabel@#(txt, pos);
      process_circledlabel;
    endgroup;
  fi
enddef;


vardef thBoxedlabel@# (expr txt, pos) =
  if length txt <= 5:
    interim defaultscale:=.8;
    pickup PenD;
    lab:=thelabel@#(txt, pos);
    process_boxedlabel;
  fi
enddef;


def thASLlabel (expr pprev,pos,pnext,txt) =
  %rot:=(angle(pprev-pos)+angle(pos-pnext))/2 + 90;
  rot:=angle(unitvector(pnext-pos)+unitvector(pos-pprev)) - 90;
  pickup PenD;
  pair zz;
  zz:=(pos + 3*unitvector(dir rot));
  draw (pos)--zz;
  begingroup 
    interim labeloffset:=2pt;
    interim defaultscale:=.8;
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


vardef thAltitudelabel@#(expr txt,pos) =
  if length txt <= 10:
    pickup pencircle scaled (u/6);
    drawdot(pos);
    interim defaultscale:=0.8;
    lab:=thelabel@#(txt, pos);
    process_label(pos,0);
  fi
enddef;

vardef ElevDiff@#(expr txt,pos) =
  if length txt <= 10:
    interim defaultscale:=.8;
    lab:=thelabel@#(txt, pos);
    process_label(pos,0);
  fi
enddef;

vardef ElevDiffPos@#(expr txt,pos) =
  if length txt <= 10:
    interim defaultscale:=.8;
    lab:=thelabel@#(diff_pos&txt, pos);
    process_label(pos,0);
  fi
enddef;

vardef ElevDiffNeg@#(expr txt,pos) =
  if length txt <= 10:
    interim defaultscale:=.8;
    lab:=thelabel@#(diff_neg&txt, pos);
    process_label(pos,0);
  fi
enddef;

