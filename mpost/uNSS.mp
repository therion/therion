
% common passage ends in NSS:
% TT too tight
% TL too low
% FS flowstone (choke)
% BD breakdown choke
% MF "mud fill" (clay choke)

def p_narrowend_NSS (expr pos,theta,sc,al)=
	p_label ("TT", pos, theta, 0);
enddef;

def p_claychoke_NSS (expr pos,theta,sc,al)=
	p_label ("MF", pos, theta, 0);
enddef;

% originally by Philip Schuchardt
def p_gradient_NSS (expr pos,theta,sc,al) =
  U:=(.4u, .6u);
  T:=identity aligned al rotated theta scaled sc shifted pos;
  pickup PenC;

  thdraw (-.2u, -.4u) -- (-.4u, .2u);
  thdraw (0u, -.3u) -- (0u, .6u);
  thdraw (.2u, -.4u) -- (.4u, .2u);
enddef;

%bristol's symbol. Based on gypsum flower code.
def p_airdraught_NSS (expr pos,theta,sc,al)=
    U:=(.25u,u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (.18u,0){left}..(0,.15u)..(.2u,.3u)..(.4u,0)..
        (.15u,-.28u)..(-.25u,0)..{dir 100}(0,.8u)..(0,u);

    p:=(-.2u,.65u){dir 20}..{dir 90}(0,u);
    thdraw p;
    thdraw p reflectedabout (origin,(0,u));
enddef;

def p_mud_NSS(expr pos,theta,sc,al) =
    U:=(.2u, .7u);
    T:=identity aligned al rotated theta scaled sc shifted pos;
    pickup PenC;
    thdraw (-.7u, .2u) -- (-.2u, .2u);
    thdraw (-.3u, -.2u) -- (.3u, -.2u);
    thdraw (.2u, .2u) -- (.7u, .2u);
enddef;

let l_chimney_NSS = l_ceilingstep_UIS;

% bristol's sheet indicates this equivalence
let l_dripline_NSS = l_lowceiling_SBE;

def l_wall_pit_NSS (expr P) =
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.25u);
  pickup PenC;
  forever:
    t := arctime cas of P;
    mark_ (P,t,0.2u);
    cas := cas + mojkrok;
    exitif cas > dlzka + (mojkrok / 3); % for rounding errors
  endfor;
  pickup PenA;
  thdraw P;
enddef;

def l_wall_chimney_NSS (expr P) =
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.5u);
  pickup PenC;
  forever:
    t := arctime cas of P;
    mark_ (P,t,-0.2u);
    cas := cas + mojkrok;
    exitif cas > dlzka + (mojkrok / 3); % for rounding errors
  endfor;
  pickup PenA;
  thdraw P;
enddef;

def l_pitchimney_NSS (expr P) = % this is the nss ceiling step with a second tick)
  T:=identity;
  cas := 0;                            % cursor to step along path
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.8u);   % symbol length nudged to be multiple of path length
  pickup PenC;
  forever:
    t1 := arctime (cas + mojkrok*1/5) of P;
    t  := arctime (cas + mojkrok/2) of P;
    t2 := arctime (cas + mojkrok*4/5) of P;
    thdraw (subpath (t1,t2) of P);
    mark_ (P,t,-0.2u);                  % change sign to -0.2u
    mark_ (P,t,0.2u);
    cas := cas + mojkrok;
    exitif cas > dlzka - (2*mojkrok/3); % for rounding errors
  endfor;
enddef;

% this is slightly modified pit_UIS
% should never have tick at end.
def l_floormeander_NSS (expr P) = 
  T:=identity;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, 0.25u);
  cas := mojkrok; %start 1 step along
  pickup PenD;
  forever:
    t := arctime cas of P;
    mark_ (P,t,0.1u);
    mark_ (P,t,-0.1u);
    cas := cas + mojkrok;
    exitif cas > dlzka - (mojkrok/3); % for rounding errors
  endfor;
  pickup PenC;
  thdraw P;
enddef;

% this should be redone at a higher density, but for now this will do.
let l_ceilingmeander_NSS = l_pitchimney_NSS;

def l_wall_flowstone_NSS (expr P) =
  T:=identity;
  cas := 0;
  dlzka := arclength P;
  mojkrok:=adjust_step(dlzka, .7u);
  if (cycle P) and (dlzka < 3.5u):   % make at least 5 curls on a cyclic path
    mojkrok := dlzka/5;
  fi;
  pickup PenC;
  t1:=0;
  % copied from uAUT wall_pit
  samplingdistance:=max(0.15u,dlzka/1500);

    path testcircle;
    % testcircle, used to test, whether the path makes a 'v'-like bend,
    % where normal sampling and offsetting gives poor results for the parallel line
    % size of testcircle= size PenA/2 + 2* (distance between outer and inner line)
    % + PenC (inner line)
    testcircle := halfcircle scaled (.5*u/10+2*.125u+.5*u/10) shifted (0,.5*u/10);
    cur:=samplingdistance/2;
    count:=0;
    forever:
      t:= arctime cur of P;
      inter:=P intersectiontimes (testcircle rotated (angle thdir(P,t)) shifted (point t of P));
      if (xpart inter) >=0:
        % we are at a 'sharp bend', so set next point to intersectionpoint
        cur:=cur+samplingdistance;
        t := max( xpart inter, arctime cur of P);
        %thdraw point t of P withcolor 0.7*green;
        forever:
          t:= arctime cur of P;
          inter:=P intersectiontimes (testcircle rotated (angle thdir(P,t)) shifted (point t of P));
          exitif (xpart inter <0);
          cur := cur + samplingdistance;
          exitif (cur >=dlzka);
        endfor;
        if (cur <dlzka):
          inner[count]:=(point t of P + .125u*unitvector(thdir(P,t) rotated 90) );
          innerdir[count]:=unitvector(thdir(P,t));
          count := count+1;
        fi;
      else:
        inner[count]:= (point t of P + .125u*unitvector(thdir(P,t) rotated 90) );
        innerdir[count]:=unitvector(thdir(P,t));
        %thdraw inner[count] withpen PenD withcolor 0.5*red;
        count:=count +1;
        cur:= cur+ samplingdistance;
      fi;
      exitif (cur >=dlzka);
    endfor;
    if count>2:
      % draw inner path
      pickup PenC;
      path innerpath;
      innerpath =
      for i=0 upto count-2:
        inner[i]{(innerdir[i])} ..
      endfor
      if cycle P: cycle;
      else:  inner[count-1];
      fi;
  forever:
    t2 := arctime (cas + mojkrok) of innerpath;
    thdraw (point t1 of innerpath){dir (angle(thdir(innerpath,t1)) + 60)} ..
           {dir (angle(thdir(innerpath,t2)) - 60)}(point t2 of innerpath);
    cas := cas + mojkrok;
    exitif cas > dlzka + (mojkrok / 3); % for rounding errors
    t1:=t2;
  endfor;
  pickup PenA;
  thdraw P;
enddef;

let l_wall_unsurveyed_NSS = l_wall_presumed_UIS;

def l_wall_underlying_NSS (expr P) =
  thdrawoptions(dashed withdots withpen PenA);
  thdraw P;
  thdrawoptions();
enddef;

def l_slope_NSS (expr P,S)(text Q) =
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
  offset:=0;
  dlzka := (arclength P);
  if dlzka>3u:
    offset := 0.3u;
  elseif dlzka>u:
    offset := 0.1u;
  fi;
  dlzka:=dlzka-2offset;
  cas := offset;
  mojkrok:=adjust_step(dlzka,1.4u) / 2;
  pickup PenC;
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
      thdraw (point t of P) + mojkrok/3 * unitvector(dir(a-90))--
        ((point t of P) + l * .8 * unitvector(dir(a-50)));

      thdraw
        (point t of P) + mojkrok/3 * unitvector(dir(a+90)) --
        ((point t of P) + l *.8 * unitvector(dir(a+50)));

      thdraw (point t of P) + .1 * l * unitvector(dir(a)) -- ((point t of P) + 
        l * unitvector(dir(a)));
    fi;

    cas := cas + mojkrok;
    par := not par;
    exitif cas > dlzka + offset + 0.1mm;  % for rounding errors
  endfor;
%pickup pencircle scaled 3pt;
%for i=0 upto li: draw point i of P; endfor;
enddef;

def a_clay_NSS (expr P) = 
  % copied out of ice_SKBB, modified to suit nss mud symbol
  T:=identity;
  pickup PenC
  path q; q = bbox p;
  picture tmp_pic, mud_NSS;
  pair z;
  mud_NSS := image(
    draw (-.7u, .2u) -- (-.2u, .2u);
    draw (-.3u, -.2u) -- (.3u, -.2u);
    draw (.2u, .2u) -- (.7u, .2u);
  );
  tmp_pic := image(
    for i = xpart llcorner q step 2u until xpart urcorner q:
      for j = ypart llcorner q step 2.2u until ypart urcorner q:
        z := ((i,j) randomized 1.8u);
        if xpart (p intersectiontimes ((bbox mud_NSS) shifted z)) < 0:
          draw mud_NSS shifted z;
        fi;
      endfor;
    endfor;
  );
  clip tmp_pic to p;
  drawoptions();
  draw tmp_pic;
enddef;
