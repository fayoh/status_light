SDCC=sdcc
SDLD=sdld
STM8FLASH=stm8flash
MCUTYPE=stm8
DEVICE=stm8s103f3
PROGRAMMER=stlinkv2


OBJECTS=main.ihx

.PHONY: all clean flash

all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)
	rm -f *asm *cdb *lst *map *mem *rel *rst *sym *lk

flash: $(OBJECTS)
	$(STM8FLASH) -c$(PROGRAMMER) -p$(DEVICE) -w $(OBJECTS)

%.ihx: %.c
	$(SDCC) -l$(MCUTYPE) -m$(MCUTYPE) --out-fmt-ihx $(CFLAGS) $(LDFLAGS) $<
