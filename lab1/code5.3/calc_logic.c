#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calc_logic.h"

int calculate_expression(char *input, double *result) {
    char operator;
    double operand1, operand2;

    if (sscanf(input, "%lf %c %lf", &operand1, &operator, &operand2) != 3) {
        return -1;
    }

    switch (operator) {
        case '+':
            *result = operand1 + operand2;
            break;
        case '-':
            *result = operand1 - operand2;
            break;
        case '*':
            *result = operand1 * operand2;
            break;
        case '/':
            if (operand2 == 0) {
                printf("MATH ERROR\n");
                return -1;
            }
            *result = operand1 / operand2;
            break;
        case '%':
            *result = (int)operand1 % (int)operand2;
            break;
        default:
            return -1; // Invalid operator
    }

    return 0;
}

