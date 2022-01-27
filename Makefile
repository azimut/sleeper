PKGS	:= upower-glib
CFLAGS	:= -Wall -Wextra -std=gnu99 -pedantic $(shell pkg-config --cflags $(PKGS))
LDFLAGS := -lX11 -lXext $(shell pkg-config --libs $(PKGS))

sleeper:

.PHONY: clean
clean:
	rm -f sleeper

.PHONY: install
install:
	mkdir -p $(HOME)/.config/systemd/user/
	cp sleeper.service $(HOME)/.config/systemd/user/
	systemctl --user stop sleeper
	cp sleeper $(HOME)/bin/
	systemctl --user start sleeper
