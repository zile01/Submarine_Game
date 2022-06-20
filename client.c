#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>

//Defines
#define DEFAULT_BUFLEN_SERVER 30
#define DEFAULT_BUFLEN_CLIENT 30
#define DEFAULT_PORT          27015

#define SETTING_COO     '1'
#define START_GAME      '2'
#define CONTINUE_GAME   '3'
#define END_GAME        '4'

//Global variables
unsigned int matrix[5][5] = {0};
unsigned int flag1 = 0;                                                                                                 //1dim matrix filled
unsigned int flag2 = 0;                                                                                                 //2dim matrix filled
unsigned int flag3 = 0;                                                                                                 //3dim matrix filled
unsigned int gameStarted = 0;                                                                                           //game started
unsigned int turn = 0;                                                                                                  //client which is on turn

//Functions
void  Menu(int* clientSock);
void  SetCoordinates();
void  PrintMatrix();
void  FillMatrix1(char v, char k);
void  FillMatrix2(char v, char k, char p);
void  FillMatrix3(char v, char k, char p);
int   MatchingCheck2(char v, char k, char p);
int   MatchingCheck3(char v, char k, char p);
void  StartGame(int* clientSock);
int   InputCheck(char* client_message);
int   HitCheck(char* client_attack_message);
int   MatrixEmpty();
void  Reset();
void  EndGame(int* cSock);
int   EndCheck(char* string);
void  printManual();

int main(int argc , char *argv[]){
    int clientSock;
    struct sockaddr_in server;

    clientSock = socket(AF_INET , SOCK_STREAM , 0);
    if (clientSock < 0)
    {
        printf("\033[0;31m");
        puts("Creating socket failed");
        printf("\033[0m");
        return 1;
    }

    printf("\033[0;32m");
    puts("Socket created successfuly");
    printf("\033[0m");

    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
    server.sin_addr.s_addr = inet_addr("192.168.85.223");

    if (connect(clientSock, (struct sockaddr *)&server , sizeof(server)) < 0){
        printf("\033[0;31m");
        perror("Connection failed");
        printf("\033[0m");
        return 1;
    }
    printf("\033[0;32m");
    puts("Connection successfuly established");
    printf("\033[0m");

    Menu(&clientSock);

    close(clientSock);
    return 0;
}

void Menu(int* clientSock){
    char tmp[5];
    memset(tmp, '\0', strlen(tmp));
    char choice;
    int* cSock = clientSock;

    while(1) {
        do {
            puts("*************************************************************************************************");
            puts("");
            puts("-----------------------------------------------Menu----------------------------------------------");
            puts("* [1] SET COORDINATES;                                                                          *");
            puts("* [2] START GAME;                                                                               *");
            puts("* [3] CONTINUE GAME;                                                                            *");
            puts("* [4] END GAME;                                                                                 *");
            puts("-------------------------------------------------------------------------------------------------");
            printf("--> ENTER THE OPERATION YOU WISH: ");
            scanf("%s", tmp);
            puts("");
        } while (strlen(tmp) != 1);

        choice = tmp[0];

        switch (choice) {
            case SETTING_COO:
                SetCoordinates(cSock);
                break;

            case START_GAME:
                if(flag1 == 1 && flag2 == 1 && flag3 == 1) {
                    if(gameStarted == 0){
                        StartGame(cSock);
                    }else{
                        printf("\033[0;31m");
                        system("clear");
                        puts("GAME HAS ALREADY STARTED");
                        puts("");
                        printf("\033[0m");
                    }
                }else{
                    printf("\033[0;31m");
                    system("clear");
                    puts("YOU HAVE TO ENTER THE COORDINATES FIRST");
                    puts("");
                    printf("\033[0m");
                }
                break;

            case CONTINUE_GAME:
                if(gameStarted == 1){
                    StartGame(cSock);
                }else{
                    printf("\033[0;31m");
                    system("clear");
                    puts("YOU HAVE TO START THE GAME FIRST");
                    puts("");
                    printf("\033[0m");
                }

                break;

            case END_GAME:
                EndGame(cSock);
                return;

            default:
                break;
        }
    }
}



void SetCoordinates(){
    char oneDim[2];
    char twoDim[3];
    char threeDim[3];
    memset(oneDim, '\0', 2);
    memset(twoDim, '\0', 3);
    memset(threeDim, '\0', 3);
    char v, k, p;
    int len;

    if(flag1 == 1 && flag2 == 1 && flag3 == 1){
        printf("\033[0;31m");
        system("clear");
        puts("YOU HAVE ALREADY SET THE COORDINATES");
        printf("\033[0m");
        puts("");
        return;
    }

    if(flag1 == 0) {
        do {
            system("clear");
            printManual();
            PrintMatrix();

            memset(oneDim, '\0', 2);
            printf("--> 1-DIM MATRIX COORDINATES: ");
            scanf("%s", oneDim);
            len = strlen(oneDim);
            v = oneDim[0];
            k = oneDim[1];

            if (oneDim[0] == 'M' || oneDim[0] == 'm') {
                return;
            }
        } while (len != 2 ||
                (v < '1' || v > '5') ||
                (k < 'A' || k > 'E')
                );

        FillMatrix1(v, k);
    }

    if(flag2 == 0) {
        do {
            system("clear");
            printManual();
            PrintMatrix();

            memset(twoDim, '\0', 3);
            printf("--> 2-DIM MATRIX COORDINATES: ");
            scanf("%s", twoDim);
            len = strlen(twoDim);
            v = twoDim[0];
            k = twoDim[1];
            p = twoDim[2];

            if (twoDim[0] == 'M' || twoDim[0] == 'm') {
                return;
            }
        } while (len != 3 ||
                (v < '1' || v > '5') ||
                (k < 'A' || k > 'E') ||
                ((p != 'H') && (p != 'V')) ||
                ((k == 'E') && (p == 'H')) ||
                ((v == '5') && (p == 'V')) ||
                (MatchingCheck2(v, k, p) == 1)
                );

        FillMatrix2(v, k, p);
    }

    if(flag3 == 0) {
        do {
            system("clear");
            printManual();
            PrintMatrix();

            memset(threeDim, '\0', 3);
            printf("--> 3-DIM MATRIX COORDINATES: ");
            scanf("%s", threeDim);
            len = strlen(threeDim);
            v = threeDim[0];
            k = threeDim[1];
            p = threeDim[2];

            if (threeDim[0] == 'M' || threeDim[0] == 'm') {
                return;
            }
        } while (len != 3 ||
                 (v < '1' || v > '5') ||
                 (k < 'A' || k > 'E') ||
                 ((p != 'H') && (p != 'V')) ||
                 ((k == 'D') && (p == 'H')) ||
                 ((k == 'E') && (p == 'H')) ||
                 ((v == '4') && (p == 'V')) ||
                 ((v == '5') && (p == 'V')) ||
                 (MatchingCheck3(v, k, p) == 1)
                );

        FillMatrix3(v, k, p);
    }

      system("clear");
      printManual();
      PrintMatrix();
}

//TODO dodaj vrste i kolone
void PrintMatrix(){
    int i, j;
    printf("\033[0;36m");
    puts("");
    printf("\tA\tB\tC\tD\tE\t\n");
    puts("      *************************************");
    for (i = 0; i < 5; i++){
        printf("%d     * ", i + 1);
        for (j = 0; j < 5; j++){
            if(j == 4){
                printf("%d *", matrix[i][j]);
            }else{
                printf("%d\t", matrix[i][j]);
            }
        }
        printf("\n");
        if(i != 4){
            printf("      * \t\t\t\t  *\n");
        }
    }
    puts("      *************************************");
    puts("");
    printf("\033[0m");
}

void FillMatrix1(char v, char k){
    int row = v - 49;
    int col = k - 65;

    matrix[row][col] = 1;

    flag1 = 1;
}

void FillMatrix2(char v, char k, char p){
    int row = v - 49;
    int col = k - 65;
    char direction = p;

    matrix[row][col] = 2;

    if(direction == 'H'){
       matrix[row][col + 1] = 2;
    }else if(direction == 'V') {
        matrix[row + 1][col] = 2;
    }

    flag2 = 1;
}

void FillMatrix3(char v, char k, char p){
    int row = v - 49;
    int col = k - 65;
    char direction = p;

    matrix[row][col] = 3;

    if(direction == 'H'){
        matrix[row][col + 1] = 3;
        matrix[row][col + 2] = 3;
    }else if(direction == 'V') {
        matrix[row + 1][col] = 3;
        matrix[row + 2][col] = 3;
    }

    flag3 = 1;
}

int MatchingCheck2(char v, char k, char p){
    int row = v - 49;
    int col = k - 65;
    char direction = p;

    if(matrix[row][col] == 1) {
        return 1;
    }

    if(direction == 'H'){
        if(matrix[row][col + 1] == 1){
            return 1;
        }
    }else if(direction == 'V'){
        if(matrix[row + 1][col] == 1){
            return 1;
        }
    }

    return 0;
}

int MatchingCheck3(char v, char k, char p){
    int row = v - 49;
    int col = k - 65;
    char direction = p;

    if(matrix[row][col] == 1 || matrix[row][col] == 2) {
        return 1;
    }

    if(direction == 'H'){
        if( matrix[row][col + 1] == 1 ||
            matrix[row][col + 1] == 2 ||
            matrix[row][col + 2] == 1 ||
            matrix[row][col + 2] == 2
            ){
            return 1;
        }
    }else if(direction == 'V'){
        if( matrix[row + 1][col] == 1 ||
            matrix[row + 1][col] == 2 ||
            matrix[row + 2][col] == 1 ||
            matrix[row + 2][col] == 2
            ){
            return 1;
        }
    }

    return 0;
}

void StartGame(int* cSock){
    int* socket = cSock;

    char turn_message[DEFAULT_BUFLEN_CLIENT/2];
    char clientAttackMessage1[DEFAULT_BUFLEN_CLIENT];
    char clientAttackMessage2[DEFAULT_BUFLEN_CLIENT];
    char clientDefenseMessage1[DEFAULT_BUFLEN_CLIENT];
    char clientDefenseMessage2[DEFAULT_BUFLEN_CLIENT];
    char coordinatesSet[DEFAULT_BUFLEN_CLIENT/2];

    memset(turn_message, '\0', DEFAULT_BUFLEN_CLIENT/2);
    memset(clientAttackMessage1, '\0', DEFAULT_BUFLEN_CLIENT);
    memset(clientAttackMessage2, '\0', DEFAULT_BUFLEN_CLIENT);
    memset(clientDefenseMessage1, '\0', DEFAULT_BUFLEN_CLIENT);
    memset(clientDefenseMessage2, '\0', DEFAULT_BUFLEN_CLIENT);
    memset(coordinatesSet, '\0', DEFAULT_BUFLEN_CLIENT/2);

    if(gameStarted == 0){
        strcpy(coordinatesSet, "1");

        if (send(*socket, coordinatesSet, strlen(coordinatesSet), 0) < 0) {
            printf("\033[0;31m");
            system("clear");
            puts("Error while starting game");
            printf("\033[0m");
            return;
        }

        system("clear");
        puts("Waiting for another player...");

        if( recv(*socket , turn_message , sizeof(turn_message) , 0) <= 0 )
        {
            printf("\033[0;31m");
            system("clear");
            puts("Error while receiving turn");
            printf("\033[0m");
            return;
        }
        turn = atoi(turn_message);
    }

    gameStarted = 1;

    while(1) {

        if(turn == 1){
            puts("It's your turn\n");
        }else{
            puts("Opponent is on turn\n");
        }

        if (turn == 1) {
            do {
                printf("--> ENTER COORDINATES YOU WISH TO ATTACK: ");
                scanf("%s", clientAttackMessage1);

                if (strcmp(clientAttackMessage1, "m") == 0 || strcmp(clientAttackMessage1, "M") == 0) {
                    return;
                }

            } while (InputCheck(clientAttackMessage1));

            if (send(*socket, clientAttackMessage1, strlen(clientAttackMessage1), 0) < 0) {
                return;
            }

            if (recv(*socket, clientDefenseMessage1, sizeof(clientDefenseMessage1), 0) > 0) {

                if(EndCheck(clientDefenseMessage1) == 0){
                    printf("\033[0;32m");
                    system("clear");
                    puts("Opponent disconnected");
                    puts("YOU WON");
                    puts("");
                    printf("\033[0m");
                    return;
                }


                if (strcmp(clientDefenseMessage1, "YOU WON") ==  0) {
                    printf("\033[0;32m");
                    system("clear");
                    puts(clientDefenseMessage1);
                    puts("");
                    printf("\033[0m");

                    Reset();
                    return;
                }

                int x = strcmp(clientDefenseMessage1, "HIT");

                memset(clientDefenseMessage1, '\0', DEFAULT_BUFLEN_CLIENT);

                if (x == 0) {
                    printf("\033[0;32m");
                    system("clear");
                    puts("HIT");
                    puts("");
                    printf("\033[0m");
                    turn = 1;
                } else {
                    printf("\033[0;31m");
                    system("clear");
                    puts("MISS");
                    puts("");
                    printf("\033[0m");
                    turn = 0;
                    continue;
                }
            }
        }



        if (turn == 0) {
            if (recv(*socket, clientAttackMessage2, sizeof(clientAttackMessage2), 0) > 0) {

                system("clear");

                if(EndCheck(clientAttackMessage2) == 0){
                    printf("\033[0;32m");
                    puts("Opponent disconnected");
                    puts("YOU WON");
                    printf("\033[0m");
                    return;
                }

                if (HitCheck(clientAttackMessage2) == 1) {
                    printf("\033[0;32m");
                    strcpy(clientDefenseMessage2, "HIT");
                    puts(clientDefenseMessage2);
                    printf("\033[0m");

                    turn = 0;
                } else {
                    printf("\033[0;31m");
                    strcpy(clientDefenseMessage2, "MISS");
                    puts(clientDefenseMessage2);
                    printf("\033[0m");
                    turn = 1;
                }

                PrintMatrix();
                if(MatrixEmpty() == 0){
                    printf("\033[0;31m");
                    puts("YOU LOST");
                    puts("");
                    printf("\033[0m");

                    strcpy(clientDefenseMessage2, "YOU WON");

                    if (send(*socket, clientDefenseMessage2, strlen(clientDefenseMessage2), 0) < 0) {
                        return;
                    }

                    Reset();
                    return;
                }

                if (send(*socket, clientDefenseMessage2, strlen(clientDefenseMessage2), 0) < 0) {
                    return;
                }

                memset(clientDefenseMessage2, '\0', DEFAULT_BUFLEN_CLIENT);

            }
        }
    }
}

int InputCheck(char* client_message){
    char buff[DEFAULT_BUFLEN_CLIENT];
    strcpy(buff, client_message);
    char row;
    char col;
    int  len;

    row  = buff[0];
    col = buff[1];
    len = strlen(buff);

    if( (len != 2) || (row < '1' || row > '5') || (col < 'A' || col > 'E') ) {
        return 1;
    }

    return 0;
}

int HitCheck(char* client_attack_message){
    int row = client_attack_message[0] - 49;
    int col = client_attack_message[1] - 65;

    if((matrix[row][col] == 1) || (matrix[row][col] == 2) || (matrix[row][col] == 3)){
        matrix[row][col] = 0;
        return 1;
    }
    return 0;
}

int MatrixEmpty(){
    int i, j;

    for (i = 0; i < 5; i++){
        for (j = 0; j < 5; j++){
            if(matrix[i][j] != 0){
                return 1;
            }
        }
    }

    return 0;
}

void Reset(){
    int i, j;

    gameStarted = 0;
    flag1 = 0;
    flag2 = 0;
    flag3 = 0;
    turn = 0;

    for (i = 0; i < 5; i++){
        for (j = 0; j < 5; j++){
            matrix[i][j] = 0;
        }
    }
}



void EndGame(int* cSock){
    int* socket = cSock;

    char tmp[DEFAULT_BUFLEN_CLIENT];
    memset(tmp, '\0', DEFAULT_BUFLEN_CLIENT);

    Reset();

    strcpy(tmp, "QQQ");

    if (send(*socket, tmp, strlen(tmp), 0) < 0) {
        return;
    }

    system("clear");
    puts("Client disconnected");

    return;
}

int EndCheck(char* string){
    if(strcmp(string, "WWW") == 0){
        return 0;
    }

    return 1;
}

void  printManual(){
    puts("*************************************************************************************************");
    puts("");
    puts("-------------------------------------------------------------------------------------------------");
    puts("* MANUAL                                                                                        *");
    puts("*                                                                                               *");
    puts("* For one dimensional matrix                                                                    *");
    puts("* Coordinates have to be set in format: NUM ID                                                  *");
    puts("* NUM represents row number and have to be in range [1, 3]                                      *");
    puts("* ID represents colomn number and have to be in range [A, C]                                    *");
    puts("*                                                                                               *");
    puts("* For two dimensional matrix                                                                    *");
    puts("* Coordinates have to be set in format: NUM ID DIR                                              *");
    puts("* NUM represents row number and have to be in range [1, 3]                                      *");
    puts("* ID represents colomn number and have to be in range [A, C]                                    *");
    puts("* DIR represents direction of submarine: 'H' - horizontal, 'V' - vertical                       *");
    puts("* NOTE: Just first coordinate is set manually, the another is set automatically:                *");
    puts("* If DIR == 'H', next coordinate is right to the first                                          *");
    puts("* If DIR == 'V', next coordinate is bellow the first                                            *");
    puts("* If first coordinate is set in 3rd column, submarine can't be written horizontally             *");
    puts("* Otherwise there will be out of bounds                                                         *");
    puts("* The same is for every out of bounds case                                                      *");
    puts("*                                                                                               *");
    puts("* For three dimensional matrix                                                                  *");
    puts("* The same principle as for two dimensional, except submarine's length is not 2 but 3           *");
    puts("*                                                                                               *");
    puts("* EXAMPLE-1: 2C - submarine will be set on section of 2nd row and 3rd column                    *");
    puts("* EXAMPLE-2: 2CH - out of bounds                                                                *");
    puts("*            2CV - submarine will be set on section of 3rd column and 2nd and 3rd row           *");
    puts("*                                                                                               *");
    puts("* FOR MENU ENTER 'm' or 'M' ANYTIME                                                             *");
    puts("-------------------------------------------------------------------------------------------------");
}
