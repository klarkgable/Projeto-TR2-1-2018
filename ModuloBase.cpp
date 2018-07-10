///Projeto final para disciplina de Teleinformática e Redes 2 1°/2018
///Alunos:
        ///Klark Gable Souza Porto  120015421
        ///Gabriel Rodrigues Diógenes Macedo  15/0126808

#include "Estrutura.h"


namespace ModuloBase{


///funçoes para recurso que serao usadas no Spider
Recurso::Recurso(std::string host, std::string nome, std::string respostaHTTP) : host(host){
	int comeco = nome.find(host);
	nomeLocal = nome.replace(comeco, host.length()+1, "");
	this->nome = host + "/" + nomeLocal;

	if(nomeLocal.empty())
        	nomeLocal = "index";

	HTTP::Header header(respostaHTTP);

	bool Html = false;
	for(unsigned int i = 0; i < header.campos.size(); i++){
		if(std::get<0>(header.campos[i]) == "Content-Type"){
			Html = std::get<1>(header.campos[i]).find("text/html") != std::string::npos;
			break;
		}
	}
	if(Html && header.PrimeiraLinha == "HTTP/1.1 200 OK"){
		dados = header.corpo;
		procuraReferencias("<a href=\"");
		valido = true;
	} else {
		valido = false;
	}
}

std::string Recurso::getNome(){
	return nome;
}

std::string Recurso::getNomeLocal(){
	return nomeLocal;
}

std::vector< Recurso::Referencia > Recurso::getRecursosReferenciados(){
	return recursosReferenciados;
}

std::vector< unsigned long int > Recurso::getReferencias() {
	return referencias;
}

bool Recurso::salvar( std::string caminhoRoot ) {
}

void Recurso::setaReferencias(std::vector< long long int > refs){
	for(unsigned int i = 0, j = 0; i < refs.size(); i++, j++){
		if(refs[i] != -1){
		    referencias.push_back(refs[i]);
		}
		else{
			recursosReferenciados.erase(recursosReferenciados.begin() + j--);
		}
	}
}


bool Recurso::Valido(){
	return valido;
}


void Recurso::printaNaArvore( unsigned int nivel, bool ultimo ) {
	unsigned int i = 0;
	for( ; i < nivel-1; i++ ) {
		printf( " %c  ", 179 );
	}
	if( i < nivel ) {
		printf( " %c%c ", ultimo?192:195, 196 );
	}
	printf( "%s", nomeLocal.c_str() );
}

void Recurso::procuraReferencias(const char* propriedadeHTML){
	std::string propriedade(propriedadeHTML);
	unsigned long long int comeco = dados.find(propriedade);
	while(comeco != std::string::npos) {
		comeco += propriedade.length();
		unsigned long long int fim = dados.find("\"", comeco);
		std::string referencia = dados.substr(comeco, fim - comeco);
		unsigned long long int achaHost;
		if(referencia.size() > 1 && referencia[0] == '/' && referencia[1] != '/'){
			achaHost = 1;
		}
		else if((achaHost = referencia.find(host), achaHost != std::string::npos)){
			achaHost += host.length() + 1;
		}
		if(achaHost != std::string::npos){
			referencia = achaHost <= referencia.length() ? referencia.substr(achaHost) : std::string("");
			recursosReferenciados.emplace_back(comeco, fim, referencia);
		}
		comeco = dados.find(propriedade, fim + 2);
	}
}

///funçoes para Spider
Spider::Spider(std::string host) : sucesso(false), nomeArvoreRoot(host){
	if(!socket.conecta(host, "80")){
		return;
	}
	std::queue< std::string > recursosDownload;
	recursosDownload.push(host);
	short contador= 0;

	printf("Caminhando para %s e computando a arvore", host.c_str());
	fflush(stdout);

	while(!recursosDownload.empty()){
		contador = (contador+1)%10;
		if(contador == 0){
			printf(".");
			fflush(stdout);
		}
		if(achaRecursos(recursosDownload.front()) == -1){
			std::string dados(recursosBaixados(host, recursosDownload.front()));
			if(dados.empty()){ 
				return;
			}
			Recurso r(host, recursosDownload.front(), dados);
			if(r.Valido()){
				 arvore.push_back( r );
			}

			// Adiciona referencias
			std::vector< Recurso::Referencia > refs = arvore.back().getRecursosReferenciados();
			for(unsigned int i = 0; i < refs.size(); i++){
				std::string proximoRecurso = host + "/" + std::get<2>(refs[i]);
				if(achaRecursos(proximoRecurso) == -1){
					recursosDownload.push(proximoRecurso);
				}
			}

		}
		recursosDownload.pop();
	}

	// Acha e seta as referencias na arvore
	for(unsigned int j = 0; j < arvore.size(); j++){
		std::vector< Recurso::Referencia > refs = arvore[j].getRecursosReferenciados();
		std::vector< long long int > achaRefs;
		for(unsigned int i = 0; i < refs.size(); i++){
			achaRefs.push_back(achaRecursos(host + "/" + std::get<2>(refs[i])));
		}
		arvore[j].setaReferencias(achaRefs);
	}
	printf("\n");
	sucesso = true;
}

bool Spider::Valido(){
	return sucesso;
}


void Spider::printaArvoreRecursiva( unsigned long int no, int nivel, bool ultimo, std::set< unsigned long int >& valoresPrintados ) {
	arvore[no].printaNaArvore( nivel, ultimo );
	if( valoresPrintados.find( no ) != valoresPrintados.end() ) {
		printf( " (recursivo.)\n" );
	} else {
		printf( "\n" );
		valoresPrintados.emplace( no );
		std::vector< unsigned long int > refs = arvore[no].getReferencias();
		for( unsigned long int i = 0; (long long int) i < (long long int) refs.size()-1; i++ ) {
			printaArvoreRecursiva( refs[i], nivel+1, false, valoresPrintados );
		}
		if( refs.size() > 0 ) printaArvoreRecursiva( refs[refs.size()-1], nivel+1, true, valoresPrintados );
	}
}


void Spider::printaArvore() {
	if( arvore.size() == 0 )
		 return;
	std::set< unsigned long int > valoresPrintados;
	printf( "Arvore hipertextual:\n%s (%s)\n", nomeArvoreRoot.c_str(), arvore[0].getNomeLocal().c_str() );
	valoresPrintados.emplace( 0 );
	std::vector< unsigned long int > refs = arvore[0].getReferencias();
	for( unsigned long int i = 0; (long long int) i < (long long int) refs.size()-1; i++ ) {
		printaArvoreRecursiva( refs[i], 1, false, valoresPrintados );
	}
	if( refs.size() > 0 ) printaArvoreRecursiva( refs[refs.size()-1], 1, true, valoresPrintados );
	printf( "\n" );
}


std::string Spider::recursosBaixados(std::string host, std::string nomeRecurso){
	int arvores = -1;

RETRY:
	arvores++;
	std::string requisicao("GET http://");
	requisicao += nomeRecurso;
	requisicao += " HTTP/1.1\r\nHost: " + host + "\r\nConnection: keep-alive\r\n\r\n";

	// Solicita recurso
	ssize_t envio = send(socket.getDescritor(), requisicao.c_str(), requisicao.length(), 0);
	if(envio < 0){
		printf("\nNao foi possivel enviar dados.");
	}

	// Recebe recurso
	struct pollfd mestre;
	mestre.fd = socket.getDescritor();
	mestre.events = POLLIN | POLLPRI;
	mestre.revents = 0;
	std::string mensagem("");
	int retornoPoll = poll(&mestre, 1, 250*240);
	if(retornoPoll > 0 && (POLLIN | POLLPRI) & mestre.revents){
		int valorlido = 0;
		do{
			char buffer[1024] = {0};
			valorlido = read( socket.getDescritor(), buffer, sizeof(buffer));
			if(valorlido == 0){
				break;
			}
			mensagem += std::string(buffer, valorlido);

			retornoPoll = poll(&mestre, 1, 250);
			if(retornoPoll < 0) {
				fprintf(stderr, "\nErro ao fazer polling no scket de spider.");
				return std::string("");
			}
		}while(retornoPoll > 0 && (POLLIN | POLLPRI) & mestre.revents);
		
		if(valorlido < 0) { // Erro ao ler socket
			fprintf(stderr, "\nNao foi possivel ler dados.");
			return std::string("");
		}
		else if(valorlido == 0) { // Fechar socket
			socket = Socket();
			if(!socket.conecta(host, "80")){
				return std::string("");
			}
			if(arvores < 3){
                goto RETRY;
			}
		}
	}
	else if(retornoPoll < 0){
		fprintf(stderr, "\nErro ao fazer polling  no socket spider.");
		return std::string("");
	}
	return mensagem;
}

long long int Spider::achaRecursos(std::string nomeRecurso){
	for(unsigned long int i = 0; i < arvore.size(); i++){
		if(arvore[i].getNome() == nomeRecurso ) return i;
	}
	return -1;
}
};
