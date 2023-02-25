
# vim: ft=make noexpandtab

OPENGL := 1
C_FILES := src/main.c src/demos.c src/poly.c

LIBS_GCC += -lm
DEFINES_GCC_NDS := -DDEMOS_NO_LISTS -DDEMOS_NO_FILES -DDEMOS_NO_LIGHTS

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mdemogl.sdl mdemglnt.exe

$(eval $(call TGTNDSLIBN,mdemogl,,mdemogl.bmp))

# Unix

$(eval $(call TGTUNIXSDL,mdemogl))

# WASM

$(eval $(call TGTWASMSDL,mdemogl))

# WinNT

$(eval $(call TGTWINNT,mdemgl))

$(eval $(call TGTWIN64GCC,mdemgl))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

