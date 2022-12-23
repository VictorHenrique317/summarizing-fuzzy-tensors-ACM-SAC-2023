NAME = paf
PAF = paf
NCLUSTERBOX = nclusterbox
# CXX = clang++ -O3 -flto -Wall -Wextra -Weffc++ -std=c++11 -pedantic -Wno-unused-parameter
# CXX = g++ -O3 -flto -Wall -Wextra -Weffc++ -std=c++11 -pedantic -Wno-unused-parameter
CXX = g++ -g -Og -Wall -Wextra -Weffc++ -std=c++11 -pedantic -Wno-unused-parameter
EXTRA_CXXFLAGS = -lboost_program_options
HELP2MAN_PAF = help2man -n 'Grow patterns, which hold in a fuzzy tensor, hierarchically amalgamate them and select a subset of the built patterns to sum up this tensor, a disjunctive box cluster model' -N
HELP2MAN_NCLUSTERBOX = help2man -n 'Modify patterns, which hold in a fuzzy tensor, to maximize their explanatory powers and select a subset of the built patterns to sum up this tensor, a disjunctive box cluster model' -N
SRC = src
COMMON_SRC = $(SRC)/utilities $(SRC)/common
PAF_SRC = $(SRC)/$(PAF)
NCLUSTERBOX_SRC = $(SRC)/$(NCLUSTERBOX)
DEPS = $(wildcard $(patsubst %,%/*.h,$(COMMON_SRC))) $(wildcard $(patsubst %,%/*.h,$(PAF_SRC))) $(wildcard $(patsubst %,%/*.h,$(NCLUSTERBOX_SRC))) Parameters.h Makefile
COMMON_CODE = $(wildcard $(patsubst %,%/*.cpp,$(COMMON_SRC)))
PAF_CODE = $(wildcard $(patsubst %,%/*.cpp,$(PAF_SRC)))
NCLUSTERBOX_CODE = $(wildcard $(patsubst %,%/*.cpp,$(NCLUSTERBOX_SRC)))
COMMON_OBJ = $(patsubst %.cpp,%.o,$(COMMON_CODE))
PAF_OBJ = $(patsubst %.cpp,%.o,$(PAF_CODE))
NCLUSTERBOX_OBJ = $(patsubst %.cpp,%.o,$(NCLUSTERBOX_CODE))
ALL = $(DEPS) $(COMMON_CODE) $(PAF_CODE) $(NCLUSTERBOX_CODE) $(PAF).1.gz $(NCLUSTERBOX).1.gz COPYING INSTALL README slice-input fiber-input

.PHONY: man_$(PAF) man_$(NCLUSTERBOX) man install_$(PAF) install_$(NCLUSTERBOX) install clean dist-gzip dist-bzip2 dist-xz dist
.SILENT: all $(PAF) $(NCLUSTERBOX) man_$(PAF) man_$(NCLUSTERBOX) man install_$(PAF) install_$(NCLUSTERBOX) install clean dist-gzip dist-bzip2 dist-xz dist

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $<

all: man_$(PAF) man_$(NCLUSTERBOX)

$(PAF): $(COMMON_OBJ) $(PAF_OBJ)
	$(CXX) -o $@ $^ $(EXTRA_CXXFLAGS)
	echo "$(PAF) built!"

$(NCLUSTERBOX): $(COMMON_OBJ) $(NCLUSTERBOX_OBJ)
	$(CXX) -o $@ $^ $(EXTRA_CXXFLAGS)
	echo "$(NCLUSTERBOX) built!"

man_$(PAF): $(PAF)
	$(HELP2MAN_PAF) ./$(PAF) | gzip > $(PAF).1.gz

man_$(NCLUSTERBOX): $(NCLUSTERBOX)
	$(HELP2MAN_NCLUSTERBOX) ./$(NCLUSTERBOX) | gzip > $(NCLUSTERBOX).1.gz

man: man_$(PAF) man_$(NCLUSTERBOX)

install_$(PAF): man_$(PAF)
	mv $(PAF) /usr/bin
	cp $(PAF).1.gz /usr/share/man/man1/
	echo "$(PAF) installed!"

install_$(NCLUSTERBOX): man_$(NCLUSTERBOX)
	mv $(NCLUSTERBOX) /usr/bin
	cp $(NCLUSTERBOX).1.gz /usr/share/man/man1/
	echo "$(NCLUSTERBOX) installed!"

install: install_$(PAF) install_$(NCLUSTERBOX)

clean:
	rm -f $(patsubst %,%/*.o,$(COMMON_SRC)) $(patsubst %,%/*.o,$(PAF_SRC)) $(patsubst %,%/*.o,$(NCLUSTERBOX_SRC)) $(patsubst %,%/*~,$(COMMON_SRC)) $(patsubst %,%/*~,$(PAF_SRC)) $(patsubst %,%/*~,$(NCLUSTERBOX_SRC)) *~

dist-gzip: man
	tar --format=posix --transform 's,^,$(NAME)/,' -czf $(NAME).tar.gz $(ALL)

dist-bzip2: man
	tar --format=posix --transform 's,^,$(NAME)/,' -cjf $(NAME).tar.bz2 $(ALL)

dist-xz: man
	tar --format=posix --transform 's,^,$(NAME)/,' -cJf $(NAME).tar.xz $(ALL)

dist: dist-bzip2
