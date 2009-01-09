CHANGES:
  * new easy setup for OpenType and TrueType fonts
  * basic support for right-to-left scripts in labels and legend
  * PDF files now contain unicode mapping information to allow searching and 
    copying texts consisting of accented and non-latin characters

- kopirovat OTF fonty, kopirovat glyphtounicode.tex do thTMPDIR
- nastavit: NFSS (ak ini subor obsahuje pdf-fonts), 
            t1convert (pre TTF vzdy off),
            otf_file[]
- v ini mat moznost zadat: mpost-path "mpost -tex=etex"

opravene grecke preklady
2c2
< el:
---
> el: σημείο χαρτογράφησης
404c404
< el: υψομετρικά
---
> el: Υψομετρικά
407c407
< el: χάρτες
---
> el: Χάρτες
413c413
< el: μήκος
---
> el: Μήκος


-lGLU pre loch makefile

splay shoty su zobrazene v mape!
14 desatinnych miest pri niektorych bodoch v xtherione (mer. body gener. z xvi)

konvertor: 
  nezobrazuju sa bodky!
  chyba spojnica k priecnemu rezu
  v priecnom reze chybaju shoty
  serie precislovat na jednoduche nazvy bodov
HEEB: thexport dodrziavat uhlove jednotky, dat tam trip comment, opakovat 3 merania polygonu, problem s pripojenim roznych dist

neprebera sa attr do survey ... - entrance ...

ak pre survey dam -entrance 0 a mam "fix A ...; equate A 0"
p_station((559.09,-695.88),0,0,"","entrance");
p_station((559.09,-695.88),0,btex \thcomment \thfb\char83 \char116 \char97 \char114 \thfc\char253 \ \thfb\char68 \char114 \char105 \char101 \char110 \char111 \char107 \mainfont{} etex,"","entrance");
p_station((605.13,-681.79),0,0,"","entrance");
p_station((605.13,-681.79),0,btex \thcomment \thfb\char74 \char80 \thfc\char169 \mainfont{} etex,"","entrance");

ak je continuation s explored udajmi, nezobrazit otaznik ale dlzku

xtherion ma do scrapu doplnit autora + rok! (pokial tam nie je)

mierka v mape \def\scale{}: round, nie int

pridat margin option pre uvodne strany atlasu, potom dat pagesetup do
  1) data.tex za opacity a scale s normalnym okrajom
  2) do th_pagedef s overlapom
  3) do legendy s normalnym




preklady (SK, EN)

altitude scaling

zacyklena hlaska v xtherione pre option -size xl

ak je label prazdny retazec, zhuci mp?

+proj=merc +latts=0 +lon0=0 +k=1 +x0=0 +y0=0 +a=6378137 +b=6378137 +units=m +nadgrids=\@null +no_defs

nejde naraz zadat mapy elev. v dvoch projekciach

ATTR_xx_numeric?? - zrusit ATTR_xx_numeric, _transform

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
		
