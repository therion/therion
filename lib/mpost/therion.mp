%%%% This file is part of Therion package
%%%% Copyright (C) 1999 Martin Budaj

%%%% MFT formatting:
%%% 
%%% point arclength arctime setbounds withpattern
%%% length infont llcorner lrcorner urcorner ulcorner label
%%% enddef beginpattern endpattern beginfig endfig

warningcheck:=0;
% variables:
path ppp,pp[],kruh,ram;
pair zz[];
picture pict[];
string ch, Strutstring;

Strutstring = "(È";


def Initialize (expr Scale) =
    Defaultthickness := (3.77 / sqrt(sqrt(Scale))) * mm;
    def PenA = pencircle scaled Defaultthickness enddef;
    def PenB = pencircle scaled (0.7 * Defaultthickness) enddef;
    def PenC = pencircle scaled (0.5 * Defaultthickness) enddef;
    def PenD = pencircle scaled (0.35 * Defaultthickness) enddef;
    Interline = 2 * Defaultthickness;		% ¹rafy v jazere
    Stemstep = 2.1 * Defaultthickness;		 	% zrazy
    Stemlength = 1.26 * Defaultthickness;		% pri zraze
    PolygonPoint = 0.8 * Defaultthickness;
    PolygonLine = 3 * Defaultthickness;
enddef;

Initialize(Scale);
input mpattern;			% we have to know all dimensions, pens \&c.

def Outline (expr Path) = 
    pickup PenA;
    draw Path;
enddef;

def Clean (expr Path) =
    erase fill Path;
enddef;

def Fill (expr Path, Pattern) =
    erase fill Path;
    fill Path withpattern Pattern;
enddef;

def Border (expr Path) =
    pickup PenD;
    draw Path;
enddef;

def Polygon (expr Path) =
    pickup PenD;
    for t = 0 upto length Path - 1:
        zz1 := point t of Path;
        zz2 := point t+1 of Path;
        if length (zz2-zz1) > 2*PolygonLine:
            draw zz1 -- zz1 + PolygonLine * unitvector(zz2 - zz1);
            draw zz2 -- zz2 + PolygonLine * unitvector(zz1 - zz2);
        else:
            draw zz1 -- zz2;
        fi;
        pp1 := fullcircle scaled PolygonPoint shifted (zz1);
        pp2 := fullcircle scaled PolygonPoint shifted (zz2);
        erase fill pp1;
        erase fill pp2;
        draw pp1;
        draw pp2;
    endfor;
enddef;

def Pit (expr Path) = 
    cas := 0;
    krok := Stemstep;
    dlzka := arclength Path;
    if krok <= dlzka:
        mojkrok := dlzka / (floor(dlzka / krok));	% krok is stretched a bit
    else:						% according to length of path
        mojkrok := dlzka;
    fi;	
    pickup PenC;	
    forever:
        t := arctime cas of Path;
        draw (point t of Path) ..
            ((point t of Path) +
            Stemlength * unitvector((direction t of Path) rotated 90));
        cas := cas + mojkrok;
        exitif cas > dlzka + (Stemstep / 2);	% for rounding errors
    endfor;
    draw Path;
enddef;

def ShiftedPath (expr Path, Dist, Az) =
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

def DoublePit (expr Path, Dist) =
    ShiftedPath (Path, Dist/2, -90);
    Pit(ppp);
    ShiftedPath (Path, Dist/2, 90);

    Pit(reverse ppp);

enddef;

def StochasticSand (expr Center, A, B, Az, MaxDots) =
    pickup PenC;
    for i=1 upto MaxDots:		% $\vert x \vert < 2$ for 95 \% of $x$
        draw Center + (normaldeviate * A/4 ,normaldeviate * B/4) rotated Az; 
    endfor;		% obrovska narocnost pri drawdot (bodka cez 24 bodov)
enddef;

def StochasticRock (expr Center, A, B, Az) =
    pickup PenC;
    for i = 0,1,2,3:
        alfa := angle(A,B);
        tmp := 90 * i + if (i mod 2) = 0:
                            alfa
                        else:
                            90 - alfa
                        fi + normaldeviate * 10;
        zz[i] := Center + (sqrt((A / 2 * cosd(tmp))**2 + (B /2 * sind(tmp))**2) 
                * (dir (tmp))) rotated Az;
    endfor;
    erase fill zz0--zz1--zz2--zz3--cycle;
    draw zz0--zz1--zz2--zz3--cycle;
    draw zz0--Center--zz2;
    draw zz1--Center--zz3;
enddef;

def Freetext(expr String,Path,Font,Scale) =
    l := length (String);				% No. of characters
    pict[0] := (String) infont (Font) scaled (Scale);
    zz[0] := (lrcorner pict[0]) - (llcorner pict[0]);
    strl := xpart zz[0];				% real length 
    pathl := arclength (Path);
    delta := (pathl - strl) / (l - 1);			% distance between chars
    cas := 0;
    for i=0 upto (l - 1):
        ch := substring(i,i+1) of (String);
        pict[0] := ch infont (Font) scaled (Scale);
        zz[0] := (lrcorner pict[0]) - (llcorner pict[0]);	    
        charwidth := xpart zz[0];			% character width
        if ASCII (ch) <> 32: 
            zz[1] := lrcorner pict[0];
            addto pict[0] also (Strutstring infont Font scaled Scale);	% like TeX's $\backslash$ strut
            zz[2] := llcorner pict[0];
            zz[3] := ulcorner pict[0];
            pict[1] := ch infont Font scaled Scale;
            setbounds pict[1] to (zz[2]--(xpart zz[1],ypart zz[2])--
                (xpart zz[1],ypart zz[3])--zz[3]--cycle);
            cas := cas + (charwidth / 2);
            t := arctime cas of (Path);			% moving along path
            label(pict[1] rotated (angle direction t of (Path)), point t of (Path));
        else:
            cas := cas + (charwidth / 2);
        fi;
        cas := cas + (charwidth / 2) + delta; 
    endfor;
enddef;

beginpattern(LakePattern);
    draw origin--10up withpen pensquare scaled (0.2 * Defaultthickness);
    patternxstep(Interline);
    patterntransform(identity rotated -45);
endpattern;
