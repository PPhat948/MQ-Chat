#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_MSG_SIZE 256
#define STUDENT_ID 6513134

struct msgbuf {
  long int mtype;
  char mtext[MAX_MSG_SIZE];
};

int main(int argc, char *argv[]) {
  if (argc != 2 || (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "2") != 0)) {
    fprintf(stderr, "Usage: %s [1 or 2]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int msgID = msgget((key_t)STUDENT_ID, 0666 | IPC_CREAT);
  if (msgID == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  bool queue_removed = false;

  while (1) {
    struct msgbuf buf;
    if (strcmp(argv[1], "1") == 0) {
      char input[MAX_MSG_SIZE];
      printf("Enter message: ");
      fgets(input, MAX_MSG_SIZE, stdin);
      buf.mtype = 1;
      if (strncmp(input, "end chat", 8) == 0) {
        strncpy(buf.mtext, "end chat", MAX_MSG_SIZE);
        if (msgsnd(msgID, &buf, MAX_MSG_SIZE, 0) == -1) {
          perror("msgsnd");
          exit(EXIT_FAILURE);
        }
        break;
      }
      strncpy(buf.mtext, input, MAX_MSG_SIZE);
      if (msgsnd(msgID, &buf, MAX_MSG_SIZE, 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
      }
    } else {
      if (msgrcv(msgID, &buf, MAX_MSG_SIZE, 1, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
      }
      printf("Received message: %s", buf.mtext);
      if (strncmp(buf.mtext, "end chat", 8) == 0) {
        break;
      }
    }
  }

  if (!queue_removed && strcmp(argv[1], "1") == 0) {
    if (msgctl(msgID, IPC_RMID, 0) == -1) {
      perror("msgctl failed\n");
      exit(EXIT_FAILURE);
    }
    queue_removed = true;
  }

  return 0;
}
