CFLAGS=-Wall -Wextra -pedantic
LDFLAGS=-lX11 -lXext

sleeper:

.PHONY: install
install:
	mkdir -p ~/.config/systemd/user/
	cp sleeper.service ~/.config/systemd/user/
	systemctl --user stop sleeper
	cp sleeper ~/bin/
