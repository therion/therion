HNED:
+ pridat line - water: permanent, conjuctural, intermittent
+ neexportovanie hidenutych symbolov + symbol-show
+ jazyk exportu (vystupov)
  * <therion|jazyk>[/special]: "text"
  * thlang.h/cxx kde bude nejake makro na preklad
  * tcl skript, ktory prehlada zdrojaky, najde texty v makrach, 
    nacita sucasne preklady a cele to apdejtne, pricom
    vytvori aj skript, ktory nastavi jazykovy hash
+ generovanie pouzitych znaciek
  * objekt znackoveho kluca
  * v ktorom ku kazdej projekcii pouzite znacky
  * a jeho export do MPostu a TeXu - kombinacie znaciek
  * zakompilovane lokalne nazvy
+ mapove mody - default, outlines
  * vynechat texty
  * vypustenie priecnych rezov pre mapu aj atlas do layoutu
    (vypustit section scrapy a section lines)
  * vynechat vsetko okrem obrysov

HOTOVO:
+ v layote:
  symbol-defaults <KLUC>
  symbol-hide <point|line|area> <ID>
  symbol-assign <point|line|area> <ID> <KLUC>
+ cislovanie MP suborov po jednom
+ colour|color <item> <COLOR>
  * items: map-fg, map-bg [0-100]|[R G B]

POTOM:
+ debugovacie mody
+ navrhnut kroky pre 3D
+ skusit odprasit cavern
+ preview beside
+ do section line pridat moznost head (begin,end,none,both) - ked to
  budu podporovat MP makra
+ rychly mod pre kompilaciu - bez spustania MPostu a bez konverzie
+ rozsirit cislovanie suborov pre metapost na podadresare + cislovanie zaradom
+ exportovat styl gridu - ked to bude podporovane v thpdf
+ statistika na mapy a scrapy 
  * thstatmap
  * kto kedy zameral a nakreslil (a kolko)
  * do layoutu dat pre celu mapu pocet meracov 
    a kreslicov vo vystupe
  * pocitame pri ich exporte v ???
* SVG - cvicna verzia
* rozsirena statistika (zakladna statistika
  + thstat (pre vybrane surveys resp. vsetky) 
  + meraci (od kedy do kedy, kolko do hromady pod zemou, na povrchu)
  + kreslici (od kedy do kedy, kolko)
  + roky (kolko m bolo zameranych)
  + uzavrete okruhy a chyby na nich
  + pocitame len pri exporte v ???

KED TO BUDE TREBA:
+ pridat do mapy moznost dat survey -> vtedy da len jeho polygon
+ pridat -map-level scrap -> rozvijanie na samotne scrapy
+ rozsirene make install 
  * odstranit z perlu co sa da - aby nebol potrebny na kompilaciu (ani tclko)
