SUBDIRS = advanced simple 

messages: rc.cpp
	$(EXTRACTRC) */*.ui > rc.cpp
	$(XGETTEXT) rc.cpp */*.cpp */*.h -o $(podir)/kfileshare.pot 
