prologues:=2;

beginfig(1);

verbatimtex \font\tenrm=cmr10 \font\tenit=cmti10 \tenrm etex;

label(btex Therion etex,(15,0));
label.rt(btex {\it therion.mp} etex, (80,35));
%label(btex thPdf etex, (80,-55));
%label(btex theps2pdf etex, (140,-55));
label(btex {\it therion.tex} etex, (180,-95));
label(btex XTherion etex, (5,57));
label.lft(btex Loch etex, (0,-110));
drawoptions(withcolor blue);
label.rt(btex \& other viewers etex, (0,-110));
%label(btex etex, (,));
drawoptions(withcolor red);
label(btex MetaPost etex, (80,0));
label.rt(btex {\it Plain base} etex, (60,50));
label(btex makempx etex, (150,15));
label(btex mpto etex, (210,15));
label(btex \TeX etex, (210,-15));
label(btex dvitomp etex, (150,-15));
label(btex {\it Plain format} etex, (210,-40));
label(btex pdfTeX etex, (105,-85));
label(btex {\it Plain format} etex, (180,-75));
%label(btex etex, (,));
drawoptions(withcolor blue);
%label(btex cavern etex, (-50,0));
%label(btex 3dtopos etex, (-50,-15));
drawoptions(withcolor (85/255,175/255,43/255));
label(btex {\it input data}  etex, (15,30));
label(btex {\it PDF maps} etex, (110,-111));
%label(btex {\it 3D model} etex, (-15,-60));
label(btex {\it info \& log files} etex, (-25,-60));
label(btex {\it maps, models, DB} etex, (15,-85));
label(btex {\it scanned sketches} etex, (-50,80));
%label(btex etex, (,));
drawoptions();
drawarrow((15,22)--(15,7));

%drawarrow((-40,-70)--(-10,50));
drawarrow((-20,-50)--(-10,50));

%drawarrow((-6,2)--(-32,2));
%drawarrow((-6,3)--(-32,13));
%drawarrow((-50,10)--(-50,-8));
%drawarrow((-27,-13)--(-6,-3));
%drawarrow((-32,-2)--(-6,-2));

drawarrow((37,2)--(54,2));
drawarrow((54,-2)--(37,-2));


drawarrow((72,43)--(80,7));
drawarrow((101,28)--(85,7));

drawarrow((104,3)--(125,13));
drawarrow((174,15)--(194,15));
drawarrow((210,10)--(210,-8));
drawarrow((194,-15)--(171,-15));
drawarrow((125,-13)--(104,-3));
drawarrow((210,-34)--(210,-22));

%drawarrow((80,-6)--(80,-47));
%drawarrow((96,-55)--(114,-55));

%drawarrow((15,-6)--(64,-48));
%drawarrow((81,-61)--(99,-77));
drawarrow((15,-6)--(99,-77));

%drawarrow((134,-61)--(106,-77));
drawarrow((148,-75)--(125,-81));
drawarrow((154,-94)--(125,-87));
drawarrow((105,-90)--(105,-102));

drawarrow((15,-90)--(15,-102));

drawarrow((15,-6)--(-2,-50));
drawarrow((15,-6)--(15,-77));
%drawarrow((15,-6)--(32,-90));

drawarrow((15,51)--(15,37));
drawarrow((15,37)--(15,51));

drawarrow((-10,74)--(10,63));

endfig;

end;
