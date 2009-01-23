metapost: warningy len v debug mode; interaktivny mod pre -d (s vysvetlenim v thbooku, ktore premenne pouzit)

do thbooku vysvetlenie o georeferencovani pdf z mailu muttonovi

do thbooku vysvetlenie o debugovani MP (mail muttonovi 15.1.09 

do thbooku: ak je cs pouzity niekde, musi byt vsade

poriadne zvacsit podklad pod legendou

zvacsovanie fontov v suradniciach

pri reze grid vpravo pretrca cez mapu (postupne skladanie)


v atlase nie je moznost pouzit mierku a sever pred vlozenim map (formdef vkladat uz v data.tex)
rozmery zrkadla sadzby zmenit len pre mapy, nie pre uvod a legendu


Mutton: 
  Where elevations are perpendicular to the co-ord system grid (viewed N, S, W, or E) then vertical grid lines (or positions if you use the default cross hairs) would be appropriate, but there is no option for this at present.  (This applies to map outputs as well)

  Using the ‘origin’ and ‘origin-label’ works well in standalone plan view but seem to work poorly when trying to use a consistent 3 dimensional tessellation for a set of plans and elevations orthogonal  to the co-ordinate system. (Of course it would not work very easily in concept it were not orthogonal – and keeping a single three dimensional atlas origin point)
  As per the attached, I have set the atlas origin so that the atlas plan with origin at bottom left is called X 12 in the navigation box, and so that the atlas elevation with origin at bottom left should be 5 X.  The atlas page is setup so that the drawing length along each of the pages is the same (ie PLAN size + 2xoverlap = ELEV size + 2x overlap).
  What I find is that the elevation labelling is incorrect (ie the origin sheet for the elevation is labelled 5 V, (not 5 X) and that because my elevation is looking west, the labels increase in the wrong direction.




[[pri definovani globalnych funkcii, typov a premennych pouzivat prefix "th" 
   resp. "TH" (napr. thprint_tex_encodings, THFONTS)]]


mapy vseobecne:
 * pri farebnych labeloch stmavit odtien farby

thpdf:

 * zoznam scrapov na danej strane v atlase -> z toho registre
   * register pouzitych nazvov - z blokov
                               - z vrstiev
   * zhrnutie udajov o listoch na konci
 * k vrstve: meno vo vertikalnom smere alebo udaj o vyske
 * zanorene OCG
 * cisla stran a odkazov cez utf2tex
 * skontrolovat ci nezapisujem nadbytocne xobjecty kvoli vrstvam
 * podmienene nacitavanie scrapov v TeXu
 * dvojstrankovy mod (Title vzdy na pravej strane), vtedy akceptovat:
   * vkladanie prazdnych stran (lepsie: atribut prava/lava pre stranu)
   * exclude pages a vkladanie prazdnych stran: mena namiesto cisel stran (?)
   * zrusit OwnPages option (jedine spolu s predch. -- problem s funkciou
     find_excluded_pages) -- namiesto cisla strany makro \getpage{.} zvacsi 
     argument o hodnotu \the\pageno na 1. strane s mapou
 * 2. sokolie oko
 * vrstvy pod aktualnou: odlisit hlbku odtienmi sedej (?)
 * mody pre preview (ziadne, linkovane, vsetky)
 * vrstva `pri' pre atlas
 * co s posunom stran v rychlom mode


thsvg:

 * uxivatelsky prefix pre ids

 * fonty - orezavacie cesty + vkladanie?
 * layers cez javascript
 * (priehladnost -> SVG 1.2)


thconvert:

 * (dorobit) CONCAT + konverzia rozdelenych textovych riadkov
 * optimalizacia zmien fontov + gsave & al.


thepsparse: 

 * grid do 2d struktur


thtexfonts:

 * upravit citanie glyphlistu aby podporovalo viac unicode hodnot
 * prepinanie fontov v makrach definovanych uzivatelom v TeXu a MP
   (equipment, continuation) aj pre cisla stran v atlase (tie cez utf2tex)
 * optimalizacia zmien fontov (delenie slov!)
 * doplnit kvazi HTML znackovanie vstupnych retazcov
 
 
tex:

 * sipka na sever je zbytocna pri priecnom a rozvinutom reze. V ich pripade
   by tam mal byt nejaky identifikator projekcie  
 * northarrow do atlasu?
 

mpost:

 * point stalctites,stalagmites, via-ferrata
 * gradientny prechod pre areas
 * poriesit odsadenie bodovych znaciek a najma labelov
 * dorobit aby fungovalo predefinovanie u,v,w - pri presumed wall, patternoch
 * initialize(Scale) nepomaha na lejble v legende -- prepisovanie mptexpre.tex?
 * color map-bg do legendboxov (hadam fg; co ak je fg farbene rozne?)

thbook:

 * varovania texu a metapostu
 * obrazky mapovych znaciek

----- prezitky dob minulych -----

 * mod pre _velke_ mapy (>10MB): scrapy vysadzat do samostatneho suboru a z 
   dalsieho na ne odkazovat pomocou Reference XObject forms -> nefunguje v AR5

---------------------------------
hotovo: 

* point snow
* line rope
* area snow, ice, clay
* opr. area debris, sand, line debris; dopln. ostatne nedef, equipment, sipka 
  na sever
 * ramik a stvorcova siet (kriziky): vylepsit alebo cez MetaPost 
   (do uvahy aj convergence)

 * vypnit miesto pod legendou bg farbou 

* text vo farbe scrapov
* suradnice ku gridu
* labels ku kladu listov
* velkost gridu (suradnic) urcit pred zvacsenim plochy podla page-gridu
