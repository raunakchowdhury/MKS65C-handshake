#include "pipe_networking.h"

// handle the SIGINT singla by deleting the well_known_pipe(WKP)
static void sighandler(int signo){
  if (signo == SIGINT){
    printf("Closing well-known-pipe...\n");
    if(unlink("well_known_pipe") == -1){
      printf("Deletion failure. Error: %s\n", strerror(errno));
    }
    else{
      printf("Pipe deleted.\n" );
    }
    exit(0);
  }
}

int main() {
  int to_client;
  int from_client;
  int pipe_end;

  int size = BUFFER_SIZE;
  char msg[size];
  signal(SIGINT, sighandler);

  while(1){
    from_client = server_handshake( &to_client );
    if (!fork()){

      // keep Reading until client closes
      while(read(from_client, msg, size)){
        //printf("msg: \'%s\'\n", msg);

        strcat(msg, " - Mr K");
        write(to_client, msg, size);
        printf("Response sent!\n");
      }
      printf("Pipe closed. Server available.\n\n" );
      exit(0);
    }
}
}
