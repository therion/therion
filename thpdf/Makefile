thpdf-unix: Main.pl Generic.pm Build.pm
	cat Main.pl Generic.pm Build.pm \
           | egrep -f .pattern > thpdf
	chmod 755 thpdf

thpdf-win32: Main.pl Generic.pm Build.pm
	type Main.pl > thpdf
	type Generic.pm >> thpdf
	type Build.pm >> thpdf
