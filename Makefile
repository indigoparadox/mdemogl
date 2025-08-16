
# vim: ft=make noexpandtab

RETROFLAT_OPENGL := 1
#RETROFLAT_SMALL3DL := 1
#RETROFLAT_DIRECT3D := 1
C_FILES := src/main.c src/demos.c src/poly.c src/dm_cube.c src/dm_obj.c src/dm_watr.c src/dm_retra.c src/dm_spher.c

LIBS_GCC += -lm
DEFINES_GCC_NDS := -DDEMOS_NO_LISTS -DDEMOS_NO_FILES -DDEMOS_NO_LIGHTS -DRETROFLAT_NDS_WASD
# TODO: Fix code so it works with SDL2/resizing.
SDL_VER_UNIX := 1
GLOBAL_DEFINES += -DRETROFLAT_DEFAULT_SCREEN_W=800 -DRETROFLAT_DEFAULT_SCREEN_H=600
GLOBAL_DEFINES += -DMAUG_CLI_ARG_LIST_SZ_MAX=40
#GLOBAL_DEFINES += -DMFILE_TRACE_LVL=1
#GLOBAL_DEFINES += -DRETROFLAT_NO_STRING
#GLOBAL_DEFINES += -DDEMOS_NO_LIGHTS
#GLOBAL_DEFINES += -DDEMOS_NO_LISTS
#GLOBAL_DEFINES += -DRETROGLU_TRACE_LVL=1
#GLOBAL_DEFINES += -DRETRO3DP_TRACE_LVL=1
#GLOBAL_DEFINES += -DRETRO3D_TRACE_LVL=1
GLOBAL_DEFINES += -DMDEMO_COMMIT_HASH="\"$(shell git rev-parse HEAD)\""
#GLOBAL_DEFINES += -DMFMT_TRACE_BMP_LVL=1
#GLOBAL_DEFINES += -DMFIX_DEBUG

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

