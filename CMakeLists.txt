
add_subdirectory( advanced ) 
add_subdirectory( simple ) 




########### install files ###############





#original Makefile.am contents follow:

#SUBDIRS = advanced simple 
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name '*.ui'` > rc.cpp
#	$(XGETTEXT) rc.cpp `find . -name '*.cpp' -or -name '*.h'` -o $(podir)/kfileshare.pot 
