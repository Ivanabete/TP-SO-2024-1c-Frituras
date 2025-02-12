#include "client.h"

int crear_conexion(char *ip, char* puerto, modulo_code modulo)
{
	t_log* error = log_create("ErrorDeConexion.log","Error de conexion",true,LOG_LEVEL_ERROR);
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.

	int socket_cliente = socket(server_info->ai_family,
                    server_info->ai_socktype,
                    server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo pero primero verificamos que se pueda conectar
	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen)==-1){
		freeaddrinfo(server_info);
		log_error(error,"Conexión fallida");
		return -1;
	}

	size_t bytes;
	int result;
	
	bytes = send(socket_cliente, &modulo, sizeof(int), 0);
	bytes = recv(socket_cliente, &result, sizeof(int), MSG_WAITALL);

	if (result != 0 || bytes < 0) {
    	exit (-1);
	}

	freeaddrinfo(server_info);

	log_destroy(error);
	
	return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}