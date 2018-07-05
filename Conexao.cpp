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




namespace Socket {


///funçoes de requisicao interna
conexaoInterna::conexaoInterna( int porta ) : porta( porta ), socketPrincipal( -1 ) {
    errno = 0;

    //criando socket em TCP(socket principal)
	socketPrincipal = socket( AF_INET, SOCK_STREAM, 0 );
	if( socketPrincipal < 0 ) {
		printf( "\nErro na criacao do Socket.\n" );
	} else {
		endereco.sin_family = AF_INET;
		endereco.sin_endereco.s_addr = htonl( INADDR_LOOPBACK );
		endereco.sin_port = htons( porta );
		memset( &(endereco.sin_zero), 0, sizeof( endereco.sin_zero ) );
		//comunicaçao com socket principal
            if( bind( socketPrincipal, (struct sockaddr *) &endereco, sizeof(endereco) ) < 0 ) {
                printf( "\nFalha ao tentar comunicar com socket.\n" );
            } else {
                FD_ZERO( &active_fd_set );
                FD_SET( socketPrincipal, &active_fd_set );
                if( listen( socketPrincipal, QUEUESIZE ) < 0 ) {
                    printf( "\nFalha ao iniciar conexao.\n" );
                }
            }
        }
}

conexaoInterna::~conexaoInterna() {
	for( int i = conexoes.size() - 1; i >= 0; i-- ) {
		FD_CLR( conexoes[i].socket, &active_fd_set );
	}

	if( socketPrincipal >= 0 ) {
		FD_CLR( socketPrincipal, &active_fd_set );
	}
}

void conexaoInterna::AceitaErecebe(){
	    read_fd_set = active_fd_set;
	    timeout.tv_sec = 0;
	    timeout.tv_usec = INTLISTENER_TIMEOUT_MS;

	    //seleçao de sockets
	    if( select( FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout ) < 0 ) {
		    printf( "\nNao pode esperar para selecionar Sockets.\n" );
	    }

	    for( int i = 0; i < FD_SETSIZE; i++ ) {
		    if( FD_ISSET(i, &read_fd_set ) ) {
			    if( i == socketPrincipal ) {
				    //Aceitando conexoes
				    DadosConexao dc;
				    socklen_t length = sizeof(struct sockaddr_in);
				    dc.socket = accept( socketPrincipal, (struct sockaddr*) &(dc.endereco), &length );
				    if( -1 < dc.socket ) {
					    conexoes.push_back( dc );
					    FD_SET( dc.socket, &active_fd_set );
				    } else {
					    printf( "\nNão foi possível aceitar o soquete.\n" );
				    }
			    } else {
				    //Recebendo dados
				    int conexaoID;
				    for( conexaoID = 0; conexaoID< (int) conexoes.size(); conexaoID++ ) {
					    if( conexoes[conexaoID].socket == i ) break;
				    }

				    int valorlido = 0;
					std::string mensagem("");
					do {
						char buffer[1024];
						valorlido = read( i, buffer, sizeof( buffer ) );
						mensagem += std::string( buffer, valorlido );
                    } while (1024 == valorlido);

				    if( 0 < valorlido) {
					    DadosMensagem dm;
					    dm.mensagem = mensagem
					    dm.IDconexaoInterna = conexaoID;
					    dm.endereco_de = std::string( inet_ntoa( conexoes[conexaoID].endereco.sin_addr ) );
					    dm.porta_de = ntohs( conexoes[conexaoID].endereco.sin_port );;
					    dm.endereco_para = "127.0.0.1";
					    dm.porta_para = porta;
					    mensagensRecebidas.push_back( dm );
				    } else if( 0 == valorlido ) {
					    FD_CLR( i, &active_fd_set );
					    conexoes.erase( conexoes.begin() + conexaoID );
				    } else {
					    printf( "\nNao foi possivel ler dados.\n" );
				    }
			    }
		    }
	    }

}

ssize_t conexaoInterna::Envio( int IDconexao, std::string mensagem ) {
		if( IDconexao < (int) conexoes.size() ) {
			ssize_t sent = send( conexoes[IDconexao].socket , mensagem.c_str(), mensagem.length(), 0 );
			if( sent < 0 ) {
				printf( "\nNao foi possivel enviar dados.\n" );
			} else {
				return sent;
			}
		} else {
			printf( "\nconexaoID invalida.\n" );
		}
	else {
		printf( "\nNao foi possivel enviar mensagem. A conexao não está em um estado válido para enviar dados.\n" );
	}
	return -1;
}



///funçoes de requisicao externa
conexaoExterna::conexaoExterna(int porta): porta(porta) {
    errno = 0;
	FD_ZERO( &active_fd_set );
}

conexaoExterna::~conexaoExterna() {
	for( int i = conexoes.size() - 1; i >= 0; i-- ) {
		FD_CLR( conexoes[i].socket, &active_fd_set );
	}
}

ssize_t conexaoExterna::Envio( int IDconexaoDeInterna, std::string endereco, int porta, std::string mensagem ) {
	InfoSocket is;

    //auxiliares para tratar gets do http com host em nomes ex:
    struct addrinfo *auxiliar1, *auxiliar2;
    struct addrinfo sugestoes;


    is.IDconexaoDeInterna = IDconexaoDeInterna;



	memset(&sugestoes, 0, sizeof(struct addrinfo));
	sugestoes.auxiliar1_family = AF_INET;
	sugestoes.auxiliar1_socktype = SOCK_STREAM;
	sugestoes.auxiliar1_flags = 0;
	sugestoes.auxiliar1_protocol = 0; /* Any protocol */

	int sugestao = getaddrinfo( endereco.c_str(), std::texto( port ).c_str(), &sugestoes, &auxiliar1 );
	if( 0 > sugestao ) {
		printf( "\nErro ao procurar nome.\n" );
		printf( "getaddrinfo: %s\n", gai_strerror( sugestao ) );
		return -1;
	}

	for( auxiliar2 = auxiliar1; auxiliar2 != NULL; auxiliar2 = aauxiliar2->auxiliar1_next ) {
		std::memcpy( &(is.serv_addr), auxiliar2->auxiliar1_addr, auxiliar2->auxiliar1_addrlen );
		is.endereco = std::string( inet_ntoa( is.serv_addr.sin_addr ) );
		is.porta = ntohs( is.serv_addr.sin_port );
		is.socket = socket(auxiliar2->auxiliar1_family, auxiliar2->auxiliar1_socktype, auxiliar2->auxiliar1_protocol);
		if( -1 == is.socket ) {
			printf( "\nFalha ao criar socket. Entre com outro endereco...\n" );
			continue;
		}
		if( 0 > connect( is.socket, auxiliar2->auxiliar1_addr, auxiliar2->auxiliar1_addrlen ) ) {
			printf( "\nFalha ao conectar com socket. Entre com outro endereco...\n" );
			close( is.socket );
		} else {
			break;
		}
	}

	if( NULL == auxiliar2 ) {
    printf( "\nNão foi possivel conectar...\n");
    }else {
        conexoes.push_back( is );
        FD_SET( is.socket, &active_fd_set );
        ssize_t sent = send( is.socket, mensagem.c_str(), mensagem.length(), 0 );
        if( sent < -1 ) {
            printf( "\nNão foi possível enviar dados.\n" );
        } else {
            freeaddrinfo(auxiliar1);
            return sent;
			}
		}
	}
    freeaddrinfo(auxiliar1);
	return -1;
}

void conexaoExterna::RecebeMensagens(){
	read_fd_set = active_fd_set;
	timeout.tv_sec = 0;
	timeout.tv_usec = EXTLISTENER_TIMEOUT_MS;

	if( select( FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout ) < 0 ) {
		printf( "\nNao pode esperar para selecionar Sockets.\n" );
	}
	for( int i = 0; i < FD_SETSIZE; i++ ) {
		if( FD_ISSET(i, &read_fd_set ) ) {
			// Recebendo dados
			int conexaoID;
			for( conexaoID = 0; conexaoID < (int) conexoes.size(); conexaoID++ ) {
				if( conexoes[conexaoID].socket == i ) break;
			}
			int valorlido = 0;
			std::string mensagem("");
			do {
				char buffer[1024];
				valorlido = read( i, buffer, sizeof( buffer ) );
				mensagem += std::string( buffer, valorlido );
            } while (1024 == valorlido);
			if( 0 < valorlido) {
				DadosMensagem dm;
				dm.mensagem= mensagem
				dm.IDconexaoInterna = conexoes[conexaoID].IDconexaoDeInterna;
				dm.endereco_de = conexoes[conexaoID].endereco;
				dm.porta_de = conexoes[conexaoID].porta;
				dm.endereco_para = "127.0.0.1";
				dm.port_para = porta;
				mensagensRecebidas.push_back( dm );
			} else if( 0 == valorlido ) {
				FD_CLR( i, &active_fd_set );
				conexoes.erase( conexoes.begin() + conexaoID );
			} else {
				printf( "\nDados nao puderam ser lidos.\n" );
			}
		}
	}
}



};
