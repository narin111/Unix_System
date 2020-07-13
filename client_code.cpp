#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define LEN 256
#define oops(msg) { perror(msg); exit(1);}
#define PORTNUM 14234

int main(int ac, char *av[])
{
    int sd;
    char buf[256];
    char rcv_Q[256];
    struct sockaddr_in sin;

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        oops("socket");

    memset((char *)&sin, '\0', sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("210.117.181.113");

    if(connect(sd, (struct sockaddr *)&sin, sizeof(sin)))
        oops("connect");

    //닉네임 받아서 서버에게 송신
    printf("퀴즈쇼 프로그램에 오신 걸 환영합니다.\n사용할 닉네임을 입력해주세요: ");
    char nickname[100];
    scanf("%s", nickname);
    write(sd, nickname, strlen(nickname));

    //문제 카테고리 번호 송신
    int num[5], n;
    printf("q : 게임 종료\nhint : 힌트 출력\nchange_n(0,1,2) : 카테고리 변경\nscore : 현재 점수 출력(맞은 문제/총 문제)\n문제 카테고리를 고르세요 (0 : 넌센스, 1 : 수도 퀴즈, 2 : 데이터 통신
 O/X)\n");
    scanf("%d", &num[0]);
    while(num[0]!=0 && num[0]!=1 && num[0]!=2){
        printf("카테고리 번호를 다시 입력하세요.\n");
        scanf("%d", &num[0]);
    }
    write(sd, num, sizeof(int));
	 int Q_n;
    int Ans_n;
    char ans[100], check[30];

    while(1){

        //문제 수신 및 답 송신
        //문자를 받아와 클라이언트 터미널에 출력
        Q_n=read(sd, rcv_Q, 256);
        char *fin_buf = "f";
        if(strncmp(rcv_Q, fin_buf, 1)==0){
            printf("카테고리의 모든 문제를 풀었습니다!\n프로그램이 종료됩니다..\n");
            break;
        }
        write(1, rcv_Q, Q_n);


        //답 송신
        scanf(" %s", ans);

        //종료
        if(strcmp(ans, "q")==0){
            write(sd, ans, strlen(ans));
            printf("프로그램을 종료합니다.\n");
            break;
        }

        //카테고리 변경
        if(strcmp(ans, "change_0")==0){
            write(sd, ans, strlen(ans));
            printf("0번 카테고리로 변경합니다.\n");
            continue;
        }
        if(strcmp(ans, "change_1")==0){
            write(sd, ans, strlen(ans));
            printf("1번 카테고리로 변경합니다.\n");
            continue;
        }
        if(strcmp(ans, "change_2")==0){
            write(sd, ans, strlen(ans));
            printf("2번 카테고리로 변경합니다.\n");
            continue;
        }

        //hint
        if(strcmp(ans, "hint")==0){
            char hint[100];
		 write(sd, ans, strlen(ans));
            read(sd, hint, 100);
            write(1, hint, strlen(hint));
            continue;
        }

        write(sd, ans, strlen(ans));

        //채점 결과 확인 RA/WA
        Ans_n=read(sd, check, 256);
        write(1, check, Ans_n);

        memset(rcv_Q, 0, sizeof(rcv_Q));
        memset(ans, 0, sizeof(ans));
    }
    printf("Client : exit\n");
    close(sd);
    return 0;
}

