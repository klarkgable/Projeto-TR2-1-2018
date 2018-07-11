///Projeto final para disciplina de Teleinformática e Redes 2 1°/2018
///Alunos:
        ///Klark Gable Souza Porto  120015421
        ///Gabriel Rodrigues Diógenes Macedo  15/0126808

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<iostream>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Estrutura.h"

using std::cout;
using std::cin;

int main(int argc, char* argv[]){
	int porta = 8228;
	int funcionalidade;

	if(argc == 3) {
		if(std::string(argv[1]) == "-p"){
			try{
				porta = std::stoi(std::string(argv[2]));
			}catch(std::exception e) {
				printf("\nPorta invalida.\n");
				return -1;
			}
		}else{
			printf("\nEntrada nao reconhecida.\nOpçoes validas:\n[-p <port>]\\Escolha outra porta diferente da padrão 8228 %d\n", porta);
			return -1;
		}
	}else if(argc != 1){
		printf("\nNumero de argumentos invalido.\n");
		return -1;
	}

	printf("\nIniciando aracne na porta # %d\n\n", porta);

	
	printf("ESCOLHA A FUNCIONALIDADE(1-INSPETOR, 2-SPIDER, 3-DUMP):" );
	scanf("%d",&funcionalidade);

	if(funcionalidade==1){
		Inspetor::ServidorProxy teste1(porta);
		while(teste1.Loop()) {};
	}
	else if(funcionalidade==2){
		std::string siteSpider;
		
		//cout << "Entre com o site no formato ''www.site...'' para gerar a arvore hipertextual: ";

		//getline(cin, siteSpider);

		ModuloBase::Spider teste2( "www.unb.br");
		teste2.printaArvore();
	}
	else{
		return -1;
		}

	return 0;
}
