#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void print_file(char *filename, bool number, bool show_ends, bool show_tabs, bool show_nonprinting,
                bool show_bytes, bool show_lines, bool show_help, bool squeeze_blank, char *output_file) {
    FILE *fp_in, *fp_out;
    int c;
    unsigned long line_count = 0, byte_count = 0;

    fp_in = fopen(filename, "r");
    if (fp_in == NULL) {
        fprintf(stderr, "Ошибка: не удалось открыть файл %s\n", filename);
        exit(EXIT_FAILURE);
    }

    if (output_file != NULL) {
        fp_out = fopen(output_file, "w");
        if (fp_out == NULL) {
            fprintf(stderr, "Ошибка: не удалось создать файл %s\n", output_file);
            exit(EXIT_FAILURE);
        }
    } else {
        fp_out = stdout;
    }

    while ((c = fgetc(fp_in)) != EOF) {
        // Обработка флага -n
        if (number) {
            if (byte_count == 0) {
                fprintf(fp_out, "%lu ", ++line_count);
            }
            if (c == '\n') {
                byte_count = 0;
                fprintf(fp_out, "%c", c);
                fprintf(fp_out, "%lu ", ++line_count);
            } else {
                byte_count++;
            }
        }
        // Обработка флага -e
        if (show_ends && c == '\n') {
            fprintf(fp_out, "$");
        }
        // Обработка флага -t
        if (show_tabs && c == '\t') {
            fprintf(fp_out, "^I");
        } else {
            // Обработка флага -v
            if (show_nonprinting) {
                if (c == '\t') {
                    fprintf(fp_out, "\\t");
                } else if (c == '\n') {
                    fprintf(fp_out, "\\n");
                } else if (c == '\r') {
                    fprintf(fp_out, "\\r");
                } else if (c == '\0') {
                    fprintf(fp_out, "\\0");
                } else {
                    fprintf(fp_out, "%c", c);
                }
            } else {
                fprintf(fp_out, "%c", c);
            }
        }
        // Обработка флага -c
        if (show_bytes) {
            byte_count++;
        }
        // Обработка флага -l
        if (show_lines && c == '\n') {
            line_count++;
        }
        // Обработка флага -s
        if (squeeze_blank && c == '\n') {
            while ((c = fgetc(fp_in)) == '\n') {
            }
            ungetc(c, fp_in);
            fprintf(fp_out, "%c", '\n');
        }
    }

    fclose(fp_in);
    if (output_file != NULL) {
        fclose(fp_out);
    }

    // Обработка флага -c
    if (show_bytes) {
        fprintf(fp_out, "%lu", byte_count);
    }
    // Обработка флага -l
    if (show_lines) {
        fprintf(fp_out, "%lu", line_count);
    }
}

int main(int argc, char **argv) {
    int i = 1;
    bool number = false;
    bool show_ends = false;
    bool show_tabs = false;
    bool show_nonprinting = false;
    bool show_bytes = false;
    bool show_lines = false;
    bool show_help = false;
    bool squeeze_blank = false;
    char *output_file = NULL;
    char *filename;

    if (argc < 2) {
        fprintf(stderr, "Ошибка: не указан файл\n");
        exit(EXIT_FAILURE);
    }

    // Обработка флагов
    while (i < argc && argv[i][0] == '-') {
        int len = strlen(argv[i]) - 1;
        for (int j = 1; j <= len; j++) {
            switch (argv[i][j]) {
                case 'n':
                    number = true;
                    break;
                case 'e':
                    show_ends = true;
                    break;
                case 't':
                    show_tabs = true;
                    break;
                case 'v':
                    show_nonprinting = true;
                    break;
                case 'c':
                    show_bytes = true;
                    break;
                case 'l':
                    show_lines = true;
                    break;
                case 'h':
                    show_help = true;
                    break;
                case 's':
                    squeeze_blank = true;
                    break;
                case 'f':
                    if (i + 1 >= argc) {
                        fprintf(stderr, "Ошибка: не указано имя файла для флага -f\n");
                        exit(EXIT_FAILURE);
                    }
                    output_file = argv[++i];
                    break;
                case 'o':
                    break;
                default:
                    fprintf(stderr, "Ошибка: неверный флаг: -%c\n", argv[i][j]);
                    exit(EXIT_FAILURE);
            }
        }
        i++;
    }

    if (show_help) {
        printf("Использование: mycat [флаги] файлы...\n");
        printf("Флаги: \n");
        printf(" -n\tнумеровать строки\n");
        printf(" -e\tпоказать символ конца строки как $\n");
        printf(" -t\tпоказать символ табуляции как ^I\n");
        printf(" -v\tпоказать неотображаемые символы как \\n, \\r, и т.д.\n");
        printf(" -c\tпоказать количество байтов в файле\n");
        printf(" -l\tпоказать количество строк в файле\n");
        printf(" -s\tисключить множественные пустые строки\n");
        printf(" -f <имя файла>\tзаписать вывод в файл\n");
        printf(" -h\tпоказать это сообщение и выйти\n");
        return 0;
    }

    // Обработка каждого файла
    while (i < argc) {
        filename = argv[i];
        print_file(filename, number, show_ends, show_tabs, show_nonprinting,
                show_bytes, show_lines, show_help, squeeze_blank, output_file);
        i++;
    }

    return 0;
}