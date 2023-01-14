
# vim: ft=make noexpandtab

MDEMO_C_FILES := src/main.c src/demos.c

CC_GCC := gcc
CC_WATCOM := wcc386
LD_WATCOM := wcl386
MD := mkdir -p

GLOBAL_DEFINES := -DRETROFLAT_OPENGL -DDEBUG_LOG

CFLAGS_GCC := $(GLOBAL_DEFINES) -Imaug/src
CFLAGS_WATCOM := $(GLOBAL_DEFINES) -imaug/src
LDFLAGS_GCC := -lm $(shell pkg-config opengl --libs)
LDFLAGS_WATCOM :=

# Optional builds.
ifneq ("$(RELEASE)","RELEASE")
	CFLAGS_WATCOM += -we -d3
	CFLAGS_GCC += -DDEBUG -Wall -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
	LDFLAGS_GCC += -g -fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

ifeq ("$(SDL_VER)","2")
	CFLAGS_GCC += -DRETROFLAT_API_SDL2 $(shell pkg-config sdl2 --cflags)
	:DFLAGS_GCC += $(shell pkg-config sdl2 --libs) -lSDL_ttf
else
	CFLAGS_GCC += -DRETROFLAT_API_SDL1 $(shell pkg-config sdl --cflags)
	LDFLAGS_GCC += $(shell pkg-config sdl --libs) -lSDL_ttf
endif

# Target-specific options.
.PHONY: clean

all: mdemogl mdemglnt.exe

# Unix

mdemogl: $(addprefix obj/$(shell uname -s)/,$(subst .c,.o,$(MDEMO_C_FILES)))
	$(CC_GCC) -o $@ $^ $(LDFLAGS_GCC)

obj/$(shell uname -s)/%.o: %.c
	$(MD) $(dir $@)
	$(CC_GCC) -c -o $@ $< -DRETROFLAT_OS_UNIX $(CFLAGS_GCC)

# WinNT

mdemglnt.exe: $(addprefix obj/nt/,$(subst .c,.o,$(MDEMO_C_FILES)))
	wlink name $@ system nt_win libr opengl32 fil {$^}

obj/nt/%.o: %.c
	$(MD) $(dir $@)
	wcc386 -bt=nt -i$(WATCOM)/h/nt -DRETROFLAT_API_WIN32 -DRETROFLAT_OS_WIN $(CFLAGS_WATCOM) -fo=$@ $(<:%.c=%)

# Clean

clean:
	rm -rf obj mdemogl *.err mdemglnt.exe

