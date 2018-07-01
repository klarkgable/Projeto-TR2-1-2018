///Projeto final para disciplina de Teleinformática e Redes 2 1°/2018
///Alunos:
        ///Klark Gable Souza Porto  120015421
        ///Gabriel Rodrigues Diógenes Macedo  15/0126808

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef ESTRUTURA_H_INCLUDED
#define ESTRUTURA_H_INCLUDED

///struct de mensagem para conexao(sockets)
namespace Socket {

typedef struct {
	std::string message;
	int internalConnectionID;

	std::string addr_from;
	int port_from;

	std::string addr_to;
	int port_to;
} MessageData;

};


///definiçoes das estruturas e funções de conexao externa
namespace Socket {

class ExtListener {
    public:
        ExtListener();
        ~ExtListener();

        ssize_t Send( int connectionIDofInternal, std::string addr, int port, std::string message );
        void ReceiveMessages();

        std::vector< MessageData > messagesReceived;
    private:
        typedef struct {
            int socket;
            std::string addr;
            int port;
            struct sockaddr_in serv_addr;
            int connectionIDofInternal;
        } SocketInfo;

        std::vector< SocketInfo > connections;
        fd_set active_fd_set;
        fd_set read_fd_set;
        struct timeval timeout;
    };

};

///definiçoes das estruturas e funções de conexao interna
namespace Socket {

class IntListener {

    public:
        IntListener( int port );
        ~IntListener();

        void AcceptAndReceive();
        ssize_t Send( int connectionID, std::string message );

        std::vector< MessageData > messagesReceived;
    private:
        typedef struct {
            int socket;
            struct sockaddr_in address;
        } ConnectionData;

        typedef enum intListenerState : short {
            NEWBORN = 0,
            SOCKET_CREATED = 1 << 0,
            SOCKET_BINDED = 1 << 1,
            INTLISTENER_STARTED = 1 << 2,
            INTLISTENER_RUNNING = 1 << 3,
            SHUTTING_DOWN = 1 << 4
        } IntListenerState;

        int port;
        int socketfd;
        short state;
        std::vector< ConnectionData > connections;
        fd_set active_fd_set;
        fd_set read_fd_set;
        struct timeval timeout;

        struct sockaddr_in address;
    };

};

///definição das estruturas e funções do Header

    //requisiçao
    namespace HTTP {

    class RequestHeader {
        public:
            RequestHeader();
            RequestHeader( std::string header );

        private:
        };

    };

    //resposta
        namespace HTTP {

        class ResponseHeader {
            public:
                ResponseHeader();
                ResponseHeader( std::string header );
            private:
            };

        };


///definição das estruturas e funções do Servidor Proxy

class ServidorProxy {

    public:
        ServidorProxy( int port );
        ~ServidorProxy();
        bool Loop();

    private:
        Socket::IntListener il;
        Socket::ExtListener el;
};


#endif // ESTRUTURA_H_INCLUDED
