% This file is part of Therion package
% Copyright (C) 1999-2000 Martin Budaj

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
