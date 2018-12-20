#include "pipe_networking.h"

int main() {

  int to_server;
  int from_server;
  int size = BUFFER_SIZE;
  char input[size];
  from_server = client_handshake( &to_server );
  while(1){
    printf("Type something in: ");
    fgets(input, size, stdin);
    input[strlen(input)-1] = 0;
    //printf("Input: \'%s\'\n", input);

    write(to_server, input, size);
    read(from_server, input, size);
    printf("Response from Server: %s\n", input);
  }
}
