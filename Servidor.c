///Projeto final para disciplina de Teleinformática e Redes 2 1°/2018
///Alunos:
        ///Klark Gable Souza Porto  120015421
        ///Gabriel Rodrigues Diógenes Macedo  15/0126808

#include Estrutura.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


void ServidorProxy( int port ) : il( port ), el() {}

void Construtor_ServidorProxy() {}

bool Loop() {
	// Leaving machine first
	il.AcceptAndReceive();
	if(0 < (int) il.messagesReceived.size() ) printf( "\nThere are %d outbound packets.\n", (int) il.messagesReceived.size() );
	for( int i = 0; i < (int) il.messagesReceived.size(); i++ ) {
		printf( "\nMessage from %s:%d to %s:%d\n%s\n"
			, il.messagesReceived[i].addr_from.c_str()
			, il.messagesReceived[i].port_from
			, il.messagesReceived[i].addr_to.c_str()
			, il.messagesReceived[i].port_to
			, il.messagesReceived[i].message.c_str()
		);
		//el.Send( il.messagesReceived[i].internalConnectionID, il.messagesReceived[i].addr_to, il.messagesReceived[i].port_to, il.messagesReceived[i].message );
	}
	il.messagesReceived.clear();

	// Arriving machine last
	el.ReceiveMessages();
	if(0 < (int) el.messagesReceived.size() ) printf( "\nThere are %d inbound packets.\n", (int) el.messagesReceived.size() );
	for( int i = 0; i < (int) el.messagesReceived.size(); i++ ) {
		printf( "\nMessage from %s:%d to %s:%d\n%s\n"
			, il.messagesReceived[i].addr_from.c_str()
			, il.messagesReceived[i].port_from
			, il.messagesReceived[i].addr_to.c_str()
			, il.messagesReceived[i].port_to
			, il.messagesReceived[i].message.c_str()
		);
		//il.Send( el.messagesReceived[i].internalConnectionID, el.messagesReceived[i].message );
	}
	el.messagesReceived.clear();
	return true;
}
