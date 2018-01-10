#
TOPDIR = ..
#
include $(TOPDIR)/make.inc

#
SUBDIRS =        \
      turple     \
      regex      \
      lambda     \
      array      \
      
#
.PHONY: all
all:
	$(MAKEALL_SUBDIRS)

#
UDOC :
	cd $(DOCDIR) && $(MAKE)

install :

.PHONY: clean
clean:
	$(MAKECLEAN_SUBDIRS)

