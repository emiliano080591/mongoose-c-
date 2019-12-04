all: Servidor

Servidor: SocketDatagrama.o PaqueteDatagrama.o Respuesta.o mainServer.cpp
		g++ mainServer.cpp SocketDatagrama.o PaqueteDatagrama.o Respuesta.o -o mainServer

SocketDatagrama.o: SocketDatagrama.cpp SocketDatagrama.h
		g++ SocketDatagrama.cpp -c

PaqueteDatagrama.o: PaqueteDatagrama.cpp PaqueteDatagrama.h
		g++ PaqueteDatagrama.cpp -c

Respuesta.o: Respuesta.cpp Respuesta.h
		g++ Respuesta.cpp -c

Solicitud.o: Solicitud.cpp Solicitud.h
		g++ Solicitud.cpp -c
