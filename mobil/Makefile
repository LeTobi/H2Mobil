control.o: control.cpp definitions.h
	g++ control.cpp -o control.o -std=c++11 -c -I/home/pi

mutual.o: mutual.cpp definitions.h
	g++ mutual.cpp -o mutual.o -std=c++11 -c -I/home/pi

video.o: video.cpp definitions.h
	g++ video.cpp -o video.o -std=c++11 -c -I/home/pi

mobil: main.cpp definitions.h control.o mutual.o video.o
	g++ main.cpp -o mobil -std=c++11 -I/home/pi control.o mutual.o video.o -L/home/pi/tobilib -ltc -lpthread -lboost_system -lboost_thread -ljpeg