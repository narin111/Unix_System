#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define oops(msg) { perror(msg); exit(1); }
#define BUF_LEN 128
#define PORTNUM 14234

void process_request(int);
void child_waiter(int signum);

int main(int ac, char *av[])
{
    char buffer[BUF_LEN];
    struct sockaddr_in server_addr, client_addr;
    char temp[20];
    int len, msg_size, server_fd, client_fd;
    int pid;
    signal(SIGCHLD, child_waiter);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        oops("socket");
    memset(&server_addr, 0x00, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNUM);

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0)
        oops("bind");

    if(listen(server_fd, 5) < 0)
        oops("listen");

    memset(buffer, 0, sizeof(buffer));
    printf("Server: waiting connection request.\n");
    len = sizeof(client_addr);

    while(1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
        if (client_fd == -1)
		 	break;

        pid = fork();
        switch(pid){
            case -1 :
                close(client_fd);
                oops("fork");

            case 0 :
                close(server_fd);
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
                printf("Server : %s client connected. \n", temp);

                while(1){
                    // 퀴즈 쇼 함수 삽입
                    process_request(client_fd);
                }
                close(client_fd);
                return 0;
            default :
                close(client_fd);
        }
    }
    close(server_fd);
    return 0;
}

void child_waiter(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void process_request(int fd)
{
    char id[128], buf[100], qst_buf[256], ans_buf[100];
    char check[100], ans_file_buf[256], cnt_buf[256];
    FILE *user_file, *qst_file, *ans_file;
    int crt_cnt = 0, wa_cnt = 0, qst_num[5], num, i;

    //클라이언트가 접속한 시간을 저장한다.
    time_t now;
    char *cp;

    time(&now);
    cp=ctime(&now);
    //sprintf(cp, "최근접속시간 : %s", cp);
    //printf("%s", cp);
    // 닉네임을 받는다.
    read(fd, id, sizeof(id));
    //printf("%s", id);
    // 닉네임에 대한 파일을 만든다.
    sprintf(buf, "nickname/%s.txt", id);
    user_file = fopen(buf, "a");

    read(fd, qst_num, sizeof(int));
    num = qst_num[0]*10 + 1;

    for(i = 0; i<10; i++){
        memset(ans_buf, 0, sizeof(ans_buf));
        memset(ans_file_buf, 0, sizeof(ans_file_buf));

        sprintf(qst_buf, "head -%d Question.txt | tail -1", num+i);
        qst_file = popen(qst_buf, "r");

        fgets(qst_buf, 256, qst_file);
        write(fd, qst_buf, strlen(qst_buf));

        // 답 수신
        read(fd, ans_buf, 30);
        char *tmpans;
        tmpans = ans_buf;

        //중간에 카테고리 바꾸기
        char *change0="change_0";
        if(strncmp(tmpans, change0, 8)==0)
        {
            num=0;
            i=0;
            continue;
        }
        char *change1="change_1";
        if(strncmp(tmpans, change1, 8)==0)
		{
            num=10;
            i=0;
            continue;
        }
        char *change2="change_2";
        if(strncmp(tmpans, change2, 8)==0)
        {
            num=20;
            i=0;
            continue;
        }

        //현재스코어 확인
        char *curr_score="score";
        if(strncmp(tmpans, curr_score, 5)==0)
        {
            sprintf(check, "현재 스코어: %d/%d\n", crt_cnt, crt_cnt+wa_cnt);
            write(fd, check, strlen(check));
            i--;
            continue;
        }

        //힌트
        char *hint_buf = "hint";
        char hint[100], h_buf[100];
        FILE *hint_file;
        if(strncmp(tmpans, hint_buf, strlen(hint_buf))==0)
        {
            sprintf(hint, "head -%d Hint.txt | tail -1", num+i);
            hint_file = popen(hint, "r");
            fgets(h_buf, 100, hint_file);
            write(fd, h_buf, sizeof(hint));
            i--;
            pclose(hint_file);
            continue;
        }

        // 종료
        char *quit="q";
        if(strncmp(tmpans, quit, 1)==0)
        {
            //printf("%s", cp);
            sprintf(cnt_buf, "맞은문제/푼 문제: %d/%d ----- 최근접속시간: %s", crt_cnt, crt_cnt+wa_cnt, cp);
            //write(1, cnt_buf, strlen(cnt_buf));
		 fputs(cnt_buf, user_file);
            sprintf(id, "%s 사용자가 게임을 종료했습니다.\n", id);
            write(1, id, strlen(id));
            memset(id, 0, sizeof(id));
         //   return;
              break;
        }

        // 답 평가, 카운트
        sprintf(ans_file_buf, "head -%d Answer.txt | tail -1", num+i);
        ans_file = popen(ans_file_buf, "r");
        fgets(ans_file_buf, BUFSIZ, ans_file);

        char *tmpfileans;
        tmpfileans = ans_file_buf;

        if(strncmp(tmpans, tmpfileans, strlen(tmpfileans)-1)==0){
            crt_cnt++;
            strcpy(check, "Right answer!\n");
            write(fd, check, strlen(check));
        }
        else{
            wa_cnt++;
            strcpy(check, "Wrong answer..\n");
            write(fd, check, strlen(check));
        }

        if(i==9)
        {
            sprintf(cnt_buf, "맞은문제/푼 문제: %d/%d ----- 최근접슥시간: %s", crt_cnt, crt_cnt+wa_cnt, cp);
            fputs(cnt_buf, user_file);
            sprintf(id, "%s 사용자가 게임을 종료했습니다.\n", id);
            write(1, id, strlen(id));
            memset(id, 0, sizeof(id));
            char fin_buf[2]="f";
            write(fd, fin_buf, strlen(fin_buf));

            //return;
            break;
        }
    }

    fclose(user_file);
    pclose(qst_file);
	pclose(ans_file);
}


