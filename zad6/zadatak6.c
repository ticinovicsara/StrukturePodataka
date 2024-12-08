#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DATE_LENGTH 30
#define BUFFER_SIZE 1024

typedef struct _artikl* ArtiklHead;
typedef struct _artikl{
    char naziv[20];
    int kolicina;
    double cijena;
    ArtiklHead next;
}Artikl;

typedef struct _racun* RacunHead;
typedef struct _racun{
    char datum[DATE_LENGTH];
    ArtiklHead anext;
    RacunHead rnext;
}Racun;

void read_racuni(char* filename, RacunHead head);
void insert_artikl_sorted(ArtiklHead* head, ArtiklHead novi);
void insert_racun_sorted(RacunHead* head, RacunHead novi);
int compare_dates(const char* date1, const char* date2);
void free_all(RacunHead head);
void pregled_po_datumu(RacunHead head, char* naziv, const char* start_date, const char* end_date);
void ispis_artikli(RacunHead head);

int main(){
    Racun racuniHead = { .datum = "", .anext = NULL, .rnext = NULL};

    read_racuni("racuni.txt", &racuniHead);

    char naziv[20];

    printf("Unesite naziv artikla: \n\n");
    ispis_artikli(&racuniHead);
    printf("\n\n: ");
    scanf("%s", naziv);

    char start_date[20];
    char end_date[20];

    printf("\n\nUnesite vremensko razdoblje (format yyyy-m-d): \n");
    printf("\nPocetni datum: ");
    scanf("%s", start_date);

    printf("\nZavrsni datum: ");
    scanf("%s", end_date);

    pregled_po_datumu(&racuniHead, naziv, start_date, end_date);

    free_all(racuniHead.rnext);

    return 0;
}

void read_racuni(char* filename, RacunHead head){
    FILE* fp = NULL;
    fp = fopen(filename, "r");
    if(!fp){
        printf("Nemoguce otvoriti datoteku %s\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    while(fscanf(fp, "%s", buffer) == 1){
        buffer[strcspn(buffer, "\n")] = '\0';

        FILE* racun = fopen(buffer, "r");
        if(!racun){
            printf("Nemoguce otvoriti racun\n");
            return;
        }

        RacunHead novi_racun = (RacunHead)malloc(sizeof(Racun));
        if(!novi_racun){
            printf("Nemoguce alocirati memoriju za racun\n");
            fclose(racun);
            return;
        }
        memset(novi_racun, 0, sizeof(Racun));

        fgets(buffer, BUFFER_SIZE, racun);
        strncpy(novi_racun->datum, buffer, DATE_LENGTH);
        
        while(fgets(buffer, BUFFER_SIZE, racun)){
            char naziv[30];
            int kolicina;
            double cijena;

            if(sscanf(buffer, "%s %d %lf", naziv, &kolicina, &cijena) == 3){
                if (kolicina <= 0) {
                    printf("Kolicina mora biti pozitivna!\n");
                    continue;
                }

                if (cijena <= 0.0) {
                    printf("Cijena mora biti pozitivna!\n");
                    continue;
                }

                ArtiklHead novi_artikl = (ArtiklHead)malloc(sizeof(Artikl));
                if(!novi_artikl){
                    printf("Nemoguce alocirati memoriju za artikl\n");
                    continue;
                }

                memset(novi_artikl, 0, sizeof(Artikl));
                strncpy(novi_artikl->naziv, naziv, sizeof(novi_artikl->naziv));
                novi_artikl->kolicina = kolicina;
                novi_artikl->cijena = cijena;

                insert_artikl_sorted(&novi_racun->anext, novi_artikl);
            }
        }

        fclose(racun);

        insert_racun_sorted(&head->rnext, novi_racun);
    }
    
    fclose(fp);
}


void insert_artikl_sorted(ArtiklHead* head, ArtiklHead novi) {
    ArtiklHead* curr = head;

    while (*curr && strcmp((*curr)->naziv, novi->naziv) < 0) {
        curr = &(*curr)->next;
    }

    if (*curr && strcmp((*curr)->naziv, novi->naziv) == 0) {
        (*curr)->kolicina += novi->kolicina;
        free(novi);
    } else {
        novi->next = *curr;
        *curr = novi;
    }
}


void insert_racun_sorted(RacunHead* head, RacunHead novi) {
    RacunHead* current = head;

    while (*current && compare_dates((*current)->datum, novi->datum) < 0) {
        current = &(*current)->rnext;
    }

    novi->rnext = *current;
    *current = novi;
}



int compare_dates(const char* date1, const char* date2){
    int y1, m1, d1;
    int y2, m2, d2;

    sscanf(date1, "%d-%d-%d", &y1, &m1, &d1);
    sscanf(date2, "%d-%d-%d", &y2, &m2, &d2);

    if(y1 != y2){
        return y1 - y2;
    }
    if(m1 != m2){
        return m1 - m2;
    }
    return d1 - d2;
}

void free_all(RacunHead head){
    while(head != NULL){
        RacunHead to_delete = head;
        head = head->rnext;

        ArtiklHead temp_artikli = to_delete->anext;

        while(temp_artikli != NULL){
            ArtiklHead to_delete_artikl = temp_artikli;
            temp_artikli = temp_artikli->next;
            free(to_delete_artikl);
        }

        free(to_delete);
    }
}

void pregled_po_datumu(RacunHead head, char* naziv, const char* start_date, const char* end_date){
    RacunHead curr = head->rnext;
    int total_quantity = 0;
    double total_amount = 0;

    while(curr){
        if(compare_dates(curr->datum, start_date) >= 0 && compare_dates(curr->datum, end_date) <= 0){
            ArtiklHead artikl = curr->anext;
            while(artikl){
                if(strcmp(artikl->naziv, naziv) == 0){
                    total_quantity += artikl->kolicina;
                    total_amount += artikl->kolicina * artikl->cijena;
                }
                artikl = artikl->next;
            }
        }
        curr = curr->rnext;
    }

    printf("\nArtikl: %s\n", naziv);
    printf("Ukupna kolicina: %d\n", total_quantity);
    printf("Ukupan trosak: %.2lf\n", total_amount);
}

void ispis_artikli(RacunHead head){
    ArtiklHead svi_artikli = NULL;

    RacunHead racun = head->rnext;

    while(racun){
        ArtiklHead artikl = racun->anext;

        while(artikl){
            ArtiklHead temp = svi_artikli;
            bool found = false;

            while(temp){
                if(strcmp(temp->naziv, artikl->naziv) == 0){
                    found = true;
                    temp->cijena = artikl->cijena;
                    break;
                }
                temp = temp->next;
            }

            if(!found){
                ArtiklHead novi_artikl = (ArtiklHead)malloc(sizeof(Artikl));
                if (novi_artikl) {
                    strncpy(novi_artikl->naziv, artikl->naziv, sizeof(novi_artikl->naziv));
                    novi_artikl->next = svi_artikli;
                    novi_artikl->cijena = artikl->cijena;
                    svi_artikli = novi_artikl;
                }
            }

            artikl = artikl->next;
        }
        racun = racun->rnext;
    }

    printf("Dostupni artikli:\n");
    ArtiklHead temp = svi_artikli;
    while (temp) {
        printf("%s\t\t%.2f\n", temp->naziv, temp->cijena);
        temp = temp->next;
    }

    while (svi_artikli) {
        ArtiklHead to_delete = svi_artikli;
        svi_artikli = svi_artikli->next;
        free(to_delete);
    }
}