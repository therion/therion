thepsparse: grid do 2d struktur


 * pri definovani globalnych funkcii, typov a premennych pouzivat prefix "th" 
   resp. "TH" (napr. thprint_tex_encodings, THFONTS)

thpdf:

 * zanorene OCG
 * cisla stran a odkazov cez utf2tex
 * skontrolovat ci nezapisujem nadbytocne xobjecty kvoli vrstvam
 * co s posunom stran v rychlom mode
 * k vrstve: meno vo vertikalnom smere alebo udaj o vyske
 * podmienene nacitavanie scrapov v TeXu
 * dvojstrankovy mod (Title vzdy na pravej strane), vtedy akceptovat:
 * vkladanie prazdnych stran (lepsie: atribut prava/lava pre stranu)
 * exclude pages a vkladanie prazdnych stran: mena namiesto cisel stran (?)
 * zrusit OwnPages option (jedine spolu s predch. -- problem s funkciou
   find_excluded_pages) -- namiesto cisla strany makro \getpage{.} zvacsi 
   argument o hodnotu \the\pageno na 1. strane s mapou
 * 2. sokolie oko
 * register pouzitych nazvov - z blokov
                             - z vrstiev
 * vrstvy pod aktualnou: odlisit hlbku odtienmi sedej (?)
 * zhrnutie udajov o listoch na konci
 * mody pre preview (ziadne, linkovane, vsetky)
 * vrstva `pri' pre atlas

thsvg:

 * uxivatelsky prefix pre ids

 * fonty - orezavacie cesty + vkladanie?
 * layers cez javascript
 * (priehladnost -> SVG 1.2)

thconvert:

 * (dorobit) CONCAT + konverzia rozdelenych textovych riadkov
 * optimalizacia zmien fontov + gsave & al.


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
