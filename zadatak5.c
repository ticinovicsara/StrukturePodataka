#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_BUFFER 1024

typedef struct _postfix* position;
typedef struct _postfix{
    double broj;
    position next;
}Postfix;

void push(position head, double broj);
double pop(position head);
void read_file(char* filename, position head);

int main(){
    Postfix head = { .broj = 0, .next = NULL};

    read_file("postfix.txt", &head);

    return 0;
}

void push(position head, double broj) {
    position q = (position)malloc(sizeof(Postfix));
    if (!q) {
        printf("Neuspjesno alocirana memorija\n");
        return;
    }

    q->broj = broj;

    q->next = head->next;
    head->next = q;
}

double pop(position head) {
    position to_delete;
    double vraceni_broj;

    if (head->next == NULL) {
        printf("Greska: Stog je prazan, postfix izraz nije ispravan.\n");
        return 0;
    }

    to_delete = head->next;
    vraceni_broj = to_delete->broj;
    head->next = to_delete->next;

    free(to_delete);

    return vraceni_broj;
}

void read_file(char* filename, position head){
    FILE* fp = NULL;
    fp = fopen(filename, "r");
    if(!fp){
        printf("datoteka se ne moze otvoriti\n");
        return;
    }

    char buffer[MAX_BUFFER];
    char* token;
    double broj;
    char operator;
    double operand1, operand2, rezultat;

    while (fgets(buffer, MAX_BUFFER, fp)) {
        head->next = NULL;
        token = strtok(buffer, " \n");

        while (token != NULL) {
            if (sscanf(token, "%lf", &broj) == 1) {
                push(head, broj);
            }
            else if (sscanf(token, " %c", &operator) == 1) {
                if (head->next && head->next->next) {
                    operand2 = pop(head);
                    operand1 = pop(head);

                    switch (operator) {
                        case '*':
                            rezultat = operand1 * operand2;
                            break;
                        case '/':
                            if (operand2 == 0) {
                                printf("Greska: Dijeljenje s nulom!\n");
                                fclose(fp);
                                exit(EXIT_FAILURE);
                            }
                            rezultat = operand1 / operand2;
                            break;
                        case '+':
                            rezultat = operand1 + operand2;
                            break;
                        case '-':
                            rezultat = operand1 - operand2;
                            break;
                        default:
                            printf("Greska: Nepoznat operator '%c'\n", operator);
                            fclose(fp);
                            exit(EXIT_FAILURE);
                    }

                    push(head, rezultat);
                }
            }
            else {
                printf("Greska: Neispravan unos '%s'\n", token);
                fclose(fp);
                exit(EXIT_FAILURE);
            }

            token = strtok(NULL, " \n");
        }

        if (head->next && head->next->next == NULL) {
        printf("Rezultat je %.2lf\n", head->next->broj);
        }
        else{
            printf("Greska: Postfix izraz nije ispravan.\n");
        }
    }

    fclose(fp);
}