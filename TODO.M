MARTIN:
* odprasit /endinput z therion.tex-u
* zmenit vystup na stdout v thpdf.cxx a thconvert.cxx
* alignment pri continuation a equipement a up-down label
* mody pre preview (ziadne, linkovane, vsetky)

THPDF:
   * opravit cislovanie listov
   * Umbilicus
   * mod bez ramikov na velku mapu (automaticke nastavenie velkosti papiera,
     bez hyperlinkov; do uvahy jedine Umbilicus a siet s rozmermi oka Hsize*Vsize)
   * nastavitelna priehladnost v PDF (zaviest TeXovsku premennu \opacity=100...)
     zaviest TransparencyGroups ked bude opravena chyba v pdfTeXu
   * schema kladu listov (samost. subor, bez linkov, 1 alebo 2 listy;
     len pre tlacenu verziu)
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

----- prezitky dob minulych -----

 * mod pre _velke_ mapy (>10MB): scrapy vysadzat do samostatneho suboru a z 
   dalsieho na ne odkazovat pomocou Reference XObject forms -> nefunguje v AR
 * navigacne sipky vypustit z tlacenej verzie (Image/Alternates/DefaultForPrint
   alebo rozne mody) 

