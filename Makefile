CC=gcc
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TARGET := bin/blocks

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CC_LIB := -I/usr/local/include
CFLAGS := -g -Wall $(CC_LIB)

LIB := -L/usr/local/lib -lSDL2 -framework GLUT -framework OpenGL
INC := -I include

$(TARGET): $(OBJECTS)
	  @echo " Linking..."
			@mkdir -p $(BINDIR)
		  @echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	  @mkdir -p $(BUILDDIR)
		  @echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	  @echo " Cleaning...";
		  @echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
