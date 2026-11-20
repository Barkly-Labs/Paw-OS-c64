CC = cl65
CFLAGS = -t c64 -O -I include
OUT = build/pawos.prg

SOURCES = src/main.c \
          src/paw_ui.c \
          src/paw_memory.c \
          src/paw_sid.c \
          src/paw_dtmf.c \
          src/paw_phone.c \
          src/paw_synth.c \
          src/paw_disk.c \
          src/paw_files.c \
          src/paw_commands.c \
          src/paw_puppy_egg.c

.PHONY: all clean

all: $(OUT)

$(OUT): $(SOURCES)
	mkdir -p build
	$(CC) $(CFLAGS) -o $(OUT) $(SOURCES)

clean:
	rm -f $(OUT)
