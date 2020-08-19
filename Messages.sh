#! /bin/sh
$XGETTEXT  `find . -name '*.cpp' -o -name '*.h' -o -name '*.qml' -o -name '*.js'` -o $podir/kfileshare.pot
