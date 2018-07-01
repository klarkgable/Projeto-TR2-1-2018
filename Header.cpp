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


///funçoes para requisiçao de header
namespace HTTP {

RequestHeader::RequestHeader() {}

RequestHeader::RequestHeader( std::string header ) {}

};

///funçoes para resposta de header
namespace HTTP {

ResponseHeader::ResponseHeader() {}

ResponseHeader::ResponseHeader( std::string header ) {}

};
