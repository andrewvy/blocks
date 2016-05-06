CC=gcc
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TARGET := bin/blocks

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CC_LIB := -I/usr/local/include
CFLAGS := -g $(CC_LIB)

LIB := -L/usr/local/lib -lglfw3 -lGLEW -framework OpenGL -framework Cocoa -framework CoreVideo -framework IOKit
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
