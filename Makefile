PKGS=upower-glib
CFLAGS_COMMON=-Wall -Wextra -std=gnu99 -pedantic #-ggdb
CFLAGS=$(CFLAGS_COMMON) $(shell pkg-config --cflags $(PKGS))
LDFLAGS=$(shell pkg-config --libs $(PKGS)) -lX11 -lXext

sleeper:

.PHONY: clean
clean:
	rm -f ./sleeper

.PHONY: install
install:
	mkdir -p ~/.config/systemd/user/
	cp sleeper.service ~/.config/systemd/user/
	systemctl --user stop sleeper
	cp sleeper ~/bin/
