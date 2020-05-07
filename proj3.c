/**
 * Kostra programu pro 3. projekt IZP 2018/19
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TOO'
 *
 */

/**
 * Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 * Ukazatel NULL u pole objektu znamena kapacitu 0.
 *
 * @param c Shluk k alokaci jeho objektu
 * @param cap Kapacita alokovaneho shluku
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c);
    assert(cap >= 0);

    //velikost clusteru bude nulova
    c->size = 0;

    //kontrola spravne hodnoty kapacity
    if (cap > 0) {

        //alokace pozadovaneho mista objektu a kontrola alokace
        if ((c->obj = malloc(cap * sizeof(struct obj_t)))) {
            c->capacity = cap;
            return;
        }
    }

    //v pripade spatne hodnoty kapacity
    c->capacity = 0;
    c->obj = NULL;
}

/**
 * Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 *
 * @param c Shluk pro dealokaci objektu
 */
void clear_cluster(struct cluster_t *c)
{
    //dealokace mista, nastaveni hodnot na nuly
    free(c->obj);
    c->size = 0;
    c->capacity = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/**
 * Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 *
 * @param c Shluk, kteremu budeme zvetsovat kapacitu
 * @param new_cap Nova kapacita shluku
 * @return Pointer na rozsireny shluk
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    //kontrola kapacity
    if (c->capacity >= new_cap)
        return c;

    //nova velikost
    size_t size = sizeof(struct obj_t) * new_cap;

    //realokace mista a kontrola
    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    //prirazeni objektu do nove alokovaneho mista
    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/**
 * Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 * nevejde.
 *
 * @param c Shluk, do ktereho se prida objekt
 * @param obj Objekt k zacleneni do shluku
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    //kontrola pozadovane kapacity
    if (c->size >= c->capacity){

        //rozsireni shluku
        resize_cluster(c, c->capacity + CLUSTER_CHUNK);
    }

    //pripojeni objektu ke shluku
    c->obj[c->size++] = obj;
}

/**
 * Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/**
 * Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 * Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 * Shluk 'c2' bude nezmenen.
 *
 * @param c1 Prvni shluk na slouceni
 * @param c2 Druhy shluk na slouceni (a nasledne vymazani)
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    //pridani vsech objektu shluku 2 do shluku 1
    for (int i = 0; i < c2->size; i++){
        append_cluster(c1, c2->obj[i]);
    }

    //serazeni noveho shluku
    sort_cluster(c1);
}

/**
 * Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 * (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 * pocet shluku v poli.
 *
 * @param carr Pole shluku, ve kterem chceme odstranit shluk
 * @param narr Velikost pole shluku
 * @param idx Pozice shluku v poli (index)
 * @return Nova velikost pole shluku
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    //nova velikost pole
    int new_size = narr - 1;

    //vymazani shluku
    clear_cluster(&carr[idx]);

    //presunuti pole o jeden prvek dopredu
    for (int i = idx; i < new_size; i++){
        carr[i] = carr[i + 1];
    }
    return new_size;
}

/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 *
 * @param o1 Prvni objekt
 * @param o2 Druhy objekt
 * @return Vzdalenost dvou objektu
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    //vypocet vzdalenosti dvou objektu
    return sqrtf(powf(o1->x - o2->x, 2.0) + powf(o1->y - o2->y, 2.0));
}

/**
 * Pocita vzdalenost dvou shluku.
 *
 * @param c1 Prvni shluk
 * @param c2 Druhy shluk
 * @return Minimalni vzdalenost dvou shluku
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    //deklarace promennych (min_distance se bude postupne blizit nule)
    float distance, min_distance = +INFINITY;

    //pocita se vzdycky pro dva objekty ze dvou ruznych shluku
    for (int i = 0; i < c1->size; i++){
        for (int j = 0; j < c2->size; j++){

            //vypocet velikosti
            distance = obj_distance(&c1->obj[i], &c2->obj[j]);

            //pokud je velikost zatim nejmensi
            if (distance < min_distance){
                min_distance = distance;
            }
        }
    }
    return min_distance;
}

/**
 * Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 * hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 * 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 * adresu 'c1' resp. 'c2'.
 *
 * @param carr Pole shluku
 * @param narr Velikost pole shluku
 * @param c1 Pointer na prvni nalezeny sousedni shluk
 * @param c2 Pointer na druhy nalezeny sousedni shluk
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    //deklarace promennych (min_distance se bude postupne blizit nule)
    float distance, min_distance = +INFINITY;

    //pocita se vzdy pro dva ruzne shluky
    for (int i = 0; i < narr; i++){
        for (int j = i + 1; j < narr; j++){

            //vypocet nejmensi velikosti dvou shluku
            distance = cluster_distance(&carr[i], &carr[j]);

            //pokud se jedna o zatim nejmensi velikost
            if (distance < min_distance){
                min_distance = distance;
                *c1 = i;
                *c2 = j;
            }
        }
    }

}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/**
 * Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
 *
 * @param c Shluk k serazeni jeho objektu
 */
void sort_cluster(struct cluster_t *c)
{
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
 * Tisk shluku 'c' na stdout.
 *
 * @param c Shluk k vytisknuti
 */
void print_cluster(struct cluster_t *c)
{
    // vytiskne vsechny objekty ve shluku na jeden radek
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/**
 * Ze souboru `filename` nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr `arr`.
 *
 * @param filename Nazev souboru pro nacteni objektu
 * @param arr Ukazatel na pole shluku nactenych ze souboru (v pripade chyby bude ukazovat na NULL)
 * @return Pocet nactenych shluku (v pripade chyby 0)
 */
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    //otevreni souboru
    FILE *file;
    file = fopen(filename, "r");

    //kontrola otevreni souboru
    if (file == NULL){
        *arr = NULL;
        fprintf(stderr, "Nebylo mozne otevrit soubor: %s\n", filename);
        return 0;
    }

    //deklarace retezce jako aktualniho radku
    char s_counter[20];

    //nacteni prvniho radku
    if (fgets(s_counter, 20, file) == NULL) {
        *arr = NULL;
        fprintf(stderr, "Nebylo mozne nacist radek ze souboru.\n");
        return 0;
    }

    //kontrola prvniho radku (zda obsahuje '=')
    char *pointer_2_int;
    if((pointer_2_int = strstr(s_counter, "=")) != NULL){

        //nacteni poctu shluku v souboru
        unsigned int counter = strtoul(++pointer_2_int, NULL, 10);

        //alokace mista pro pole shluku, deklarace pomocnych promennych a pomocneho objektu temp1
        struct obj_t temp1;
        float x_pos, y_pos;
        int id;
        *arr = malloc(counter * sizeof(struct cluster_t));

        //kontrola alokace mista pro pole shluku
        if (*arr == NULL){
            fprintf(stderr, "Nebylo mozne alokovat misto pro pole shluku.\n");
            fclose(file);
            return 0;
        }

        //nacteni vsech shluku do pole shluku (kazdy radek reprezentuje jeden shluk)
        for (unsigned int i = 0; i < counter; i++) {

            //inicializace clusteru
            init_cluster(&(*arr)[i], 1);

            //kontrola uspesneho prideleni mista pro dalsi cluster
            if ((*arr)[i].capacity == 0){
                fprintf(stderr, "Nebylo mozne alokovat dalsi misto pro shluk v poli shluku.\n");
                for (unsigned int j = 0; j < i; j++){
                    clear_cluster(&(*arr)[j]);
                }
                fclose(file);
                return 0;
            }

            //nacteni a kontrola nacteni dalsiho radku ze souboru
            if (fgets(s_counter, 20, file) == NULL){
                fprintf(stderr, "Nebylo mozne nacist dalsi (konkretne %d.) radek radek ze souboru.\n", i + 1);
                for (unsigned int j = 0; j <= i; j++){
                    clear_cluster(&(*arr)[j]);
                }
                *arr = NULL;
                fclose(file);
                return 0;
            }

            //extrakce dat z nacteneho radku
            if (sscanf(s_counter, "%d %f %f", &id, &x_pos, &y_pos) < 3){
                fprintf(stderr, "Chyba ve vstupnim radku objektu (radek cislo %d).\n", i + 1);
                for (unsigned int j = 0; j <= i; j++){
                    clear_cluster(&(*arr)[j]);
                }
                *arr = NULL;
                fclose(file);
                return 0;
            };

            //kontrola jedinecnosti ID objektu
            for (unsigned int j = 0; j < i; j++){
                if (id == (*arr)[j].obj->id){
                    fprintf(stderr, "Kolize v ID vice objektu. Shluk cislo %d koliduje se shlukem cislo %d.\n", j + 1, i + 1);
                    for (unsigned int k = 0; k <= i; k++){
                        clear_cluster(&(*arr)[k]);
                    }
                    *arr = NULL;
                    fclose(file);
                    return 0;
                }

            }

            //kontrola souradnic
            if (x_pos < 0 || x_pos > 1000 || y_pos < 0 || y_pos > 1000){
                fprintf(stderr, "Zadali jste spatne souradnice. Shluk cislo: %d\n", i + 1);
                for (unsigned int j = 0; j <= i; j++){
                    clear_cluster(&(*arr)[j]);
                }
                *arr = NULL;
                fclose(file);
                return 0;
            }

            //predani nactenych dat pomocnemu objektu
            temp1.id = id;
            temp1.x = x_pos;
            temp1.y = y_pos;

            //vlozeni objektu do clusteru
            append_cluster(&(*arr)[i], temp1);
        }

        //uvolneni pameti a ukonceni
        fclose(file);
        return counter;

    } else {

        //v pripade spatneho prvniho radku
        *arr = NULL;
        fprintf(stderr, "Ve zvolenem souboru chybi pocet shluku na prvnim radku.\n");
        fclose(file);
        return 0;
    }
}

/**
 * Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 * Tiskne se prvnich 'narr' shluku.
 *
 * @param carr Ukazatel na prvni polozku (shluk)
 * @param narr Pocet shluku k vytisknuti
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    //vytiskne kazdy shluk zvlast
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    //kontrola poctu argumentu
    if (argc <= 1){
        fprintf(stderr, "Nebyl zadan zadny argument.\n");
        return  EXIT_FAILURE;
    }
    if (argc > 3){
        fprintf(stderr, "Bylo zadano prilis mnoho argumentu.\n");
        return  EXIT_FAILURE;
    }

    //deklarace pole shluku a jeho velikosti, nacteni pole po jednom shluku
    struct cluster_t *clusters;
    unsigned int size = (unsigned int) load_clusters(argv[1], &clusters);

    //kontrola pole shluku
    if (clusters == NULL || size == 0){

        //uvolneni pameti
        free(clusters);
        fprintf(stderr, "Nebylo mozne nacist shluky ze souboru.\n");
        return  EXIT_FAILURE;
    }

    //nacteni druheho argumentu
    unsigned int aimed_size;
    if (argc == 3) {

        //prevod do unsigned int
        aimed_size = strtoul(argv[2], NULL, 10);

        //kontrola desetinneho cisla
        char *check_pointer = strchr(argv[2], '.');

        //kontrola spravnosti druheho argumentu
        if (argv[2][0] == '-' || aimed_size == 0 || check_pointer != NULL){

            //uvolneni pameti
            for (unsigned int i = 0; i < size; i++){
                clear_cluster(&clusters[i]);
            }
            free(clusters);
            fprintf(stderr, "Druhy argument je ve spatnem formatu.\n"
                            "Cislo musi byt ve formatu INT (cele cislo) a musi byt vetsi nez 0.\n");

            return EXIT_FAILURE;
        }
    } else{

        // vychozi hodnota 1
        aimed_size = 1;
    }

    //shlukovani pole na pozadovanou velikost
    while (aimed_size < size){
        int num_of_clust_1, num_of_clust_2;
        find_neighbours(clusters, size, &num_of_clust_1, &num_of_clust_2);
        merge_clusters(&clusters[num_of_clust_1], &clusters[num_of_clust_2]);
        size = remove_cluster(clusters, size, num_of_clust_2);
    }

    //tisk pole na stdout
    print_clusters(clusters, size);

    //uvolneni pameti
    for (unsigned int i = 0; i < size; i++){
        clear_cluster(&clusters[i]);
    }
    free(clusters);

    return EXIT_SUCCESS;
}