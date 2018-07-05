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


ServidorProxy::ServidorProxy( int porta ) : ci( porta ), ce() {}

ServidorProxy::~ServidorProxy() {}

bool ServidorProxy::Loop() {
    //primeiro(pacotes que saem)
	ci.AceitaERecebe();
	if(0 < (int) ci.mensagensRecebidas.size() )
        printf( "\nTemos %d pacotes saindo.\n", (int) ci.mensagensRecebidas.size() );
	for( int i = 0; i < (int) ci.mesagensRecebidas.size(); i++ ) {
        //resgatando parametro do cabeçalho
        HTTP::Header header( ci.mensagensRecebidas[i].mensagem );

		printf( "\nMensagem de %s:%d para %s:%d\n%s\n"
            , ci.mensagensRecebidas[i].endereco_de.c_str()
			, ci.mensagensRecebidas[i].porta_de
			, header.host.c_str()
            , header.porta.c_str()
			, ci.mensagensRecebidas[i].mensagem.c_str()
		);
		ce.Envio( ci.mensagensRecebidas[i].IDconexaoInterna, header.host.c_str(), std::atoi( header.porta.c_str() ), ci.mensagensRecebidas[i].mensagem );
	}
	ci.mensagensRecebidas.clear();


	//por ultimo (pacotes que entram)
	ce.RecebeMensagens();
	if(0 < (int) ce.mensagensRecebidas.size() )
        printf( "\nTemos %d pacotes entrando.\n", (int) ce.mensagensRecebidas.size() );
	for( int i = 0; i < (int) ce.mensagensRecebidas.size(); i++ ) {
        //resgatando parametro do cabeçalho
        HTTP::Header header( ce.mensagensRecebidas[i].mensagem );

		printf( "\nMensagem de %s:%d para %s:%d\n%s\n"
			, ci.mensagensRecebidas[i].endereco_de.c_str()
			, ci.mensagensRecebidas[i].porta_de
			, header.host.c_str()
            , header.porta.c_str()
			, header.texto( false ).c_str()
		);
		ci.Envio( ce.mensagensRecebidas[i].IDconexaoInterna, ce.mensagensRecebidas[i].mensagem );
	}
	ce.mensagensRecebidas.clear();
	return true;
}
