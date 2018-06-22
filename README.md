# Projeto-TR2-1-2018

Universidade de Brasília

Departamento de ciência da computação

Disciplina: Teleinformática e redes 2 - 1°/2018

Professor: João Gondim 


Projeto da disciplina- Um Inspetor HTTP baseado em Proxy Server

Alunos:

        Klark Gable Souza Porto - 120015421
        
        Gabriel Rodrigues Diógenes Macedo - 15/0126808



INTRODUÇÃO TEÓRICA


Cliente-servidor

        O modelo cliente-servidor,é uma estrutura de aplicação que distribui as tarefas e cargas de trabalho entre os fornecedores de um recurso ou serviço(servidores), e os requerentes dos serviços(clientes).
        Geralmente os clientes e servidores comunicam através de uma rede em computadores distintos, mas tanto o cliente quanto o servidor podem residir no mesmo computador.
        Um servidor é um host que está executando um ou mais serviços ou programas que compartilham recursos com os clientes. Um cliente não compartilha seus recursos, mas solicita um conteúdo ou função do servidor. Os clientes iniciam sessões de comunicação com os servidores que aguardam requisições de entrada.
        
        Características do Cliente:

                Inicia pedidos para servidores;
                Espera por respostas;
                Recebe respostas;
                Conecta-se a um pequeno número de servidores de uma só vez ;
                Normalmente interage diretamente com os servidores através de seu software aplicação especifico, que lhe possibilita a comunicação com o servidor;
                Utiliza recursos da rede.

        Características do Servidor:

                Sempre espera por um pedido de um cliente;
                Atende os pedidos e, em seguida, responde aos clientes com os dados solicitados;
                Podem se conectar com outros servidores para atender uma solicitação específica do cliente; jamais podem se             comunicar.
                Fornece recursos de rede.
                Normalmente interage diretamente com os usuários finais através de qualquer interface com o usuário;
                Estrutura o sistema.

       
       
Requisições HTTP
  
        HTTP é a sigla de "Hyper Text Transfer Protocol", o que traduzido seria "Protocolo de Tranferência de Hiper Texto". Quando acessamos a pígina do google, o navegador se conecta na porta 80 do servidor - isto mesmo, o protocolo HTTP roda sob a porta 80 - e envia uma requisição HTTP para este. Isso não ocorre apenas com píginas WEB - podemos fazer download de arquivos de som, fotos, músicas, etc.
        Cliente --------------> Servidor (1)
        Cliente <-------------- Servidor (2)

        (1) O cliente se conecta na porta 80 do servidor; Envia uma requisição HTTP.
        (2) O servidor obtém esta requisição, e dependendo do seu conteúdo, envia uma determinada resposta ao cliente.
       
       Esta é a estrutura bísica de uma requisição HTTP. Cada "comando" equivale a um header (ou cabeçalho). Poderíamos resumir da seguinte forma:

        [CLIENTE]
        cabeçalhos

        --------------------------

        [SERVIDOR]
        cabeçalhos de resposta

        (código do arquivo requisitado)

        --------------------------
        
        Este header diz ao servidor que o cliente está requisitando o arquivo principal do diretório

Sockets

        Especificamente em computação, um soquete pode ser usado em ligações de redes de computadores para um fim de um elo bidirecional de comunicação entre dois programas. A interface padronizada de soquetes surgiu originalmente no sistema operacional Unix BSD(Berkeley Software Distribution); portanto, eles são muitas vezes chamados de Berkeley Sockets. É também uma abstração computacional que mapeia diretamente a uma porta de transporte (TCP ou UDP) e mais um endereço de rede. Com esse conceito é possível identificar unicamente um aplicativo ou servidor na rede de comunicação IP.
        Um socket identifica univocamente um usuário TCP 
        Permite a associação entre processos de aplicação 
        O identificador da porta é concatenado ao endereço IP, onde a entidade TCP está rodando, definindo um socket.
        Função socket ()
                exemplo de uso
                        socket (familia, tipo, protocolo);
          O nome de um socket sempre está relacionado a um espaço de nomes, também chamado de domínio (socket domain).Cada espaço de nomes é definido por uma macro na forma PF_* (que vem do termo Protocol Family). Os sockets são divididos em tipos. São eles: SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET, SOCK_RAW, SOCK_RDM, SOCK_PACKET.
          
          

                      
      
