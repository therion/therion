HOT LIST:
+ verzia do thbooku
+ xtherion - pomenuvanie scrapov podla suborov
+ xtherion - vytiahnut do .ini velkost okna
+ xtherion - Ctrl+F - maximalizuje search a hodi focus do neho
+ xtherion - Ctrl+Del - aby del uz nebralo
+ color map-fg <criterion> <colormap>
  ked altitude - tak podla nastavenia gridu
+ line rope 
  anchor on/off/auto (point option)
  line-adjust on/off/auto
+ samples
+ ??? point stalactites, stalagmites, via-ferrata
+ ??? line rope - prepinac auto/manual, anchor on/off/auto pre kazdy bod (???)
+ ??? line fixed-ladder, rope-ladder, via-ferrata (??? nesmie byt krivka,
  maximalne 2 body)
+ checkovat ci rozmer scrapu nepresahuje MP limit - ak
  hej, tak nejaky rozumny error (vyplut v akej mierke minimalne
  sa to da zobrazit)
+ do scrapu pridat option -flip horizontal|vertical|(off), ktora ho pred
  kalibraciou flipne
+ grid top/bottom/off v layoute
+ pridat special symboly:
  s_hgrid() - na siet v plane
  s_vgrid() - na hor. ciary v bokorysoch (sirka X)
+ oznacovanie suradnic a hlbky
+ depth-bar [-100 - 200| off ] - podpora pre viac depthbarov
+ grid-bar [-100 - 200|off] [-100 - 200|off] - podpora pre viac
  s_depthbar() - rozdelit na casti po Z usekoch, viac db
  s_gridbar() - rozdelit po usekoch, viac gb
+ grid origin uvazovat pri exporte nadmorskych vysok aj Xiek a Yiek
+ vymysliet a spravit point camera/picture/hyperlink
+ export vsetkych znackovych klucov (PDF, ? HTML)
+ 3d: opravit walls on/off
+ 3d: point dimensions + UD z LRUD
+ statistiku meracov aj ked vlozene ako centerline
  -> ked vytvarame mapu k surveyu, vytvorit jej aj datamap strukturu
+ therion --protractor <options>
+ 2d: vymysliet a spravit ako dat survey do mapy aj s LRUD obalkou
+ layout: [...] oznacovanie texovskych textov
+ 2d: pospajat zoznamy na joinovanie, prv ako sa zacne joinovat
+ add rotation of 3D surface into surface calibration
+ add centerline flags for sink, spring and doline and map these
  flags to metapost symbols in export
+ xtherion: move config file editor to compiler, add warning about
  unused open files to log window
+ xtherion: remove empty lines and areas when saving the file
+ xtherion: shortcus for mostly used types/sybtypes
+ xtherion: rightclick on map-item with changes of types and subtypes
+ layout: color map-fg chapter|explo-date|topo-date
+ layout: rozsirit color map-fg na uzivatelsky definovane tabulku
  color map-fg <criterion> <colormap> <value> <value> ... <value>
  color map-fg <criterion> <color> <value> <color> ... <value> <color>
  - colormap <name>[:saturation]

WOULD BE NICE LIST:
+ find different distortion measure
+ xtherion: add join toolbar simmilar to area
+ xtherion: pohyb viacerymi bodmi sucasne
+ xtherion: move several objects betweens scraps
+ xtherion: get frozen from time to time - multithreading?
+ xtherion: splitting of curves with id
+ export database: format text|txt
+ vyber povrchov na zaklade toho co exportujeme 
  2D: vsetky co pokryvaju, zoradene podla pokrytia a velkosti
+ SQL: exportovat uzavrete okruhy (chyby, zoznam shotov a zoznam bodov)
+ ak je v layoute rotate pri bokoryse, tak sa prenesie do projekcie
+ ked LRUD tak export do nejakeho vektoroveho TCL formatu, ktory
  by sa dal nacitat rovnako ako obrazok na pozadi (body, polygon,
  siet a stanicenia)
+ xtherion: inak spravit ukoncenie vkladania ciary
+ posuvanie map pri oznacovani pomocou selectu + spajacie sipky
+ x: nezobrazuje encoding pre thconfig
+ x: kontrolovat ci subor nebol modifikovany vonku. Ak hej, zavriet 
  - nacitat - znova otvorit v tom istom pohlade (2D)
+ systemy jednotiek
+ kontrolovat duplikovane body v scrape
+ preview beside
+ rychly mod pre kompilaciu - bez spustania MPostu a bez konverzie
+ rozsirit cislovanie suborov pre metapost na podadresare
+ SVG - cvicna verzia (aspon outlines)
+ pridat -map-level scrap -> rozvijanie na samotne scrapy
+ reportovat error ked su dva rovnake body v scrape (v plane a poodoryse)


===============================================================================
Internal remarks and algorithm proposals
===============================================================================


MY CHANGES:

+ xtherion: ked sa druhy krat klikne na prave vlozeny bod, 
  koniec vkladania bodov
+ xtherion: premenovat noname0X na (new file)
+ xtherion: prilinkovat eventy na zltu ciarocku
+ xtherion: odstranene size controls z pointu a r-size control
  z line-u
+ xtherion: vylepsene automaticke increasovane stationov
+ xtherion: pri vkladani bodov v xtherione sa ich cisla automaticky
  increasuju (alebo decreasuju, v rozvinutom reze)
+ xtherion: v mapovom a textovom editore sa pri sejvovani checkuju
  timestampy
+ xtheroin: meno suboru s koncovkou .mp a .tex nebrat do uvahy pri error
  messages
+ xtherion: ak sa oznaci ciara - drawing area sa scrollne na jej koncovy bod
+ xtherion: novy scrap sa vklada za sucastny
+ xtherion: odstraneny bug pri vkladani typov plooch
+ xtherion: me te File -> Auto save (ak zapnute, kazdu minutu)
+ xtherion: dlhsi list typu (dlzka listu v xtherion.ini)
+ xtherion: maximalizovanie toolbarov pri focuse
+ xtherion: Ctrl-D v map editore zmaze pismenko - odstraneny bug
+ xtherion: zopar novych itemov v xtherion.ini (sirka toolbaru)
+ xtherion: new shortcuts
  insert area -- ctrl-a
  insert scrap -- ctrl-r
  scroll up/down in side panel -- pageup/down
  scroll up/down in file commands window -- shift pageup/down


SAMPLES:
+ cislovanie XXYYY - XX - cislo, YYY - 555 by default
+ vymysliet sofistikovanejsi system testov (s HTML vystupom)
  kapitola 
    TITLE - popis kapitoly
    DESCRIPTION - ak existuje, tak v nom bude popis kapitoly
  podkapitola
    TITLE - popis kapitoly
    DESCRIPTION - ak existuje, tak v nom bude popis podkapitoly
    SOURCE - zoznam suborov, ktore idu do zdrojoveho archivu
    PNNNNN - paragrapf XXX, pricom prvy riadok obsahuje
             nazov suboru, z ktoreho treba spravit thunbail a v
             nasledujucich riadkoch nasleduje HTML popis
+ make config -> vytvori zoznam adresarov, v ktorych sa pusta make
  (kde existuje makefile)
+ make -> v kazdom adresari spusti make
+ nakoniec zavola skript, ktory vytori adresar html a v nom
  nasledujucu strukturu stranok
  index.html -> TOC
  XXXXX.html -> stranka s danou kapitolou
  SRCNNNNN.xxx -> zdrojove subory
  IMGNNNNN.png -> pngcka v 96dpi
  THBNNNNN.png -> thunbails do 160 x 120 bodov?
+ Input structure
  * Hello world! - basic principles
  * Surveye
  * Scrapy
    + obrys scrapu
    + vnutorny obrys scrapu
    + ina ciara ako obrys vonkajsi/vnutorny
    + neviditelna stena
  * Kalibracia scrapov
    + viac meracskych bodov
    + jeden meracsky bod
    + flip
    + ziadna projekcia - bez meracskych bodov
    + debug - distortion
    + debug - invalid station name
    + debug - duplicate station name
  * Spajanie scrapov
    + spojenie 2 scrapov
    + spojenie 3 a viac scrapov
    + spojenie symbolov na spojnici scrapov
    + debug - invalid join
  * Mapy
    + vkladanie scrapov
    + vkladanie surveyov
    + vkladanie map
    + previews
  * Konstrukcia rozvinuteho rezu
    + jeden scrap doprava
    + vetva dolava
    + druhy scrap
    + defaultne spojenie scrapov
    + custom spojenie scrapov
+ Map symbols
  * point
  * line
  * area
  * steny 
  * vyskopis (zrazy, gradienty, vysky, nadm. vysky)
  * priecne rezy
  * lejble
  * vyplne + voda
  * specialne symboly (datum, prievan, vchod, pokracovanie)
  * vyzdoba
  * vystroj
+ Map layout
  * scaling & rotation
  * symbols
  * legends
  * colors & transparencies
  * bitmaps

