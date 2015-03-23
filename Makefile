DYNAMIC = -lSDL_ttf -lSDL_mixer -lSDL_image -lSDL -lm
CFLAGS = -O3 -fsingle-precision-constant
GENERAL_TWEAKS = -ffast-math
#==PC defaults==
FLAGS = -g -DDESKTOP $(GENERAL_TWEAKS)
SDL = `sdl-config --cflags`

LEGO_CC = /opt/pandora/arm-2011.09/bin/arm-none-linux-gnueabi-gcc
LEGO_BASE = ../Ev3-base
LEGO_INCLUDE = -I$(LEGO_BASE)

SPARROW_FOLDER = ../../sparrow3d
SPARROW3D_LIB = libsparrow3d.so
SPARROWNET_LIB = libsparrowNet.so
SPARROWSOUND_LIB = libsparrowSound.so

ifdef TARGET
include $(SPARROW_FOLDER)/target-files/$(TARGET).mk

BUILD = ./build/$(TARGET)
SPARROW_LIB = $(SPARROW_FOLDER)/build/$(TARGET)/sparrow3d
else
TARGET = "Default (change with make TARGET=otherTarget. See All targets with make targets)"
BUILD = .
SPARROW_LIB = $(SPARROW_FOLDER)
endif
LIB += -L$(SPARROW_LIB)
INCLUDE += -I$(SPARROW_FOLDER)
DYNAMIC += -lsparrow3d

CFLAGS += $(PARAMETER) $(FLAGS)

all: control_client control_server
	@echo "=== Built for Target "$(TARGET)" ==="

targets:
	@echo "The targets are the same like for sparrow3d. :P"

control_client: control_client.c makeBuildDir
	cp -u $(SPARROW_LIB)/$(SPARROW3D_LIB) $(BUILD)
	$(CC) $(CFLAGS) $(LINK_FLAGS) $< $(SDL) $(INCLUDE) $(LIB) $(STATIC) $(DYNAMIC) -o $(BUILD)/$@$(SUFFIX)


control_server: control_server.c makeBuildDir
	$(LEGO_CC) control_server.c $(LEGO_BASE)/d_lcd.o $(LEGO_INCLUDE) -o $(BUILD)/control_server

makeBuildDir:
	 @if [ ! -d $(BUILD) ]; then mkdir -p $(BUILD);fi

clean:
	rm -f *.o
	rm -f control_client
