#! /bin/sh
$EXTRACTRC `find . -name '*.ui'` >> rc.cpp || exit 11
$XGETTEXT rc.cpp `find . -name '*.cpp' -o -name '*.h'` -o $podir/kfileshare.pot 
