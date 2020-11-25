
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include<stdlib.h>


#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct accountinfo
{
    char name[20] ;
    char passward[20];
    int online;
    int position;
    int ifwatch; 
    int score_win;
    int score_lose;
    int score_drawn;
    char checkerboard[20] ;
    int oppo_pos;
    char watch_person[20];


};
int result(char checkboard[])
{
        if((checkboard[1] == 'O' && checkboard[2] == 'O' && checkboard[3] == 'O') || (checkboard[4] == 'O' && checkboard[5] == 'O' && checkboard[6] == 'O') ||
                        (checkboard[7] == 'O' && checkboard[8] == 'O' && checkboard[9] == 'O') || (checkboard[1] == 'O' && checkboard[4] == 'O' && checkboard[7] == 'O') ||
                        (checkboard[2] == 'O' && checkboard[5] == 'O' && checkboard[8] == 'O') || (checkboard[3] == 'O' && checkboard[6] == 'O' && checkboard[9] == 'O') ||
                        (checkboard[1] == 'O' && checkboard[5] == 'O' && checkboard[9] == 'O') || (checkboard[3] == 'O' && checkboard[5] == 'O' && checkboard[7] == 'O'))
                        {
                            return 1;
                        }
                         else if((checkboard[1] == 'X' && checkboard[2] == 'X' && checkboard[3] == 'X') || (checkboard[4] == 'X' && checkboard[5] == 'X' && checkboard[6] == 'X') ||
                                (checkboard[7] == 'X' && checkboard[8] == 'X' && checkboard[9] == 'X') || (checkboard[1] == 'X' && checkboard[4] == 'X' && checkboard[7] == 'X') ||
                                (checkboard[2] == 'X' && checkboard[5] == 'X' && checkboard[8] == 'X') || (checkboard[3] == 'X' && checkboard[6] == 'X' && checkboard[9] == 'X') ||
                                (checkboard[1] == 'X' && checkboard[5] == 'X' && checkboard[9] == 'X') || (checkboard[3] == 'X' && checkboard[5] == 'X' && checkboard[7] == 'X'))
                                {
                                    return 1;

                                }
                    else if(checkboard[1] != ' ' && checkboard[2] != ' ' && checkboard[3] != ' ' &&
                                checkboard[4] != ' ' && checkboard[5] != ' ' && checkboard[6] != ' ' &&
                                checkboard[7] != ' ' && checkboard[8] != ' ' && checkboard[9] != ' ' )
                                {
                                    return 2;
                                }
                    else
                    {
                        return 0;
                    }
}


int main()
{

    FILE *accountfile;

    struct accountinfo name[20];

    int num =0;

    accountfile = fopen("account.txt", "a+");

    while(fscanf(accountfile, "%s", name[num].name) !=EOF)
    {
        fscanf(accountfile, "%s", name[num].passward);

        memset(name[num].checkerboard, ' ', sizeof(name[num].checkerboard));
        name[num].checkerboard[10] = '\n';
        name[num].checkerboard[11] = '\0';
        name[num].checkerboard[0] = '2';

        name[num].online = 0;

        name[num].position = 0;

        name[num].ifwatch = 0;

        name[num].score_win = 0; 

        name[num].score_lose = 0; 

        name[num].score_drawn = 0; 

        name[num].oppo_pos=0;

        memset(name[num].watch_person, 0, sizeof(name[num].watch_person));
        num++;
    }
    

    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);


    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);


    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");



    char system[500] ;
    memset(system, 0, sizeof(system));
    strcpy(system, "your turn\n");

    char whoplayer1[100];
    memset(whoplayer1, 0, sizeof(whoplayer1));
    char whoplayer2[100];
    memset(whoplayer2, 0, sizeof(whoplayer2));
    char which[100];
    memset(which, 0, sizeof(which));

    int judge =0;
                               
    char tmp[20];
    memset(tmp, 0, sizeof(tmp));

    int player_1_pos = 0, player_2_pos = 0;

    while(1)
    {
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        for(i = 1; i <= max_socket; ++i)
        {
            if (FD_ISSET(i, &reads))
            {

                if (i == socket_listen)
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen,(struct sockaddr*) &client_address,&client_len);
                    
                    if (!ISVALIDSOCKET(socket_client))
                    {
                        fprintf(stderr, "accept() failed. (%d)\n",GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                    {
                        max_socket = socket_client;
                        //printf("%d\n", socket_client);
                    }
                    
                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0,NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);


                }
                 else
                {
                    char read[1024];
                    memset(read, 0, sizeof(read));
                    int bytes_received = recv(i, read, 1024, 0);
                    if (bytes_received < 1)
                    {
                        FD_CLR(i, &master);
                        close(i);
                        continue;
                    }

                    if(strcmp(read, "register\n") == 0)                                                                                                 //register
                    {
                        accountfile = fopen("account.txt", "a+");

                        char register_account[500] ;
                        memset(register_account, 0, sizeof(register_account));
                        strcpy(register_account, "enter your account:\n");
                        send(i, register_account, strlen(register_account), 0);
                        char account[500];
                        memset(account, 0, sizeof(account));
                        recv(i, account, 500, 0);
                        //account[strlen(account)-1] = '\0';

                        fwrite(account, 1, strlen(account), accountfile);

                        //fscanf(accountfile, "%s", name[num].name);
                        if(account[strlen(account)-1]=='\n')account[strlen(account)-1]='\0';
                        strcpy(name[num].name, account);

                        char register_password[500];
                        memset(register_password, 0, sizeof(register_password));
                        strcpy(register_password, "enter your passward:\n");
                        send(i, register_password, strlen(register_password), 0);
                        char passward[500];
                        memset(passward, 0, sizeof(passward));
                        recv(i, passward, 500, 0);
                        //passward[strlen(passward)-1] = '\0';

                        fwrite(passward, 1, strlen(passward), accountfile);
                        if(passward[strlen(passward)-1]=='\n')passward[strlen(passward)-1]='\0';
                        
                        strcpy(name[num].passward, passward);

                        memset(name[num].checkerboard, ' ', sizeof(name[num].checkerboard));
                        name[num].checkerboard[10] = '\n';
                        name[num].checkerboard[11] = '\0';
                        name[num].checkerboard[0] = '2';

                        name[num].online = 0;

                        name[num].position = 0;


                        name[num].ifwatch = 0;


                        name[num].score_win = 0; 

                        name[num].score_lose = 0; 

                        name[num].score_drawn = 0; 

                        name[num].oppo_pos=0;

                        memset(name[num].watch_person, 0, sizeof(name[num].watch_person));

                        num++;

                        fclose(accountfile);

                    }

                    if(strcmp(read, "login\n") == 0)                                                                                                 //login
                    {
                        int ture = 1;
                        while(ture)
                        {
                            char input[500] ;
                            memset(input, 0, sizeof(input));
                            strcpy(input, "enter your account:\n");
                            //memset(buffer, 0, sizeof(buffer));
                            //printf("%s\n", input);
                            send(i, input, strlen(input), 0);
                            char account[500];
                            memset(account, 0, sizeof(account));
                            recv(i, account, 500, 0);
                            account[strlen(account)-1] = '\0';

                            //strcpy(name[num].passward, account);
                           
                            //account[strlen(account)+1] = '\0';
                            //printf("%s\n", account);
                            char input2[500];
                            memset(input2, 0, sizeof(input2));
                            strcpy(input2, "enter your passward:\n");
                            //printf("%s\n", input);
                            send(i, input2, strlen(input2), 0);
                            char passward[500];
                            memset(passward, 0, sizeof(passward));
                            recv(i, passward, 500, 0);
                            passward[strlen(passward)-1] = '\0';

                            //passward[strlen(passward)+1] = '\0';
                            //printf("%s%s", account, passward );

                            for(int j = 0; j<num; j++)
                            {
                                if(strcmp(account, name[j].name) == 0 && strcmp(passward, name[j].passward) == 0)
                                {
                                    char online[500] ;
                                    memset(online, 0, sizeof(online));
                                    strcpy(online, account);
                                    strcat(online, " is online\n");
                                    name[j].online = 1;

                                    send(i, online, strlen(online), 0);
                                    
                                    name[j].position = i;

                                    ture = 0;
                                    break;
                                }
                            }

                            if(ture == 1)
                            {
                                char error[500] ;
                                memset(error, 0, sizeof(error));
                                strcpy(error, "please try again\n");
                                send(i, error, strlen(error), 0);
                            }

                        }
                    }

                    if(strcmp(read, "list\n") == 0)                                                                                                 //list players
                    {
                        for(int n =0; n<num; n++)
                        {
                            
                            if(name[n].online == 1)
                            {
                                //printf("%s\n", name[n].name);
                                char online[500] ;
                                memset(online, 0, sizeof(online));
                                strcpy(online, name[n].name);
                                strcat(online, "\n");
                                send(i, online, strlen(online), 0);
                                //printf("55555555\n");
                            }


                        }
                    }

                    if(strcmp(read, "leave\n") == 0)                                                                                                 //leave
                    {                  
                        //printf("%s\n", name[n].name);
                        char offline[500] ;
                        memset(offline, 0, sizeof(offline));
                        strcpy(offline, "Are you sure?(yes/no)\n");
                        send(i, offline, strlen(offline), 0);
                        //printf("55555555\n");

                        char answer[500] ;
                        memset(answer, 0, sizeof(answer));
                        recv(i, answer, 500, 0);
                        //answer[strlen(answer)-1] = '\0';

                        if(strcmp(answer, "yes\n") == 0)
                        {
                            char status[500] ;
                            memset(status, 0, sizeof(status));
                            strcpy(status, "you are offline\n");
                            for(int j =0; j<num; j++)
                            {
                                //char speaker[500] ;
                                if(i == name[j].position )
                                {
                                    name[j].online = 0;
                                }

                            }        
                            
                            send(i, status, strlen(status), 0);
                               

                        }

                    }

                    if(strncmp(read, "chat", 4) == 0)                                                                                                 //chat
                    {                  
                        //printf("%s\n", name[n].name);
                        char *chat = strstr(read, " ");
                        chat = chat +1;
                        char speaker[500] ;
                        for(int j =0; j<num; j++)
                        {
                            //char speaker[500] ;
                            if(i == name[j].position )
                            {
                                
                                memset(speaker, 0, sizeof(speaker));
                                //player_1_pos = name[j].position;                                                                                 //player_1_pos O
                                strcpy(speaker, name[j].name);
                                //printf("%s\n", speaker);
                            }

                        }
                        for (int j =0; j<num;j++)
                        {
                            if(strncmp(chat, name[j].name, 3) == 0)
                            {
                                chat = chat +4;
                                char text[500] ;
                                memset(text, 0, sizeof(text));
                                //printf("%s\n", speaker);
                                //strcpy(text, speaker);
                                //strcat(text,":");
                                //strcat(text,chat);
                                sprintf(text, "%s:%s",speaker,chat);
                                send(name[j].position, text, strlen(text), 0);

                            }
                        }

                    }

                    

                    if(strcmp(read, "challenge\n") == 0)                                                                                                 //challenge
                    {
                        char who[500] ;
                        memset(who, 0, sizeof(who));
                        strcpy(who, "who do you want to fight?\n");
                        //strcat(online, "\n");
                        send(i, who, strlen(who), 0);

                        char opponent[500] ;
                        memset(opponent, 0, sizeof(opponent));
                        recv(i, opponent, 500, 0);
                        opponent[strlen(opponent)-1] = '\0';

                        char someone[500] ;
                        memset(someone, 0, sizeof(someone));
                        
                        for(int j =0; j<num; j++)
                        {
                            if(strcmp(opponent, name[j].name) == 0 )
                            {
                                player_2_pos = name[j].position; 
                                name[j].oppo_pos = i;                                                                                        //player__pos X
                            }

                        }

                        for(int j =0; j<num; j++)
                        {
                            if(i == name[j].position )
                            {
                                player_1_pos = name[j].position;                                                                                          //player_1_pos O
                                strcpy(someone, name[j].name);
                                name[j].oppo_pos = player_2_pos;
                            }
                        }


                        char invite[500] ;
                        memset(invite, 0, sizeof(invite));
                        strcpy(invite, someone);
                        strcat(invite, " want to challenge you, (yes/no)?\n");
                        //strcat(online, "\n");
                        //printf("%d\n",name[j].position );
                        send(player_2_pos, invite, strlen(invite), 0);                                                              //opponent  = name[j].position,

                        char response[500] ;
                        memset(response, 0, sizeof(response));
                        recv(player_2_pos, response, 500, 0);
                        response[strlen(response)-1] = '\0';

                        if(strcmp(response, "yes") == 0 )                                                                                                           //p.k.
                        {
                            //char response[500] ;
                            memset(response, 0, sizeof(response));
                            strcpy(response, "let's fight\n");
                            //strcat(online, "\n");
                            send(player_1_pos, response, strlen(response), 0);
                            send(player_1_pos, system, strlen(system), 0);
                        }
                        else
                        {
                            memset(response, 0, sizeof(response));
                            strcpy(response, "he don't want\n");
                            //strcat(online, "\n");
                            send(player_1_pos, response, strlen(response), 0);
                        }
 
                    }


                    if(read[0] == 'O')                                                                                                 //O
                    {
                        
                        for(int j=0;j<num; j++)                                                             //O
                        {
                            if(i == name[j].position )
                            {
                                int pos = 0;

                                pos = read[1] - '0';
                                name[j].checkerboard[pos] = 'O' ;
                                

                                char board[100];
                                memset(board, 0, sizeof(board));

                                sprintf(board, "%c%c%c\n%c%c%c\n%c%c%c\n", 
                                name[j].checkerboard[1],name[j].checkerboard[2],name[j].checkerboard[3],
                                name[j].checkerboard[4],name[j].checkerboard[5],name[j].checkerboard[6],
                                name[j].checkerboard[7],name[j].checkerboard[8],name[j].checkerboard[9]);

                                
                                send(i, board,strlen(board), 0);
                                //send(name[j].oppo_pos, system, strlen(system), 0);
                                for(int k=0;k<num; k++) 
                                {
                                    if(name[j].oppo_pos == name[k].position )
                                    {
                                        int pos2= 0;

                                        pos2 = read[1] - '0';
                                        name[k].checkerboard[pos2] = 'O' ;
                                        

                                        char board[100];
                                        memset(board, 0, sizeof(board));

                                        sprintf(board, "%c%c%c\n%c%c%c\n%c%c%c\n", 
                                        name[k].checkerboard[1],name[k].checkerboard[2],name[k].checkerboard[3],
                                        name[k].checkerboard[4],name[k].checkerboard[5],name[k].checkerboard[6],
                                        name[k].checkerboard[7],name[k].checkerboard[8],name[k].checkerboard[9]);

                                        
                                        send(name[k].position, board,strlen(board), 0);
                                        send(name[k].position, system, strlen(system), 0);
                                        //send(name[j].oppo_pos, system, strlen(system), 0);
                                    }
                                }

                                //int judge =0;
                                judge = result(name[j].checkerboard);
                                //char tmp[20];
                                //memset(tmp, 0, sizeof(tmp));

                                if(judge == 1)
                                {
                                    char win[500];
                                    memset(win, 0 , sizeof(win));
                                    strcpy(win, "you win!\n");
                                    send(name[j].position, win, strlen(win), 0);

                                    
                                    name[j].score_win++;
                          

                                    char lose[500];
                                    memset(lose, 0 , sizeof(lose));
                                    strcpy(lose, "you lose!\n");
                                    send(name[j].oppo_pos, lose, strlen(lose), 0);

                                    for(int k =0; k<num; k++)
                                    {
                                        if(name[k].position == name[j].oppo_pos  )
                                        {
                                            name[k].score_lose++;
                                            memset(name[k].checkerboard, ' ', sizeof(name[k].checkerboard));
                                        }

                                    }
                                    strcpy(tmp, name[j].checkerboard);
                                    memset(name[j].checkerboard, ' ', sizeof(name[j].checkerboard));

                                    for(int k =0; k<num; k++)                                                                             //watch
                                    {

                                        if(name[k].ifwatch == 1 && (strcmp(name[k].watch_person, whoplayer1) == 0 ))
                                        {
                                            char announce[100];
                                            memset(announce, 0, sizeof(announce));
                                            sprintf(announce, "%s win \n", whoplayer1);
                                            send(name[k].position, announce,strlen(announce), 0);
                                            //send(name[j].position, board,strlen(board), 0);
                                        }

                                    }
                                }
                                else if(judge == 2)
                                {
                                    char drawn_game [500];
                                    memset(drawn_game, 0 , sizeof(drawn_game));
                                    strcpy(drawn_game, "drawn game!\n");
                                    send(name[j].position, drawn_game, strlen(drawn_game), 0);
                                    send(name[j].oppo_pos, drawn_game, strlen(drawn_game), 0);

                                    name[j].score_drawn++;
                                    strcpy(tmp, name[j].checkerboard);
                                    memset(name[j].checkerboard, ' ', sizeof(name[j].checkerboard));
                                    
                                    for(int k =0; k<num; k++)
                                    {
                                        if(name[k].position == name[j].oppo_pos  )
                                        {
                                            name[k].score_drawn++;
                                            memset(name[k].checkerboard, ' ', sizeof(name[k].checkerboard));
                                        }

                                    }

                                    

                                    for(int k =0; k<num; k++)
                                    {

                                        if(name[k].ifwatch == 1 &&  (strcmp(name[k].watch_person, whoplayer1) == 0 ))
                                        {
                                            char announce[100];
                                            memset(announce, 0, sizeof(announce));
                                            sprintf(announce, "This is drawn game\n");
                                            send(name[k].position, announce,strlen(announce), 0);
                                            //send(name[j].position, board,strlen(board), 0);
                                        }

                                    }
                                }



                            }

                        }


                        for(int j =0; j<num; j++)
                        {
                            if(name[j].position == i  )
                            {
                                strcpy(whoplayer1, name[j].name);
                            }
                        }

                        for(int j =0; j<num; j++)
                        {
                            if(name[j].position == i  )
                            {
                                for(int k =0; k<num; k++)
                                {
                                    if(name[j].oppo_pos == name[k].position  )
                                    {
                                        strcpy(whoplayer2, name[k].name);
                                    }
                                }
                            }
                        }

                        for(int j =0; j<num; j++)
                        {
                            if((name[j].ifwatch == 1 )&& (strcmp(name[j].watch_person, whoplayer1) == 0 ))
                            {

                                //char which[100];
                                memset(which, 0, sizeof(which));
                                sprintf(which, "%s is O , %s is X\n", whoplayer1,whoplayer2);
                                send(name[j].position, which,strlen(which), 0);

                                for(int k =0; k<num; k++)
                                {
                                    if(strcmp(name[k].name, whoplayer1) == 0 ) 
                                    {
                                        //printf("%s\n", name[k].name);
                                        char board[100];
                                        memset(board, 0, sizeof(board));

                                        sprintf(board, "%c%c%c\n%c%c%c\n%c%c%c\n", 
                                        name[k].checkerboard[1],name[k].checkerboard[2],name[k].checkerboard[3],
                                        name[k].checkerboard[4],name[k].checkerboard[5],name[k].checkerboard[6],
                                        name[k].checkerboard[7],name[k].checkerboard[8],name[k].checkerboard[9]);
                                        
                                        if(judge == 1 || judge ==2)
                                        {
                                            memset(board, 0, sizeof(board));

                                            sprintf(board, "%c%c%c\n%c%c%c\n%c%c%c\n", 
                                            tmp[1],tmp[2],tmp[3],
                                            tmp[4],tmp[5],tmp[6],
                                            tmp[7],tmp[8],tmp[9]);

                                            send(name[j].position, board,strlen(board), 0);
                                        }
                                        else
                                        {
                                            send(name[j].position, board,strlen(board), 0);
                                        }
                                        
                                        
                                    }
                                }
                                
                            }

                        }

                    }

                    if(read[0] == 'X')     
                    {

                        for(int j=0;j<num; j++)                                                             //X
                        {
                            if(i == name[j].position )
                            {
                                int pos = 0;

                                pos = read[1] - '0';
                                name[j].checkerboard[pos] = 'X' ;
                                

                                char board[100];
                                memset(board, 0, sizeof(board));

                                sprintf(board, "%c%c%c\n%c%c%c\n%c%c%c\n", 
                                name[j].checkerboard[1],name[j].checkerboard[2],name[j].checkerboard[3],
                                name[j].checkerboard[4],name[j].checkerboard[5],name[j].checkerboard[6],
                                name[j].checkerboard[7],name[j].checkerboard[8],name[j].checkerboard[9]);

                                
                                send(i, board,strlen(board), 0);
                                //send(name[j].oppo_pos, system, strlen(system), 0);
                                for(int k=0;k<num; k++) 
                                {
                                    if(name[j].oppo_pos == name[k].position )
                                    {
                                        int pos2 = 0;

                                        pos2 = read[1] - '0';
                                        name[k].checkerboard[pos2] = 'X' ;
                                        

                                        char board[100];
                                        memset(board, 0, sizeof(board));

                                        sprintf(board, "%c%c%c\n%c%c%c\n%c%c%c\n", 
                                        name[k].checkerboard[1],name[k].checkerboard[2],name[k].checkerboard[3],
                                        name[k].checkerboard[4],name[k].checkerboard[5],name[k].checkerboard[6],
                                        name[k].checkerboard[7],name[k].checkerboard[8],name[k].checkerboard[9]);

                                        
                                        send(name[k].position, board,strlen(board), 0);
                                        send(name[k].position, system, strlen(system), 0);
                                        //send(name[j].oppo_pos, system, strlen(system), 0);
                                    }
                                }

                                //int judge =0;
                                judge = result(name[j].checkerboard);

                                if(judge == 1)
                                {
                                    char win[500];
                                    memset(win, 0 , sizeof(win));
                                    strcpy(win, "you win!\n");
                                    send(name[j].position, win, strlen(win), 0);

                                    
                                    name[j].score_win++;
                                    strcpy(tmp, name[j].checkerboard);
                                    memset(name[j].checkerboard, ' ', sizeof(name[j].checkerboard));
                          

                                    char lose[500];
                                    memset(lose, 0 , sizeof(lose));
                                    strcpy(lose, "you lose!\n");
                                    send(name[j].oppo_pos, lose, strlen(lose), 0);

                                    for(int k =0; k<num; k++)
                                    {
                                        if(name[k].position == name[j].oppo_pos  )
                                        {
                                            name[k].score_lose++;
                                            memset(name[k].checkerboard, ' ', sizeof(name[k].checkerboard));
                                        }

                                    }

                                    //memset(name[j].checkerboard, ' ', sizeof(name[j].checkerboard));

                                    /*for(int k =0; k<num; k++)                                                                             //watch
                                    {

                                        if(name[k].ifwatch == 1 &&  (strcmp(name[k].watch_person, whoplayer1) == 0 ))
                                        {
                                            char announce[100];
                                            memset(announce, 0, sizeof(announce));
                                            sprintf(announce, "%s win\n", whoplayer1);
                                            send(name[k].position, announce,strlen(announce), 0);
                                            //send(name[j].position, board,strlen(board), 0);
                                        }

                                    }*/
                                }
                                else if(judge == 2)
                                {
                                    char drawn_game [500];
                                    memset(drawn_game, 0 , sizeof(drawn_game));
                                    strcpy(drawn_game, "drawn game!\n");
                                    send(name[j].position, drawn_game, strlen(drawn_game), 0);
                                    send(name[j].oppo_pos, drawn_game, strlen(drawn_game), 0);

                                    name[j].score_drawn++;
                                     strcpy(tmp, name[j].checkerboard);
                                    memset(name[j].checkerboard, ' ', sizeof(name[j].checkerboard));
                                    
                                    for(int k =0; k<num; k++)
                                    {
                                        if(name[k].position == name[j].oppo_pos  )
                                        {
                                            name[k].score_drawn++;
                                            memset(name[k].checkerboard, ' ', sizeof(name[k].checkerboard));
                                        }

                                    }

                                    

                                    /*for(int k =0; k<num; k++)
                                    {

                                        if(name[k].ifwatch == 1 && (strcmp(name[k].watch_person, whoplayer1) == 0 ))
                                        {
                                            char announce[100];
                                            memset(announce, 0, sizeof(announce));
                                            sprintf(announce, "This is drawn game\n");
                                            send(name[k].position, announce,strlen(announce), 0);
                                            //send(name[j].position, board,strlen(board), 0);
                                        }

                                    }*/
                                }
                            }

                        }


                        for(int j =0; j<num; j++)
                        {
                            if(name[j].position == i  )
                            {
                                strcpy(whoplayer1, name[j].name);    //X
                            }
                        }

                        for(int j =0; j<num; j++)
                        {
                            if(name[j].position == i  )
                            {
                                for(int k =0; k<num; k++)
                                {
                                    if(name[j].oppo_pos == name[k].position  )
                                    {
                                        strcpy(whoplayer2, name[k].name);              //O
                                    }
                                }
                            }
                        }

                        
                        for(int j =0; j<num; j++)
                        {
                            if((name[j].ifwatch == 1 )&& (strcmp(name[j].watch_person, whoplayer2) == 0 ))
                            {

                                //char which[100];
                                memset(which, 0, sizeof(which));
                                sprintf(which, "%s is O , %s is X\n", whoplayer2,whoplayer1);
                                send(name[j].position, which,strlen(which), 0);

                                for(int k =0; k<num; k++)
                                {
                                    if(strcmp(name[k].name, whoplayer1) == 0 ) 
                                    {
                                        char board[100];
                                        memset(board, 0, sizeof(board));

                                        sprintf(board, "%c%c%c\n%c%c%c\n%c%c%c\n", 
                                        name[k].checkerboard[1],name[k].checkerboard[2],name[k].checkerboard[3],
                                        name[k].checkerboard[4],name[k].checkerboard[5],name[k].checkerboard[6],
                                        name[k].checkerboard[7],name[k].checkerboard[8],name[k].checkerboard[9]);

                                        if(judge == 1 || judge ==2)
                                        {
                                            memset(board, 0, sizeof(board));

                                            sprintf(board, "%c%c%c\n%c%c%c\n%c%c%c\n", 
                                            tmp[1],tmp[2],tmp[3],
                                            tmp[4],tmp[5],tmp[6],
                                            tmp[7],tmp[8],tmp[9]);

                                            send(name[j].position, board,strlen(board), 0);
                                        }
                                        else
                                        {
                                            send(name[j].position, board,strlen(board), 0);
                                        }
                                        //send(name[j].position, board,strlen(board), 0);
                                    }
                                }


                                
                            }

                        }

                    } 

                    if(strcmp(read, "watch\n") == 0)                                                                                                 //watch
                    {
                        for(int j =0; j<num; j++)
                        {
                            if(i == name[j].position )
                            {
                                name[j].ifwatch = 1;

                                char ask[500];
                                memset(ask, 0, sizeof(ask));
                                strcpy(ask, "watch who?\n");
                                send(i, ask, strlen(ask), 0);
                                char want_watch[500];
                                memset(want_watch, 0, sizeof(want_watch));
                                recv(name[j].position, want_watch, 500, 0);
                                want_watch[strlen(want_watch)-1] = '\0';

                                strcpy(name[j].watch_person, want_watch);
                                //printf("%s\n", want_watch);

                            }

                        }

                    }

                    if(strcmp(read, "record\n") == 0)                                                                                                 //record
                    {
                        for(int j =0; j<num; j++)
                        {
                            char record[500];
                            memset(record, 0, sizeof(record));
                            sprintf(record, "%s -> win:%d, lose:%d, drawn game:%d\n", name[j].name, name[j].score_win, name[j].score_lose, name[j].score_drawn);
                            send(i, record, strlen(record), 0);
                        }

                    }

                    if(strcmp(read, "help\n") == 0)                                                                                                 //help
                    {
                        char help[500];
                        memset(help, 0, sizeof(help));
                        sprintf(help, "1.list\n2.record\n3.chat\n4.challenge\n5.watch\n6.leave\n");
                        send(i, help, strlen(help), 0);
                        
                    }


                    



                }//else

            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)



    printf("Closing listening socket...\n");
    close(socket_listen);

#if defined(_WIN32)
    WSACleanup();
#endif


    printf("Finished.\n");

    return 0;
}
