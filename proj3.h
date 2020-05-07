#ifndef PROJ3_H_INCLUDED
#define PROJ3_H_INCLUDED

/**
 * @mainpage
 * Kostra hlavickoveho souboru 3. projekt IZP 2018/19
 * a pro dokumentaci Javadoc.
 *
 * @brief Jednoducha shlukova analyza: 2D nejblizsi soused. [Single linkage]\n
 * @author Daniel Patek [xpatek08@fit.vutbr.cz]
 * @date 12/2018
 */

 /**
 *   Struktura objektu:\n
        identifikator a souradnice
 */

struct obj_t {
    int id;
    float x;
    float y;
};
/**
 *   Shluk objektu:\n
 *      pocet objektu ve shluku\n
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli)\n
 *      ukazatel na pole shluku
 */
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};
/**

 * @defgroup clusters Prace s jednotlivymi shluky
 * @{

 */

/**
 * Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 * Ukazatel NULL u pole objektu znamena kapacitu 0.
 *
 * @param c Shluk k alokaci jeho objektu
 * @param cap Kapacita alokovaneho shluku
 * @pre c nesmi byt NULL.
 * @pre cap musi byt vetsi nez 0.
 * @post Shluk bude mit alokovanou pamet pro urcenou kapacitu cap.
 * @post V pripade chyby bude mit nulovou kapacitu.
*/
void init_cluster(struct cluster_t *c, int cap);

/**
 * Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 *
 * @param c Shluk pro dealokaci objektu
 * @post Dojde k uvolneni pameti pro cluster c.
 * @post Kapacita i velikost shluku bude nulova.
 */
void clear_cluster(struct cluster_t *c);

/// Chunk of cluster objects. Value recommended for reallocation.
extern const int CLUSTER_CHUNK;

/**
 * Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 *
 * @param c Shluk, kteremu budeme zvetsovat kapacitu
 * @param new_cap Nova kapacita shluku
 * @pre c nesmi byt NULL.
 * @pre new_cap musi byt vetsi nez 0.
 * @pre new_cap musi byt vetsi nez soucasna kapacita.
 * @post c bude mit zvetsenou kapacitu o new_cap.
 * @post V pripade chyby funkce vraci NULL.
 * @return Pointer na rozsireny shluk
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 * nevejde.
 *
 * @param c Shluk, do ktereho se prida objekt
 * @param obj Objekt k zacleneni do shluku
 * @post Na konec shluku c je vlozen objekt obj.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 * Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 * Shluk 'c2' bude nezmenen.
 *
 * @param c1 Prvni shluk na slouceni
 * @param c2 Druhy shluk na slouceni (a nasledne vymazani)
 * @pre Oba shluky nesmi byt NULL.
 * @post Objekty shluku c2 budou vlozeny na konec shluku c1.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
 *
 * @param c Shluk k serazeni jeho objektu
 * @post Objekty shluku budou serazeny vzestupne.
 */
void sort_cluster(struct cluster_t *c);

/**
 * Tisk shluku 'c' na stdout.
 *
 * @param c Shluk k vytisknuti
 */
void print_cluster(struct cluster_t *c);

 /**

 * @}

 */

 /**

 * @defgroup array Prace s polem shluku
 * @{

 */

 /**
 * Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 * (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 * pocet shluku v poli.
 *
 * @param carr Pole shluku, ve kterem chceme odstranit shluk
 * @param narr Velikost pole shluku
 * @param idx Pozice shluku v poli (index)
 * @pre idx musi byt mensi nez delka celeho pole narr.
 * @pre narr musi byt vetsi nez 0.
 * @post Pole bude mit o jednu mensi velikost.
 * @post Shluk na pozici idx bude vymazan.
 * @return Nova velikost pole shluku
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 *
 * @param o1 Prvni objekt
 * @param o2 Druhy objekt
 * @pre Oba objekty nesmi byt NULL.
 * @return Vzdalenost dvou objektu
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * Pocita vzdalenost dvou shluku.
 *
 * @param c1 Prvni shluk
 * @param c2 Druhy shluk
 * @pre Oba shluky nesmi byt NULL.
 * @pre Oba objekty musi mit velikost vetsi nez 0.
 * @return Minimalni vzdalenost dvou shluku
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

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
 * @post Indexy dvou nejblizsich shluku budou ulozeny v c1 a c2.
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Ze souboru `filename` nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 * jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr `arr`.
 *
 * @param filename Nazev souboru pro nacteni objektu
 * @param arr Ukazatel na pole shluku nactenych ze souboru (v pripade chyby bude ukazovat na NULL)
 * @pre arr nesmi byt NULL.
 * @pre filename musi obsahovat nazev souboru
 * @post Data ze souboru budou ulozeny po jednom shluku s jednim objektem v poli shluku arr.
 * @post V pripade chyby bude ukoncen program a vypsano chybove hlaseni.
 * @return Pocet nactenych shluku (v pripade chyby 0)
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 * Tiskne se prvnich 'narr' shluku.
 *
 * @param carr Ukazatel na prvni polozku (shluk)
 * @param narr Pocet shluku k vytisknuti
*/
void print_clusters(struct cluster_t *carr, int narr);

/**

 * @}

 */

#endif // PROJ3_H_INCLUDED
