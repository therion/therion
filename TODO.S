HNED:
+ pre vstupny subor zadavat stupne:minuty:sekundy (12:23:23.123123)
+ nefunguje preview above pri savoltovych priestoroch pri -map-level basic
+ pokial je preview - tak je nastaveny na posledne
  aby isla cela mapa hore, len nieco dole :)))
+ nefunguje preview cez komplikovane mapy
+ pridat pre 2D data object option -visibility on|off
+ error ked iba jeden kontrolny bod ???
+ beside - aby sa zobrazovalo normalne, len by sa orezavalo a dalo
  by sa na neho skakat
+ navrhnut kroky pre 3D

HOTOVO:
+ v therion.ini
  tmp-path <PATH>
  tmp-remove <COMMAND>
+ ak nema specifikovany source => error
+ ak skonci s chybou, tak caka na ENTER z klavesnice
+ pridane teamove ulohy: instruments (insts), assistant (dog)
+ centerline/centreline je podporovane
+ v centreline nove data: up ceiling down floor left right ignore

TROCHU NESKOOR:
+ spisat ovladanie xtheriona (mys a pod.)
+ do section line pridat moznost head (begin,end,none,both) - ked to
  budu podporovat MP makra
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
* debugovacie mody
* skusit odprasit cavern
* 3D

V BUDUCNOSTI:
+ pridat do mapy moznost dat survey -> vtedy da len jeho polygon
+ pridat -map-level scrap -> rozvijanie na samotne scrapy

KED TO BUDE TREBA:
+ rozsirene make install 
  * odstranit z perlu co sa da - aby nebol potrebny na kompilaciu (ani tclko)
