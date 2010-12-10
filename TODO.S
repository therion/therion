HOT LIST:

+ interpolovane LRUD na zaklade shotov
+ vylucit z interpolacie tie shoty, ktore nemaju 
  oba body explicitne zadane (cez stations uid)

+ layout pri exporte XHTML
+ priradenie farieb mapam
+ priame proj4 retazce do CS
+ calibracia surface v lochu
+ survey selection v loch-u
+ loch entrance/fix/station/label off-screen rendering
+ splayshoty previest na LRUD
+ jazyky parsovat na vystupe - odskusat
+ chybaju meracske body v legende

+ thbook: units up|down|left|right|dimensions
+ thbook: splay shots using "-" or "." as station name
+ thbook: in therion.ini otf2pfb on|off
+ thbook: in therion.ini pdf-fonts
+ thbook: -filter (on|off)
+ thbook: more join description
+ thbook: flags arch & overhang
+ thbook: export cave-list -surveys (on)|off
+ thbook: symbol-colour
+ thbook: new symbol groups

+ include maps with previews
+ loch surface handling
+ add options menu to xtherion
+ support iso date format YYYY-MM-DD
+ add lookup tables to configuration files
+ check label size switch and tex export
+ add grid-range to layout
+ fix bug with default layout size/page-setup etc...
+ KML table export
+ samples.doc: add coordinates of stations to cave list
+ create cave penetration distance calculation code + label
+ sort caves in export according to source position!
+ add shot type (leg flag) to attribute export
+ layout: default text orientation switch 
+ automatic label flip if they are up-side down according to given orientation
+ attributes sample
+ show/hide U symbols
+ add cave structure to continuation-list
+ add -source on|(off) to table export
+ user defined attributes to shots
+ scrap morphing algorithm setup to configuration file
+ 3D generation algorithm - do not remove join points
+ 3D generation algorithm - take xelevation data into account
+ scrap name with "_" exported with "." when debug scrap-names
+ fix hash export in AutoCAD DXF file format	
+ fix usage of extra morphing points (sometimes are ignored)
+ support cross sections (LRUD) in XVI
+ allow (direction + distance) instead of LRUD
+ polyline to bezier conversion mode
+ more complex attribute export
+ link same continuations within different projections
+ symbol show/hide of user defined symbols (using subtypes)
+ add shot/stations flags to attributes when exported in metapost
+ add -survey-lines on|off|auto option to scrap - auto by default -
  if there is some line survey, it will be off
+ add rotation of 3D surface into surface calibration
+ negative values to point-dimensions
+ add selections/unselections to export queue
+ redo database export
+ add gradient to area
+ add line moonmilk and flowstone areas surrounded by line flowstone or moonmilk
+ solve transparency of sketches
+ add more layers to DXF export (lines, points, areas?) with
  some symbols
+ add legends to SVG export
+ import of 3D and PLT via lxFile
+ LRUD envelope generation by lxFile
+ LRUD walls into 3D shapefiles
+ associate line section with section scrap
+ solve section names and sections far from passage
+ add links to NWES page numbers in atlas
+ point camera
+ point picture/hyperlink
+ not to use -walls off when PLT file is exported
+ NOT_LRUD flag to lox shots, when there exists shot point in some
  scrap with outline
+ warning if reading/backreading difference is big
+ no joins where starting/ending points are identical
+ new centerline station flags: spring, sink, lake, doline
+ special symbols centerline features: entrance, spring, sink, 
  lake, doline
+ new surface symbols:
  line dripline
  line cliff
  line landform
  line surface-contour (-altitude??)
+ individual coloring of previews
+ map coloring 
  - add user defined color tables into configuration files
  - chapter|explo-date|topo-date colormodes
  - saturation as color parameter
  - use grid settings in altitudes
+ add bookmarks to PDF files (maps?)
+ add user define map headers into layout (meta language simmilar
  to HTML)
+ 3d: fix bug with internal hole, where no part of it is wall
+ point rock or block
+ line rope 
  anchor on/off/auto (point option)
  line-adjust on/off/auto
+ line fixed-ladder, rope-ladder, via-ferrata
+ depth-bar [position| off ] - support for multiple depth-bars
+ grid-bar [position|off] [position|off] - support for multiple grid bars
  s_depthbar() - divide into pieces (to fit into figure size)
  s_gridbar() - divide into pieces (to fit into figure size)
+ add centerline flags for sink, spring and doline and map these
  flags to metapost symbols in export
+ add statistics survey <survey-name>
+ add survey name under scrap name when debug scrap-names
+ therion --protractor <options>
+ MacOS X installator (e.g. http://s.sudre.free.fr/Software/Iceberg.html)
+ single temporary directory, change name if process file exists
+ remove temp-dir warning
  
xtherion:

+ vtopo import
+ generate error if therion is not found
+ search & replace toolbar to compiler
+ moving split bar in compiler window
+ commands for tabbing/commenting out blocks of texts
+ rightclick on map-item with changes of types and subtypes
+ multiselection for properties changing
+ fix file saving when drive/device have been unplugged
+ fix: the scrollbar does not appear automatically in
  compilation window after a compile. We must resize the window or go back
  in image window (and go back in compile window) to make it appear
+ save/load xtherion startup script to $HOME/.xtherion
+ edit line -> to bezier / to polyline option
+ simple line digitizer
+ add map editor import filter
+ SVG import into map editor (using tcldom)
+ do not save empty lines/areas/scraps
+ Ctrl+F binding in text/map editor - switch focus to Find 
  control panel and maximize it
+ add "Filter" and "Unfilter" button for search & select
+ add filter menus
+ warn if symbols are outside of scraps
+ autoincrease scrap names (not scrapN)
+ area indicator to map editor (type and rotation)
+ fix MacOS X key bindings
+ disable "new file" and "open" when one file is opened in map editor
+ multiplatform init files
+ shortcuts and macros for common actions
+ right click on line
+ undo/redo buttons
+ context sensitive controls for most commonly used options
  (subtype, text, name, visibility, align, scale, ...)
+ rebuild compiler (no more cp text editor)


WOULD BE NICE LIST:

therion:

+ navigation pane labeling
+ redo database export
+ morphing: add additional features (line extra, point extra:point|distance)
+ export rigging list + -text option to point anchor
+ add morphing features browsing code
+ add fast mode for xtherion - just to generate survey and map structures
+ surface selection according to coverage
  2D: all surfaces that covers current map sorted by size and coverage
  3D: make a surface selection and clipping
+ export all information database
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
+ add encoding control of configuration file
+ solve splitting of curves with id
+ let xtherion compilation run without configuration file 
  (only with opened files)
