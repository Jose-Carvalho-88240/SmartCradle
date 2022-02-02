# Directories
DOCDIR = doc
BINDIR := bin
SOURCEDIR := src
MDRIVERDIR := driver_motor
DDRIVERDIR := driver_dht

# Documentation
DOXYGEN = /usr/local/bin/doxygen
DOXYFILE = Doxyfile

######################################

all: localsystem drivers documentation

localsystem:
	mkdir -p $(BINDIR)
	$(MAKE) -C $(SOURCEDIR)

drivers:
	$(MAKE) -C $(MDRIVERDIR)
	$(MAKE) -C $(DDRIVERDIR)

documentation:
	mkdir -p $(DOCDIR)
	$(DOXYGEN) $(DOXYFILE)

# Remove all built files
clean:
	rm -rf $(BINDIR) $(DOCDIR)