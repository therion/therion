% This file is part of Therion package
% Copyright (C) 1999-2000 Martin Budaj

def StochasticSand_A (expr Center, A, B, Az, MaxDots) =
    pickup PenC;
    for i=1 upto MaxDots:		% $\vert x \vert < 2$ for 95 \% of $x$
        draw Center + (normaldeviate * A/4 ,normaldeviate * B/4) rotated Az; 
    endfor;		% obrovska narocnost pri drawdot (bodka cez 24 bodov)
enddef;

def StochasticRock_A (expr Center, A, B, Az) =
    pickup PenC;
    Mi := angle(A,B);
    Sigma := min(Mi/4, (90-Mi)/4);
    A_ := A / cosd(Mi);            % ellipse is magnified
    B_ := B / cosd(Mi);            % (A and B should be dimensions of stone) 
    for i = 0,1,2,3:
        tmp := 90 * i + if (i mod 2) = 0:
                            Mi
                        else:
                            90 - Mi
                        fi + Sigma * normaldeviate;
        zz[i] := Center + (sqrt((A_ / 2 * cosd(tmp))**2 + (B_ /2 * sind(tmp))**2) 
                * (dir (tmp))) rotated Az;
    endfor;
    erase fill zz0--zz1--zz2--zz3--cycle;
    draw zz0--zz1--zz2--zz3--cycle;
    draw zz0--Center--zz2;
    draw zz1--Center--zz3;
enddef;
