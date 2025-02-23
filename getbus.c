#include <dirent.h>
#include <sys/param.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_BUS 10

extern char* optarg; // Stop vscode wingeing

int get_from_sysfs(int default_bus, bool verbose);

int get_bus(int argc, char **argv)
{
    int bus = DEFAULT_BUS;
    bool set_from_cli = false;
    bool verbose = false;
  
    while (true) {
        int opt = getopt(argc,argv, "b:hv");
        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'b':
                bus = atoi(optarg);
                set_from_cli = true;
                break;
            case 'h':
                fprintf(stderr, 
                    "Usage: %s [-b bus] [-h]\n"
                    "Default bus is %d\n", argv[0], DEFAULT_BUS);
                exit(0);
            case 'v':
                verbose = true;
                break;
            default:
                break;
        }
    }

    if (!set_from_cli) {
        bus = get_from_sysfs(bus, verbose);
    }

    if (verbose) {
        fprintf(stderr, "Using bus: i2c-%d", bus);
        if (bus != DEFAULT_BUS) {
            fprintf(stderr, ", compiled in default is i2c-%d", DEFAULT_BUS);
        }
        fprintf(stderr, "\n");
    }

    return bus;
}

int get_from_sysfs(int default_bus, bool verbose)
{
    // Inspired by the code for listing busses in
    // i2cdetect
    int bus = default_bus;
    int len;
    const char *path = "/sys/class/i2c-dev";
    char namepath[NAME_MAX];
    char name[120];
    DIR *dir;
    struct dirent *de;
    FILE *fp;

    if (verbose) {
        fprintf(stderr, "Attempting to find bus from sysfs\n");
    }

    if (NULL != (dir = opendir(path))) {
        while (NULL != (de = readdir(dir))) {
            if (!strcmp(de->d_name, ".")) {
               continue;
            }
            if (!strcmp(de->d_name, "..")) {
               continue;
            }

            len = snprintf(namepath, NAME_MAX, "%s/%s/name", path, de->d_name);
            if (len >= NAME_MAX) {
                fprintf(stderr, "%s: path truncated\n", namepath);
                continue;
            }

            if (NULL != (fp = fopen(namepath, "r"))) {
                fgets(name, 120, fp);
                fclose(fp);
                if (strstr(name, "MCP2221") != NULL) {
                    int found_bus;
                    if (sscanf(de->d_name, "i2c-%d", &found_bus) != EOF) {
                        if (verbose) {
                            fprintf(stderr, "Found MCP2221 on bus i2c-%d from sysfs\n", found_bus);
                        }
                        bus = found_bus;
                        break;
                    }
                }
            }
        }

        closedir(dir);
    }
    else {
        perror("Could not open sysfs dir");
    }

    return bus;
}
