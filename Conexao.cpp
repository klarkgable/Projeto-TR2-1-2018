///Projeto final para disciplina de Teleinformática e Redes 2 1°/2018
///Alunos:
        ///Klark Gable Souza Porto  120015421
        ///Gabriel Rodrigues Diógenes Macedo  15/0126808

#include "Estrutura.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define INTLISTENER_TIMEOUT_MS 25
#define QUEUESIZE 5

#define EXTLISTENER_TIMEOUT_MS 25



///funçoes de requisicao externa
namespace Socket {

ExtListener::ExtListener() {
    errno = 0;
	FD_ZERO( &active_fd_set );
}

ExtListener::~ExtListener() {
	for( int i = connections.size() - 1; i >= 0; i-- ) {
		if( close( connections[i].socket ) < 0 ) {
			closeError();
		}
		FD_CLR( connections[i].socket, &active_fd_set );
	}
}

ssize_t ExtListener::Send( int connectionIDofInternal, std::string addr, int port, std::string message ) {
	SocketInfo si;
	si.socket = -1;
	si.addr = addr;
	si.port = port;
	si.connectionIDofInternal = connectionIDofInternal;

	if( (si.socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
		printf( "\nSocket creation error\n" );
		socketError();
	} else {
		memset( &(si.serv_addr), '0', sizeof( si.serv_addr ) );

		si.serv_addr.sin_family = AF_INET;
		si.serv_addr.sin_port = htons( port );

		// Convert IPv4 and IPv6 addresses from text to binary form
		if( inet_pton( AF_INET, addr.c_str(), &(si.serv_addr.sin_addr) ) <= 0 ) {
			printf( "\nInvalid address / Address not supported.\n" );
		} else if( connect( si.socket, (struct sockaddr *) &(si.serv_addr), sizeof(si.serv_addr) ) < 0 ) {
			printf( "\nConnection Failed\n" );
			connectError();
		} else {
			connections.push_back( si );
			FD_SET( si.socket, &active_fd_set );
			ssize_t sent = send( si.socket, message.c_str(), message.length(), 0 );
			if( sent < -1 ) {
				printf( "\nCould not send data.\n" );
				sendError();
			} else {
				return sent;
			}
		}
	}
	return -1;
}

void ExtListener::ReceiveMessages() {
	read_fd_set = active_fd_set;
	timeout.tv_sec = 0;
	timeout.tv_usec = EXTLISTENER_TIMEOUT_MS;

	if( select( FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout ) < 0 ) {
		printf( "\nCould not wait on select for sockets.\n" );
		selectError();
	}
	for( int i = 0; i < FD_SETSIZE; i++ ) {
		if( FD_ISSET(i, &read_fd_set ) ) {
			// Receiving data
			int connectionID;
			for( connectionID = 0; connectionID < (int) connections.size(); connectionID++ ) {
				if( connections[connectionID].socket == i ) break;
			}
			char buffer[1024];
			int valread = read( i, buffer, 1024 );
			if( 0 < valread) {
				MessageData md;
				md.message = std::string( buffer, valread );
				md.internalConnectionID = connections[connectionID].connectionIDofInternal;
				md.addr_from = connections[connectionID].addr;
				md.port_from = connections[connectionID].port;
				md.addr_to = "127.0.0.1";
				md.port_to = 8228;
				messagesReceived.push_back( md );
			} else if( 0 == valread ) {
				if( close( i ) < 0 ) {
					closeError();
				}
				FD_CLR( i, &active_fd_set );
				connections.erase( connections.begin() + connectionID );
			} else {
				printf( "\nCould not read data.\n" );
				readError();
			}
		}
	}
}

};

///funçoes de requisicao interna
namespace Socket {

IntListener::IntListener( int port ) : port( port ), socketfd( -1 ), state( 0 ) {
    errno = 0;
	socketfd = socket( AF_INET, SOCK_STREAM, 0 );
	if( socketfd < 0 ) {
		printf( "\nSocket creation error\n" );
		socketError();
	} else {
		state |= intListenerState::SOCKET_CREATED;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
		address.sin_port = htons( port );
		memset( &(address.sin_zero), 0, sizeof( address.sin_zero ) );
		if( bind( socketfd, (struct sockaddr *) &address, sizeof(address) ) < 0 ) {
			printf( "\nFailed to bind socket.\n" );
			bindError();
		} else {
			state |= intListenerState::SOCKET_BINDED;
			FD_ZERO( &active_fd_set );
			FD_SET( socketfd, &active_fd_set );
			if( listen( socketfd, QUEUESIZE ) < 0 ) {
				printf( "\nFailed to start listening.\n" );
				listenError();
			} else {
				state |= intListenerState::INTLISTENER_STARTED;
			}
		}
	}
}

IntListener::~IntListener() {
	state = intListenerState::SHUTTING_DOWN;
	for( int i = connections.size() - 1; i >= 0; i-- ) {
		if( close( connections[i].socket ) < 0 ) {
			closeError();
		}
		FD_CLR( connections[i].socket, &active_fd_set );
	}
	if( socketfd >= 0 ) {
		if( close( socketfd ) < 0 ) {
			closeError();
		}
		FD_CLR( socketfd, &active_fd_set );
	}
	state = 0;
}

void IntListener::AcceptAndReceive() {
    if( state & intListenerState::INTLISTENER_STARTED ) {
	    state |= intListenerState::INTLISTENER_RUNNING;

	    read_fd_set = active_fd_set;
	    timeout.tv_sec = 0;
	    timeout.tv_usec = INTLISTENER_TIMEOUT_MS;
	    if( select( FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout ) < 0 ) {
		    printf( "\nCould not wait on select for sockets.\n" );
		    selectError();
	    }
	    for( int i = 0; i < FD_SETSIZE; i++ ) {
		    if( FD_ISSET(i, &read_fd_set ) ) {
			    if( i == socketfd ) {
				    // Accepting connections
				    ConnectionData cd;
				    socklen_t length = sizeof(struct sockaddr_in);
				    cd.socket = accept( socketfd, (struct sockaddr*) &(cd.address), &length );
				    if( -1 < cd.socket ) {
					    connections.push_back( cd );
					    FD_SET( cd.socket, &active_fd_set );
				    } else {
					    printf( "\nCould not accept socket.\n" );
					    acceptError();
				    }
			    } else {
				    // Receiving data
				    int connectionID;
				    for( connectionID = 0; connectionID < (int) connections.size(); connectionID++ ) {
					    if( connections[connectionID].socket == i ) break;
				    }
				    char buffer[1024];
				    int valread = read( i, buffer, 1024 );
				    if( 0 < valread) {
					    MessageData md;
					    md.message = std::string( buffer, valread );
					    md.internalConnectionID = connectionID;
					    md.addr_from = std::string( inet_ntoa( connections[connectionID].address.sin_addr ) );
					    md.port_from = connections[connectionID].address.sin_port;
					    md.addr_to = "127.0.0.1";
					    md.port_to = port;
					    messagesReceived.push_back( md );
				    } else if( 0 == valread ) {
					    if( close( i ) < 0 ) {
						    closeError();
					    }
					    FD_CLR( i, &active_fd_set );
					    connections.erase( connections.begin() + connectionID );
				    } else {
					    printf( "\nCould not read data.\n" );
					    readError();
				    }
			    }
		    }
	    }
    }
}

ssize_t IntListener::Send( int connectionID, std::string message ) {
	if( state & intListenerState::INTLISTENER_RUNNING ) {
		if( connectionID < (int) connections.size() ) {
			ssize_t sent = send( connections[connectionID].socket , message.c_str(), message.length(), 0 );
			if( sent < 0 ) {
				printf( "\nCould not send data.\n" );
				sendError();
			} else {
				return sent;
			}
		} else {
			printf( "\nInvalid connectionID.\n" );
		}
	} else {
		printf( "\nCannot send message. Listener is not in a valid state for sending data.\n" );
	}
	return -1;
}

};
