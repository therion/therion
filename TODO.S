HNED:
+ xtherion: vkladanie objektov mimo scrapu
+ vytahovat typy bodov do mapy z centerline-u
+ mark <station1> .. <stationN> <type>
+ point snow
+ area blocks, snow, ice
+ line ceiling/floor-meander, rope (aj s kotveniami)
+ therion --protractor <options>
+ pozriet sa co by obnasal DXF export
+ vytvorit novy 3D prehliadac aj 3D export type - loch
+ export vsetkych znackovych klucov
+ komplet navrhnut a zmenit sposob 3D exportu (layout, thm uplne inak)
+ vyber povrchov na zaklade toho co exportujeme 
  2D: vsetky co pokryvaju, zoradene podla pokrytia a velkosti
  3D: najmensi z tych co pokryvaju najviac, orezany bboxom + okraj
      (10%)
+ povrch do 3Dcka ako surface (s priehladnostou)
+ xtherion: switch na perspektivu
+ xtherion: switche na viditelnost, zobrazenie a priehladnost povrchu
+ xtherion: roozne vyfarbovanie jaskyne (podla hlbky alebo zelenou)
+ 2d: otacanie vystupnej mapy aj sipky na sever v layoute
+ upravit definiciu sipky a mierky
+ LRUD
+ spravit LRUD outline do 3D exportu 
  -> kazdej zamere priradi zoznam bodov na zaciatku a na konci a
     z nich hladke rury
  -> urobi convex hull pre vsetky spojnicove body a vytvori z nich
     dalsie hladke telesa
+ export model: -wall-source map:projection|centerline|none|all
+ 2d: vymysliet a spravit ako dat survey do mapy aj s LRUD obalkou
+ vymysliet a spravit point camera/picture
+ 3d: vyskusat nacitavanie chybajucich LRUD zo scrapov + automaticke 
      dogenerovanie neexistujucich dat
+ 3d: interpolovanie Z na zaklade triangulacie (treba rychle priesecniky)
+ 3d: zjednodusovat outlines
+ 3d: interpolovanie U a D na zaklade sietovych algoritmov
+ 3d: point dimensions
+ 3d: up, down, left, right
+ 3d: model export aj centerline, walls, bbox
+ x: prerobit ovladanie xtheriona
+ x: vytiahnut co sa da do xtherion.ini
+ SQL: exportovat uzavrete okruhy (chyby, zoznam shotov a zoznam bodov)
+ export database: format text|txt
+ layout: [...] oznacovanie texovskych textov
+ 2d: pospajat zoznamy na joinovanie, prv ako sa zacne joinovat
+ exportovat grid do mapy/atlasu a aj jeho styl
  
LRUD:
+ nove flagove data v centerline:
  - shape: oval, rectangle, diamond, triangle, tunnel
  - vertical-treshold (kedy sa LRUD na zmeni na WENS): 0 - 90
+ do datalegu pridu nasledujuce data:
  from-UDLR, to-UDLR, sklon-rezu (najprv uhol kedy sa to meni,
  v postprocessingu na presny sklon), smer-up-u v postprocessingu,
  section-type, walls-on
+ novy data type dimensions [station up down left right] 
  len na vytvaranie lokalneho station listu
+ vytvaranie UDLR obalky v thdb1d:
  -najvyssiu prioritu ma to co je zadane
  -potom udaj z dimension listu v data objekte (ten je 
   specifikovany najprv explicitne, potom implicitne
  -nakoniec udaj z centralneho dimension listu
  => algoritmus:
   -vytvorit lokalne dimension listy (zo shotov)
   -vytvorit globalny dimension list
   -prejst vsetky zamery a doplnit tam kde existuju lokalne
    najprv z lokalnych a potom z globalnych (suradnicu po suradnici)
   -ak ani jeden - walls-on na false. Ak aspon jeden - walls-on na true 
    a ostatne vymysliet:
    ->ak iba jeden => vsetky rovnake
    ->ak dva protilahle => ostatne dva v priemere
    ->ak dva vedla seba => ostatne dva rovnake
    ->vyska max 1.75 metra -> inak v zlatom reze
+ vysledok - pre kazdy data leg bude nastaveny walls-on atribut a ak 
  ak bude true, tak musia existovat vsetky udaje ohladne rezu


HOTOVO:
+ tex-env on|(off) (therion.ini - puziva sa iba pri win32)
+ centerline: mozna zmena nastaveni medzi group-endgroup
  plati skoro pre vsetko calibrate, units, sd, data, flags etc...
+ ked scrapy a ziadna mapa v projekcii - exportuje vsetky scrapy
  pod seba (mapa nie je potrebna)
+ xtherion: zvacseny tick na zaciatku ciary
+ xtherion: vkladanie noveho scrapu na zaciatok suboru
+ xtherion: error sources like hyperlinks (jumps to open file, or
  open file in text editor)
+ xtherion: opraveny bug ked sa maze posledny bod na ciare
+ xtherion: ked zmena typu ciray - mazu sa aj rotacie a velkosti

POTOM:
+ posuvanie map pri oznacovani pomocou selectu + spajacie sipky
+ x: nezobrazuje encoding pre thconfig
+ x: kontrolovat ci subor nebol modifikovany vonku. Ak hej, zavriet 
  - nacitat - znova otvorit v tom istom pohlade (2D)
+ area blocks
+ systemy jednotiek
+ kontrolovat duplikovane body v scrape
+ preview beside
+ rychly mod pre kompilaciu - bez spustania MPostu a bez konverzie
+ rozsirit cislovanie suborov pre metapost na podadresare
+ SVG - cvicna verzia (aspon outlines)


KED TO BUDE TREBA:
+ pridat -map-level scrap -> rozvijanie na samotne scrapy


