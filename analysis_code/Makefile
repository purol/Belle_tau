CXX = g++ -std=c++14
RM = rm -rf
CP = cp -f

BINDIR = ./bin
INCDIR = ./include
SRCDIR = ./src
TMPDIR = ./tmp
LIBDIR = ./lib

CFLAGS = `root-config --cflags` -I$(ROOTSYS)/include
LDFLAGS = `root-config --ldflags --glibs`
LIBS = -lRooFit -lRooStats -lRooFitCore -lMinuit -lFastBDT_static
INCLUDES = -I$(INCDIR) -I$(ROOTSYS)/include -I../Belle2_analysis/include -I../Belle2_analysis/include_FBDT

MAINS = $(wildcard $(SRCDIR)/*.cc)
TARGETS = $(MAINS:$(SRCDIR)/%.cc=$(BINDIR)/%)
OBJS = $(MAINS:$(SRCDIR)/%.cc=$(TMPDIR)/%.o)

.PHONY : all clean

all: $(BINDIR) $(TMPDIR) $(TARGETS)

$(BINDIR):
	@mkdir -p $(BINDIR)

$(TMPDIR):
	@mkdir -p $(TMPDIR)

clean:
	@ echo '<< cleaning directory >>'
	@ $(RM) *~ */*~ \#*\#* */\#*\#*
	@ $(RM) $(BINDIR)/* $(TMPDIR)/*

$(TARGETS): $(BINDIR)/% : $(TMPDIR)/%.o
	@ echo '<< creating executable $@ >>'
	@ $(CXX) -o $@ $< $(LDFLAGS) $(LIBS) $(INCLUDES) -L$(LIBDIR)
	@ echo '<< compilation succeeded!! >>'

$(OBJS): $(TMPDIR)/%.o : $(SRCDIR)/%.cc
	@ echo '<< compiling $@ >>'
	@ $(CXX) $(CFLAGS) $(INCLUDES) -L$(LIBDIR) -c $< -o $@
