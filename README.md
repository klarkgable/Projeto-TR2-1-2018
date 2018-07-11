# Projeto-TR2-1-2018

Universidade de Brasília

Departamento de ciência da computação

Disciplina: Teleinformática e redes 2 - 1°/2018

Professor: João Gondim 


Projeto da disciplina- Um Inspetor HTTP baseado em Proxy Server

Link do repositório GitHUb: https://github.com/klarkgable/Projeto-TR2-1-2018


Alunos:

        Klark Gable Souza Porto - 120015421
        
        Gabriel Rodrigues Diógenes Macedo - 15/0126808



*COMO RODAR O PROJETO*

	Para rodar projeto, é bem simples. Sistema usado tem que ser distribuição Linux.
	Ter instalado o compilador G++ e o Make para rodar o arquivo makefile
	
	Passos:
		1)Baixar arquivos fontes do projeto, copiar para alguma pasta
		
		2)Abrir terminal e caminhar até a pasta do projeto(comando CD)
		
		3)Rodar arquivo makefile( comando "make -f makefile")
		
		4)Será gerado os arquivos objetos e o executável aracne
		
		5)Rodar aracne( comando ./aracne) que irá rodar na porta 8228 caso não passe outra porta como argumento. Para fazer os requests e responses o navegador web que for utilizar deve ter o proxy configurado no servidorProxy:127.0.0.1 e porta 8228.
		6) Escolha opção 1 para rodar o interceptor ou opção 2 para rodar o spider. Dumper não foi possível implementar
		7) No caso do interceptor, abra o navegador e entre nos sites para gerar as requisições e respostas.
		6)Para rodar o Spider, precisa abrir a main e setar a chamada ao Spider com por exemplo Spider("www.sitequalquer.com"). Caso não faça isso ele irá rodar o spider no site configurado previamente.
		
		

          
*ARQUITETURA DO PROJETO*

        O projeto foi feito em C++, usando API padrões e várias bibliotecas de suporte para aplicações em redes. Foi feito em C++, pois acreditamos que na visão orientada a objetos seria melhor estruturado o trabalho e a comunicação entre classes através dos seus respectivos módulos dariam uma visão mais tangível e simples de entender e interagir.
	
       Para isso, dividimos o projeto em vários módulos:
       
                
		Servidor(módulo para servidorProxy, por onde passam as requisições e respostas)
                
		Conexao( módulo geral para sockets e conexao interna e externa)
                
		Header( responsavél por setar ou recuperar cabeçahos HTTP de requisição ou resposta)
                       Obs: os modulos conexao e servidor em conjunto e utilizando das funções do Header, formam o Inspetor HTTP
                
		ModuloBase( Serve como base para recursos, Spider e dump)
                
		main(É o módulo principal que chama as rotinas, e seleciona entre inspeção, spider ou cliente recursivo)
       
       Para definição das funções e estruturas do projeto, temos o módulo Estrutura.h.
       
       O interceptor tem um lado que recebe e envia para o navegador e um outro lado que recebe envia para a internet. Nesse caso, o servidor será o responsável por fazer essa comunicação entre os lados.
	O spider irá percorrer todo o site procurando por outros html linkados e monta a árvore de referência.
	O cliente recursivo irá fazer o dump, que seria parecido com o spider, porém não só com html, e no final baixa para uma pasta local.


	Definição das classes:
        	///classe para Sockets

	classe Socket 
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
   

	///definiçoes das estruturas e funções de conexao interna
	class conexaoInterna 

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



	///definiçoes das estruturas e funções de conexao externa
	class conexaoExterna 
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


	///definição das estruturas e funções do Servidor Proxy

	class ServidorProxy 

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


	///definição das estruturas e funções do Spider e Cliente recursivo



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


	class Spider 
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


        
               
	      

                
                      
      
