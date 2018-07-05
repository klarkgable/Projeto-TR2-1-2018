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
#include <tuple>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//para header
#define field std::tuple< std::string, std::string >


///struct de mensagem para conexao(sockets)
namespace Socket {


typedef struct {
	std::string mensagem;
	int IDconexaoInterna;

	std::string endereco_de;
	int porta_de;

	std::string endereco_para;
	int porta_para;
} DadosMensagem;



///definiçoes das estruturas e funções de conexao interna
class conexaoInterna {

    public:
        conexaoInterna( int porta );
        ~conexaoInterna();

        void AceitaErecebe();
        ssize_t Envio( int IDconexao, std::string mensagem );

        std::vector< DadosMensagem > mensagensRecebidas;

    private:
        typedef struct {
            //SOCKETS INTERMEDIARIOS
                int socket;
            struct sockaddr_in endereco;
        } DadosConexao;

        int porta;
        //SOCKET PRINCIPAL
            int socketPrincipal;

        //criando vetor para abrir sockets caso multiplas requisiçoes
            std::vector< DadosConexao > conexoes;

        fd_set active_fd_set;
        fd_set read_fd_set;
        struct timeval timeout;

        struct sockaddr_in endereco;
    };


///definiçoes das estruturas e funções de conexao externa
class conexaoExterna {
    public:
        conexaoExterna(int porta);
        ~conexaoExterna();

        ssize_t Envio( int IDconexaoDeInterna, std::string endereco, int porta, std::string mensagem );
        void RecebeMensagens();


        std::vector< DadosMensagem > mensagensRecebidas;

    private:
        typedef struct {
            int socket;
            std::string endereco;
            int porta;
            struct sockaddr_in servidor_endereco;
            int IDconexaoDeInterna;
        } InfoSocket;

        //criando vetor para abrir sockets caso multiplas requisiçoes
            int porta
            std::vector< InfoSocket > conexoes;

        fd_set active_fd_set;
        fd_set read_fd_set;
        struct timeval timeout;
    };


};


///definição das estruturas e funções do Servidor Proxy

class ServidorProxy {

    public:
        ServidorProxy( int porta );
        ~ServidorProxy();
        bool Loop();

    private:
        Socket::conexaoInterna ci;
        Socket::conexaoExterna ce;
};



///definição das estruturas e funções do Header


namespace HTTP {
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


#endif // ESTRUTURA_H_INCLUDED
