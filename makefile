NAME=chess
CC=cl
ODIR=obj
IDIR=W:/jadel/export/include
_DEPS=include
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))
CFLAGS=/I$(IDIR) 
DEBUGDEFS=/DDEBUG
RELEASEDEFS=/DRELEASE /O2
DEBUGLIBS=W:/jadel/export/lib/jadelmain.lib W:/jadel/export/lib/jadelDEBUG.lib
RELEASELIBS=W:/jadel/export/lib/jadelmain.lib W:/jadel/export/lib/jadel.lib
_OBJ=*.obj
OBJ=$($(wildcard patsubst %,$(ODIR)/%,$(_OBJ)))
source:=$(wildcard src/*.cpp)
debugobjects:=$(patsubst src/%.cpp,obj/debug/%.obj,$(source))
releaseobjects:=$(patsubst src/%.cpp,obj/release/%.obj,$(source))

SCRDIR=$(NAME)

.PHONY:
all: debug

obj/debug/%.obj: src/%.cpp
	$(CC) /c /Zi $(DEBUGDEFS) /EHsc /Iinclude $(CFLAGS) /std:c++latest $^ /Foobj/debug/ 
	
obj/release/%.obj: src/%.cpp
	$(CC) /c /Zi $(RELEASEDEFS) /EHsc /Iinclude $(CFLAGS) /std:c++latest $^ /Foobj/release/

.PHONY:
remakejadelr:
	(cd W:/jadel && make cleanrelease release)

.PHONY:
remakejadeld: 
	(cd W:/jadel && make cleandebug debug)

.PHONY:
jadeld: remakejadeld copyjadeld cleandebug debug

.PHONY:
jadel: jadeld

.PHONY:
objectsdebug: $(debugobjects)

.PHONY:
objectsrelease: $(releaseobjects)

.PHONY:
debug: $(debugobjects)
	$(CC) /Zi $^ $(DEBUGDEFS) $(DEBUGLIBS) /Febin/$(NAME)DEBUG.exe /link /SUBSYSTEM:WINDOWS

.PHONY:
d: debug

.PHONY:
dfull: cleandebug debug

.PHONY:
release: $(releaseobjects)
	$(CC) /Zi $^ $(RELEASEDEFS) $(RELEASELIBS) /Febin/$(NAME).exe /link /SUBSYSTEM:WINDOWS

.PHONY:
r: release

.PHONY:
rfull: cleanrelease release

.PHONY:
jadelr: remakejadelr copyjadelr rfull

.PHONY:
copyjadeld: cleandebug
	copy W:\jadel\export\lib\jadelDEBUG.dll bin\jadelDEBUG.dll

.PHONY:
copyjadelr: cleanrelease
	copy W:\jadel\export\lib\jadel.dll bin\jadel.dll

.PHONY:
copyjadel: copyjadeld

.PHONY:
cleanrelease:
	del obj\\release\\*.obj bin\\$(NAME).exe

.PHONY:
cr: cleanrelease

.PHONY:
cleandebug:
	del obj\\debug\\*.obj  bin\\$(NAME)DEBUG.exe

.PHONY: 
cd: cleandebug

.PHONY:
clean: cleandebug cleanrelease