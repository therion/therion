
def s_northarrow_SKBB = 
  T:=identity;
  begingroup
    interim defaultscale:=3;
    label(if lang="sk": "S" else: "N" fi, origin);
  endgroup;
  drawarrow (0,-1cm)--(0,1cm);
enddef;

def s_scalebar_SKBB (expr l, units, txt) = 
%  l:=20; string units; units:="m";
  tmpl:=l * cm;
  tmpl:=tmpl/Scale * if units="m": 100 elseif units="ft": 31 fi / 2;
  pickup PenC;
  draw (-tmpl,0)--(tmpl,0);
  draw (-tmpl,0)--(-tmpl,2bp);
  draw (tmpl,0)--(tmpl,2bp);
  begingroup
    interim labeloffset:=2bp;
%    label.ulft(decimal l, origin);
%    label.urt(units, origin);
    label.top(txt,origin);
  endgroup
enddef;

