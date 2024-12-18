#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <getopt.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "library.h"
#include "pthread.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
uint64_t fact = 1;
int port = -1;

struct factArg {
  uint64_t start;
  uint64_t finish;
  uint64_t module;
};

uint64_t Factorial(const struct factArg *args) {
	uint64_t ans = 1;
	for (uint64_t i = args->start; i < args->finish; ++i) {
		ans = MultModulo(ans, i, args->module);
	}

	return ans;
}

void *ThreadFactorial(void *args) {
	struct factArg *fargs = (struct factArg *)args;
	return (void *)(uint64_t *)Factorial(fargs);
}

int main(int argc, char **argv) {
  int tnum = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"port", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        port = atoi(optarg);
        // TODO: your code here
        break;
      case 1:
        tnum = atoi(optarg);
        // TODO: your code here
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Unknown argument\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (port == -1 || tnum == -1) {
    fprintf(stderr, "Using: %s --port 1 --tnum 4\n", argv[0]);
    return 1;
  }

  unsigned int servers_num = 0;
  FILE* fptr = fopen("servers.txt", "r");
	int serv;
  char a[255];
  int b;
  for(int i = 0; !feof(fptr); i++) {
    if(i == port){
		  fscanf(fptr, "%s %d", a, &serv);
      break;
    }
		fscanf(fptr, "%s %d", a, &b);
	}

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    fprintf(stderr, "Can not create server socket!");
    return 1;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons((uint16_t)serv);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  int opt_val = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
  int err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
  if (err < 0) {
    fprintf(stderr, "Can not bind to socket!\n");
    return 1;
  }

  err = listen(server_fd, 128);
  if (err < 0) {
    fprintf(stderr, "Could not listen on socket\n");
    return 1;
  }

  printf("Server listening at %d\n", port);

  while (true) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0) {
      fprintf(stderr, "Could not establish new connection\n");
      continue;
    }

    while (true) {
      unsigned int buffer_size = sizeof(uint64_t) * 3;
      char from_client[buffer_size];
      int read = recv(client_fd, from_client, buffer_size, 0);

      if (!read)
        break;
      if (read < 0) {
        fprintf(stderr, "Client read failed\n");
        break;
      }
      if (read < buffer_size) {
        fprintf(stderr, "Client send wrong data format\n");
        break;
      }
      uint64_t begin = 0;
      uint64_t end = 0;
      uint64_t mod = 0;
      memcpy(&begin, from_client, sizeof(uint64_t));
      memcpy(&end, from_client + sizeof(uint64_t), sizeof(uint64_t));
      memcpy(&mod, from_client + 2 * sizeof(uint64_t), sizeof(uint64_t));

      fprintf(stdout, "Receive: %lu %lu %lu\n", begin, end, mod);
      pthread_t threads[tnum];
      struct factArg arg[tnum];
      for(int i = 0; i<tnum; i++){
          arg[i].start = begin+(((end-begin)*i)/tnum)+1;
          arg[i].finish = begin+(((end-begin)*(i+1))/tnum)+1;
          arg[i].module = mod;
      }
      for(int i = 0; i<tnum; i++){
        printf("params port %d: %lu %lu %lu\n", serv, arg[i].start, arg[i].finish, arg[i].module);
          if (pthread_create(&threads[i], NULL, ThreadFactorial, (void *)&arg[i])) {
              printf("Error: pthread_create failed!\n");
              return 1;
          }
      }
			for (uint32_t i = 0; i < tnum; i++) {
				uint64_t result = 0;
				pthread_join(threads[i], (void **)&result);
				fact = MultModulo(fact, result, mod);
			}
      char buffer[sizeof(fact)];
      memcpy(buffer, &fact, sizeof(fact));
      err = send(client_fd, buffer, sizeof(fact), 0);
      if (err < 0) {
        fprintf(stderr, "Can't send data to client\n");
        break;
      }
    }

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
  }

  return 0;
}
