%% therion source code
%%
%% thRandom -- therion project file of type metapost symbol library
%%
%% This file defines macros for random fills
%%
%% $Date: 2002/07/30 15:10:22 $
%% $RCSfile: thRandom.mp,v $
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
% thRandom --

% @SYMBOL
% thRandomsand_UIS --
%
% @PREVIEW
% 
% thRandomsand_UIS((.5,.5) inscale, 20, 20, 10, 30, 0);

def thRandomsand_UIS (expr center, theta, A, B, Max, rmode) =
    T:=identity rotated theta shifted center;
    pickup PenD;
		thdraw superellipse((A,0),(0,B),(-A,0),(0,-B),.707) 
		       dashed evenly withcolor red;
%    pair zz;
%    for i=1 upto Max:		% $\vert x \vert < 2$ for 95 \% of $x$
%        if rmode = 0:
%            zz:=(normaldeviate * A/4 ,normaldeviate * B/4)  
%        else:
%            zz:=(uniformdeviate A, uniformdeviate B) shifted (-A/2,-B/2)
%        fi rotated theta;
%        thdraw zz; 
%    endfor;		            % drawdot consumes a lot of space
enddef;

% @SYMBOL
% thRandomdebris_SKBB --
%
% @PREVIEW
% thRandomdebris_SKBB((.5,.5) inscale, 20, 30, 20, 20, 0, 0);

def thRandomdebris_SKBB (expr center, theta, A, B, Max, orient, rmode) =
    T:=identity shifted center;
    pickup PenC;
    pair zz;
    for i=1 upto Max:
        if rmode = 0:
            zz:=(normaldeviate * A/4 ,normaldeviate * B/4)  
        else:
            zz:=(uniformdeviate A, uniformdeviate B) shifted (-A/2,-B/2)
        fi rotated theta;
        if orient = 0:
            rot := uniformdeviate(360)
        elseif orient = 1:
            rot := angle(zz)
        else:
            rot := 0
        fi;
        thdraw (zz + (.2u,0) rotated rot) -- (zz - (.2u,0) rotated rot);
    endfor;
enddef;

