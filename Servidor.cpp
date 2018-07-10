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


namespace Inspetor {

ServidorProxy::ServidorProxy( int porta ) : continuando(true), requisicoesRecebidas(ci.requisicoesRecebidas), respostasRecebidas(ce.respostasRecebidas), ci( porta ), ce() {}

ServidorProxy::~ServidorProxy() {}

bool ServidorProxy::Loop() {
	
	continuando &= ci.aceitaConexoes();
	continuando &= ci.recebeRequisicoes();

	// Autoriza requests
	if( requisicoesRecebidas.size() > 0 ) std::swap( requisicoesRecebidas, requisicoesEnvio );

	// Envia requests
	if( requisicoesEnvio.size() > 0 ) {
		printf( "\nEnviando %lu requisicoes.\n", (long int) requisicoesEnvio.size() );
		for( auto it = requisicoesEnvio.begin(); it != requisicoesEnvio.end(); it++ ) {
			printf( "\nEnviando para %s:%s...\n%s\n\n", std::get<1>(*it).host.c_str(), std::get<1>(*it).porta.c_str(), std::get<1>(*it).texto(false).c_str() );
			if( -1 == ce.enviaRequisicao( std::get<0>(*it), std::get<1>(*it) ) ) {
				fprintf( stderr, "\nNao pode completar envio de dados. Fechando... Tente novamente.\n" );
				ci.fechaSocket( std::get<0>(*it).lock()->getDescritor() );
			}
		}
		requisicoesEnvio.clear();
	}

	continuando &= ce.recebeRespostas();

	// Autoriza requests
	if( respostasRecebidas.size() > 0 ) std::swap( respostasRecebidas, respostasEnvio );

	// Envia requests
	if( respostasEnvio.size() > 0 ) {
		printf( "\nEnviando %lu respostas.\n", (long int) respostasEnvio.size() );
		for( auto it = respostasEnvio.begin(); it != respostasEnvio.end(); it++ ) {
			printf( "\nEnviando...\n%s\n\n", std::get<1>(*it).texto(false).c_str() );
			if( -1 == ci.enviaResposta( std::get<0>(*it), std::get<1>(*it) ) ) {
				fprintf( stderr, "\nNao pode completar envio de dados. Fechando... Tente novamente.\n" );
				ci.fechaSocket( std::get<0>(*it).lock()->getDescritor() );
			}
		}
		respostasEnvio.clear();
	}

	return continuando;
}

};//fechando namespace Inspetor
