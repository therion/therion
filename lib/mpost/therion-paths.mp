% This file is part of Therion package
% Copyright (C) 1999-2000 Martin Budaj

def Outline_A (expr Path) = 
    pickup PenA;
    draw Path;
enddef;

def Outline_B (expr Path) =
    path_setup_(Path, Stemstep);
    pickup PenD;
    forever:
        t := arctime cas of Path;
        t1 := arctime (cas + mojkrok/2) of Path;
        draw (point t of Path) ..
            ((point t of Path) +
            1.2 * Stemstep * unitvector((direction t of Path) rotated (-30+5*normaldeviate)));
        draw (point t1 of Path) ..
            ((point t1 of Path) +
            1.2 * Stemstep * unitvector((direction t of Path) rotated (-150+5*normaldeviate)));
        cas := cas + mojkrok;
        exitif cas > dlzka - (mojkrok / 3);	% for rounding errors
    endfor;
    pickup PenC;
    draw Path;
enddef;

def Clean (expr Path) =
    erase fill Path;
enddef;

def Fill (expr Path, Pattern) =
    erase fill Path;
    fill Path withpattern Pattern;
enddef;

def Border_A (expr Path) =
    pickup PenC;
    draw Path;
enddef;

def Polygon_A (expr Path, Col) =
    pickup PenD;
    for t = 0 upto length Path - 1:
        zz1 := point t of Path;
        zz2 := point t+1 of Path;
        if length (zz2-zz1) > 2*PolygonLine:
            draw zz1 -- zz1 + PolygonLine * unitvector(zz2 - zz1) withcolor Col;
            draw zz2 -- zz2 + PolygonLine * unitvector(zz1 - zz2) withcolor Col;
        else:
            draw zz1 -- zz2 withcolor Col;
        fi;
        pp1 := fullcircle scaled PolygonPoint shifted (zz1);
        pp2 := fullcircle scaled PolygonPoint shifted (zz2);
        erase fill pp1;
        erase fill pp2;
        draw pp1 withcolor Col;
        draw pp2 withcolor Col;
    endfor;
enddef;

def Polygon_B (expr Path, Col) =
    pickup PenD;
    for t = 0 upto length Path - 1:
        zz1 := point t of Path;
        zz2 := point t+1 of Path;
        draw zz1 -- zz2 withcolor Col;
        pp1 := fullcircle scaled PolygonPoint shifted (zz1);
        pp2 := fullcircle scaled PolygonPoint shifted (zz2);
        filldraw pp1 withcolor Col;
        filldraw pp2 withcolor Col;
    endfor;
enddef;
def path_setup_ (expr Path, Step) =
    cas := 0;
    krok := Step;
    dlzka := arclength Path;
    if krok <= dlzka:
        mojkrok := dlzka / (floor(dlzka / krok));	% krok is stretched a bit
    else:			                    			% according to length of path
        mojkrok := dlzka;
    fi;	
enddef;


def Pit_A (expr Path) = 
    path_setup_(Path, Stemstep);
    pickup PenC;	
    forever:
        t := arctime cas of Path;
        draw (point t of Path) ..
            ((point t of Path) +
            Stemlength * unitvector((direction t of Path) rotated 90));
        cas := cas + mojkrok;
        exitif cas > dlzka + (krok / 3);	% for rounding errors
    endfor;
    draw Path;
enddef;

def Pit_B (expr Path) =
    path_setup_(Path, Stemstep);
    pickup PenC;
    forever:
        t1 := arctime cas of Path;
        t := arctime (cas + mojkrok/2) of Path;
        t2 := arctime (cas + mojkrok) of Path;
        zz3 := ((point t of Path) +
            Stemlength * unitvector((direction t of Path) rotated 90));
        fill (subpath (t1,t2) of Path) --
            (point t2 of Path)  -- zz3 -- cycle;
%        draw    (point t2 of Path)  -- zz3 -- (point t1 of Path);

        cas := cas + mojkrok;
        exitif cas > dlzka + (krok / 3);	% for rounding errors
    endfor;
    draw Path;
enddef;

def Slope_A (expr Path, Lt, Line) = 
    cas := 0;
    krok := 4/3 * Stemstep;
    dlzka := arclength Path;
    if krok <= dlzka:
        mojkrok := dlzka / (floor(dlzka / (2 * krok)));	% krok is stretched a bit
	mojkrok := mojkrok / 2;
    else:			                    			% according to length of path
        mojkrok := dlzka;
    fi;	
    pickup PenC;
    par := 1;	
    forever:
        t := arctime cas of Path;
        draw (point t of Path) ..
            ((point t of Path) +
            if par = 1: 
		Lt
    	    else:
		.33 * Lt
	    fi
	    * unitvector((direction t of Path) rotated 90));
        cas := cas + mojkrok;
	if par = 0: par := 1 else: par := 0 fi;
        exitif cas > dlzka + (krok / 3);	% for rounding errors
    endfor;
    if Line = 1: draw Path fi;
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

def Water_A (expr Path) =
    path_setup_(Path, 1.5*Stemstep);
    pickup PenD;	
    vardef azim = 50 + 15*normaldeviate enddef;
    az1 := azim;
    sgn1 := 1;
    sgn2 := -1;
    ppp := point 0 of Path;
    forever:
        t1 := arctime cas of Path;
        t2 := arctime (cas+mojkrok) of Path;
        if cas+1.1*mojkrok > dlzka:
            az2 := 0;
        else:
            az2 := azim;
        fi;
        d1 := angle(direction t1 of Path) + sgn1 * az1;
        d2 := angle(direction t2 of Path) + sgn2 * az2;
        ppp := ppp & (point t1 of Path){dir d1} .. {dir d2}(point t2 of Path);
        az1 := az2;
        sgn1 := -1 * sgn1;
        sgn2 := -1 * sgn2;
        cas := cas + mojkrok;
        exitif cas > dlzka + mojkrok/3;    	% for rounding errors
    endfor;
    drawarrow ppp;
enddef;