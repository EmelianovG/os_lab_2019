#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "library.h"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct Server {
  char ip[255];
  int port;
};
uint64_t fact = 1;

struct serverArg {
    uint64_t start;
    uint64_t finish;
    uint64_t module;
    struct Server server;
};

void* factThread(void* args) {
  struct serverArg* arg = (struct serverArg*) args;
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		fprintf(stderr, "Can not create server socket!");
		exit(1);
	}
	struct hostent *hostname = gethostbyname(arg->server.ip);

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(arg->server.port);
	server.sin_addr.s_addr = *((uint64_t*)hostname->h_addr_list[0]);

	if (connect(server_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
		fprintf(stderr, "Can not connect to server!");
		exit(1);
	}

  uint64_t finish = arg->finish;
  uint64_t module = arg->module;
  uint64_t start = arg->start;
  char task[sizeof(uint64_t) * 3];
  memcpy(task, &start, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &finish, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &module, sizeof(uint64_t));

	if (send(server_fd, task, sizeof(task), 0) < 0) {
    fprintf(stderr, "Send failed\n");
    exit(1);
  }

  printf("%s:%d send: %lu %lu %lu\n", arg->server.ip, arg->server.port, start, finish, module);

	char response[sizeof(uint64_t)];
  if (recv(server_fd, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }

  uint64_t tmp = 0;
  memcpy(&tmp, response, sizeof(uint64_t));
  printf("%s:%d answer: %lu\n", arg->server.ip, arg->server.port, tmp);
  
  pthread_mutex_lock(&mutex);
  fact = MultModulo(fact, tmp, module);
  pthread_mutex_unlock(&mutex);
  
  close(server_fd);  
}


bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        // TODO: your code here
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        // TODO: your code here
        break;
      case 2:
        // TODO: your code here
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  // TODO: for one server here, rewrite with servers from file
  unsigned int servers_num = 0;
  FILE* fptr = fopen("servers.txt", "r");
  struct Server *to = malloc(sizeof(struct Server) * 10);
		printf("Servers finder started\n");
	for(int i = 0; !feof(fptr); i++) {
    servers_num++;
		fscanf(fptr, "%s %d", to[i].ip, &to[i].port);
		printf("Server %s:%d\n", to[i].ip, to[i].port);
	}
	fclose(fptr);

  pthread_t threads[servers_num];
  struct serverArg arg[servers_num];
  for(int i = 0; i<servers_num; i++){
      arg[i].start = ((k*i)/servers_num)+1;
      arg[i].finish = ((k*(i+1))/servers_num)+1;
      arg[i].module = mod;
      arg[i].server = to[i];
      //printf("args: %d (%d-%d)\n", i, arg[i].start, arg[i].finish);
  }
  for(int i = 0; i<servers_num; i++){
      if (pthread_create(&threads[i], NULL, factThread, (void *)&arg[i])) {
          printf("Error: pthread_create failed!\n");
          return 1;
      }
  }
  for (int i = 0; i < servers_num; i++) {
      pthread_join(threads[i], NULL);
  }

  free(to);
  printf("Final answer: %lu\n", fact);
          
  return 0;
}
