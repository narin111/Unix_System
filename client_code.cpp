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

    //�г��� �޾Ƽ� �������� �۽�
    printf("����� ���α׷��� ���� �� ȯ���մϴ�.\n����� �г����� �Է����ּ���: ");
    char nickname[100];
    scanf("%s", nickname);
    write(sd, nickname, strlen(nickname));

    //���� ī�װ� ��ȣ �۽�
    int num[5], n;
    printf("q : ���� ����\nhint : ��Ʈ ���\nchange_n(0,1,2) : ī�װ� ����\nscore : ���� ���� ���(���� ����/�� ����)\n���� ī�װ��� ������ (0 : �ͼ���, 1 : ���� ����, 2 : ������ ���
 O/X)\n");
    scanf("%d", &num[0]);
    while(num[0]!=0 && num[0]!=1 && num[0]!=2){
        printf("ī�װ� ��ȣ�� �ٽ� �Է��ϼ���.\n");
        scanf("%d", &num[0]);
    }
    write(sd, num, sizeof(int));
	 int Q_n;
    int Ans_n;
    char ans[100], check[30];

    while(1){

        //���� ���� �� �� �۽�
        //���ڸ� �޾ƿ� Ŭ���̾�Ʈ �͹̳ο� ���
        Q_n=read(sd, rcv_Q, 256);
        char *fin_buf = "f";
        if(strncmp(rcv_Q, fin_buf, 1)==0){
            printf("ī�װ��� ��� ������ Ǯ�����ϴ�!\n���α׷��� ����˴ϴ�..\n");
            break;
        }
        write(1, rcv_Q, Q_n);


        //�� �۽�
        scanf(" %s", ans);

        //����
        if(strcmp(ans, "q")==0){
            write(sd, ans, strlen(ans));
            printf("���α׷��� �����մϴ�.\n");
            break;
        }

        //ī�װ� ����
        if(strcmp(ans, "change_0")==0){
            write(sd, ans, strlen(ans));
            printf("0�� ī�װ��� �����մϴ�.\n");
            continue;
        }
        if(strcmp(ans, "change_1")==0){
            write(sd, ans, strlen(ans));
            printf("1�� ī�װ��� �����մϴ�.\n");
            continue;
        }
        if(strcmp(ans, "change_2")==0){
            write(sd, ans, strlen(ans));
            printf("2�� ī�װ��� �����մϴ�.\n");
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

        //ä�� ��� Ȯ�� RA/WA
        Ans_n=read(sd, check, 256);
        write(1, check, Ans_n);

        memset(rcv_Q, 0, sizeof(rcv_Q));
        memset(ans, 0, sizeof(ans));
    }
    printf("Client : exit\n");
    close(sd);
    return 0;
}

