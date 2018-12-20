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
  from_client = server_handshake( &to_client );
  signal(SIGINT, sighandler);
  while(1){
    server_process();
  }

}
