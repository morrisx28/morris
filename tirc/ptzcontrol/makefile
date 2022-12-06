SerialPort.o: SerialPort.cpp
	g++ -c -o SerialPort.o SerialPort.cpp
TDQueue.o: TDQueue.cpp
	g++ -c -o TDQueue.o TDQueue.cpp
PacketParser.o: PacketParser.cpp
	g++ -c -o PacketParser.o PacketParser.cpp
PtzProtocolHandler.o: PtzProtocolHandler.cpp
	g++ -c -o PtzProtocolHandler.o PtzProtocolHandler.cpp
ThermalControl.o: ThermalControl.cpp
	g++ -c -o ThermalControl.o ThermalControl.cpp
ThermalDemo.o: ThermalDemo.cpp
	g++ -c -o ThermalDemo.o ThermalDemo.cpp		
thermal.o: thermal.cpp
	g++ -c -o thermal.o thermal.cpp	

thermal: SerialPort.o TDQueue.o PacketParser.o PtzProtocolHandler.o ThermalControl.o ThermalDemo.o thermal.o 
	g++ -o thermal TDQueue.o PacketParser.o PtzProtocolHandler.o ThermalControl.o ThermalDemo.o thermal.o SerialPort.o -lpthread

clean: 
	rm -f thermal
	rm -f *.o


