#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  remove("well_known_pipe");
  if (!mkfifo("well_known_pipe", 0644)){
    printf("Pipe initialized!\n");
  }

  char *message = "Message received!";
  char *pipe_name = calloc(256, sizeof(char));

  int fd;
  fd = open("well_known_pipe", O_RDONLY);
  if (fd == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
    exit(0);
  }

  if (!read(fd, pipe_name, 256)){
    printf("Server read error: %s\n", strerror(errno));
  }

  printf("Setting up other pipe %s...\n", pipe_name);
  *to_client = open(pipe_name, O_WRONLY);
  if (*to_client == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
    exit(0);
  }
  if (!write(*to_client, "Request received!", 256)){
    printf("Server read error: %s\n", strerror(errno));
  }

  char response[256];
  printf("Awaiting response from client...\n");
  if (!read(fd, response, 256)){
    printf("Server read error: %s\n", strerror(errno));
    exit(0);
  }
  else{
    // set up the read portion of the secret pipe, remove general pipe
    fd = open(response, O_RDONLY);
    remove("well_known_pipe");
    printf("Response from client: \'%s\'\n", response);
  }
  printf("============ENDING HANDSHAKE==============\n\n");

  return fd;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  printf("============SENDING HANDSHAKE==============\n");
  if (!mkfifo("secret_pipe", 0644)){
    printf("Pipe initialized!\n");
  }

  *to_server = open("well_known_pipe", O_WRONLY);
  if (*to_server == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
    exit(0);
  }
  printf("Transmitting message to server...\n" );
  char *pipe = "secret_pipe";
  if (!write(*to_server, pipe, strlen(pipe))){
    printf("Server write error: %s\n", strerror(errno));
  }

  char response[256];
  int secret_response = open(pipe, O_RDONLY);
  remove(pipe);
  if (secret_response == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }

  // read response
  printf("Reading response from server...\n" );
  if (!read(secret_response, response, 256)){
    printf("Client read error: %s\n", strerror(errno));
  }
  else{
    printf("Response from server: \'%s\'\n", response);
  }

  // send back a message
  printf("Setting a second pipe...\n\n");

  // build a second pipe
  if (!mkfifo("secret2", 0644)){
    printf("Second pipe initialized!\n");
  }
  char * second_pipe = "secret2";

  if (!write(*to_server, second_pipe, strlen(second_pipe))){
    printf("Client write error: %s\n", strerror(errno));
  }

  *to_server = open(second_pipe, O_WRONLY);
  remove(second_pipe);
  return secret_response;
}
