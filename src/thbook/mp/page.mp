prologues:=2;

beginfig(1);

defaultfont:="cmssdc10";
defaultscale:=1.2;
ahlength:=8;
labeloffset:=5bp;

verbatimtex \font\tenrm=cmr10 \font\tenit=cmti10 \tenrm etex;

drawoptions(withcolor blue);

draw(0,0)--(397,0)--(397,482)--(0,482)--cycle;

draw(26,36)--(369,36)--(369,454)--(26,454)--cycle dashed evenly;

drawdblarrow(26,46)--(369,46);
label.top("page-width",(270,46));

drawdblarrow(188,36)--(188,454);
label.top("page-height",(188,250)) rotatedaround((188,250),90);

drawdblarrow(56,160)--(339,160);
label.top("size (width)",(100,160));

drawdblarrow(158,142)--(158,424);
label.top("size (height)",(158,250)) rotatedaround((158,250),90);

drawdblarrow(222,425)--(222,453);
label.rt("overlap",(222,439));

drawdblarrow(0,385)--(26,385);
label.rt("left-margin",(13,385)) rotatedaround((13,385),90);

drawdblarrow(105,454)--(105,482);
label.rt("top-margin",(105,468));

drawdblarrow(0,20)--(397,20);
label.bot("paper-width",(220,20));

drawdblarrow(387,0)--(387,482);
label.top("paper-height",(387,250)) rotatedaround((387,250),90);

drawdblarrow(42,184)--(127,184);
label.top("grid-size (width)",(84,184));

drawdblarrow(333,212)--(333,297);
label.top("grid-size (height)",(333,255)) rotatedaround((333,255),90);

endfig;

end;
