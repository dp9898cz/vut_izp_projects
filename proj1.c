/*
Proudový editor textu
autor: Daniel Pátek (xpatek08)
VUT FIT 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1002 //1000 znaku + znak konce radku (\n) + znak konce retezce (\0)

void help(void);
void check_argc(int argc);
void print_curr(const char *curr);
void remove_eol(char *curr);
void replace_words(char *curr, const char *line, const char *oldW,const char *newW, int a);
int get_line(char *c);
int read_line(FILE *name, char *c);
int tree(char *curr, char *comm, FILE *name);
int get_char(char *s, int i);
int go_to(char *comm, FILE *name);
int goto_X(int a, int b, char *comm, FILE *name);
int S_replace(char *curr, char *comm);
int next(char *curr, const char *comm);
int next_count(int a, int b, char *curr);
int insert(const char *comm);
int after(char *curr, char *comm);
int before(char *curr, char *comm);
int delete_line(char *curr, const char *comm);

int main(int argc, char *argv[]){
    char curr_line[MAX];
    char comm[MAX];
    check_argc(argc);
    FILE *fr = fopen(argv[1], "r");                 //otevre soubor s prikazy
    if (get_line(curr_line) == 1){                  //nacte prvni radek
        fprintf(stderr, "Chyba vstupu ze stdin. Ukonceni programu.\n");
        fclose(fr);
        return -1;
    }
    else{
        while (read_line(fr, comm) != 1){           //pokud najde prikaz, provede pomocí tree, co je treba
            if (tree(curr_line, comm, fr) != 0){
                fclose(fr);
                return -1;
            }
        }
    }
    fclose(fr);
    return 1;
}
//pomocna zprava
void help(void){
    printf("Pokracujte spustenim programu s prave jednim argumentem - nazvem souboru s prikazy, napr: prikazy.txt. \nProgram slouzi jako proudovy editor textu. Pomoci ruznych prikazu radek po radku edituje vstupni text. \nSeznam prikazu: \niCONTENT(insert) - vlozi pred aktualni radek radek s obsahem CONTENT.\nbCONTENT (before) vlozi na zacatek aktualniho radku obsah CONTENT.\naCONTENT (after/append) vlozi na konec aktualniho radku obsah CONTENT.\nd (delete) - smaze aktualni radek.\ndN smaze N radku (N > 0).\nr (remove EOL) - na aktualnim radku odstrani znak konce radku.\ns PATTERN REPLACEMENT (substitute) na aktualnim radku provede substituci (nahrazeni) vyskytu retezce PATTERN za retezec REPLACEMENT. PATTERN musi byt neprazdny.\ns:PATTERN:REPLACEMENT - substituce retezce, pricemz PATTERN a REPLACEMENT mohou byt od sebe oddelene libovolnym tisknutelnym znakem.\nS:PATTERN:REPLACEMENT - stejne jako s:PATTERN:REPLACEMENT, ale nahrazuje vsechny vyskyty PATTERN.\nn (next) - posun aktualniho radku o jeden niz, tj. tiskne aktualni radek.\nnN - posun aktualniho radku o N niz. (N > 0).\nq (quit) - ukonci editaci, dalsi radky nezpracovava a ani netiskne.\ngX (goto) preskoc s aktualnim prikazem pro editaci na X-ty prikaz.\n");
    exit(0);
}
//kontrola argumentu
// argument - pocet argumentu
void check_argc(int argc){
    if (argc < 2) {
        fprintf(stderr, "Nezadali jste zadny argument.\n");
        help();
    }
    else if (argc > 2) {
        fprintf(stderr, "Zadali jste prilis mnoho argumentu.\n");
        help();
    }
}
//tiskne aktualni radek
void print_curr(const char *curr){
    printf("%s", curr);
}
//nacteni radku ze stdin
// argumenty - nazev stringu na ulozeni aktualniho radku
int get_line(char *c){
    char buffer[MAX];
    if((fgets(buffer, MAX, stdin)) != NULL){
    	for(unsigned int i = 0; i <= (strlen(buffer)); i++){
            c[i] = buffer[i];
    	}
    	return 0;
    }else{
        return 1;
    }
}
//nacteni radku ze souboru s prikazy
// argumenty - nazev souboru s prikazy a nazev stringu k ulozeni radku
int read_line(FILE *name, char *c){
    char buffer[MAX];
    if((fgets(buffer, MAX, name)) != NULL){
    	for(unsigned int i = 0; i <= strlen(buffer); i++){
            c[i] = buffer[i];
    	}
    	return 0;
    }else{
        return 1;
    }
}
//zjisti jaky prikaz provadet
// argumenty - nazev stringu s aktualnim radkem, nazev stringu s aktualnim prikazem, nazev souboru s prikazy
int tree(char *curr, char *comm, FILE *name){
    int sym = comm[0];
    switch (sym){
    case 'n':
        if (next(curr, comm) != 0)
            return 1;
        break;
    case 'q':
        return 1;
    case 'i':
        if (insert(comm) != 0)
            return -1;
        break;
    case 'b':
        if (before(curr, comm) != 0)
            return -1;
        break;
    case 'a':
        if (after(curr, comm) != 0)
            return -1;
        break;
    case 'd':
        if (delete_line(curr, comm) != 0)
            return 1;
        break;
    case 'r':
        remove_eol(curr);
        break;
    case 'g':
        if (go_to(comm, name) != 0)
            return -1;
        break;
    case 'S':
    case 's':
        if (S_replace(curr, comm) != 0)
            return -1;
        break;
    default:
        fprintf(stderr, "Prikaz %c: Nepodporovany prikaz. ", comm[0]);
        return -1;
    }
    return 0;
}
//odstrani urcity pocet radku
// argumenty - nazev stringu s aktualnim radkem, nazev stringu s aktualnim prikazem
int delete_line(char *curr, const char *comm){
    int number[3];
    if ((comm[1] - '0') >= 0 && (comm[1] - '0') < 10){
        unsigned int i = 1;
        while (i < strlen(comm)){
            if ((comm[i] - '0') >= 0 && (comm[i] - '0') < 10){
                number[i] = (comm[i] - '0');
            }else{
                break;
            }
            i++;
        }
        if (i > 2){
            int j = 0;
            while (j < ((number[1]*10) + number[2])){
                if (get_line(curr) != 0)
                    return 1;
                j++;
            }
        }else{
            int j = 0;
            while (j < number[1]){
                if (get_line(curr) != 0)
                    return 1;
                j++;
            }
        }
        }else{
            if (get_line(curr) != 0)
                return 1;
        }
    return 0;
}
//odstrani znak konce radku
// argument - nazev stringu s aktualnim radkem
void remove_eol(char *curr){
    int i = 0;
    while(curr[i] != '\0'){
        i++;
    }
    curr[i-1] = '\0';
}
//vlozi text za aktualni radek
// argumenty - nazev stringu s aktualnim radkem, nazev stringu s aktualnim prikazem
int after(char *curr, char *comm){
    if (strlen(comm) > 1){
        memmove(comm, comm+1, strlen(comm));
        int i = 0;
        while(curr[i] != '\0'){
            i++;
        }
        curr[i-1] = '\0';
        strcat(curr, comm);
    }else{
        fprintf(stderr, "Prikaz %c: Nenalezeno slovo k napsani. \n", comm[0]);
        return -1;
    }
    return 0;
}
//vlozi text pred aktualni radek
// argumenty - nazev stringu s aktualnim radkem, nazev stringu s aktualnim prikazem
int before(char *curr, char *comm){
    if (strlen(comm) > 1){
        memmove(comm, comm+1, strlen(comm));
        int i = 2;
        while(comm[i] != '\0'){
            i++;
        }
        comm[i-1] = '\0';
        strcat(comm, curr);
        for(unsigned int i = 0; i <= (strlen(comm)); i++){
            curr[i] = comm[i];
        }
    }else{
        fprintf(stderr, "Prikaz %c: Nenalezeno slovo k napsani. \n", comm[0]);
        return -1;
    }
    return 0;
}
//vlozi jiny radek pred aktualni radek
// argumenty - nazev stringu s aktualnim prikazem
int insert(const char *comm){
    if (strlen(comm) > 1){
        for(unsigned int i = 1; i < strlen(comm); i++){
            putchar(comm[i]);
        }
    }else{
        fprintf(stderr, "Prikaz %c: Nenalezeno slovo k napsani. \n", comm[0]);
        return -1;
    }
    return 0;
}
//spocita kolikrat zahodit radek a prejde na urcity radek
// argumenty - pocet desitek radku, pocet jednotek radku, nazev stringu s aktualnim prikazem, nazev souboru s prikazy
int goto_X(int a, int b, char *comm, FILE *name){
    rewind(name);
    int i = 1;
    while(i != ((a*10)+b)){
        if (read_line(name, comm) != 0){
            fprintf(stderr, "Prikaz g: Pozadovany radek neexistuje. ");
            return -1;
        }
        i++;
    }
    return 0;
}
//prejde na urcity radek
// argumenty - nazev stringu s aktualnim radkem, nazev stringu s aktualnim prikazem
int next(char *curr, const char *comm){
    int number[3];
    if ((comm[1] - '0') >= 0 && (comm[1] - '0') < 10){
        unsigned int i = 1;
        while (i < strlen(comm)){
            if ((comm[i] - '0') >= 0 && (comm[i] - '0') < 10){
                number[i] = (comm[i] - '0');
            }else{
                break;
            }
            i++;
        }
        if (i > 2){
            if (next_count(number[1], number[2], curr) != 0)
                return 1;
        }else{
            if (next_count(0, number[1], curr) != 0)
                return 1;
        }
    }else{
        if (next_count(0, 1, curr) != 0)
            return 1;
    }
    return 0;
}
//spocita kolikrat zmenit radek a provede to
// argumenty - pocet desitek radku, pocet jednotek radku, nazev stringu s aktualnim radkem
int next_count(int a, int b, char *curr){
    int j = 0;
    while (j < ((a*10) + b)){
        print_curr(curr);
        if (get_line(curr) != 0){
            return 1;
        }
        j++;
    }
    return 0;
}
//prejde na urcity radek
// argumenty - nazev stringu s aktualnim prikazem, nazev souboru s prikazy
int go_to(char *comm, FILE *name){
    int i = 1;
    int number[3];
        while(i <= 2){
            if ((comm[i] - '0') > 0 && (comm[i] - '0') < 10){
                number[i] = (comm[i] - '0');
            }else{
                break;
            }
            i++;
        }
        if (!((comm[1] - '0') > 0 && (comm[1] - '0') < 10)){
            fprintf(stderr, "Prikaz %c: Nenalezeno cislo radku. \n", comm[0]);
            return -1;
        }else if (i < 3){
            if (goto_X(0, number[1], comm, name) != 0)
                return -1;
        }else{
            if (goto_X(number[1], number[2], comm, name) != 0)
                return -1;
        }
    return 0;
}
//vymena patter za replacement v celem retezci
// argumenty - nazev stringu s aktualnim radkem, nazev stringu s aktualnim prikazem
int S_replace(char *curr, char *comm){
    char pattern[MAX];
    char replacement[MAX];
    const char spacer[2] = {comm[1]};
    char *token;
    token = strtok(comm, spacer);                               // zahodí první token (S)
    if ((token = strtok(NULL, spacer)) != NULL){                // importuje pattern
        for(unsigned int i = 0; i <= (strlen(token)); i++){
            pattern[i] = token[i];
    	}
    }else{
        fprintf(stderr, "Prikaz %c: Nenalezeno slovo k nahrazeni. \n", comm[0]);
        return -1;

    }
    if ((token = strtok(NULL, spacer)) != NULL){                // importuje replacement
        unsigned int i = 0;
        for(; i <= (strlen(token)); i++){
            replacement[i] = token[i];
    	}
    	replacement[i-2] = '\0';
    }else{
        fprintf(stderr, "Prikaz %c: Nenalezeno slovo k nahrazeni. \n", comm[0]);
        return -1;

    }
    if (comm[0] == 'S'){
        replace_words(curr, curr, pattern, replacement, -1);
    }else{
        replace_words(curr, curr, pattern, replacement, 1);
    }
    return 0;
}
//zmeni pattern za replacement
// argumenty - nazev stringu s aktualnim radkem, znovu pro kopii, nazev patternu, nazev replacementu, cislo pro rozeznani prikazu s a S
void replace_words(char *curr, const char *line, const char *oldW, const char *newW, int a){
	int i = 0;
	int newWlen = strlen(newW);
	int oldWlen = strlen(oldW);
    char result[MAX];
	while (*line){
		if (strstr(line, oldW) == line && ((a > 0) || (a < 0))){
			strcpy(&result[i], newW);
			i += newWlen;
			line += oldWlen;
			a--;
		}
		else
            result[i++] = *line++;
	}
	result[i] = '\0';
	for(unsigned int i = 0; i <= (strlen(result)); i++){
            curr[i] = result[i];
    	}
}
