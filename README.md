# Projeto-TR2-1-2018

Universidade de Brasília

Departamento de ciência da computação

Disciplina: Teleinformática e redes 2 - 1°/2018

Professor: João Gondim 


Projeto da disciplina- Um Inspetor HTTP baseado em Proxy Server

Alunos:

        Klark Gable Souza Porto - 120015421
        
        Gabriel Rodrigues Diógenes Macedo - 15/0126808



Introdução Teórica

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

        
      
