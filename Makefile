
# vim: ft=make noexpandtab

OPENGL := 1
C_FILES := src/main.c src/demos.c src/poly.c

LIBS_GCC += -lm

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mdemogl.sdl mdemglnt.exe

# Unix

$(eval $(call TGTUNIXSDL,mdemogl))

# WASM

$(eval $(call TGTWASMSDL,mdemogl))

# WinNT

$(eval $(call TGTWINNT,mdemgl))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

