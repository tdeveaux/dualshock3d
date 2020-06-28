dualshock3d: main.o
	cc -framework CoreFoundation -framework IOKit -o dualshock3d main.o

main.o: main.c
	cc -c -Wall main.c

.PHONY: clean
clean:
	rm -f dualshock3d main.o

.PHONY: install
install: dualshock3d
	sudo mkdir -p /usr/local/bin
	sudo install dualshock3d /usr/local/bin
	cp com.tdeveaux.dualshock3d.plist ~/Library/LaunchAgents
	launchctl load ~/Library/LaunchAgents/com.tdeveaux.dualshock3d.plist

.PHONY: uninstall
uninstall:
	launchctl unload ~/Library/LaunchAgents/com.tdeveaux.dualshock3d.plist
	rm -f ~/Library/LaunchAgents/com.tdeveaux.dualshock3d.plist
	-killall dualshock3d
	sudo rm -f /usr/local/bin/dualshock3d
	-sudo rmdir /usr/local/bin
