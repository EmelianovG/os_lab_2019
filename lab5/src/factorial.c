#include <stdio.h>
#include <pthread.h>
#include <getopt.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int fact = 1;

struct factArg {
    int start;
    int finish;
    int module;
};

void* factThread(void* args) {
    struct factArg* arg = (struct factArg*) args;
    int finish = arg->finish;
    int module = arg->module;
    for(int i = arg->start; i<finish; i++){
        pthread_mutex_lock(&mutex);
        fact*=i;
        fact%=module;
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char** argv) {
    int k = -1;
    int pnum = -1;
    int mod = -1;

    while (1) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"k", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"mod", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        k = atoi(optarg);
                        break;
                    case 1:
                        pnum = atoi(optarg);
                        break;
                    case 2:
                        mod = atoi(optarg);
                        break;
                    defalut:
                        printf("Index %d is out of options\n", option_index);
                }
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

    if (k == -1 || pnum == -1 || mod == -1) {
        printf("Usage: --k \"num\" --pnum \"num\" --mod \"num\"\n");
        return 1;
    }
    pthread_t threads[pnum];
    struct factArg arg[pnum];
    for(int i = 0; i<pnum; i++){
        arg[i].start = ((k*i)/pnum)+1;
        arg[i].finish = ((k*(i+1))/pnum)+1;
        arg[i].module = mod;
        //printf("args: %d (%d-%d)\n", i, arg[i].start, arg[i].finish);
    }
    for(int i = 0; i<pnum; i++){
        if (pthread_create(&threads[i], NULL, factThread, (void *)&arg[i])) {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Answer: %d\n", fact);

}