prologues:=1;

beginfig(1);

path p;

pair a,b,c,d,x,y;
a = (0,0);
b = (100,10);
c = (10,10);
d = (60,20);

x = whatever[a,b];
(c-x) = whatever * ((b-a)rotated 90);
y = whatever[a,b];
(d-y) = whatever * ((b-a)rotated 90);


p = a .. controls c and d .. b;

draw p withcolor 0.5*white;
draw a--c dashed evenly withcolor 0.5*white;
draw d--b dashed evenly withcolor 0.5*white;

pickup pencircle scaled 0.4 mm;

draw c--x dashed withdots scaled 0.5 withcolor 0.5*white;
draw d--y dashed withdots scaled 0.5 withcolor 0.5*white;

pickup pencircle scaled 0.35 mm;

draw a--x withcolor blue;
draw y--b withcolor blue;

pickup pencircle scaled 0.75 mm;

draw c withcolor red;
draw d withcolor red;

endfig;

end.
