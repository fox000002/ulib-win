#
TOPDIR = ..
#
include $(TOPDIR)/make.inc

#
SUBDIRS =        \
array \
auto \
begin \
bind \
decltype \
enum \
lambda \
meminit \
move \
nullptr \
rawstring \
regex \
static_assert \
turple \
type_traits \
      
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

