MAKE=make

HEXFILE=status_light.hex
PYCLIENT=statuslight_client*whl

SUBDIRS= src client/python

.PHONY: all clean flash test fw

all: $(HEXFILE) $(PYCLIENT)

test: $(HEXFILE) $(PYCLIENT)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir test; \
	done

fw: $(HEXFILE)

client: $(PYCLIENT)

flash:
	$(MAKE) -C src flash

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
	rm -f $(HEXFILE)
	rm -f $(PYCLIENT)

$(HEXFILE):
	$(MAKE) -C src

$(PYCLIENT):
	$(MAKE) -C client/python
