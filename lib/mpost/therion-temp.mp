def StochasticSand_B (expr Center, A, B, Az, MaxDots) = enddef;


def OrezObdl (expr Path) =
    pair zzzz[];
    path orezcesta;
    zzzz1 := point 0 of Path;
    zzzz2 := point infinity of Path;
    orezcesta := zzzz1 -- (xpart(zzzz2), ypart(zzzz1)) -- zzzz2 -- 
	(xpart(zzzz1),ypart(zzzz2)) --
	cycle;
    clip currentpicture to orezcesta;
enddef;

def Stena (expr Path) =
    pickup PenAA;
    draw Path;
enddef;

def BielaStena (expr Path) = enddef;

def Vrasa (expr pp, sx, sy, sd, su) =
    path pd, pu, ppp;
    ShiftedPath(pp, sd, -90);
    pd = ppp;
    ShiftedPath(pp, su,90);
    pu = ppp;
    
    fill pd -- reverse pu -- cycle withcolor 0.85 * white;
    
    pickup PenD;
    
    pair novybod[], starybod, normala;
    

    % horna polka

    path vrasa[];
    pair novybod[], starybod, normala;

    vrasa[0] := interpath(0.5, pd, pu);
    m := floor(arclength(vrasa[0]) / sx);
    n := floor((sd + su) / (2 * sy));
    draw vrasa[0];

    for i = 0 upto m:
	bod[i] := i*sx;
    endfor;
    maxbod := m;
    
    
    for i=1 upto n+1:
	vrasa[i] = interpath(i/(n+1), vrasa[0], pu);
	draw vrasa[i];
	
	for j = 0 upto maxbod:
	    t := arctime(bod[j]) of vrasa[i-1];
	    starybod := point t of vrasa[i-1];
	    normala :=  unitvector ((direction t of vrasa[i-1]) rotated -90);
	    novybod[j] := vrasa[i] intersectiontimes ((starybod-200*normala) .. (starybod+200*normala));
	    draw starybod .. (point xpart(novybod[j]) of vrasa[i]);
	    bodd[j] := arclength( subpath(0,xpart(novybod[j])) of vrasa[i] );
	endfor;
	
	dl := arclength(vrasa[i]);
	maxx := -1;
	if ((bodd[0] + bodd[1])/2)>sx:
	    maxx := maxx + 1;
	    bod[maxx] := ((bodd[0] + bodd[1])/2) - sx;
	fi;

	for j = 1 upto maxbod:
	    maxx := maxx + 1;
	    bod[maxx] := (bodd[j] + bodd[j-1]) / 2;
	endfor;

	if (bod[maxx]+sx) < dl:
	    maxx := maxx + 1;
	    bod[maxx] := (bod[maxx]+sx);
	fi;

        maxbod := maxx;
    endfor;    

    % dolna polka

    path vrasa[];
    pair novybod[], starybod, normala;

    vrasa[0] := interpath(0.5, pd, pu);
    draw vrasa[0];

    for i = 0 upto (m-1):
	bod[i] := (i+0.5)*sx;
    endfor;
    maxbod := (m-1);
    
    
    for i=1 upto n+1:
	vrasa[i] = interpath(i/(n+1), vrasa[0], pd);
	draw vrasa[i];
	
	for j = 0 upto maxbod:
	    t := arctime(bod[j]) of vrasa[i-1];
	    starybod := point t of vrasa[i-1];
	    normala :=  unitvector ((direction t of vrasa[i-1]) rotated -90);
	    novybod[j] := vrasa[i] intersectiontimes ((starybod-200*normala) .. (starybod+200*normala));
	    draw starybod .. (point xpart(novybod[j]) of vrasa[i]);
	    bodd[j] := arclength( subpath(0,xpart(novybod[j])) of vrasa[i] );
	endfor;
	
	dl := arclength(vrasa[i]);
	maxx := -1;
	if ((bodd[0] + bodd[1])/2)>sx:
	    maxx := maxx + 1;
	    bod[maxx] := ((bodd[0] + bodd[1])/2) - sx;
	fi;

	for j = 1 upto maxbod:
	    maxx := maxx + 1;
	    bod[maxx] := (bodd[j] + bodd[j-1]) / 2;
	endfor;

	if (bod[maxx]+sx) < dl:
	    maxx := maxx + 1;
	    bod[maxx] := (bod[maxx]+sx);
	fi;

        maxbod := maxx;
    endfor;    
    
enddef;
