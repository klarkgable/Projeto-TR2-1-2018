///Projeto final para disciplina de Teleinformática e Redes 2 1°/2018
///Alunos:
        ///Klark Gable Souza Porto  120015421
        ///Gabriel Rodrigues Diógenes Macedo  15/0126808


#ifndef ESTRUTURA_H_INCLUDED
#define ESTRUTURA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <memory>
#include <tuple>
#include <poll.h>
#include <queue>
#include <unistd.h>
#include <netdb.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//para header
#define field std::tuple< std::string, std::string >


///classe para Sockets
class Socket {
public:
	Socket();
	Socket( int Descritor );
	~Socket();

	//funçoes para atribuiçao padrao para sockets
	bool conecta( std::string nome, std::string porta, int socketFamily = AF_INET, int socketType = SOCK_STREAM, int protocol = 0 );
	bool percorre( std::string nome, std::string porta, int queueSize = 1, int socketFamily = AF_INET, int socketType = SOCK_STREAM, int protocol = 0 );

	bool Valido();
	int getDescritor();
	unsigned int getFamily();
	uint16_t getPorta();
	uint32_t getEndereco();
	std::string getStringPorta();
	std::string getStringEndereco();
	
	bool operator==( Socket &rhs );
private:
	bool valido;
	int Descritor;
	struct sockaddr_in endereco;
};


namespace HTTP {
///definição das estruturas e funções do Header

    //Uma´unica estrutura para cabeçalho http que identifica e separa os campos para tratar de requisiçao e resposta
    struct Header {

        //metodo para separar os campos
        Header( std::string& palavra );

        //metodo que transforma struct para string para envio na rede
            std::string texto( bool incluiCorpo = true );

        //primeira linha e campos http
            std::string PrimeiraLinha;
            std::vector< field > campos;

        //corpo da mensagem http
            std::string corpo;

        // caminho e porta
            std::string host;
            std::string porta;
    };

};


namespace Inspetor {

///definiçoes das estruturas e funções de conexao interna
class conexaoInterna {

    public:
        conexaoInterna( int porta );
        ~conexaoInterna();
	typedef std::tuple< std::weak_ptr< Socket >, HTTP::Header > Requisicao;


	bool aceitaConexoes();
	bool recebeRequisicoes();
	void fechaSocket( int Descritor );
ssize_t enviaResposta( std::weak_ptr< Socket > recebendoSocket, HTTP::Header resposta );

	std::vector< Requisicao > requisicoesRecebidas;	

    private:
	Socket escutandoSocket;
	std::vector< std::shared_ptr< Socket > > socketsConectados;

    };


///definiçoes das estruturas e funções de conexao externa
class conexaoExterna {
    public:
        conexaoExterna(int porta);
	conexaoExterna();
        ~conexaoExterna();
	typedef std::tuple< std::weak_ptr< Socket >, HTTP::Header > Resposta;

	ssize_t enviaRequisicao( std::weak_ptr< Socket > requisitandoSocket, HTTP::Header requisicao );
	bool recebeRespostas();

	std::vector< Resposta > respostasRecebidas;
	
    private:
        typedef std::tuple< std::shared_ptr< Socket >,std::weak_ptr< Socket > > parSocket;
	int acharParSocket( std::weak_ptr< Socket > s_w_ptr );
	int acharParSocket( std::shared_ptr< Socket > s_s_ptr );
	void arrumarSockets();

	std::vector< parSocket > socketsCriados;
    };


///definição das estruturas e funções do Servidor Proxy

class ServidorProxy {

    public:
        ServidorProxy( int porta );
        ~ServidorProxy();
        bool Loop();

	bool continuando;
	std::vector< conexaoInterna::Requisicao > &requisicoesRecebidas;
	std::vector< conexaoInterna::Requisicao > requisicoesEnvio;
	std::vector< conexaoExterna::Resposta > &respostasRecebidas;
	std::vector< conexaoExterna::Resposta > respostasEnvio;

    private:
        conexaoInterna ci;
        conexaoExterna ce;
  };


};//fechando namespace Inspetor


///definição das estruturas e funções do Spider e Cliente recursivo

namespace ModuloBase{


struct Recurso {

	public:
		typedef std::tuple< unsigned long long int, unsigned long long int, std::string > Referencia;
		Recurso( std::string host, std::string nome, std::string respostaHTTP );
	
		std::string getNome();
		std::string getNomeLocal();
		std::vector< Referencia > getRecursosReferenciados();
		void setaReferencias( std::vector< long long int > refs );
		bool salvar( std::string caminhoRoot );
		bool Valido();

		
	private:
		void procuraReferencias( const char* propriedadeHTML );
		std::string host;
		std::string nome;
		std::string nomeLocal;
		std::string dados;
		std::vector< Referencia > recursosReferenciados;
		std::vector< unsigned long int > referencias;
		bool valido;

};


class Spider {
	public:
		Spider( std::string host );
		bool Valido();
	private:
		std::string recursosBaixados( std::string host, std::string nomeRecurso );
		long long int achaRecursos( std::string nomeRecurso );
	
		Socket socket;
		bool sucesso;
		std::string nomeArvoreRoot;
		std::vector< Recurso > arvore;
};


};//fechando namespace ModuloBase



#endif // ESTRUTURA_H_INCLUDED
