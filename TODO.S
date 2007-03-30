HOT LIST:

therion:

+ thbook: map in offset (<map> <offset> <preview>)
+ thbook: point/line map-connection
+ thbook: air-draught: subtype winter|summer|none
+ thbook: layout survey-level
+ thbook: point extra -dist <distance> -from <station>
+ thbook: area flowstone, moonmilk
+ thbook: line moonmilk
+ thbook: layout survey-level
+ thbook: config-file sketch-warp line|plquette
+ thbook: 2D contour DXF export
+ thbook: LRUD walls in PDF (area dimensions)

+ user defined points/lines/areas
+ allow scraps without stations, CS specification for scrap (in plan
  projection)
+ add rotation of 3D surface into surface calibration
+ add gradient to area
+ add line moonmilk and flowstone areas surrounded by line flowstone or moonmilk
+ solve transparency of sketches
+ add more layers to DXF export (lines, points, areas?) with
  some symbols
+ morphing: add additional features (line extra, point extra:point|distance)
+ add morphing features browsing code
+ import of 3D and PLT via lxFile
+ LRUD envelope generation by lxFile
+ add absolute coordinate specification in centerline and scraps
+ LRUD walls into 3D shapefiles
+ areas into shapefiles
+ associate line section with section scrap
+ solve section names and sections far from passage
+ add links to NWES page numbers in atlas
+ point camera
+ point picture/hyperlink
+ not to use -walls off when PLT file is exported
+ NOT_LRUD flag to lox shots, when there exists shot point in some
  scrap with outline
+ allow -text option with continuation
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
  
xtherion:

+ do not save empty lines/areas
+ groups of symbol types
+ import of DXF/SVG into map editor
+ window initialization geometry
+ warn if symbols are outside of scraps
+ autoincrease scrap names (not scrapN)
+ area indicator to map editor (type and rotation)
+ fix MacOS X key bindings
+ disable "new file" and "open" when one file is opened in map editor
+ multiplatform init files
+ shortcuts and macros for common actions
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

+ export database: format text|txt, dbf
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

