thpdf-unix: Main.pl Generic.pm Convert.pm Build.pm
	cat Main.pl Generic.pm Convert.pm Build.pm \
           | egrep -f .pattern > ../../bin/thpdf
	chmod 755 ../../bin/thpdf

thpdf-win32: Main.pl Generic.pm Convert.pm Build.pm
	type Main.pl > ..\..\bin\thpdf
	type Generic.pm >> ..\..\bin\thpdf
	type Convert.pm >> ..\..\bin\thpdf
	type Build.pm >> ..\..\bin\thpdf
