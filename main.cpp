///Projeto final para disciplina de Teleinformática e Redes 2 1°/2018
///Alunos:
        ///Klark Gable Souza Porto  120015421
        ///Gabriel Rodrigues Diógenes Macedo  15/0126808

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Estrutura.h"


#define DEFAULT_PORT 8228

int main( int argc, char* argv[] ) {

	int porta = DEFAULT_PORT;
	if( 3 == argc ) {
		if( std::string( argv[1] ) == "-p" ) {
			try {
				porta = std::stoi( std::string( argv[2] ) );
			} catch( std::exception e ) {
				printf( "\nPorta invalida.\n" );
				return -1;
			}
		} else {
			printf( "\nEntrada nao reconhecida.\nOpçoes validas:\n[-p <port>]\Escolha outra porta diferente da padrão %d\n", porta );
			return -1;
		}
	} else if( 1 != argc ) {
		printf( "\nNumero de argumentos invalido.\n" );
		return -1;
	}

	
	printf( "\nIniciando aracne na porta# %d\n", porta );

	Inspetor::ServidorProxy teste1( porta );

	while( teste1.Loop() ) {};

	return 0;
}
