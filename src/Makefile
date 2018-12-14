SDCC=sdcc
SDLD=sdld
STM8FLASH=stm8flash

MCUTYPE=stm8
DEVICE=stm8s103f3
PROGRAMMER=stlinkv2

HEXFILE=status_light.hex
OBJECTS=main.rel
HEADERS=cmds.h

CFLAGS=--Werror
MFLAGS=-m$(MCUTYPE)
LFLAGS=-l$(MCUTYPE) --out-fmt-ihx

all: $(HEXFILE)
	cp $(HEXFILE) ..

$(HEXFILE): $(OBJECTS)
	$(SDCC) $(MFLAGS) $(LFLAGS) $(OBJECTS) -o $(HEXFILE)

clean:
	rm -f $(OBJECTS)
	rm -f *asm *cdb *lst *map *mem *rel *rst *sym *lk
	rm -f $(HEXFILE)


flash: $(OBJECTS)
	$(STM8FLASH) -c$(PROGRAMMER) -p$(DEVICE) -w $(OBJECTS)

%.rel: %.c $(HEADERS)
	$(SDCC) -c $(MFLAGS) $(CFLAGS) $<
