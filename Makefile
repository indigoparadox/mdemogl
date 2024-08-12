
# vim: ft=make noexpandtab

OPENGL := 1
C_FILES := src/main.c src/demos.c src/poly.c

LIBS_GCC += -lm
DEFINES_GCC_NDS := -DDEMOS_NO_LISTS -DDEMOS_NO_FILES -DDEMOS_NO_LIGHTS -DRETROFLAT_NDS_WASD
GLOBAL_DEFINES := -DRETROFLAT_DEFAULT_SCREEN_W=800 -DRETROFLAT_DEFAULT_SCREEN_H=600 -DMAUG_CLI_ARG_LIST_SZ_MAX=40 -DRETROFLAT_NO_STRING
GLOBAL_DEFINES += -DRETROGLU_TRACE_LVL=1

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mdemogl.sdl mdemoglnt.exe mdemoglnts.exe mdemogl64.exe mdemoglntgl.exe

$(eval $(call TGTNDSLIBN,mdemogl,mdemogl.bmp))

# Unix

$(eval $(call TGTSDLICO,mdemogl))

$(eval $(call TGTUNIXSDL,mdemogl))

$(eval $(call TGTUNIXGLUT,mdemogl))

# WASM

$(eval $(call TGTWASMSDL,mdemogl))

# WinNT

$(eval $(call TGTWINICO,mdemogl))

$(eval $(call TGTWINNT,mdemogl))

$(eval $(call TGTWINNTGL,mdemogl))

$(eval $(call TGTWINNTGCC,mdemogl))

$(eval $(call TGT_GCC64_WIN_WIN32,mdemogl))

$(eval $(call TGTWINSDL,mdemogl))

$(eval $(call TGT_WATCOM386_WIN_GLUT,mdemogl))

# OS/2

$(eval $(call TGTOS2GL,mdemogl))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

