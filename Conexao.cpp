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

#define QUEUESIZE 5

///funçoes para tratamento de Sockets
Socket::Socket(int Descritor) : valido(false), Descritor(Descritor){
	socklen_t addr_len = sizeof(endereco);
	
	//resgatando socket valido usando getsockname e setando validez
	if(getsockname(Descritor, (struct sockaddr*) &endereco, &addr_len) || addr_len != sizeof(endereco) == -1){
		fprintf(stderr, "\nErro no getsockname.\n");
	}
	else{
		valido = true;
	}
}

Socket::Socket() : valido(false), Descritor(-1), endereco(){}

//destrutor da classe, setando validez como falsa
Socket::~Socket() {
	if(valido){
		if(close(Descritor == -1)){}
		else{
			valido = false;
			Descritor = -1;
		}
	}
}

//abre uma conexao com socket
bool Socket::conecta(std::string nome, std::string porta, int socketFamily, int socketType, int protocol){
	struct addrinfo *primeiro, *ultimo;	
	struct addrinfo sugestoes;

	memset(&sugestoes, 0, sizeof(struct addrinfo));
	sugestoes.ai_family = socketFamily;
	sugestoes.ai_socktype = socketType;
	sugestoes.ai_flags = 0;
	sugestoes.ai_protocol = protocol;

		
	int s = getaddrinfo(nome.c_str(), porta.c_str(), &sugestoes, &primeiro);
	if(s < 0){
		fprintf(stderr, "\nErro ao procurar por nome.\n");
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
	}
	else{
		for(ultimo = primeiro; ultimo != NULL; ultimo = ultimo->ai_next){
			//verificando validez na abertura do socket
			Descritor = socket(ultimo->ai_family, ultimo->ai_socktype, ultimo->ai_protocol);
			if(Descritor == -1) {
				fprintf(stderr, "\nFalha ao criar socket. Tente outro endereco.\n");
				continue;
			}
			if(connect(Descritor, ultimo->ai_addr, ultimo->ai_addrlen ) == -1){
				fprintf(stderr, "\nFalha ao criar socket. Tente outro endereco.\n");
				close(Descritor);
			}
			else{
				valido = true;
				std::memcpy(&(endereco), ultimo->ai_addr, ultimo->ai_addrlen);
				break;
			}
		}
		if(ultimo == NULL){
			fprintf(stderr, "\nNao foi possivel concetar com %s:%s...\n", nome.c_str(), porta.c_str());
		}
	}
	freeaddrinfo(primeiro);
	return valido;
}

//
bool Socket::percorre(std::string nome, std::string porta, int queueSize, int socketFamily, int socketType, int protocol){
	struct addrinfo *primeiro, *ultimo;
	
	struct addrinfo sugestoes;
	memset(&sugestoes, 0, sizeof(struct addrinfo));
	sugestoes.ai_family = socketFamily;
	sugestoes.ai_socktype = socketType;
	sugestoes.ai_flags = 0;
	sugestoes.ai_protocol = protocol;

	int s = getaddrinfo(nome.c_str(), porta.c_str(), &sugestoes, &primeiro);
	if(s < 0){
		fprintf(stderr, "\nErro ao procurar nome.\n");
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
	}
	else{
		for(ultimo = primeiro; ultimo != NULL; ultimo = ultimo->ai_next){
			//verificando validez na abertura do socket
			Descritor = socket(ultimo->ai_family, ultimo->ai_socktype, ultimo->ai_protocol);
			if(Descritor == -1){
				fprintf(stderr, "\nFalha ao criar socket. Tente outro endereco.\n");
				continue;
			}
			if(bind(Descritor, ultimo->ai_addr, ultimo ->ai_addrlen) == -1){
				fprintf(stderr, "\nFalha ao criar socket de ligacao. Tente outro endereco.\n");
				close(Descritor);
				continue;
			}
			if(listen( Descritor, queueSize) == -1){
				fprintf(stderr, "\nFalha ao iniciar percorrimento e conexao com socket. Tente outro endereco.\n");
				close(Descritor);
			}
			else{
				valido = true;
				std::memcpy(&(endereco), ultimo->ai_addr, ultimo->ai_addrlen);
				break;
			}
		}
		if(ultimo == NULL){
			fprintf(stderr, "\nNao foi possivel conectar com %s:%s...\n", nome.c_str(), porta.c_str());
		}
	}
	freeaddrinfo(primeiro);
	return valido;
}


//Metodos para gets de atributos de socket

bool Socket::Valido(){
	return valido;
}

int Socket::getDescritor(){
	return Descritor;
}

unsigned int Socket::getFamily(){
	return endereco.sin_family;
}

uint16_t Socket::getPorta(){
	return endereco.sin_port;
}

uint32_t Socket::getEndereco(){
	return endereco.sin_addr.s_addr;
}

std::string Socket::getStringPorta(){
	return std::to_string(ntohs(endereco.sin_port));
}

std::string Socket::getStringEndereco(){
	return std::string(inet_ntoa(endereco.sin_addr));
}

bool Socket::operator==(Socket &rhs){
	return Descritor == rhs.Descritor;
}

namespace Inspetor{

///funçoes de requisicao interna

//construtor
conexaoInterna::conexaoInterna(int porta){
	//configurando socket para conexao interna
	if(!escutandoSocket.percorre("127.0.0.1", std::to_string(porta), QUEUESIZE)){
		fprintf(stderr, "\nNao foi possivel configurar o socket.\n");
	}
}

//destrutor
conexaoInterna::~conexaoInterna(){
	while(socketsConectados.size() > 0){
		socketsConectados.erase(socketsConectados.end());
	}
}

//faz verificaçao de sockets para autorizar conexao
bool conexaoInterna::aceitaConexoes(){
	if(!escutandoSocket.Valido()){
		return false;
	}
	bool retornaValor = true;
	struct pollfd auxiliar;

	auxiliar.fd = escutandoSocket.getDescritor();
	auxiliar.events = POLLIN | POLLPRI;
	auxiliar.revents = 0;

	int n = poll(&auxiliar, 1, 0); // Verifica se escutandoSocket tem conexoes em aguardo
	
	if(n < 0){ // n negativo significa erro
		fprintf(stderr, "\nErro ao fazer polling de escutandoSocket.");
		retornaValor = false;
	}
	else if(n > 0){ // n positivo significa que tem conexoes em aguardo
		int novoSocketFd = accept(escutandoSocket.getDescritor(), nullptr, nullptr);
		if(novoSocketFd == -1){
			fprintf(stderr, "\nNao pode aceitar socket.");
			retornaValor = false;
		}
		else{
			socketsConectados.emplace_back(new Socket(novoSocketFd));
		}
	}
	return retornaValor;
}

//verifica sockets para recebimento de requisições
bool conexaoInterna::recebeRequisicoes(){
	if(socketsConectados.size() == 0){
		return true;
	}
	
	bool retornaValor = true;
	
	// Lista de pollfds representa cada socket que pode ter atualizacao
	struct pollfd* auxiliar = new struct pollfd[socketsConectados.size()];
	for(long int i = 0; i < (long int) socketsConectados.size(); i++){
		auxiliar[i].fd = socketsConectados[i]->getDescritor();
		auxiliar[i].events = POLLIN | POLLPRI;
		auxiliar[i].revents = 0;
	}

	for(long int i = (long int) socketsConectados.size() - 1; i >= 0; i--){
		int retornoPoll = poll(auxiliar+i, 1, 0);
		if(retornoPoll > 0 && (POLLIN | POLLPRI) & auxiliar[i].revents){
			int valorlido = 0;
			std::string mensagem("");
	    	do{
	    		char buffer[1024] = {0};
				valorlido = read(socketsConectados[i]->getDescritor(), buffer, sizeof(buffer));
				if(valorlido == 0){
					break;
				}
				mensagem += std::string(buffer, valorlido);
				
				retornoPoll = poll(auxiliar+i, 1, 25);
				if(retornoPoll < 0){
					fprintf(stderr, "\nErro ao fazer polling com socket externo %ld.", i);
					retornaValor = false;
	    		}
	    	}while(retornoPoll > 0 && (POLLIN | POLLPRI) & auxiliar[i].revents);
			if(valorlido < 0){ // Erro ao ler socket
				fprintf(stderr, "\nNao foi possivel ler dados.");
				retornaValor = false;
			}
			else if(valorlido == 0){ // Fechar socket
				socketsConectados.erase(socketsConectados.begin() + i);
			}
			else{ // Registra mensagem recebida
				requisicoesRecebidas.push_back( std::make_tuple( std::weak_ptr< Socket >(socketsConectados[i]), HTTP::Header(mensagem)));
			}
	    }
	    else if(retornoPoll < 0){
			fprintf(stderr, "\nErro ao fazer polling com socket externo %ld.", i);
			retornaValor = false;
	    }
	}

	// Desaloca lista de pollfds
	delete[] auxiliar;
	return retornaValor;
}

void conexaoInterna::fechaSocket(int Descritor){
	for(long int i = 0; i < (long int) socketsConectados.size(); i++){
		if(socketsConectados[i]->getDescritor() == Descritor){
			socketsConectados.erase( socketsConectados.begin() + i);
			return;
		}
	}
}

//recebe resposta header e envia caso conexao em aberto
ssize_t conexaoInterna::enviaResposta(std::weak_ptr< Socket > recebendoSocket, HTTP::Header resposta){
    if(recebendoSocket.expired()){
    	printf("\nNao pode enviar dados. Conexao foi fechada.\n");
    	return -1;
    }
	ssize_t sent = send(recebendoSocket.lock()->getDescritor(), resposta.texto().c_str(), resposta.texto().length(), 0);

	if(sent < 0){
		printf("\nNao foi possivel enviar dados.\n");
	}
	else{
		return sent;
	}
	return -1;
}

///funçoes de requisicao externa

//construtor
conexaoExterna::conexaoExterna(){}

//destrutor
conexaoExterna::~conexaoExterna(){
	while(socketsCriados.size() > 0){
		socketsCriados.erase(socketsCriados.end());
	}
}

//abre conexao externa para enviar requisição, se sucesso envia dados de requisicao
ssize_t conexaoExterna::enviaRequisicao(std::weak_ptr< Socket > requisitandoSocket, HTTP::Header requisicao){
	parSocket parSocket;
	int n = acharParSocket(requisitandoSocket);
	if(n == -1){ // Nao encontrou um par
		Socket* s = new Socket();
		if(s->conecta(requisicao.host, requisicao.porta)){ // Sucesso ao conectar
			parSocket = std::make_tuple(std::shared_ptr<Socket>(s), requisitandoSocket);
			socketsCriados.push_back(parSocket);
		}
		else{ // Falha ao conectar
			fprintf(stderr, "\nNao foi possivel conectar para enviar requisicao.\n");
			delete s;
			return -1;
		}
	}
	else{ // Achou um par
		parSocket = socketsCriados[n];
	}
	ssize_t sent = send(std::get<0>(parSocket)->getDescritor(), requisicao.texto().c_str(), requisicao.texto().length(), 0);
	if(sent < 0){
		printf("\nNao foi possivel enviar dados.");
	}
	return sent;
}

//abre conexao para receber respostas
bool conexaoExterna::recebeRespostas(){
	arrumarSockets();
	if(socketsCriados.size() <= 0){
		return true;
	}

	bool retornaValor = true;
	
	// Lista de pollfds representa cada socket que pode ter atualizacao
	struct pollfd* auxiliar = new struct pollfd[socketsCriados.size()];
	for(long int i = 0; i < (long int) socketsCriados.size(); i++){
		auxiliar[i].fd = std::get<0>(socketsCriados[i])->getDescritor();
		auxiliar[i].events = POLLIN | POLLPRI;
		auxiliar[i].revents = 0;
	}

	for(long int i = (long int) socketsCriados.size() - 1; i >= 0; i--){
		int retornoPoll = poll(auxiliar+i, 1, 0);
		if(retornoPoll > 0 && (POLLIN | POLLPRI) & auxiliar[i].revents){
			int valorlido = 0;
			std::string mensagem("");
	    	do{
	    		char buffer[1024] = {0};
				valorlido = read(std::get<0>(socketsCriados[i])->getDescritor(), buffer, sizeof(buffer));
				if(valorlido == 0){
					break;
				}
				mensagem += std::string(buffer, valorlido);
				
				retornoPoll = poll(auxiliar+i, 1, 25);
				if(retornoPoll < 0){
					fprintf(stderr, "\nErro ao fazer polling de socket externo %ld.", i);
					retornaValor = false;
	    		}
	    	}while(retornoPoll > 0 && (POLLIN | POLLPRI) & auxiliar[i].revents);
			if(valorlido < 0) { // Erro ao ler socket
				fprintf(stderr, "\nNao foi possivel ler dados.");
				retornaValor = false;
			}
			else if(valorlido == 0) { // Fechar socket
				socketsCriados.erase(socketsCriados.begin() + i);
			}
			else{ // Registra mensagem recebida
				respostasRecebidas.push_back(std::make_tuple(std::get<1>(socketsCriados[i]), HTTP::Header(mensagem)));
			}
	    }
	    else if(retornoPoll < 0){
			fprintf(stderr, "\nErro ao fazer polling socket externo %ld.", i);
			retornaValor = false;
	    }
	}

	// Desaloca lista de pollfds
	delete[] auxiliar;
	return retornaValor;
}

//achar com weak_ptr
int conexaoExterna::acharParSocket(std::weak_ptr< Socket > s_w_ptr){
	arrumarSockets();
	if(s_w_ptr.expired()){
		return -1;
	}
	for(long int i = 0; i < (long int) socketsCriados.size(); i++){
		std::shared_ptr< Socket > inner(std::get<1>(socketsCriados[i]).lock());
		std::shared_ptr< Socket > outer(s_w_ptr.lock());
		if(inner.get() == outer.get()){ // Mesmo endereco
			if(*inner.get() == *outer.get()){ // Objs identicos
				return i;
			}
		}
	}
	return -1; // Nao encontrou
}

//achar com shared_ptr
int conexaoExterna::acharParSocket(std::shared_ptr< Socket > s_s_ptr){
	arrumarSockets();
	if(s_s_ptr.get() == nullptr){
		return -1;
	}
	for(long int i = 0; i < (long int) socketsCriados.size(); i++){
		if(std::get<0>(socketsCriados[i]) == s_s_ptr){ // Sao identicos
			return i;
		}
		else{
			std::shared_ptr< Socket > inner(std::get<0>(socketsCriados[i]));
			if(inner.get() == nullptr){ // Expirou
				continue;
			}
			if(inner.get() == s_s_ptr.get()){ // Mesmo endereco
				if(*inner.get() == *s_s_ptr.get()){ // Objs identicos
					return i;
				}
			}
		}
	}
	return -1; // Nao encontrou
}


void conexaoExterna::arrumarSockets(){ // Exclui pares cujo o socket interno foi excluido
	for(long int i = (long int) socketsCriados.size() - 1; i >= 0 ; i--){
		std::weak_ptr< Socket > s_w_ptr = std::get<1>(socketsCriados[i]);
		if(s_w_ptr.expired()) socketsCriados.erase(socketsCriados.begin() + i);
	}
}

}; //fechando namespace Inspetor
