#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("need at least one arg");
        return 0;
    }

    printf("these are hello from child_process()\n");
    for (int i = 1; i < argc; i++) {
        printf("hello! %s\n", argv[i]);
    }
    return 0;
}
