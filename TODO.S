HNED:
+ k 2d objektu pridat typ objektu, na ktory sa vztahuje 
  -context point label
-------------------------------------------------------------------------------
+ pridat nove symboly:
  ciary - ceiling/floor-meander, rope (aj s kotveniami)
+ 2d: -adjust horizontal|vertical pre line point (zakazat v projection plan)
+ 2d: pridat do mapy moznost dat survey -> vtedy da len jeho polygon
+ 3d: zjednodusovat outlines
+ 3d: interpolovanie Z na zaklade triangulacie
+ 3d: interpolovanie U a D na zaklade sietovych algoritmov
+ 3d: point dimensions
+ 3d: up, down, left, right
+ 3d: model export aj centerline, walls, bbox
+ export model: -enable|disable [walls, centerline, bbox]
+ x: prerobit ovladanie xtheriona
+ x: vytiahnut co sa da do xtherion.ini
+ SQL: exportovat uzavrete okruhy (chyby, zoznam shotov a zoznam bodov)
+ export database: format text|txt
+ layout: [...] oznacovanie texovskych textov
* SVG - cvicna verzia (aspon outlines)
+ 2d: pospajat zoznamy na joinovanie, prv ako sa zacne joinovat
+ 2d: otacanie vystupnej mapy aj sipky na sever
+ exportovat grid do mapy/atlasu a aj jeho styl

  
HOTOVO:
+ francuzky preklad (Eric Madelaine)
+ BUG pri line splite odstraneny
+ odstranit seg-fault pri vlozeni dat bez surveya
+ invalid command context -> missing xxx command before yyy command
+ x: reload v 3D prehliadaci (Ctrl+R)
+ x: outline pocitat pre walls

POTOM:
+ x: nezobrazuje encoding pre thconfig
+ x: kontrolovat ci subor nebol modifikovany vonku. Ak hej, zavriet 
  - nacitat - znova otvorit v tom istom pohlade (2D)
+ area blocks
+ systemy jednotiek
+ kontrolovat duplikovane body v scrape
+ preview beside
+ rychly mod pre kompilaciu - bez spustania MPostu a bez konverzie
+ rozsirit cislovanie suborov pre metapost na podadresare

KED TO BUDE TREBA:
+ pridat -map-level scrap -> rozvijanie na samotne scrapy


