% This file is part of Therion package
% Copyright (C) 1999-2000 Martin Budaj

warningcheck:=0;
% variables:
path ppp,pp[],kruh,ram;
pair zz[];
picture pict[];
string ch, Strutstring;

Strutstring = "(È";


def Initialize (expr Scale) =
    Defaultthickness := (1.7 / sqrt(sqrt(Scale))) * mm;
    def PenA = pencircle scaled Defaultthickness enddef;
    def PenAA = pencircle scaled (1.2 * Defaultthickness) enddef;
    def PenB = pencircle scaled (0.7 * Defaultthickness) enddef;
    def PenC = pencircle scaled (0.5 * Defaultthickness) enddef;
    def PenD = pencircle scaled (0.35 * Defaultthickness) enddef;
    Interline = 2 * Defaultthickness;		% ¹rafy v jazere
    Stemstep = 2.1  * Defaultthickness;		 	% zrazy
    Stemlength = 1.26 * Defaultthickness;		% pri zraze
    PolygonPoint = 0.8 * Defaultthickness;
    PolygonLine = 3 * Defaultthickness;
    ahlength := 1.5 * Defaultthickness;
enddef;

Initialize(Scale);
input mpattern;			% we have to know all dimensions, pens \&c.
