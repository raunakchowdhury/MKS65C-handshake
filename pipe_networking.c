#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  if (!mkfifo("well_known_pipe", 0644)){
    printf("Pipe initialized!\n");
  }

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
  printf("============ENDING HANDSHAKE==============\n\n");

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

  printf("============SENDING HANDSHAKE==============\n");
  if (!mkfifo("secret_pipe", 0644)){
    printf("Pipe initialized!\n");
  }

  fd = open("well_known_pipe", O_WRONLY);
  if (fd == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  printf("Transmitting message to server...\n" );
  if (!write(fd, "secret_pipe", strlen("secret_pipe"))){
    printf("Server write error: %s\n", strerror(errno));
  }

  char response[256];
  int secret_response = open("secret_pipe", O_RDONLY);
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
  printf("Final exchange...\n\n");
  int final_exchange = open("well_known_pipe", O_WRONLY);
  if (final_exchange == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  char * final_response = "Done!";
  if (!write(final_exchange, final_response, strlen(final_response))){
    printf("Client write error: %s\n", strerror(errno));
  }

  *to_server = secret_response;
  return fd;
}

/*=========================
  client_process
  args: none

  Client sends messages to the server using the pipe.
  =========================*/
void client_process(){
  int len;
  if (!mkfifo("secret_pipe_two", 0644)){
    printf("Created second pipe!\n" );
  }

  printf("User, input some stuff to be modified by the server (255 chars max): ");
  char input[256];
  fgets(input, 256, stdin);
  len = strlen(input);
  input[len-1] = 0;

  // open and write the message to the first pipe
  int secret_pipe = open("secret_pipe", O_WRONLY);
  if (secret_pipe == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  if (!write(secret_pipe, input, 256)){
    printf("Write error: %s\n", strerror(errno));
  }

  //await the response from the server and display the response
  char response[256];
  int secret_pipe_two = open("secret_pipe_two", O_RDONLY);
  if (secret_pipe_two == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  if (!read(secret_pipe_two, response, 1000)){
    printf("Read error: %s\n", strerror(errno));
  }
  else{
    printf("Server returned: %s\n\n", response);
  }
}

/*=========================
  client_process
  args: none

  Server modifies the client messages and returns them.
  =========================*/
void server_process(){
  char message[400];

  // open and read the message
  int secret_pipe = open("secret_pipe", O_RDONLY);
  if (secret_pipe == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  if (!read(secret_pipe, message, 256)){
    printf("Read error: %s\n", strerror(errno));
  }
  printf("Received response! Writing back...\n");
  // printf("Message: \'%s\'\n", message);
  strcat(message, " Mr K says hi!");
  message[strlen(message)] = 0;
  // printf("Message after change: \'%s\'\n", message);

  //Write response to the client
  int secret_pipe_two = open("secret_pipe_two", O_WRONLY);
  if (secret_pipe_two == -1){
    printf("Open Attempt Error: %s\n", strerror(errno));
  }
  if (!write(secret_pipe_two, message, 400)){
    printf("Write error: %s\n", strerror(errno));
  }
  else{
    printf("Response sent!\n\n");
  }
}
