xth calibrate bitmap: zlucit filtre img/pdf

Makefile: make library pri zmene prekladov (neaktualne therion.tcl!)

Pacher: 
  * obrovske patterny
  * interim bboxmargin:=0 bez skupiny pri l_wall_ice
  * systemove farbenie znaciek
  * --print-symbols: nepodchyti vsetky patterny 
    (nie vsetky znacky v LEGENDLISTE? -- chybaju v ALL_PATTERNS)

podpora NFSS pri --print-symbols (ENC_NEW.write_enc_files(); + skopirovat fonty)

do sql: ostatne flagy; commenty pre stations

(mutton) ATTR__scale pre vsetky znacky v MP

pri kresleni s xvi na pozadi: ak nad mer. bod v xvi vkladam dva mer. body, druhy
  ma nespravne meno

neumoznit shift 0 0 pre mapy

kontrola nulovych segmentov na ciare pre MP

stopy/palce pomocou '/"

pri ctrl-p aktivovat point dialog a po vlozeni bodu aplikovt zmeny

grades pre polygon aj fixne body

meno obrazku do uvodzoviek: -sketch [Sketches/Greenlink Plan J2 sketch 210707.jpg] 0 -784

sketche do samostatneho layera

map-comments v spravnom poradi


konvertor: 
  serie precislovat na jednoduche nazvy bodov
HEEB: 
  premenovat subor na pth (?)
  pri manipulacii s fix bodmi (pridanie; najprv fix potom pripojenie na koncovy
    pri merani od konca) thexport len "FIX\r\n"
/  chyba spojnica k priecnemu rezu
/  v priecnom reze chybaju shoty a mer. bod
/  preco sa vo vsetkych nacrtkoch opakuje bod 0 (fix?)
/  thexport dodrziavat uhlove jednotky, 
/  dat tam trip comment, 
/  opakovat 3 merania polygonu, 
/  problem s pripojenim roznych dist

neprebera sa attr do survey ... - entrance ...

ak je continuation s explored udajmi, nezobrazit otaznik ale dlzku

xtherion ma do scrapu doplnit autora + rok! (pokial tam nie je)

pridat margin option pre uvodne strany atlasu, potom dat pagesetup do
  1) data.tex za opacity a scale s normalnym okrajom
  2) do th_pagedef s overlapom
  3) do legendy s normalnym


altitude scaling

ak je label prazdny retazec, zhuci mp?

nejde naraz zadat mapy elev. v dvoch projekciach

LOG-subor: 
X m surveyed (of which Y approximate) + about Z unsurveyed but explored
(see the continuation-list and survey-list for details)

do xtherionu info o tom ktory ini precital, + warning ak nerozparsoval ini

xtherion linux -- delete nezmaze oznaceny blok textu, len znak na kurzore

label k line section

preco nepouzivame if else v Makefile namiesto komentovania casti?

pri centerline so zadanou deklinaciou alebo data nosurvey nepozadovat
datum na vypocet automatickej deklinacie

obvod scrapu a rozdelena stena 

spajanie ciar

*****************************************************************************
georeferencovanie bitmap aj pomocou jgw/pgw:

A world file file is a plain ASCII text file consisting of six values separated by newlines. The format is:

pixel X size
rotation about the Y axis (usually 0.0)
rotation about the X axis (usually 0.0)
negative pixel Y size
X coordinate of upper left pixel center
Y coordinate of upper left pixel center
      
************************************************
loch pada pri exporte

X Error of failed request:  GLXBadContext
Major opcode of failed request:  144 (GLX)
Minor opcode of failed request:  5 (X_GLXMakeCurrent)
Serial number of failed request:  70567
Current serial number in output stream:  70567
	
The program 'loch' received an X Window System error.
This probably reflects a bug in the program.
The error was 'GLXBadContext'.
(Details: serial 23 error_code 155 request_code 144 minor_code 5)
(Note to programmers: normally, X errors are reported asynchronously;
 that is, you will receive the error a while after causing it.
 To debug your program, run it with the --sync command line
 option to change this behavior. You can then get a meaningful
 backtrace from your debugger if you break on the gdk_x_error() function.)
		
