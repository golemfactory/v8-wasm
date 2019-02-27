#include <stdio.h>

int main(int argc, char** argv) {
    FILE* f_in = fopen("in.txt", "r");
    FILE* f_out = fopen("out.txt", "w");

    fprintf(f_out, "Hello world!\n");

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f_in)) != -1) {
      fprintf(f_out, "%s\n", line);
    }

    fprintf(f_out, "%s\n", argv[1]);

    fclose(f_out);
    fclose(f_in);
    return 0;
}
