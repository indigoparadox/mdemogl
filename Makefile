
# vim: ft=make noexpandtab

OPENGL := 1
C_FILES := src/main.c src/demos.c src/poly.c

LIBS_GCC += -lm
DEFINES_GCC_NDS := -DDEMOS_NO_LISTS -DDEMOS_NO_FILES

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mdemogl.sdl mdemglnt.exe

$(eval $(call TGTNDSLIBN,mdemogl))

# Unix

$(eval $(call TGTUNIXSDL,mdemogl))

# WASM

$(eval $(call TGTWASMSDL,mdemogl))

# WinNT

$(eval $(call TGTWINNT,mdemgl))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

