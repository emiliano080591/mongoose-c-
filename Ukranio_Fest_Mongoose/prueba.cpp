/*
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include<pthread.h>

int main(int argc, char const *argv[]) {
    //char ip[16] = "127.0.0.1";
    //Solicitud solicitud;
    int puerto = 7200;
    int origen, nbytes;
    int offset = 0;
    const char* libro = "hola.txt";
    char buffer[10];
    struct timeval timestamp;

    pthread_t canalA, canalB;

    if((origen = open(libro, O_RDONLY)) == -1) {
        perror(libro);
        exit(-1);
    }
    
    //Se inicia el socket cliente
    //SocketDatagrama socketDatpuertoagrama(6666);
    //Se crea el paquete que ira al servidor
    //PaqueteDatagrama paqueteDatagrama_Envio((char*)&num,2 * sizeof(int),(char*)argv[1],puerto);
    //Se envia el paquete al servidor
    //socketDatagrama.envia(paqueteDatagrama_Envio);
    while((nbytes = read(origen, buffer, sizeof(buffer))) > 0) {

        std::cout << buffer << std::endl;

        offset = offset + 34;
    }


    return 0;
}
*/


