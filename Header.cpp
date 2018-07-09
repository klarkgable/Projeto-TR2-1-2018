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


namespace HTTP {

///metodo para Header
Header::Header( std::string& palavra ) : PrimeiraLinha(), corpo(), host(), porta() {
	if( !palavra.empty() ) {
		//percorrendo texto do header
            unsigned int primeiro = 0;
            unsigned int ultimo = palavra.find( "\r\n" );

		//abstraindo a primeira linha do cabeçalho
            PrimeiraLinha = palavra.substr( primeiro, ultimo );

		primeiro = ultimo + 2;

		while( ( ultimo = palavra.find( "\r\n", primeiro ), ultimo ) > primeiro ) {

			//salva linha
			std::string linha = palavra.substr( primeiro, ultimo-primeiro );
			unsigned int divisao = linha.find( ":" );

			//abstrai propriedade do cabeçalho
			std::string propriedade = linha.substr( 0, divisao );
			std::string valor = linha.substr( divisao + 2 );

            //identidficando host
			if( "Host" == propriedade ) {
				divisao = valor.rfind( ":" );
				host = valor.substr( 0, divisao );
				if( divisao < valor.size() )
					porta = valor.substr( divisao + 1 );
            }


			campos.push_back( std::make_tuple( propriedade, valor ) );

			primeiro = ultimo + 2;
		}

		if( host.empty() ) {
			unsigned int begin = PrimeiraLinha.find( "//" ) + 2;
			unsigned int end = PrimeiraLinha.find( "/", begin );
			std::string valor = PrimeiraLinha.substr( begin, end-begin );
			unsigned int divisao = valor.rfind( ":" );
			host = valor.substr( 0, divisao );
			if( divisao < valor.size() )
				porta = valor.substr( divisao + 1 );
		}
		if( '[' == host[0] ) {
			host = host.substr( 1, host.size()-2 );
		}
		if( porta.empty() ) {
			//porta padrao http
			porta = "80";
		}
        //pegando corpo da mensagem
		corpo = palavra.substr( primeiro + 2 );
	}
}

///metodo struct para string
std::string Header::texto(bool incluiCorpo) {
	std::string requisicao( "" );

	requisicao += PrimeiraLinha + "\r\n";

	//percoorendo campos e colocando em uma string de formato da mensagem http para enviar de volta a internet
	for( unsigned int i = 0; i < campos.size(); i++ ) {
		requisicao += std::get<0>( campos[i] ) + ": " + std::get<1>(campos[i] ) + "\r\n";
	}

	//enter e espçamentos para mensagem
	requisicao += "\r\n";
	requisicao += incluiCorpo ? corpo : "<corpo>";

	return requisicao;
}



};
