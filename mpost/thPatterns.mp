%% therion source code
%%
%% thPatterns -- therion project file of type metapost symbol library
%%
%% This file defines macros for pattern fills
%%
%% $Date: 2002/07/30 15:10:22 $
%% $RCSfile: thPatterns.mp,v $
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

if unknown thLakePattern_UIS: else: endinput fi;

% @LIBRARY
% thPatterns --
%
% This file contains definitions of pattern fills. Use patterns with
% thfill command instead of fill. Before use, set <T>T:=identity</T>.
% (In the definition of pattern you are allowed to use MetaPost's
% draw, fill etc.)


% @SYMBOL
% thLakePattern_UIS --
%
% @PREVIEW
%
% fill ((.1,.4)..(.6,.2)..(.8,.4)..(.45,.7){dir 160}..cycle) inscale withpattern thLakePattern_UIS;

beginpattern(thLakePattern_UIS);
    draw origin--10up withpen pensquare scaled (0.02u);
    patternxstep(.18u);
    patterntransform(identity rotated 45);
endpattern;

% @SYMBOL
% thSumpPattern_UIS --
%
% @PREVIEW
%
% fill ((.1,.4)..(.6,.2)..(.8,.4)..(.45,.7){dir 160}..cycle) inscale withpattern thSumpPattern_UIS;

beginpattern(thSumpPattern_UIS);
    draw origin--(0,.25u) withpen pensquare scaled (0.02u);
    draw origin--(.25u,0) withpen pensquare scaled (0.02u);
    patterntransform(identity rotated 45);
endpattern;

% @SYMBOL
% thSandPattern_UIS --
%
% @PREVIEW
%
% fill ((.1,.4)..(.6,.2)..(.8,.6)..(.45,.7){dir 130}..cycle) inscale withpattern thSandPattern_UIS;


beginpattern(thSandPattern_UIS);
    pickup PenC;
    draw (0.2u,.3u);
    draw (0.35u,.7u);
    draw (0.6u,.5u);
    draw (0.8u,.1u);    
endpattern;

% @SYMBOL
% thDebrisPattern_SKBB --
%
% @PREVIEW
%
% fill ((.05,.4)..(.6,.1)..(.9,.6)..(.45,.9){dir 100}..cycle) inscale withpattern thDebrisPattern_SKBB;
% @ENDSYMBOL

beginpattern(thDebrisPattern_SKBB);
    pickup PenC;
    p:=(-.15u,0)--(.15u,0);
    draw p rotated -40 shifted (.2u,.3u);
    draw p rotated 70 shifted (.9u,1.5u);
    draw p rotated 20 shifted (1.5u,.8u);
%    draw p rotated 95 shifted (.9u,.8u);
endpattern;

