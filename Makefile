build:
	gcc -std=c99 src/main.c -Wall `pkg-config --cflags --libs  gtk+-3.0 webkit2gtk-4.0` -o jethelp
clean:
	rm jethelp
install:
	sudo mkdir /usr/share/jethelp/ -vp
	sudo cp ui/* /usr/share/jethelp/
	sudo cp css/* /usr/share/jethelp/
	sudo mv jethelp /usr/bin
	
