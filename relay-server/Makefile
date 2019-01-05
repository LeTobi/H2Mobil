ctrl_core.o: ctrl_core.cpp definitions.h
	g++ ctrl_core.cpp -c -std=c++11 -o ctrl_core.o -I/home/pi

ctrl_captain.o: ctrl_captain.cpp definitions.h
	g++ ctrl_captain.cpp -c -std=c++11 -o ctrl_captain.o -I/home/pi

ctrl_vehicle.o: ctrl_vehicle.cpp definitions.h
	g++ ctrl_vehicle.cpp -c -std=c++11 -o ctrl_vehicle.o -I/home/pi

ctrl_viewer.o: ctrl_viewer.cpp definitions.h
	g++ ctrl_viewer.cpp -c -std=c++11 -o ctrl_viewer.o -I/home/pi

ctrl_admin.o: ctrl_admin.cpp definitions.h
	g++ ctrl_admin.cpp -c -std=c++11 -o ctrl_admin.o -I/home/pi

mutual.o: mutual.cpp definitions.h
	g++ mutual.cpp -c -std=c++11 -o mutual.o -I/home/pi

transmit.o: transmit.cpp definitions.h
	g++ transmit.cpp -c -std=c++11 -o transmit.o -I/home/pi

relay_server: main.cpp ctrl_core.o ctrl_captain.o ctrl_vehicle.o ctrl_viewer.o ctrl_admin.o mutual.o transmit.o
	g++ main.cpp -std=c++11 -o relay_server -I/home/pi ctrl_core.o ctrl_captain.o ctrl_vehicle.o ctrl_viewer.o ctrl_admin.o mutual.o transmit.o -L/home/pi/tobilib -ltc -lpthread -lboost_system -lboost_thread