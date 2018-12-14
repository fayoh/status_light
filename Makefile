MAKE=make

HEXFILE=status_light.hex
PYCLIENT=statuslight_client*whl

.PHONY: clean flash

all: $(HEXFILE) $(PYCLIENT)

fw: $(HEXFILE)

client: $(PYCLIENT)

flash:
	$(MAKE) -C src flash

clean:
	$(MAKE) -C src clean
	$(MAKE) -C client/python clean
	rm -f $(HEXFILE)
	rm -f $(PYCLIENT)

$(HEXFILE):
	$(MAKE) -C src

$(PYCLIENT):
	$(MAKE) -C client/python
