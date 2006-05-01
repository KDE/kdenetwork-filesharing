#! /bin/sh
$EXTRACTRC `find . -name '*.ui'` >> rc.cpp || exit 11
$XGETTEXT rc.cpp `find . -name '*.cpp' -or -name '*.h'` -o $podir/kfileshare.pot 
