HNED:
+ odstranit bug s LRUD pri interleaved data (wookey)
+ switch ci to dat pred alebo za pri interleaved mode (alebo aspon
  start-up switch)
+ pozriet sa na zuby prazdnym riadkom
+ debugovacie mody (-D bude terajsie -d, -d bude vykreslovat 
  obrysy a meracske body pred posuvanim a pred joinami) - makra:
  dbg_visible_wall, dbg_invisible_wall, dbg_station  
+ otacanie vystupnej mapy aj sipky na sever
+ [...] oznacovanie texovskych textov
+ x: pridat button move to scrap alebo strom
+ x: area edit window
+ x: kontrolovat ci subor nebol modifikovany vonku. Ak hej, zavriet 
  - nacitat - znova otvorit v tom istom pohlade (2D)
+ -adjust horizontal|vertical pre line point (zakazat v projection plan)

HOTOVO:

POTOM:
+ systemy jednotiek
+ kontrolovat duplikovane body v scrape
+ navrhnut kroky pre 3D
+ skusit odprasit cavern
+ preview beside
+ prispoosobujuce sa body na krivke
+ rychly mod pre kompilaciu - bez spustania MPostu a bez konverzie
+ rozsirit cislovanie suborov pre metapost na podadresare
+ exportovat styl gridu - ked to bude podporovane v thpdf
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
