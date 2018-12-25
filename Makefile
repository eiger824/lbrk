CXX          := g++
CXXFLAGS     := -c -g -Wall -Wextra -Wpedantic -std=c++1z
LDFLAGS      := -g -Wall -Wextra -Wpedantic -std=c++1z

PROGRAM      := lbrk
OBJS         := lbrk.o main.o

OUTDIR       := bin
INSTALL_DEST := /usr/bin
TARGET       := $(addprefix $(OUTDIR)/, $(PROGRAM))
OBJS         := $(addprefix $(OUTDIR)/, $(OBJS))

all: $(OUTDIR) $(TARGET)

$(OUTDIR):
	test -d $(OUTDIR) || mkdir -p $(OUTDIR)

$(TARGET): $(OBJS) 
	$(CXX) -o $@ $^ $(LDFLAGS)
	rm -f $(shell basename $@)
	ln -s $@

$(OUTDIR)/%.o: %.cc %.hh
	$(CXX) -o $@ $< $(CXXFLAGS)

$(OUTDIR)/%.o: %.cc
	$(CXX) -o $@ $< $(CXXFLAGS)

install:
	install -m0755 $(PROGRAM) $(INSTALL_DEST)

uninstall:
	test -f $(INSTALL_DEST)/$(PROGRAM) && rm -f $(INSTALL_DEST)/$(PROGRAM)

clean:
	rm -rf $(TARGET) *~ $(OUTDIR)

test:
	@echo "Test: read from stdin behaves similarly as reading from file."
	bash -c "diff <(./lbrk -tuw40 data/foo.txt) <(cat data/foo.txt | ./lbrk -tuw40)"
