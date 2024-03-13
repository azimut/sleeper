CC      := clang
PKGS	:= upower-glib libpq
CFLAGS	:= -Wall -Wextra -std=gnu99 -pedantic -ggdb $(shell pkg-config --cflags $(PKGS))
LDFLAGS := -lX11 -lXext $(shell pkg-config --libs $(PKGS))
SRC     := $(wildcard src/*.c)
HDR     := $(wildcard src/*.h)

sleeper: $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)

.PHONY: clean
clean: ; rm -f sleeper

.PHONY: install
install: sleeper
	mkdir -p $(HOME)/.config/systemd/user/
	install -C sleeper.service $(HOME)/.config/systemd/user/
	systemctl --user daemon-reload
	systemctl --user stop sleeper
	cp sleeper $(HOME)/bin/
	systemctl --user start sleeper

.PHONY: deps
deps: ; sudo dnf install -y upower-devel

compile_commands.json:
	bear -- make
