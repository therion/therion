HNED:
+ do section line pridat moznost head (begin,end,none,both)
+ viacnasobne spajanie scrapov (join s1 s2 -count 2)

TROCHU NESKOOR:
+ rozlisit makra pre tex-mapu, tex-atlas a metapost (neskoor SVG)
+ rychly mod pre kompilaciu - bez spustania MPostu a bez konverzie
+ vypustenie priecnych rezov pre mapu aj atlas do layoutu
  * vypustit section scrapy a section lines
+ vyplnit zvysne polia pre thpdf
+ rozsirit cislovanie suborov pre metapost na podadresare
+ zjednotit export textov do TeXu - pripravit sa na fonty
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
+ rozsirene make install 
  * odstranit z perlu co sa da - aby nebol potrebny na kompilaciu (ani tclko)

