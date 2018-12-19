#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  mkfifo("well_known_pipe", 0644);
  printf("Pipe initialized!\n");

  char *message = "Message received!";
  char pipe_name[256];

  int fd;
  fd = open("well_known_pipe", O_RDONLY);
  if (fd == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }

  if (!read(fd, pipe_name, 256)){
    printf("Server read error: %s\n", strerror(errno));
  }

  printf("Setting up other pipe %s...\n", pipe_name);
  int secret_pipe = open(pipe_name, O_WRONLY);
  if (fd == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  if (!write(secret_pipe, "Request received!", 256)){
    printf("Server read error: %s\n", strerror(errno));
  }

  char response[256];
  printf("Awaiting response from client...\n");
  if (!read(fd, response, 256)){
    printf("Server read error: %s\n", strerror(errno));
  }
  else{
    printf("Response from client: \'%s\'\n", response);
  }


  *to_client = secret_pipe;

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
  int fd;
  int new_pipe;

  mkfifo("secret_pipe", 0644);
  printf("Pipe initialized!\n");

  fd = open("well_known_pipe", O_WRONLY);
  if (fd == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  printf("Transmitting message to server...\n" );
  if (!write(fd, "secret_pipe", strlen("secret_pipe"))){
    printf("Server write error: %s\n", strerror(errno));
  }

  char response[256];
  fd = open("secret_pipe", O_RDONLY);
  if (fd == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }

  // read response
  printf("Reading response from server...\n" );
  if (!read(fd, response, 256)){
    printf("Client read error: %s\n", strerror(errno));
  }
  else{
    printf("Response from server: \'%s\'\n", response);
  }

  // send back a message
  printf("Final exchange...\n");
  fd = open("well_known_pipe", O_WRONLY);
  if (fd == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  char * final_response = "Done!";
  if (!write(fd, final_response, strlen(final_response))){
    printf("Client write error: %s\n", strerror(errno));
  }


  return fd;
}
