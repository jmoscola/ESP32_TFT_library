#include <stdio.h>
#include "tftfontfiles.h"


int main(int argc, char** argv) {
    
    if (argc != 3) {
        fprintf(stderr, "\nIncorrect number of arguments\n");
        fprintf(stderr, "===============================\n");
        fprintf(stderr, "USAGE:\n");
        fprintf(stderr, "    c2fon <C-formatted font file> <.fon output file>\n\n");
        fprintf(stderr, "Example:\n");
        fprintf(stderr, "    c2fon ./c_font_files/dejavu.c  ./compiled_font_files/dejavu.fon\n\n");
        exit(EXIT_FAILURE);
    }
    
    char* font_file = argv[1];
    char* output_file = argv[2];
    compile_font_file(font_file, output_file, 1);
    
    return 0;
}
