# documentation
DOXYGEN = /usr/local/bin/doxygen
DOXYFILE = Doxyfile
DOCDIR = doc

BINDIR := bin
SOURCEDIR := src

MDRIVERDIR := driver_motor
DDRIVERDIR := driver_dht

all: localsystem drivers documentation

localsystem:
	$(MAKE) -C $(SOURCEDIR)

drivers:
	$(MAKE) -C $(MDRIVERDIR)
	$(MAKE) -C $(DDRIVERDIR)

documentation:
	$(DOXYGEN) $(DOXYFILE)

# remove all built files
clean:
	rm -f -r $(BINDIR)/* $(DOCDIR)/*