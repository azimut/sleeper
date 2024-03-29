CC      := gcc
PKGS	:= upower-glib sqlite3
CFLAGS	:= -Wall -Wextra -std=gnu99 -pedantic -ggdb $(shell pkg-config --cflags $(PKGS))
LDFLAGS := -lX11 -lXext $(shell pkg-config --libs $(PKGS))
SRC     := $(wildcard src/*.c)
HDR     := $(wildcard src/*.h)
OS       = $(shell lsb_release -si)

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
deps:
ifeq ($(OS),Fedora)
	sudo dnf install -y upower-devel
else ifeq ($(OS),Ubuntu)
	sudo apt install -y libupower-glib-dev libx11-dev libxext-dev libsqlite3-dev
endif

compile_commands.json:
	bear -- make
