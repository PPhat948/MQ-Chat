//Puthipong Yomabut 6513134
//Phattaradanai Sornsawang 6513172
//Patiharn Kamenkit 6513170
//Praphasiri wannawong 6513116
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define MAX_MSG_SIZE 80
#define STUDENT_ID 6513134

struct msgbuf {
  long int mtype;
  char mtext[MAX_MSG_SIZE];
};

static void sig_end() { exit(EXIT_SUCCESS); }

int main(int argc, char *argv[]) {
  int msgid, nbytes;
  struct msgbuf buf;
  pid_t pid;

  if (argc < 2 || (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "2") != 0) || argc > 2) {
    fprintf(stderr, "Usage: %s [1 or 2]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  msgid = msgget((key_t)STUDENT_ID, 0666 | IPC_CREAT);
  if (msgid == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  signal(SIGUSR1, sig_end);
  argv++;

  if (strcmp(*argv, "1") == 0) {
    pid = fork();
    switch (pid) {
      case -1:
        perror("Forking failed");
        exit(EXIT_FAILURE);
      case 0:
        while (strncmp(buf.mtext, "end chat", 8) != 0) {
          if (msgrcv(msgid, (void *)&buf, MAX_MSG_SIZE, 1, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
          }
          printf("%s", buf.mtext);
        }
        break;
      default:
        while (strncmp(buf.mtext, "end chat", 8) != 0) {
          printf("");
          fgets(buf.mtext, MAX_MSG_SIZE, stdin);
          buf.mtype = 2;
          if (msgsnd(msgid, (void *)&buf, MAX_MSG_SIZE, 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
          }
        }
        break;
    }
  } else if (strcmp(*argv, "2") == 0) {
    pid = fork();
    switch (pid) {
      case -1:
        perror("Forking failed");
        exit(EXIT_FAILURE);
      case 0:
        while (strncmp(buf.mtext, "end chat", 8) != 0) {
          if (msgrcv(msgid, (void *)&buf, MAX_MSG_SIZE, 2, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
          }
          printf("%s", buf.mtext);
        }
        break;
      default:
        while (strncmp(buf.mtext, "end chat", 8) != 0) {
          printf("");
          fgets(buf.mtext, MAX_MSG_SIZE, stdin);
          buf.mtype = 1;
          if (msgsnd(msgid, (void *)&buf, MAX_MSG_SIZE, 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
          }
        }
        break;
    }
  }

  if (pid > 0)
    kill(pid, SIGUSR1);
  else if (pid == 0)
    kill(getppid(), SIGUSR1);

  return 0;
}
