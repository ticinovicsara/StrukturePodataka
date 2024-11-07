#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _element* position;
typedef struct _element{
    int coeff;
    int exp;
    position next;
}Element;

//prototypes
void free_poly(position head);
position create_element(int coeff, int exp);
void insert_after(position curr, position new);
int insert_sorted(position head, position new_el);
void delete_after(position temp);
position* read_file(char* filename, position** polynomials, int* brojac);
void add_polynomials(position* polynomials, int count, position result);
void multiply_polynomials(position* polynomials, int count, position result);
void print_polynomial(position head);

int main(){
    position* polynomials = NULL;
    int brojac_polinoma = 0;
    position sum = create_element(0, 0);
    position product = create_element(0, 0);

    read_file("polinomi.txt", &polynomials, &brojac_polinoma);

    add_polynomials(polynomials, brojac_polinoma, sum);
    multiply_polynomials(polynomials, brojac_polinoma, product);

    printf("Suma polinoma\n");
    print_polynomial(sum);

    printf("Produkt polinoma\n");
    print_polynomial(product);

    free_poly(sum);
    free_poly(product);

    for(int i=0; i<brojac_polinoma; i++){
        free_poly(polynomials[i]);
    }
    free(polynomials);

    return 0;
}

void free_poly(position head){
    while(head != NULL){
        position temp = head;
        head = head->next;
        free(temp);
    }
}

position create_element(int coeff, int exp){
    position new_el = NULL;
    new_el = (position)malloc(sizeof(Element));
    if(new_el == NULL){
        printf("neuspjesna alokacija memorije\n");
        return NULL;
    }

    new_el->coeff = coeff;
    new_el->exp = exp;
    new_el->next = NULL;

    return new_el;
}

void insert_after(position curr, position new){
    if (curr != NULL && new != NULL) {
        new->next = curr->next;
        curr->next = new;
    }
}

int insert_sorted(position head, position new_el){
    position temp = head;

    while(temp->next && (temp->next->exp < new_el->exp)){ //ogranicili se da je lista puna s barem jednim elementom
        temp = temp->next;
    }

    if(temp->next == NULL || temp->next->exp != new_el->exp){
        insert_after(temp, new_el);
    }
    else{
        int resultCoeff = temp->next->coeff + new_el->coeff;
        if(resultCoeff == 0){
            delete_after(temp);
            free(new_el);
        }
        else{
            temp->next->coeff = resultCoeff;
            free(new_el);
        }
    }
    return 0;
}

void delete_after(position temp){
    if(temp == NULL || temp->next == NULL){
        return;
    }
    position todelete = temp->next;
    temp->next = todelete->next;
    free(todelete);
}

position* read_file(char* filename, position** polynomials, int* brojac){
    FILE* file = NULL;
    file = fopen(filename, "r");
    if(!file){
        printf("nemoguce otvoriti datoteku\n");
        return NULL;
    }

    char buffer[1024];
    int c, e, numBytes;
    *brojac = 0;

    *polynomials = NULL;

    while(fgets(buffer, sizeof(buffer), file) != NULL){
        position head = create_element(0, 0);   //head novog polinoma
        char* line = buffer;   

        while(strlen(line) > 0){
            //moramo savit prazninu da ne zbunjuje sscanf
            int status = sscanf(line, " %d %d %n", &c, &e, &numBytes); //numBytes broji koliko je obradjeno znakova
            //ocekujemo da status bude 2
            if(status == 2){
                position q = create_element(c, e);
                insert_sorted(head, q);
            }
            else{
                printf("datoteka nije valjana\n");  
                fclose(file);
                return NULL;
            }

            line += numBytes;
        }

        (*brojac)++;
        position* temp = realloc(*polynomials, (*brojac) * sizeof(position));   
        if(!temp){
            printf("neuspjesna alokacija memorije\n");
            fclose(file);
            return NULL;
        }
        *polynomials = temp;
        (*polynomials)[(*brojac) - 1] = head;   //novi polinom je pohranjen
    }

    fclose(file);
    return *polynomials;
}

void add_polynomials(position* polynomials, int count, position result){
    for(int i=0; i<count; i++){
        position p = polynomials[i]->next;
        while(p != NULL){
            insert_sorted(result, create_element(p->coeff, p->exp));
            p = p->next;
        }
    }
}

void multiply_polynomials(position* polynomials, int count, position result){
    if(count == 0) return;

    //inicijalizacija rezultata na 1
    position temp_result = create_element(0, 0);
    insert_sorted(temp_result, create_element(1, 0));  

    for(int i=0; i<count; i++){
        //privremeni rezultat za trenutni korak
        position inter = create_element(0, 0);  
        position p1 = temp_result->next;  

        //mnozenje svakog clana temp_result sa svakim clanom polynomials[i]
        while(p1 != NULL){
            position p2 = polynomials[i]->next;
            while(p2 != NULL){
                int new_coeff = p1->coeff * p2->coeff;
                int new_exp = p1->exp + p2->exp;
                insert_sorted(inter, create_element(new_coeff, new_exp));
                p2 = p2->next;
            }
            p1 = p1->next;
        }

        //brisanje temp_result osim glave jer se popunio clanovima inter
        //cije clanove ne zelimo ostaviti
        while(temp_result->next != NULL){
            delete_after(temp_result);
        }

        //zamjena temp_result s inter, temp_result sada pokazuje na sve clanove iz inter
        temp_result->next = inter->next;
        free(inter);
    }

    result->next = temp_result->next;
    free(temp_result);
}

void print_polynomial(position head){
    position temp = head->next;
    while(temp){
        printf("%dx^%d ", temp->coeff, temp->exp);
        if(temp->next){
            printf("+ ");
        }
        temp = temp->next;
    }
    printf("\n");
}
