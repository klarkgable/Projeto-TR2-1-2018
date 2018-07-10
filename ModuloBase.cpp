///Projeto final para disciplina de Teleinformática e Redes 2 1°/2018
///Alunos:
        ///Klark Gable Souza Porto  120015421
        ///Gabriel Rodrigues Diógenes Macedo  15/0126808

#include "Estrutura.h"


namespace ModuloBase{


///funçoes para recurso que serao usadas no Spider
Recurso::Recurso( std::string host, std::string nome, std::string respostaHTTP ) : host(host) {
	int comeco = nome.find( host );
	nomeLocal = nome.replace( comeco, host.length()+1, "" );
	this->nome = host + "/" + nomeLocal;

	if( nomeLocal.empty() )
        nomeLocal = "index";

	HTTP::Header header( respostaHTTP );
	dados = header.corpo;


	bool procura = false;
	for( unsigned long int i = 0; i < header.campos.size(); i++ ) {
		if( std::get<0>( header.campos[i] ) == "Content-Type" ) {
			unsigned long long int pos = std::get<1>( header.campos[i] ).find( "text/html" );
			procura |= pos != std::string::npos;
			break;
		}
	}
	if( procura ) {
		procuraReferencias( "src=\"" );
		procuraReferencias( "href=\"" );
	}
}

std::string Recurso::getNome() {
	return nome;
}

std::string Recurso::getNomeLocal() {
	return nomeLocal;
}

std::vector< Recurso::Referencia > Recurso::getRecursosReferenciados() {
	return recursosReferenciados;
}

bool Recurso::salvar( std::string caminhoRoot ) {
}

void Recurso::procuraReferencias( const char* propriedadeHTML ) {
	std::string propriedade( propriedadeHTML );
	unsigned long long int comeco = dados.find( propriedade );
	while( comeco != std::string::npos ) {
		comeco += propriedade.length();
		unsigned long long int fim = dados.find( "\"", comeco );
		std::string referencia = dados.substr(comeco, fim - comeco);
		unsigned long long int achaHost;
		if( referencia.size() > 1 && referencia[0] == '/' && referencia[1] != '/' ) {
			achaHost = 1;
		} else if( (achaHost = referencia.find( host ), achaHost != std::string::npos) ) {
			achaHost += host.length() + 1;
		}
		if( achaHost != std::string::npos ) { // So adiciona se for local, e adiciona somente o caminho relativo (depois da primeria "/")
			referencia = achaHost <= referencia.length() ? referencia.substr( achaHost ) : std::string( "" );
			recursosReferenciados.emplace_back( comeco, fim, referencia );
		}
		comeco = dados.find( propriedade, fim + 2 );
	}
}



///funçoes para Spider
Spider::Spider( std::string host ) : sucesso(false), nomeArvoreRoot(host) {
	if( !socket.conecta(host, "80") ) return;
	std::queue< std::string > recursosDownload;
	recursosDownload.push( host );

	while( !recursosDownload.empty() ) {
		if( achaRecursos( recursosDownload.front() ) == -1 ) {
			std::string recurso( recursosBaixados( host, recursosDownload.front() ) );
			if( recurso.empty() ) return;
			arvore.emplace_back( host, recursosDownload.front(), recurso );

			// Adiciona referencias
			std::vector< Recurso::Referencia > r = arvore.back().getRecursosReferenciados();
			for( unsigned int i = 0; i < r.size(); i++ ) {
				std::string proximoRecurso = host + "/" + std::get<2>( r[i] );
				if( achaRecursos( proximoRecurso ) == -1 ) recursosDownload.push( proximoRecurso );
			}

		}
		recursosDownload.pop();
	}

	// Acha e seta as referencias na arvore
	for( unsigned int j = 0; j < arvore.size(); j++ ) {
		std::vector< Recurso::Referencia > r = arvore[j].getRecursosReferenciados();
		for( unsigned int i = 0; i < r.size(); i++ ) {
			long long int numeroRef = achaRecursos( host + "/" + std::get<2>( r[i] ) );
			if( -1 == numeroRef )
                return;
			arvore[j].referencias.push_back( numeroRef );
		}
	}
	sucesso = true;
}

bool Spider::Valido() {
	return sucesso;
}

std::string Spider::recursosBaixados( std::string host, std::string nomeRecurso ) {
	int arvores = -1;

RETRY:
	arvores++;
	std::string requisicao( "GET http://" );
	requisicao += nomeRecurso;
	requisicao += " HTTP/1.1\r\nHost: " + host + "\r\nConnection: keep-alive\r\n\r\n";

	// Solicita recurso
	ssize_t envio = send( socket.getDescritor(), requisicao.c_str(), requisicao.length(), 0 );
	if( envio < 0 ) {
		printf( "\nNao foi possivel enviar dados." );
	}

	// Recebe recurso
	struct pollfd mestre;
	mestre.fd = socket.getDescritor();
	mestre.events = POLLIN | POLLPRI;
	mestre.revents = 0;
	std::string mensagem("");
	int retornoPoll = poll( &mestre, 1, 250*240 );
	if( retornoPoll > 0 && (POLLIN | POLLPRI) & mestre.revents ) {
		int valorlido = 0;
		do {
			char buffer[1024] = {0};
			valorlido = read( socket.getDescritor(), buffer, sizeof( buffer ) );
			if( 0 == valorlido ) break;
			mensagem += std::string( buffer, valorlido );

			retornoPoll = poll( &mestre, 1, 250 );
			if( retornoPoll < 0 ) {
				fprintf( stderr, "\nErro ao fazer polling no scket de spider." );
				return std::string("");
			}
		} while( retornoPoll > 0 && (POLLIN | POLLPRI) & mestre.revents );
		if( valorlido < 0 ) { // Erro ao ler socket
			fprintf( stderr, "\nNao foi possivel ler dados." );
			return std::string("");
		} else if( 0 == valorlido ) { // Fechar socket
			socket = Socket();
			if( !socket.conecta( host, "80" ) ) return std::string("");
			if( arvores < 3 )
                goto RETRY;
		}
	} else if( retornoPoll < 0 ) {
		fprintf( stderr, "\nErro ao fazer polling  no socket spider." );
		return std::string("");
	}
	return mensagem;
}

long long int Spider::achaRecursos( std::string nomeRecurso ) {
	for( unsigned long int i = 0; i < arvore.size(); i++ ) {
		if( arvore[i].getNome() == nomeRecurso ) return i;
	}
	return -1;
}


};
