#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_BUS 10

extern char* optarg; // Stop vscode wingeing

int get_bus(int argc, char **argv)
{
    int bus = DEFAULT_BUS;
  
    while (true) {
        int opt = getopt(argc,argv, "b:h");
        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'b':
                bus = atoi(optarg);
                break;
            case 'h':
                fprintf(stderr, 
                    "Usage: %s [-b bus] [-h]\n"
                    "Default bus is %d\n", argv[0], DEFAULT_BUS);
                exit(0);
            default:
                break;
        }
    }

    return bus;
}
