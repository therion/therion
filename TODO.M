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
 * vylepsit legendu (clenenie); mozno do samostatneho suboru
 * zhrnutie udajov o listoch na konci
 * ramik a stvorcova siet (kriziky): vylepsit alebo cez MetaPost 
   (do uvahy aj convergence)
 * mody pre preview (ziadne, linkovane, vsetky)
 * vrstva `pri' pre atlas


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

 * doladit viacstlpcove makra
 * sipka na sever je zbytocna pri priecnom a rozvinutom reze. V ich pripade
   by tam mal byt nejaky identifikator projekcie  
 * northarrow do atlasu?
 

mpost:

 * poriesit odsadenie bodovych znaciek a najma labelov
 * dorobit aby fungovalo predefinovanie u,v,w - pri presumed wall, patternoch
 * alignment pri continuation a equipement
 * slope - 
   poriadne funguje ak su vo vsetkych bodoch smery -1 (vtedy su luce kolme)
    alebo sa smer (velmi) nemeni; s dlzkou problemy nie su
   (bude treba vymysliet algoritmus na priemerovanie krajnych hodnot, kedze to, 
   ktorym poloblukom pojdes, zavisi od orientacie krivky)
 * initialize(Scale) nepomaha na lejble v legende -- prepisovanie mptexpre.tex?

thbook:

 * SQL export v kap. What we get
 * varovania texu a metapostu
 * obrazky mapovych znaciek

----- prezitky dob minulych -----

 * mod pre _velke_ mapy (>10MB): scrapy vysadzat do samostatneho suboru a z 
   dalsieho na ne odkazovat pomocou Reference XObject forms -> nefunguje v AR5

