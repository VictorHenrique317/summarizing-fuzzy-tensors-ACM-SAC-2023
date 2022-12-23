NAME = num-noise
CC = g++ -O3 -Wall -Wextra -Weffc++ -std=c++98 -pedantic
SRC = src
OBJ = $(SRC)/num-noise.o
DATA = COPYING INSTALL makefile README

.PHONY: install clean dist-gzip dist-bzip2 dist
.SILENT: $(NAME) install clean dist-gzip dist-bzip2 dist

%.o: %.cpp
	$(CC) -c -o $@ $<

$(NAME): $(OBJ)
	$(CC) -o $@ $^
	echo "$(NAME) build!"

install: $(NAME)
	mv $(NAME) /usr/bin
	echo "$(NAME) installed!"

clean:
	rm -f $(SRC)/*.o *~ $(SRC)/*~

dist-gzip: clean
	tar --format=posix --transform 's,^,$(NAME)/,' -czf $(NAME).tar.gz $(SRC) $(DATA)

dist-bzip2: clean
	tar --format=posix --transform 's,^,$(NAME)/,' -cjf $(NAME).tar.bz2 $(SRC) $(DATA)

dist: dist-bzip2