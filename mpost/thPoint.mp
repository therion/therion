%% therion source code
%%
%% This file defines macros for point map symbols
%%
%% $Date: 2003/07/01 09:06:44 $
%% $RCSfile: thPoint.mp,v $
%% $Revision: 1.8 $
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
% thPoint --
%
% All point symbols are drawn in own relative coordinates. Recommended
% ranges are <-.5u,.5u> in both axes. For drawing and filling
% use <I>thdraw</I> and <I>thfill</I>
% commands instead of MetaPost's <I>draw</I> and <I>fill</I> equivalents.
% These commands draw paths transformed as specified in <I>T</I> variable.
% (Before drawing is necessary to set this variable according actual parameters
% <I>theta</I> for rotation, <I>pos</I> for shift and 
% <I>sc</I> for scaling.
% If you need other drawing commands (like <I>drawdot</I>), it's 
% necessary to define them (take definition of <I>thdraw</I> as an example). 


% @DESCRIPTION
% 
% <H3>Symbols</H3> argument to point symbol signatures is position, rotation
% and scale.
% <BR><H4>Speleothems:</H4>

% @SYMBOL
% thStalactite_UIS --
%
% @PREVIEW
%
% thStalactite_UIS((.5,.5) inscale,0,1);

pair U;

def p_stalactite_UIS(expr pos,theta,sc,al) =
    U:=(.15u, .4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,-.4u) -- (0,.15u) -- (-.15u,.4u);
    thdraw (0,.15u) -- (.15u,.4u);
enddef;

% @SYMBOL
% thStalagmite_UIS --
%
% @PREVIEW
%
% thStalagmite_UIS((.5,.5) inscale,0,1);

def p_stalagmite_UIS(expr pos,theta,sc,al) =
    U:=(.15u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,.4u) -- (0,-.15u) -- (-.15u,-.4u);
    thdraw (0,-.15u) -- (.15u,-.4u);
enddef;

% @SYMBOL
% thPillar_UIS --
%
% @PREVIEW
%
% thPillar_UIS((.5,.5) inscale,0,1);

def p_pillar_UIS(expr pos,theta,sc,al) =
    U:=(.15u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,.15u)--(0,-.15u);
    thdraw (-.15u,-.4u)--(0,-.15u)--(.15u,-.4u);
    thdraw (-.15u,.4u)--(0,.15u)--(.15u,.4u);
enddef;

% @SYMBOL
% thCurtain_UIS --
%
% @PREVIEW
%
% thCurtain_UIS((.5,.5) inscale,0,1);

def p_curtain_UIS(expr pos,theta,sc,al) =
    U:=(.15u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.15u,.4u)--(0,.15u)--(.15u,.4u);
    thdraw (0,.15u)--(0,0){dir 180}..(-.12u,-.11u)..{dir 0}(0,-.22u)--(0,-.4u);
enddef;

% @SYMBOL
% thHelectite_UIS --
%
% @PREVIEW
%
% thHelectite_UIS((.5,.5) inscale,0,1);

def p_helictite_UIS(expr pos,theta,sc,al) =
    U:=(.2u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,.4u)--(0,-.4u);
    p := (-.2u,.4u)---(-.2u,.1u){down}..{dir -10}(-.1u,0.02u)---origin;
    thdraw p;
    thdraw p reflectedabout (origin,(0,u)) reflectedabout (origin,(u,0));
enddef;

% @SYMBOL
% thSodaStraw_UIS --
%
% @PREVIEW
%
% thSodaStraw_UIS((.5,.5) inscale,0,1);

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



% @SYMBOL
% thCrystal_UIS --
%
% @PREVIEW
%
% thCrystal_UIS((.5,.5) inscale,0,1);

def p_crystal_UIS (expr pos,theta,sc,al)=
    U:=(.35u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := ((-.35*u,0)--(.35*u,0));
    thdraw p;
    thdraw p rotated (60);
    thdraw p rotated (120);
enddef;

% @SYMBOL
% thFlowstoneSymbol_UIS --
%
% Can be used for <I>Clay</I> symbol (like in CCNP)
%
% @PREVIEW
%
% thFlowstoneSymbol_UIS((.5,.5) inscale,0,1);

def p_flowstone_UIS (expr pos,theta,sc,al)=
    U:=(.45u,.1u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := ((-.15*u,0)--(.15*u,0));
    thdraw p shifted (.3u,-.1u);
    thdraw p shifted (-.3u,-.1u);
    thdraw p shifted (0,.1u);
enddef;

% @SYMBOL
% thMoonmilkSymbol_UIS --
%
% @PREVIEW
%
% thMoonmilkSymbol_UIS((.5,.5) inscale,0,1);

def p_moonmilk_UIS (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,-.2u){up}..{dir -60}(-.18u,-.105u){dir 80}..
        {dir-80}(.18u,-.105u){dir 60}..{down}(.5u,-.2u);
enddef;

% @SYMBOL
% thWallcalciteSymbol_UIS --
%
% @PREVIEW
%
% thWallcalciteSymbol_UIS((.5,.5) inscale,0,1);

def p_wallcalcite_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.2u,-.2u)--(0,.1u)--(.2u,-.2u);
enddef;

% @SYMBOL
% thPopcorn_UIS --
%
% Changed name from <I>Cauliflowercalcite</I> according to CCNP rules
%
% @PREVIEW
%
% thPopcorn_UIS((.5,.5) inscale,0,1);

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

% @SYMBOL
% thDisk_UIS --
%
% @PREVIEW
%
% thDisk_UIS((.5,.5) inscale,0,1);

def p_disk_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.2u,-.3u)--origin--(.2u,-.3u);
    p:=fullcircle scaled .3u shifted (0,.15u);
    thclean p;
    thdraw p;
enddef;

% @SYMBOL
% thGypsum_CCNP --
%
% @PREVIEW
%
% thGypsum_CCNP((.5,.5) inscale,0,1);

def p_gypsum_CCNP (expr pos,theta,sc,al)=
    U:=(.25u,.25u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.25u,0)--(.25u,0);
    thdraw (0,-.25u)--(0,.25u);
enddef;

% @SYMBOL
% thAragonite_CCNP --
%
% @PREVIEW
%
% thAragonite_CCNP((.5,.5) inscale,0,1);

def p_aragonite_CCNP (expr pos,theta,sc,al)=
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

% @SYMBOL
% thGypsumflower_CCNP --
%
% @PREVIEW
%
% thGypsumflower_CCNP((.5,.5) inscale,0,1);
% @ENDSYMBOL

def p_gypsumflower_CCNP (expr pos,theta,sc,al)=
    U:=(.4u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
%    thdraw (.2u,0){left}..(0,.2u)..(.25u,.35u)..(.5u,0)..
%        (.1u,-.3u)..{dir 125}(-.5u,.2u);
    thdraw (.18u,0){left}..(0,.15u)..(.2u,.3u)..(.4u,0)..
        (.15u,-.28u)..{dir 120}(-.4u,.15u);
enddef;



% @SYMBOL
% thRimstonepoolSymbol_ASF --
%
% @PREVIEW
%
% thRimstonepoolSymbol_ASF((.5,.5) inscale,0,1);

def p_rimstonepool_ASF (expr pos,theta,sc,al)=
    U:=(.4u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p := (-.4u,.2u){dir -70}..{dir 70}(.4u,.2u);
    thfill buildcycle(p,((.5u,.1u)--(-.5u,.1u)));
    thdraw p;
enddef;

% @SYMBOL
% thRimstonedamSymbol_ASF --
%
% @PREVIEW
%
% thRimstonedamSymbol_ASF((.5,.5) inscale,0,1);

def p_rimstonedam_ASF (expr pos,theta,sc,al)=
    U:=(.4u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.4u,.2u){dir -70}..{dir 70}(.4u,.2u);
enddef;





% @DESCRIPTION
% <H4>Erosive forms:</H4>

% @SYMBOL
% thAnostomosen_UIS --
%
% @PREVIEW
%
% thAnostomosen_UIS((.5,.5) inscale,0,1);

def p_anastomosis_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=(-.4u,-.2u)--(-.3u,-.3u)--(0,.3u)--(.1u,.2u);
    thdraw p;
    thdraw p shifted (.3u,0);
enddef;

% @SYMBOL
% thKarren_UIS --
%
% @PREVIEW
%
% thKarren_UIS((.5,.5) inscale,0,1);

def p_karren_UIS (expr pos,theta,sc,al)=
    U:=(.5u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,-.3u){dir -85}..{dir 85}(-.32u,-.3u)..(-.28u,.3u){dir 85}..
        {dir -85}(-.12u,.3u)..(-.08u,-.3u){dir-85}..{dir 85}(.08u,-.3u)..
        {dir85}(.12u,.3u)..(.28u,.3u){dir -85}..(.32u,-.3u){dir -85}..
        {dir 85}(.5u,-.3u);
enddef;

% @SYMBOL
% thScallop_UIS --
%
% @PREVIEW
%
% thScallop_UIS((.5,.5) inscale,0,1);

def p_scallop_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,-.4u){dir 125}..{dir 105}(-.2u,.1u){up}..
        {down}(.2u,.1u){dir -105}...{dir -125}(0,-.4u);
enddef;

% @SYMBOL
% thFlute_UIS --
%
% @PREVIEW
%
% thFlute_UIS((.5,.5) inscale,0,1);

def p_flute_UIS (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,.2u)--(.5u,.2u);
    thdraw (-.3u,.2u){dir -70}..{dir 70}(.3u,.2u);
enddef;

% @SYMBOL
% thRaft_CCNP --
%
% @PREVIEW
%
% thRaft_CCNP((.5,.5) inscale,0,1);

def p_raft_CCNP (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,.2u){dir -80}..(-.3u,-.2u)..origin..(.3u,.2u)..
        {dir -80}(.5u,-.2u);
enddef;

% @SYMBOL
% thRaftcone_CCNP --
%
% @PREVIEW
%
% thRaftcone_CCNP((.5,.5) inscale,0,1);

def p_raftcone_CCNP (expr pos,theta,sc,al)=
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

% @DESCRIPTION
% <H4>End of passage:</H4>

% @SYMBOL
% thNarrowend_UIS --
%
% Rotated perpendicular to passage walls can symbolize low end of the passage
%
% @PREVIEW
%
% thNarrowend_UIS((.5,.5) inscale,0,1);

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

% @SYMBOL
% thLowend_CCNP --
%
% @PREVIEW
%
% thLowend_CCNP((.5,.5) inscale,0,1);

def p_lowend_CCNP (expr pos,theta,sc,al)=
    U:=(.4u,0);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.4u,0)--(.4u,0);
enddef;

% @SYMBOL
% thFlowstonechoke_CCNP --
%
% @PREVIEW
%
% thFlowstonechoke_CCNP((.5,.5) inscale,0,1);

def p_flowstonechoke_CCNP (expr pos,theta,sc,al)=
    U:=(.4u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=(-.4u,-.4u){right}..{dir120}(-.2u,-.2u){dir -20}..{up}(0,0);
    thdraw p;
    thdraw p shifted (0,.2u);
    thdraw p reflectedabout (origin,down);
    thdraw p reflectedabout (origin,down) shifted (0,.2u);
enddef;

% @SYMBOL
% thBreakdownchoke_CCNP --
%
% @PREVIEW
%
% thBreakdownchoke_CCNP((.5,.5) inscale,0,1);

def p_breakdownchoke_CCNP (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,-.1u)--(-.15u,.05u)--(-.48u,.2u)--cycle;
    thdraw (-.2u,-.2u)--(.15u,-.18u)--(-.08u,.14u)--cycle;
    thdraw (.1u,0)--(.4u,-.1u)--(.5u,.2u)--(.1u,.2u)--cycle;
enddef;



% @DESCRIPTION
% <H4>Symbolic passage fills:</H4>


% @SYMBOL
% thBedrock_ASF --
%
% @PREVIEW
%
% thBedrock_ASF((.5,.5) inscale,0,1);

def p_bedrock_ASF (expr pos,theta,sc,al)=
    U:=(.5u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.5u,0)--(.5u,0);
    thdraw (-.3u,0)--(-.3u,.2u);
    thdraw (.3u,0)--(.3u,-.2u);
enddef;

% @SYMBOL
% thSandSymbol_UIS --
%
% @PREVIEW
%
% thSandSymbol_UIS((.5,.5) inscale,0,1);

def p_sand_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.2u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenB;
    thdraw (0,.2u);
    thdraw (0,.2u) rotated 120;
    thdraw (0,.2u) rotated 240;
enddef;

let p_clay_UIS = p_sand_UIS;

% @SYMBOL
% thPebblesSymbol_UIS --
%
% @PREVIEW
%
% thPebblesSymbol_UIS((.5,.5) inscale,0,1);

def p_pebbles_UIS (expr pos,theta,sc,al)=
    U:=(.45u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=superellipse((.2u,0),(0,.1u),(-.2u,0),(0,.-.1u),.75);
    thdraw p rotated 20 shifted (0,.25u);
    thdraw p rotated -37 shifted (.25u,-.25u);
    thdraw p rotated -62 shifted (-.25u,-.25u);
enddef;

% @SYMBOL
% thDebrisSymbol_UIS --
%
% @PREVIEW
%
% thDebrisSymbol_UIS((.5,.5) inscale,0,1);

def p_debris_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.3u,-.4u)--(0,-.22u)--(-.4u,-.1u)--cycle;
    thdraw (.2u,-.4u)--(.4u,-.1u)--(.1u,-.1u)--cycle;
    thdraw (-.1u,0)--(.2u,.3u)--(-.2u,.3u)--cycle;
enddef;

% @SYMBOL
% thBlocksSymbol_UIS --
%
% @PREVIEW
%
% thBlocksSymbol_UIS((.5,.5) inscale,0,1);

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

% @SYMBOL
% thWaterSymbol_UIS --
%
% @PREVIEW
%
% thWaterSymbol_UIS((.5,.5) inscale,0,1);
%
% @ENDSYMBOL
% @USE thLakePattern_UIS

def p_water_UIS (expr pos,theta,sc,al)=
    U:=(.425u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenD;
    p:=fullcircle xscaled (.85u) yscaled (.6u);
    thfill p withpattern a_water_UIS;
    thdraw p;
enddef;

% @SYMBOL
% thIceSymbol_UIS --
%
% @PREVIEW
%
% thIceSymbol_UIS((.5,.5) inscale,0,1);

def p_ice_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenD;
    thdraw (-.4u,-.1u)--(.4u,-.1u) dashed evenly;
    thdraw (-.2u,.2u)--(.2u,.2u) dashed evenly;
    pickup PenC;
    p := (0,.05u)--(0,.35u);
    thdraw p;
    thdraw p shifted (-.2u,-.3u);
    thdraw p shifted (.2u,-.3u);
enddef;


% @DESCRIPTION
% <H4>Various materials:</H4>

% @SYMBOL
% thArcheological_UIS --
%
% @PREVIEW
%
% thArcheological_UIS((.5,.5) inscale,0,1);

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

% @SYMBOL
% thPaleontological_UIS --
%
% @PREVIEW
%
% thPaleontological_UIS((.5,.5) inscale,0,1);

def p_paleomaterial_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.4u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    thfill (-.4u,-.2u)..(-.2u,-.08u)---(-.1u,.05u)---(0,.2u)..(-.03u,.4u)..
        (.2u,.3u)..(.35u,.25u)..(.2u,.2u)---origin---(-.1u,-.17u)..
        (-.08u,-.25u){dir -120}..(-.3u,-.23u)
        ..cycle;
enddef;

% @SYMBOL
% thGuano_UIS --
%
% @PREVIEW
%
% thGuano_UIS((.5,.5) inscale,0,1);

def p_guano_UIS (expr pos,theta,sc,al)=
    U:=(.4u,.35u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.4u,.2u){dir 40}..{down}(0,-.35u){up}..{dir-40}(.4u,.2u);
enddef;

% @SYMBOL
% thVegetabledebris_ASF --
%
% @PREVIEW
%
% thVegetabledebris_ASF((.5,.5) inscale,0,1);

def p_vegetabledebris_ASF (expr pos,theta,sc,al)=
    U:=(.38u,.3u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.3u,-.3u)--(0,.3u);
    thdraw (0,-.3u)--(.3u,.3u);
    thdraw (-.25u,.13u)--(.38u,.13u);
    thdraw (-.38u,-.13u)--(.25u,-.13u);
enddef;

% @SYMBOL
% thRoot_ASF --
%
% @PREVIEW
%
% thRoot_ASF((.5,.5) inscale,0,1);

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

% @DESCRIPTION
% <H4>Arrows:</H4>

% @SYMBOL
% thEntrancearrow_UIS --
%
% @PREVIEW
%
% thEntrancearrow_UIS((.5,.5) inscale,0,1);

def p_entrance_UIS (expr pos,theta,sc,al)=
    U:=(.2u,.5u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    thfill (-.2u,-.5u)--(0,.5u)--(.2u,-.5u)--cycle;
enddef;

% @SYMBOL
% thPaleoflow_UIS --
%
% @PREVIEW
%
% thPaleoflow_UIS((.5,.5) inscale,0,1);

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

% @SYMBOL
% thGradientarrow_UIS --
%
% @PREVIEW
%
% thGradientarrow_UIS((.5,.5) inscale,0,1);

def p_gradient_UIS (expr pos,theta,sc,al)=
    U:=(.15u,u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (0,-u)--(0,u);
    p:=(-.15u,.6u)--(0,u)--(.15u,.6u)--cycle;
    thfill p;
    thdraw p;
enddef;

% @SYMBOL
% thPermanentstreamarrow_UIS --
%
% @PREVIEW
%
% thPermanentstreamarrow_UIS((.5,.5) inscale,0,1);

def p_waterflow_permanent_UIS (expr pos,theta,sc,al)=
    U:=(.15u,.5u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    p:=(0,.5u){down}..(.12u,.3u)..(-.15u,.15u)..(.13u,0)..
        (-.08u,-.2u)..{down}(0,-.5u);
    p:=p rotated 180;
    thdraw p;
    oldahlength:=ahlength; ahlength:=2.5pt;
    thfill arrowhead p;
    ahlength:=oldahlength;
enddef;

% @SYMBOL
% thIntermittentstreamarrow_UIS --
%
% @PREVIEW
%
% thIntermittentstreamarrow_UIS((.5,.5) inscale,0,1);

def p_waterflow_intermittent_UIS (expr pos,theta,sc,al)=
  thdrawoptions(dashed evenly scaled .5);
  p_waterflow_permanent_UIS (pos,theta,sc,al);
  thdrawoptions();
enddef;

% @SYMBOL
% thAirflowarrow_UIS --
%
% @PREVIEW
%
% thAirflowarrow_UIS((.5,.5) inscale,0,1,3);

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

def p_sectionarrow_SKBB (expr pos,theta)=
%    T:=identity rotated theta shifted pos;
%    pickup PenC;
%    thdraw (0,0)--(0,.8u);
%    thdraw (-.1u,.55u)--(0,.8u)--(.1u,.55u);
enddef;


% @DESCRIPTION
% <H4>Survey stations:</H4>

% @SYMBOL
% thFixedstation_ASF --
%
% @PREVIEW
%
% thFixedstation_ASF((.5,.5) inscale);

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


% @SYMBOL
% thNaturalstation_ASF --
%
% @PREVIEW
%
% thNaturalstation_ASF((.5,.5) inscale);

def p_station_natural_ASF (expr pos)=
    T:=identity shifted pos;
    pickup PenD;
    thclean fullcircle scaled .3u;
    thdraw fullcircle scaled .3u;
    thdraw origin;
enddef;
 
% @SYMBOL
% thTemporarystation_ASF --
%
% @PREVIEW
%
% thTemporarystation_ASF((.5,.5) inscale);

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

def p_station_NoMorph (expr pos)=
    T:=identity shifted pos;
    thdraw origin withpen pencircle scaled 3pt withcolor blue;
enddef;

 
% @DESCRIPTION
% 
% <H3>Equipment</H3>

% @SYMBOL
% thEquipmentSymbol_SKBB --
%
% @PREVIEW
%
% thEquipmentSymbol_SKBB((.5,.5) inscale,2,"us");
% @USE thCircledlabel

% alignment!

def p_rope_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,1) enddef;          
def p_fixedladder_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,2) enddef;   
def p_ropeladder_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,3) enddef;    
def p_steps_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,4) enddef;         
def p_bridge_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,5) enddef;        
def p_traverse_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,6) enddef;
def p_anchor_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,7) enddef;
def p_camp_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,8) enddef;
def p_noequipment_SKBB (expr pos,r,s,al) = thp_equipment_SKBB(pos,9) enddef;


def thp_equipment_SKBB(expr pos,type) =
  begingroup
%  interim defaultscale:=0.8;
  p_label(
  if lang="en":
    if type=1: "R"        % rope
  	elseif type=2: "Lf"   % fixed ladder
  	elseif type=3: "Lr"   % rope ladder
  	elseif type=4: "S"    % steps
  	elseif type=5: "B"    % bridge
  	elseif type=6: "T"    % traverse
  	elseif type=7: "A"    % anchor
  	elseif type=8: "C"    % camp
  	elseif type=9: "!"    % not equipped
  	fi
  elseif lang="sk":
    if type=1: "L"
  	elseif type=2: "R"
  	elseif type=3: "Rl"
  	elseif type=4: "S"
  	elseif type=5: "M"
  	elseif type=6: "T"
  	elseif type=7: "K"
  	elseif type=8: "B" % bivak
  	elseif type=9: "!"
  	fi
	fi
    ,pos,0,5);
  endgroup;
enddef;

% doesn't call process_label();

def p_continuation_UIS(expr pos,theta,sc,al) =
   draw thelabel("?", pos);
enddef;
