%% therion source code
%%
%% thArea -- therion project file of type metapost symbol library
%%
%% This file defines macros for plane (filled) figures
%%
%% $Date: 2002/07/30 15:10:22 $
%% $RCSfile: thPlane.mp,v $
%% $Revision: 1.2 $
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
% thPlane --
%
% This library contains definitions of plane objects


def_transparent_rgb(tr_white, 1, 1, 1);
def_transparent_rgb(tr_black, 0, 0, 0);

% @SYMBOL
% thPatternFill --
%
% @LEGEND
% p := ((.1,.4)..(.6,.2)..(.8,.6)..(.45,.7){dir 130}..cycle) inscale;
% thPatternFill(p, thSandPattern_UIS);
% @ENDSYMBOL
% @USE thSandPattern_UIS

def thPatternFill (expr Path, Pattern) =
  T:=identity;
  thfill Path withtransparentcolor tr_white;
  thfill Path withpattern Pattern;
enddef;
