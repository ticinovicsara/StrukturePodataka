#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_OF -1
#define BUFFER_SIZE 1024
#define MAX_BODOVI 100.0

typedef struct{
    char ime[15];
    char prezime[30];
    int bodovi;
}student;

int count_rows(char* filename);
student* read_students(char* filename, int broj_studenata);
void print_students(student* studenti, int broj_studenata);


int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Unijeti ./%s filename\n", argv[0]);
        return ERROR_OF;
    }
    char* filename = argv[1];
    int broj_studenata = count_rows(filename);

    if(broj_studenata == ERROR_OF){
        return ERROR_OF;
    }

    student* studenti = read_students(filename, broj_studenata);
    if(studenti == NULL){
        return ERROR_OF;
    }

    print_students(studenti, broj_studenata);

    free(studenti);

    return 0;
}


int count_rows(char* filename){
    FILE *fp = NULL;
    fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Nesupjesno otvaranje datoteke\n");
        return ERROR_OF;
    }

    char buffer[BUFFER_SIZE] = {0};
    int count=0;


    while(fgets(buffer, BUFFER_SIZE, fp) != NULL){
        count++;
    }

    fclose(fp);
    return count;
}

student* read_students(char* filename, int broj_studenata){
    FILE *fp = NULL;
    fp = fopen(filename, "r");
    if(fp == NULL){
        printf("Neuspjesno otvaranje datoteke\n");
        return NULL;
    }

    student *studenti = (student*)malloc(broj_studenata * sizeof(student));
    if(studenti == NULL){
        printf("Greska pri alokaciji memorije\n");
        fclose(fp);
        return NULL;
    }

    for(int i=0; i<broj_studenata; i++){
        fscanf(fp, "%s %s %d", studenti[i].ime, studenti[i].prezime, &studenti[i].bodovi);
    }

    fclose(fp);
    return studenti;
}

void print_students(student* studenti, int broj_studenata){
    printf("Ime\tPrezime\tBodovi\tRelativni bodovi\n");
    for(int i=0; i<broj_studenata; i++){
        double relativni_bodovi = ((double)studenti[i].bodovi / MAX_BODOVI) * 100;
        printf("%s\t%s\t%d\t%.2f\n", studenti[i].ime, studenti[i].prezime, studenti[i].bodovi, relativni_bodovi);
    }
}