* namiesto "cout <<" pouzivat thprintf - aby to slo aj do log suboru
* pri vypisoch medzery " ... "
* pri definovani globalnych funkcii, typov a premennych pouzivat prefix "th" 
  resp. "TH" (napr. thprint_tex_encodings, THFONTS)

thpdf:

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
 * graficka mierka + sever (minimalne pre samostatne mapy 
     do samostatneho PDF ako legendu?)
 * mody pre preview (ziadne, linkovane, vsetky)
 * vrstva `pri' pre atlas


thconvert:

 * (dorobi») CONCAT + konverzia rozdelenych textovych riadkov
 * optimalizacia zmien fontov + gsave & al.


thtexfonts:

 * prepinanie fontov v makrach definovanych uzivatelom v TeXu a MP
   (\scalebar, equipment, continuation) aj pre cisla stran v atlase
 * optimalizacia zmien fontov
 * zaviest kvazi HTML znackovanie vstupnych retazcov? <it>..<it>, <br> a pod.
 

mpost:

 * alignment pri continuation a equipement
 * slope - 
   poriadne funguje ak su vo vsetkych bodoch smery -1 (vtedy su luce kolme)
    alebo sa smer (velmi) nemeni; s dlzkou problemy nie su
   (bude treba vymysliet algoritmus na priemerovanie krajnych hodnot, kedze to, 
   ktorym poloblukom pojdes, zavisi od orientacie krivky)
 * sectionline cez kontrolne body -> inak
 * dorobit aby fungovalo predefinovanie u,v,w - pri polygone, nadm.vyskach, 
   presumed wall, patternoch


----- prezitky dob minulych -----

 * mod pre _velke_ mapy (>10MB): scrapy vysadzat do samostatneho suboru a z 
   dalsieho na ne odkazovat pomocou Reference XObject forms -> nefunguje v AR
 * navigacne sipky vypustit z tlacenej verzie (Image/Alternates/DefaultForPrint
   alebo rozne mody) 
