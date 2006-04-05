HOT LIST:

therion:

+ some comment to continuation
+ report line number when error
+ shp file export
+ warning if reading/backreading difference is big
+ no joins where starting/ending points are identical
+ fix 64bit BUGs
+ area bedrock
+ new surface symbols:
  line dripline
  line cliff
  line landform
  line surface-contour (-altitude??)
+ new centerline symbols
  point spring
	point sink
	point lake
	point doline
	point entrance
	point comment
+ map coloring 
  - add user defined color tables into configuration files
  - chapter|explo-date|topo-date colormodes
  - saturation as color parameter
  - use grid settings in altitudes
+ 2d: LRUD envelope of exported survey maps
+ add bookmarks to PDF files (maps?)
+ export scrap walls into XVI images
+ point camera/picture/hyperlink
+ plt and 3d import: read LRUD data and survey structure a special way
+ add user define map headers into layout (meta language simmilar
  to HTML)
+ add rotation of 3D surface into surface calibration
+ 3d: fix bug with internal hole, where no part of it is wall
+ new 3D export structure (layout-3d, OpenGL wxWidgets 3D viewer)
+ area -gradient [fx fy tx ty] option
+ area flowstone, bedrock
+ redo tests directory structure
+ add projection to metapost - different symbols in elevation and plan
  (stalactites)
+ line rope 
  anchor on/off/auto (point option)
  line-adjust on/off/auto
+ line fixed-ladder, rope-ladder, via-ferrata
+ depth-bar [position| off ] - support for multiple depth-bars
+ grid-bar [position|off] [position|off] - support for multiple grid bars
  s_depthbar() - divide into pieces (to fit into figure size)
  s_gridbar() - divide into pieces (to fit into figure size)
+ export all symbol sets is some reasonable format (PDF, ? HTML)
+ therion --protractor <options>
+ add centerline flags for sink, spring and doline and map these
  flags to metapost symbols in export
+ import: shift x y z units
+ automatic selection of top-level maps up to basic maps if exists
+ add statistics survey <survey-name>
+ add survey name under scrap name when debug scrap-names
  
xtherion:

+ xvi shifting BUG
+ auto shift picture when inserted
+ undo/redo buttons
+ no selection when file is opened in editor
+ rightclick on map-item with changes of types and subtypes
+ context sensitive controls for most commonly used options
  (subtype, text, name, visibility, align, scale, ...)
+ modify layout according to Dzach improvements
+ thconfig file modified outside therion
+ let xtherion compilation run without configuration file 
	(only with opened files)
+ translation of types in the ComboBox
+ remove empty lines and areas when saving the file
+ shortcus for mostly used types/sybtypes
+ default scrap names constructed from file name
+ initial window size into .ini file
+ Ctrl+F binding in text/map editor - switch focus to Find 
  control panel and maximize it
+ add gradient control to area when supported


WOULD BE NICE LIST:

therion:

+ find different distortion measure
+ export database: format text|txt
+ surface selection according to coverage
  2D: all surfaces that covers current map sorted by size and coverage
  3D: make a surface selection and clipping
+ export database - export loop closure data
+ output of under/overlying maps in a different place + draw joining
  arrows to previews
+ add support for output units (date format, feets etc...)
+ preview beside
+ add support for multiple metapost files (more than 4096 scraps)
+ add -map-level scrap to select -> each scrap will be a separate map
+ multiple selections in thconfig file
+ error when 2 simmilar points in scrap

xtherion:

+ add endscrap name to the scrap
+ add join toolbar simmilar to area
+ select and move several points together
+ select and move several objects betweens scraps together
+ get frozen from time to time - multithreading?
+ solve splitting of curves with id
+ add encoding control of configuration file
+ new way of line point insertion end
+ add warning about unused open files to log window


===============================================================================
Internal remarks - do not try to understand :)
===============================================================================

TESTS:
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
+ make - spusti make v kazdom adresari a 
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
    + plan
    + elevation
    + extended elevation
    + vkladanie surveyov
    + vkladanie scrapov
    + vkladanie map
    + previews
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
  * special symbols
  * legends
  * colors
  * transparencies
  * bitmaps
  * vector images

