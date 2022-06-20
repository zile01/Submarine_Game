#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

//Defines
#define DEFAULT_BUFLEN_SERVER 30
#define DEFAULT_BUFLEN_CLIENT 30
#define DEFAULT_PORT          27015

//Global variables
unsigned int clientSock[2];                                                                                             //array of clients
unsigned int indx = 0;                                                                                                  //number of clients connected
unsigned int turn = 1;                                                                                                  //client which is on turn
unsigned int gameStarted = 0;                                                                                           //if game is started
unsigned int cooSet1 = 0;                                                                                               //1st client coo set
unsigned int cooSet2 = 0;                                                                                               //2nd client coo set

//Functions
void ReceiveCoordinates();
void SendTurn();
int  FirstAttacks();
int  SecondAttacks();
int  EndCheck(char* string);

int main(int argc , char *argv[]){
    int i = 0;
    int serverSock, c;
    struct sockaddr_in server, client;
    int a;
    int b;
    char clientAttackMessage[DEFAULT_BUFLEN_CLIENT];
    char clientDefenseMessage[DEFAULT_BUFLEN_CLIENT];

    char winMessage[DEFAULT_BUFLEN_CLIENT*2];
    strcpy(winMessage, "WWW");

    memset(clientAttackMessage, '\0', DEFAULT_BUFLEN_CLIENT);
    memset(clientDefenseMessage, '\0', DEFAULT_BUFLEN_CLIENT);

    serverSock = socket(AF_INET , SOCK_STREAM , 0);
    if (serverSock == -1)
    {
        printf("Creating socket failed");
    }
    puts("Socket created successfuly");

    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if( bind(serverSock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("Binding failed");
        return 1;
    }
    puts("Binding successful");

    if(listen(serverSock, 2) < 0){
        perror("Listen failed");
        return 1;
    }
    puts("Listen successful");

    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);

    while( (clientSock[indx] = accept(serverSock, (struct sockaddr*)&client, (socklen_t*)&c )) ){
        if (clientSock < 0){
            perror("Connection failed");
            return 1;
        }
        puts("Client connected successfuly");
        indx++;

        if(indx > 1){
            break;
        }
    }

    while(1) {

        if (gameStarted == 0) {
            ReceiveCoordinates();
            SendTurn();
        }

        gameStarted = 1;

        if (cooSet1 == 1 && cooSet2 == 1) {

            if (turn == 1) {
                a = FirstAttacks();

                if(a == 1){
                    puts("There was an error while sending messages");
                    return a;
                }else if(a == 2){
                    puts("Server is closing");
                    return a;
                }

                if (turn == 2) {
                    continue;
                }

            }else if (turn == 2) {
                b = SecondAttacks();

                if(b == 1){
                    puts("There was an error while sending messages");
                    return b;
                }else if(b == 2){
                    puts("Server is closing");
                    return b;
                }

                if (turn == 1) {
                    continue;
                }
            }
        }
    }

    close(serverSock);
    for(i = 0; i < indx; i++){
        close(clientSock[i]);
    }

    return 0;
}

void ReceiveCoordinates(){
    char coordinatesSet[DEFAULT_BUFLEN_CLIENT/2];
    memset(coordinatesSet, '\0', DEFAULT_BUFLEN_CLIENT/2);

    if (turn == 1) {
        if (recv(clientSock[0], coordinatesSet, sizeof(coordinatesSet), 0) > 0) {
            cooSet1 = atoi(coordinatesSet);
        }

        if (recv(clientSock[1], coordinatesSet, sizeof(coordinatesSet), 0) > 0) {
            cooSet2 = atoi(coordinatesSet);
        }
    }else if (turn == 2) {
        if (recv(clientSock[1], coordinatesSet, sizeof(coordinatesSet), 0) > 0) {
            cooSet2 = atoi(coordinatesSet);
        }

        if (recv(clientSock[0], coordinatesSet, sizeof(coordinatesSet), 0) > 0) {
            cooSet1 = atoi(coordinatesSet);
        }
    }
}

void SendTurn(){
    char turnMessage[DEFAULT_BUFLEN_CLIENT/2];
    memset(turnMessage, '\0', DEFAULT_BUFLEN_CLIENT/2);

    if (turn == 1) {
        strcpy(turnMessage, "1");
        if (send(clientSock[0], turnMessage, strlen(turnMessage), 0) < 0) {
            return;
        }

        strcpy(turnMessage, "0");
        if (send(clientSock[1], turnMessage, strlen(turnMessage), 0) < 0) {
            return;
        }

    } else if (turn == 2) {
        strcpy(turnMessage, "1");
        if (send(clientSock[1], turnMessage, strlen(turnMessage), 0) < 0) {
            return;
        }

        strcpy(turnMessage, "0");
        if (send(clientSock[0], turnMessage, strlen(turnMessage), 0) < 0) {
            return;
        }

    }
}

int FirstAttacks(){
    char clientAttackMessage[DEFAULT_BUFLEN_CLIENT];
    char clientDefenseMessage[DEFAULT_BUFLEN_CLIENT];

    char winMessage[DEFAULT_BUFLEN_CLIENT*2];
    strcpy(winMessage, "WWW");

    memset(clientAttackMessage, '\0', DEFAULT_BUFLEN_CLIENT);
    memset(clientDefenseMessage, '\0', DEFAULT_BUFLEN_CLIENT);

    if (recv(clientSock[0], clientAttackMessage, sizeof(clientAttackMessage), 0) > 0) {

        if(EndCheck(clientAttackMessage) == 0){
            if (send(clientSock[1], winMessage, strlen(winMessage), 0) < 0) {

                return 1;
            }
            puts("CLIENT 1 DISCONNECTED");
            puts("CLIENT 2 WON");
            return 2;
        }

        if (send(clientSock[1], clientAttackMessage, strlen(clientAttackMessage), 0) < 0) {
            return 1;
        }
    }

    if (recv(clientSock[1], clientDefenseMessage, sizeof(clientDefenseMessage), 0) > 0) {

        if(EndCheck(clientDefenseMessage) == 0){
            if (send(clientSock[0], winMessage, strlen(winMessage), 0) < 0) {

                return 1;
            }
            puts("CLIENT 2 DISCONNECTED");
            puts("CLIENT 1 WON");
            return 2;
        }

        if (strcmp(clientDefenseMessage, "YOU WON") == 0) {
            gameStarted = 0;
        }

        if (strcmp(clientDefenseMessage, "HIT") == 0) {
            turn = 1;
        } else {
            turn = 2;
        }

        if (send(clientSock[0], clientDefenseMessage, strlen(clientDefenseMessage), 0) < 0) {
            return 1;
        }
    }

    memset(clientDefenseMessage, '\0', DEFAULT_BUFLEN_CLIENT);

    return 0;
}

int SecondAttacks(){
    char clientAttackMessage[DEFAULT_BUFLEN_CLIENT];
    char clientDefenseMessage[DEFAULT_BUFLEN_CLIENT];

    char winMessage[DEFAULT_BUFLEN_CLIENT*2];
    strcpy(winMessage, "WWW");

    memset(clientAttackMessage, '\0', DEFAULT_BUFLEN_CLIENT);
    memset(clientDefenseMessage, '\0', DEFAULT_BUFLEN_CLIENT);

    if (recv(clientSock[1], clientAttackMessage, sizeof(clientAttackMessage), 0) > 0) {

        if(EndCheck(clientAttackMessage) == 0){
            if (send(clientSock[0], winMessage, strlen(winMessage), 0) < 0) {

                return 1;
            }
            puts("CLIENT 2 DISCONNECTED");
            puts("CLIENT 1 WON");
            return 2;
        }

        if (send(clientSock[0], clientAttackMessage, strlen(clientAttackMessage), 0) < 0) {
            return 1;
        }
    }

    if (recv(clientSock[0], clientDefenseMessage, sizeof(clientDefenseMessage), 0) > 0) {

        if(EndCheck(clientDefenseMessage) == 0){
            if (send(clientSock[1], winMessage, strlen(winMessage), 0) < 0) {

                return 1;
            }
            puts("CLIENT 1 DISCONNECTED");
            puts("CLIENT 2 WON");
            return 2;
        }

        if (strcmp(clientDefenseMessage, "YOU WON") == 0) {
            gameStarted = 0;
        }

        if (strcmp(clientDefenseMessage, "HIT") == 0) {
            turn = 2;
        } else {
            turn = 1;
        }

        if (send(clientSock[1], clientDefenseMessage, strlen(clientDefenseMessage), 0) < 0) {
            return 1;
        }
    }

    memset(clientDefenseMessage, '\0', DEFAULT_BUFLEN_CLIENT);

    return 0;
}

int EndCheck(char* string){
    if(strcmp(string, "QQQ") == 0){
        return 0;
    }

    return 1;
}
