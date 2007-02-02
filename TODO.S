HOT LIST:

therion:

+ output of under/overlying maps moved to a different place and draw
  joining arrows to previews  
+ LRUD walls into 3D shapefiles
+ import of 3D and PLT via lxFile
+ LRUD envelope generation by lxFile
+ additional morphing features (+LRUD morphing)
+ add absolute coordinate specification in centerline and scraps
+ add line moonmilk and flowstone areas surrounded by line flowstone or moonmilk
+ associate line section with section scrap
+ areas into shapefiles
+ import: origin [x y z units]
+ point camera
+ point picture/hyperlink
+ redo tests directory structure
+ not to use -walls off when PLT file is exported
+ NOT_LRUD flag to lox shots, when there exists shot point in scrap
+ some comment to continuation
+ add fast mode for xtherion - just to generate survey and map structures
+ warning if reading/backreading difference is big
+ no joins where starting/ending points are identical
+ new surface symbols:
  line dripline
  line cliff
  line landform
  line surface-contour (-altitude??)
+ map coloring 
  - add user defined color tables into configuration files
  - chapter|explo-date|topo-date colormodes
  - saturation as color parameter
  - use grid settings in altitudes
+ 2d: LRUD envelope of exported survey maps
+ add bookmarks to PDF files (maps?)
+ add user define map headers into layout (meta language simmilar
  to HTML)
+ add rotation of 3D surface into surface calibration
+ 3d: fix bug with internal hole, where no part of it is wall
+ area flowstone
+ line rope 
  anchor on/off/auto (point option)
  line-adjust on/off/auto
+ line fixed-ladder, rope-ladder, via-ferrata
+ depth-bar [position| off ] - support for multiple depth-bars
+ grid-bar [position|off] [position|off] - support for multiple grid bars
  s_depthbar() - divide into pieces (to fit into figure size)
  s_gridbar() - divide into pieces (to fit into figure size)
+ therion --protractor <options>
+ add centerline flags for sink, spring and doline and map these
  flags to metapost symbols in export
+ new centerline symbols
  point spring
	point sink
	point lake
	point doline
	point entrance
	point comment
+ add statistics survey <survey-name>
+ add survey name under scrap name when debug scrap-names
  
xtherion:

+ fix insertion of objects outside scraps
+ shortcuts
+ right click on line
+ undo/redo buttons
+ rightclick on map-item with changes of types and subtypes
+ context sensitive controls for most commonly used options
  (subtype, text, name, visibility, align, scale, ...)
+ rebuild compiler (no more cp text editor)
+ let xtherion compilation run without configuration file 
	(only with opened files)
+ translation of types in the ComboBox
+ remove empty scraps/lines/areas when saving the file
+ default scrap names by previous scrap
+ Ctrl+F binding in text/map editor - switch focus to Find 
  control panel and maximize it


WOULD BE NICE LIST:

therion:

+ find different distortion measure
+ export database: format text|txt
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

