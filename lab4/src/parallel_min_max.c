#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int pnum = -1;
pid_t pids[];
void killer(){
  for(int i = 0; i<pnum; i++){
      int s;
      pid_t pid = waitpid(pids[i], &s, WNOHANG);
      if(pid == 0){
        printf("Process %d was killed\n", i);
        kill(pids[i], SIGKILL);
      }
    }
}


int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  bool with_files = false;
  int timeout = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        // srand(seed);
                        // your code here
                        // error handling
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        // your code here
                        // error handling
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        // your code here
                        // error handling
                        break;
                    case 3:
                        with_files = true;
                        break;
                    case 4:
                        timeout = atoi(optarg);
                        break;
                    defalut:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }
  FILE* file_min;
  FILE* file_max;
  int p[2];
  if(with_files == true){
    file_min = fopen("file_min.txt", "w");
    file_max = fopen("file_max.txt", "w");
  }
  else{
    if(pipe(p) == -1){
      printf("Error in pipe");
      return 1;
    }
  }
  
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  pids[pnum];
  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        if(active_child_processes == 2 || active_child_processes == 5){
          sleep(2);
        }
        struct MinMax min_max;
        min_max.min = INT_MAX;
        min_max.max = INT_MIN;

        for(int j = i; j<array_size; j+=pnum){
          if(array[j] < min_max.min) min_max.min=array[j];
          if(array[j] > min_max.max) min_max.max=array[j];
        }

        if (with_files) {
          fwrite(&min_max.min, sizeof(int), 1, file_min);
          fwrite(&min_max.max, sizeof(int), 1, file_max);
        } else {
          write(p[1], &min_max.min, sizeof(int));
          write(p[1], &min_max.max, sizeof(int));
        }
        return 0;
      }
      else{
        pids[i] = child_pid;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if (timeout != -1){
    alarm(timeout);
    signal(SIGALRM, killer);
  }

  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

  if (with_files) {
    fclose(file_max);
    fclose(file_min);
    file_min = fopen("file_min.txt", "r");
    file_max = fopen("file_max.txt", "r");
  } else {
    close(p[1]);
  }
  
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;
  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      fread(&min, 1, sizeof(int), file_min);
      fread(&min, 1, sizeof(int), file_max);
    } else {
      read(p[0], &min, sizeof(int));
      read(p[0], &max, sizeof(int));
    }
    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }
  if (with_files) {
    fclose(file_max);
    fclose(file_min);
  } else {
    close(p[0]);
  }
  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
