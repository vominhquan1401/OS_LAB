#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc_logic.h"

#define MAX_INPUT 100

void get_input(char *input) {
    printf(">> ");
    fgets(input, MAX_INPUT, stdin);
    // Remove the newline character
    input[strcspn(input, "\n")] = 0;
}

int main() {
    char input[MAX_INPUT];
    double result = 0.0;
    int exit_flag = 0;

    while (!exit_flag) {
        get_input(input);

        // Check if the input is "EXIT"
        if (strcmp(input, "EXIT") == 0) {
            printf("EXIT\n");
            exit_flag = 1;
            continue;
        }

        // Replace ANS with the last result
        char *ans_pos = strstr(input, "ANS");
        if (ans_pos) {
            char buffer[MAX_INPUT];
            snprintf(buffer, MAX_INPUT, "%.*s%.2f%s", (int)(ans_pos - input), input, result, ans_pos + 3);
            strcpy(input, buffer);
        }

        if (calculate_expression(input, &result) == 0) {
            printf("%.2f\n", result);
        } else {
            printf("SYNTAX ERROR\n");
        }
    }

    return 0;
}

