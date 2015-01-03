CC = cc
CCLIBS = -lcrypto -lrt -DSHM  -D_POSIX_C_SOURCE=200112L 

ifeq ($(OS),Windows_NT)
	CCLIBS += -lintl
endif

BUILT_INFO=kancil-built.mak
BUILT_BAKDIR=built
VER_MAJOR=0
VER_MINOR=1
VER_PATCH=0

# Componen.
UTILITY_OBJ=faedah.o tampilan.o soket.o pesan.o

# Compiler flags.
CFLAGS =-std=c99
CCFLAGS_DEVEL = -O0 $(CFLAGS) -ggdb3 -Wall -pedantic -W -Wextra -DCOMPILE_MODE_DEVEL
CCFLAGS_BUILD_INTERNAL = -O2 $(CFLAGS) -g -Wall -DCOMPILE_MODE_BUILD_INTERNAL
#CCFLAGS_BUILD= -O2 $((CFLAGS)) -g -Wall
CCFLAGS_BUILD = -O2 $(CFLAGS) -DCOMPILE_MODE_BUILD

# Compiler mode selector.
ifeq ($(mode), built)
	CCFLAGS_0 = $(CCFLAGS_BUILD)
else ifeq ($(mode), internal)
	CCFLAGS_0 = $(CCFLAGS_BUILD_INTERNAL)
else
	mode = devel
	CCFLAGS_0 = $(CCFLAGS_DEVEL)
endif

# If nofork.
ifeq ($(nofork), yes)
	CCFLAGS_1 = $(CCFLAGS_0) -DNOFORK
else
	CCFLAGS_1 = $(CCFLAGS_0)
endif

# Architecture
# ARCH = $(shell getconf LONG_BIT)
# ARCH_32=-D_FILE_OFFSET_BITS=32
# ARCH_64=-D_FILE_OFFSET_BITS=64

# Combine.
CCFLAGS = $(CCFLAGS_1)
CCF=$(CC) $(CCFLAGS) $(ARCH_$(ARCH))

# Built info.
ifneq ($(wildcard $(BUILT_INFO)), )
	include $(BUILT_INFO)
endif
ifeq ($(VERSION_MAJOR),)
	VERSION_MAJOR=$(VER_MAJOR)
endif
ifeq ($(VERSION_MINOR),)
	VERSION_MINOR=$(VER_MINOR)
endif
ifeq ($(VERSION_PATCH),)
	VERSION_PATCH=$(VER_PATCH)
endif
ifeq ($(BUILT_PELADEN),)
	BUILT_PELADEN=1
endif
ifeq ($(BUILT_GERBANG),)
	BUILT_GERBANG=1
endif
ifeq ($(BUILT_KLIEN),)
	BUILT_KLIEN=1
endif

# Versioning
VERSIONING= \
	-D__VERSION_MAJOR=$(VERSION_MAJOR) \
	-D__VERSION_MINOR=$(VERSION_MINOR) \
	-D__VERSION_PATCH=$(VERSION_PATCH) \
	-D__BUILT_TIME=$(BUILT_TIME)
VERSION=$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

# Built time.
BUILT_TIME=$(shell date +'%s')

# Start.
all: information peladen klien clean
.PHONY: information built-info clean distclean-all

information:
ifneq ($(mode),built)
ifneq ($(mode),internal)
ifneq ($(mode),devel)
	@echo "Invalid build mode." 
	@echo "Please use 'make mode=built' or 'make mode=internal' or 'make mode=devel'"
	@exit 1
endif
endif
endif
	@echo "Build Kancil on "$(ARCH)"-bit "$(mode)" mode"
	@echo "...................................."
ifeq ($(nofork), yes)
	@echo "Add setting: nofork"
endif
	@echo "Built with flags:"
	@echo $(CCFLAGS) $(CCLIBS)

peladen:information built-peladen built-info
	@mkdir -p $(BUILT_BAKDIR)
	@echo "Create backup to $(BUILT_BAKDIR)/peladen-$(BUILT_TIME)-$(VERSION)-$(BUILT_PELADEN)-$(mode).bak."
	@cp -p peladen $(BUILT_BAKDIR)/peladen-$(BUILT_TIME)-$(VERSION)-$(BUILT_PELADEN)-$(mode).bak

klien:information built-klien built-info
	@mkdir -p $(BUILT_BAKDIR)
	@echo "Create backup to $(BUILT_BAKDIR)/klien-$(BUILT_TIME)-$(VERSION)-$(BUILT_KLIEN)-$(mode).bak."
	@cp -p klien $(BUILT_BAKDIR)/klien-$(BUILT_TIME)-$(VERSION)-$(BUILT_KLIEN)-$(mode).bak

built-klien:klien.c $(UTILITY_OBJ) klien-anak.o
	@echo "Build klien with built number $(BUILT_KLIEN)..."
	@ld -r $(UTILITY_OBJ) klien-anak.o -o klien.o
	@$(CCF) $(VERSIONING) -D__BUILT_NUMBER=$(BUILT_KLIEN) klien.o klien.c -o klien $(CCLIBS)

built-peladen:peladen.c $(UTILITY_OBJ) peladen-anak.o
	@echo "Build peladen with built number $(BUILT_PELADEN)..."
	@ld -r $(UTILITY_OBJ) peladen-anak.o -o peladen.o
	@$(CCF) $(VERSIONING) -D__BUILT_NUMBER=$(BUILT_PELADEN) peladen.o peladen.c -o peladen $(CCLIBS)

peladen-anak.o:peladen-anak.c
	@echo "Build $@..."
	@$(CCF) -c peladen-anak.c -o peladen-anak.o $(CCLIBS)

klien-anak.o:klien-anak.c
	@echo "Build $@..."
	@$(CCF) -c klien-anak.c -o klien-anak.o $(CCLIBS)

pesan.o:pesan.c
	@echo "Build $@..."
	@$(CCF) -c pesan.c -o pesan.o $(CCLIBS)

soket.o:soket.c
	@echo "Build $@..."
	@$(CCF) -c soket.c -o soket.o $(CCLIBS)

tampilan.o:tampilan.c
	@echo "Build $@..."
	@$(CCF) -c tampilan.c -o tampilan.o $(CCLIBS)

faedah.o:faedah.c
	@echo "Build $@..."
	@$(CCF) -c faedah.c -o faedah.o $(CCLIBS)
	

built-info:klien.c peladen.c
ifeq ($(MAKECMDGOALS), klien)
	@echo "VERSION_MAJOR=$(VERSION_MAJOR)"  > $(BUILT_INFO)
	@echo "VERSION_MINOR=$(VERSION_MINOR)"  >> $(BUILT_INFO)
	@echo "VERSION_PATCH=$(VERSION_PATCH)"  >> $(BUILT_INFO)
	@echo "BUILT_KLIEN=$$(( $(BUILT_KLIEN) + 1))" >> $(BUILT_INFO)
	@echo "BUILT_PELADEN=$(BUILT_PELADEN)" >> $(BUILT_INFO)
	@echo "BUILT_GERBANG=$(BUILT_GERBANG)" >> $(BUILT_INFO)
else ifeq ($(MAKECMDGOALS), peladen)
	@echo "VERSION_MAJOR=$(VERSION_MAJOR)"  > $(BUILT_INFO)
	@echo "VERSION_MINOR=$(VERSION_MINOR)"  > $(BUILT_INFO)
	@echo "VERSION_PATCH=$(VERSION_PATCH)"  > $(BUILT_INFO)
	@echo "BUILT_KLIEN=$(BUILT_KLIEN)" >> $(BUILT_INFO)
	@echo "BUILT_PELADEN=$$(( $(BUILT_PELADEN) + 1))" >> $(BUILT_INFO)
	@echo "BUILT_GERBANG=$(BUILT_GERBANG)" >> $(BUILT_INFO)
else
	@echo "VERSION_MAJOR=$(VERSION_MAJOR)"  > $(BUILT_INFO)
	@echo "VERSION_MINOR=$(VERSION_MINOR)"  >> $(BUILT_INFO)
	@echo "VERSION_PATCH=$(VERSION_PATCH)"  >> $(BUILT_INFO)
	@echo "BUILT_KLIEN=$$(( $(BUILT_KLIEN) + 1))" >> $(BUILT_INFO)
	@echo "BUILT_PELADEN=$$(( $(BUILT_PELADEN) + 1))" >> $(BUILT_INFO)
	@echo "BUILT_GERBANG=$(BUILT_GERBANG)" >> $(BUILT_INFO)
endif
	
clean:
	rm -rfv *.o

distclean:
	rm -rfv *.o peladen klien

distclean-all:
	rm -rfv *.o *.exe *.stackdump peladen klien $(BUILT_BAKDIR)
