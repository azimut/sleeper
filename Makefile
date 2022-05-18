PKGS	:= upower-glib
CFLAGS	:= -Wall -Wextra -std=gnu99 -pedantic $(shell pkg-config --cflags $(PKGS))
LDFLAGS := -lX11 -lXext $(shell pkg-config --libs $(PKGS))
SRCS    := main.c file.c dpms.c upower.c suspend.c

sleeper: $(SRCS) config.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean: ; rm -f sleeper

.PHONY: install
install: sleeper
	mkdir -p $(HOME)/.config/systemd/user/
	install -C sleeper.service $(HOME)/.config/systemd/user/
	systemctl --user stop sleeper
	cp sleeper $(HOME)/bin/
	systemctl --user start sleeper

.PHONY: deps
deps: ; sudo dnf install -y upower-devel

compile_commands.json:
	bear make
