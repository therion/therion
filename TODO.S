HNED:
+ skusit HOMEDRIVE + HOMEPATH - ked to neni HOME
+ kontrolovat ci retazce vo vstupnych suboroch su korektne --
  najmae pre utf-8 vstupne kodovanie
+ do section line pridat moznost head (begin,end,none,both)
+ rozlisit makra pre tex-mapu, tex-atlas a metapost (neskoor SVG)
+ viacnasobne spajanie scrapov (join s1 s2 -count 2)

HOTOVO:

TROCHU NESKOOR:
+ urobit Linuxovske RPMka
+ rychly mod pre kompilaciu - bez spustania MPostu a bez konverzie
+ vypustenie priecnych rezov pre mapu aj atlas do layoutu
  * vypustit section scrapy a section lines
+ rozsirit cislovanie suborov pre metapost na podadresare
+ exportovat styl gridu - ked to bude podporovane v thpdf
+ statistika na mapy a scrapy 
  * thstatmap
  * kto kedy zameral a nakreslil (a kolko)
  * do layoutu dat pre celu mapu pocet meracov 
    a kreslicov vo vystupe
  * pocitame pri ich exporte v ???
+ jazyk exportu (vystupov) do therion.ini
  * pripravit spravu jazykov
+ znackove kluce
  * objekt znackoveho kluca
  * v ktorom ku kazdej projekcii pouzite znacky
  * a jeho export do MPostu a TeXu - kombinacie znaciek
  * zakompilovane lokalne nazvy
  
POTOM:
* SVG - cvicna verzia
* rozsirena statistika (zakladna statistika
  + thstat (pre vybrane surveys resp. vsetky) 
  + meraci (od kedy do kedy, kolko do hromady pod zemou, na povrchu)
  + kreslici (od kedy do kedy, kolko)
  + roky (kolko m bolo zameranych)
  + uzavrete okruhy a chyby na nich
  + pocitame len pri exporte v ???
* fonty v texu
* znackove kluce
* debugovacie mody
* skusit odprasit cavern
* 3D

V BUDUCNOSTI:
+ pridat do mapy moznost dat survey -> vtedy da len jeho polygon
+ pridat -map-level scrap -> rozvijanie na samotne scrapy

KED TO BUDE TREBA:
+ rozsirene make install 
  * odstranit z perlu co sa da - aby nebol potrebny na kompilaciu (ani tclko)
