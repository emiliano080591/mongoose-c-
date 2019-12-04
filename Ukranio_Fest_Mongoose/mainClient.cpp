#define MG_EV_HTTP_PART_BEGIN 122
#define MG_EV_HTTP_PART_DATA 123
#define MG_EV_HTTP_PART_END 124
#include "mongoose.h"
#include "Solicitud.h"
#include "control.h"
#include <fcntl.h>
#include<fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include<pthread.h>

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

void* AbreComunicacionA(void* buffer);
void* AbreComunicacionB(void* buffer);
int numeroPalabras = 0;
int numeroComplejidad = 0;
int comunicacion = 0;
float complejidad;

struct aux {
    char buffer[5000];
    int tam;
};

struct file_writer_data {
  FILE *fp;
  size_t bytes_written;
};
struct timeval timestamp;

pthread_t canalA, canalB, canalC;
static void handle_upload(struct mg_connection *nc, int ev, void *p) {
  struct file_writer_data *data = (struct file_writer_data *) nc->user_data;
  struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;

  switch (ev) {
    case MG_EV_HTTP_PART_BEGIN: {
      if (data == NULL) {
        data = (struct file_writer_data *)calloc(1, sizeof(struct file_writer_data));
        data->fp =  fopen(mp->file_name, "w+");
        data->bytes_written = 0;
        if (data->fp == NULL) {
          mg_printf(nc, "%s",
                    "HTTP/1.1 500 Failed to open a file\r\n"
                    "Content-Length: 0\r\n\r\n");
          nc->flags |= MG_F_SEND_AND_CLOSE;
          return;
        }

        nc->user_data = (void *) data;
      }
      break;
    }
    case MG_EV_HTTP_PART_DATA: {
      if (fwrite(mp->data.p, 1, mp->data.len, data->fp) != mp->data.len) {
        mg_printf(nc, "%s",
                  "HTTP/1.1 500 Failed to write to a file\r\n"
                  "Content-Length: 0\r\n\r\n");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        return;
      }
      data->bytes_written += mp->data.len;
      break;
    }
    case MG_EV_HTTP_PART_END: {
      mg_printf(nc,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n\r\n"
                "Written %ld of POST data to a temp file\n\n",
                (long) ftell(data->fp));
      nc->flags |= MG_F_SEND_AND_CLOSE;

      fclose(data->fp);
      free(data);
      printf("%s\n", mp->file_name);

      struct aux buffers[3];
      int count = 0;
      
      //std::string libro = mp->file_name;
      std::string line;
      std::string aux;
	    std::ifstream myLibro;
      myLibro.open(mp->file_name);

      while( getline (myLibro,line) ) {
        
        if((line.length() >= 0) && (line!="\0")){

            count++; 

            char buffer[line.size() + 1];
            strcpy(buffer, line.c_str());
            std::cout << "BUFFER: " <<  buffer << " COUNT:" << count << std::endl;
          
            buffers[count - 1].tam =  line.size() + 1;
            strncpy(buffers[count - 1].buffer, buffer, buffers[count - 1].tam);

            if((!getline(myLibro,aux)) && (count > 0) ) {
              
              if(comunicacion > 0) {
                if(comunicacion == 3) {
                  pthread_create(&canalA, NULL, AbreComunicacionA, &buffers[0]);
                  pthread_join(canalA, NULL);
                } else if (comunicacion == 4) {
                  pthread_create(&canalB, NULL, AbreComunicacionB, &buffers[0]);
                  pthread_join(canalB, NULL);
                }
                comunicacion = 0;
              } else {
                
                pthread_create(&canalA, NULL, AbreComunicacionA, &buffers[0]);
                pthread_join(canalA, NULL);
                std::cout << "----------ENTRE AQUIII------------" << std::endl;
              }
            }
            
            
            if(count == 2) {
              
              if(comunicacion > 0) {
                std::cout << "Comunicacion: ---->" << comunicacion << std::endl;
                if(comunicacion == 3) {
                  pthread_create(&canalA, NULL, AbreComunicacionA, &buffers[0]);
                  pthread_join(canalA, NULL);
                  pthread_create(&canalA, NULL, AbreComunicacionA, &buffers[1]);
                  pthread_join(canalA, NULL);
                } else if (comunicacion == 4){
                  pthread_create(&canalB, NULL, AbreComunicacionB, &buffers[0]);
                  pthread_join(canalB, NULL);
                  pthread_create(&canalB, NULL, AbreComunicacionB, &buffers[1]);
                  pthread_join(canalB, NULL);
                }
                comunicacion = 0;
              } else {
                
                pthread_create(&canalA, NULL, AbreComunicacionA, &buffers[0]);
                pthread_join(canalA, NULL);
                pthread_create(&canalB, NULL, AbreComunicacionB, &buffers[1]);
                pthread_join(canalB, NULL);
              }

              
                
              std::cout << "Variables globales" << numeroPalabras << "," << numeroComplejidad << std::endl;
              count = 0;
            }         

        }

        }
        complejidad = 100 - ((numeroComplejidad * 100) / numeroPalabras);
        std::cout << "Complejidad ---->" << complejidad << "%" << std::endl; //fin de getline
      break;
    }
  }
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_REQUEST) {
    mg_serve_http(nc, (struct http_message *)ev_data, s_http_server_opts);
  }
}


void* AbreComunicacionA(void* buffer) {
    Solicitud solicitud;
    control respuesta;
    aux *param = (aux *) buffer;
    //const char* ip = "127.0.0.1";
    const char* ip = "192.168.43.218";
    int puerto = 7200;

    memcpy((char*)&respuesta,solicitud.doOperation((char*)ip,puerto,3, param->buffer, param->tam,1),sizeof(control));

    if ( respuesta.perdidaComunicacion == 1) {

      comunicacion = respuesta.perdidaComunicacion + 3;
       
    } else {
      numeroPalabras = numeroPalabras + respuesta.PalabrasLeidas;
      numeroComplejidad =  numeroComplejidad + respuesta.PorcentajeComplejidad;
      comunicacion = respuesta.perdidaComunicacion;

      std::cout << "Respuesta recibida:" << respuesta.PalabrasLeidas << "," << respuesta.PorcentajeComplejidad << std::endl;
    }


    solicitud.getSocket()->~SocketDatagrama();
}

void* AbreComunicacionB(void* buffer) {
    Solicitud solicitud;
    control respuesta;
    aux *param = (aux *) buffer;
    const char* ip = "192.168.43.217";
    //const char* ip = "10.100.69.161";
    int puerto = 7200;

    memcpy((char*)&respuesta,solicitud.doOperation((char*)ip,puerto,3, param->buffer, param->tam,1),sizeof(control));

    if ( respuesta.perdidaComunicacion == 1) {

      comunicacion = respuesta.perdidaComunicacion + 2;
       
    } else {
      numeroPalabras = numeroPalabras + respuesta.PalabrasLeidas;
      numeroComplejidad =  numeroComplejidad + respuesta.PorcentajeComplejidad;
      comunicacion = respuesta.perdidaComunicacion;

      std::cout << "Respuesta recibida:" << respuesta.PalabrasLeidas << "," << respuesta.PorcentajeComplejidad << std::endl;
    }


    solicitud.getSocket()->~SocketDatagrama();
    
}

/*
void* AbreComunicacionC(void* buffer) {
    Solicitud solicitud;
    control respuesta;
    //const char* ip = "127.0.0.1";
    const char* ip = "127.0.0.1";
    int puerto = 7200;

    memcpy((char*)&respuesta,solicitud.doOperation((char*)ip,puerto,3,(char*)buffer,1),sizeof(timeval));

    numeroPalabras = numeroPalabras + respuesta.PalabrasLeidas;
    numeroComplejidad =  numeroComplejidad + respuesta.PorcentajeComplejidad;

    std::cout << "Respuesta recibida:" << respuesta.PalabrasLeidas << "," << respuesta.PorcentajeComplejidad << std::endl;

    solicitud.getSocket()->~SocketDatagrama();
}
*/

int main(int argc, char const *argv[])
{

  //char ip[16] = "127.0.0.1";
  int offset = 0;
  struct mg_mgr mgr;
	struct mg_connection *nc;
	mg_mgr_init(&mgr, NULL);

	printf("Starting web server on port %s\n", s_http_port);
	nc = mg_bind(&mgr, s_http_port, ev_handler);
	if (nc == NULL) {
		printf("Failed to create listener\n");
		return 1;
	}
	// Set up HTTP server parameters
	s_http_server_opts.document_root = ".";  // Serve current directory
	mg_register_http_endpoint(nc, "/upload", handle_upload);
	mg_set_protocol_http_websocket(nc);

	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

    return 0;
}
