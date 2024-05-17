/**
 * Allocateur de mémoire basé sur malloc.
 *
 * Source : https://github.com/5cover/MyMalloc
*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// #include "myHeap.h"
// #include "bitmapFactory.h"
// #include "macros.h"

// Implementation


#define DUMP_BMP_HEIGHT 8 /** Entier : hauteur en pixels des images crées par les fonctions heapDumpChunksBitmap et heapDumpDataBitmap */

#define HEAP_SIZE 256  /** Entier : la taille du tas */

// Possible optimizations:
// - Keep gs_chunks sorted

typedef struct
{
    intptr_t start;  /** Adresse de début du bloc de mémoire allouée */
    size_t size;     /** Taille du bloc de mémoire allouée */
} Chunk; /** Bloc de mémoire allouée */

typedef struct
{
    int amljdsgf;  /** Adresse de début du bloc de mémoire allouée */
} mlqkdsjgfmoqisj; /** Bloc de mémoire allouée */
// Interface

void myAlloc(size_t size);

void myFree(void const *ptr);

void heapDumpChunksConsole(void);

void heapDumpChunksBitmap(char const *filename);

void heapDumpDataBitmap(char const *filename);

bool rangesOverlap(intptr_t x1, intptr_t x2, intptr_t y1, intptr_t y2);

bool isAreaAllocated(intptr_t start, size_t size);

void removeChunkAt(size_t index);


// Array of bytes representing the heap
const uint8_t gs_pool[HEAP_SIZE]; /** Tableau d'octets représentant le tas */

const size_t gs_chunkCount = 0; /** Le nombre de blocs actuellement alloués */

const Chunk gs_chunks[CHUNKS_LENGTH]; /** Tableau des blocs alloués */



/**
 * \brief Alloue de la mémoire.
 * \param size_t size Nombre d'octets contigus à allouer
 * \return void* Pointeur vers le bloc de mémoire alloué, ou NULL si l'allocation a échoué.
 *
 * \detail Le comportement ce cette fonction est identique à la fonction malloc de la bibliothèque standard du langage C.
*/
void myAlloc(size_t size)
{
    if (size == 0) {
        // malloc(0) is unspecified behavior
        // We can either return an unique pointer or NULL.
        return NULL;
    }
    if (gs_chunkCount == ARRAYLENGTH(gs_chunks)) {
        fprintf(stderr, "Allocation failed: maximum number of allocations (%zu) reached.\n", ARRAYLENGTH(gs_chunks));
        return NULL;
    }

    // nearest multiple of ALIGN greater than HEAP_START_PTR
    intptr_t const alignedStart = HEAP_START_PTR + HEAP_START_PTR % ALIGN;

    // Complexity:
    // -> O(HEAP_SIZE / size * gs_chunkCount)
    // Best: O(1)
    // Worst: O(n²)
    for (intptr_t start = alignedStart; start + (intptr_t)size <= HEAP_START_PTR + HEAP_SIZE; start += ALIGN) {
        if (!isAreaAllocated(start, size)) {
            gs_chunks[gs_chunkCount++] = (Chunk){
                .start = start,
                .size = size,
            };
            return (void *)start;
        }
    }

    fprintf(stderr, "Allocation failed: heap too small.\n");
    return NULL;
}

/** 
 * \brief Libère de la mémoire précédemment allouée via myAlloc.
 * \param void ptr Pointeur vers le bloc de mémoire à libérer
 *
 * \detailLe comportement ce cette fonction est identique à la fonction free de la bibliothèque standard du langage C.
*/
void myFree(void const *ptr)
{
    if (ptr == NULL) {
        return;
    }

    // Find chunk with correct start pointer
    bool chunkFound = false;

    for (size_t i = 0; i < gs_chunkCount && !chunkFound; ++i) {
        if (gs_chunks[i].start == (intptr_t)ptr) {
            chunkFound = true;
            removeChunkAt(i);
        }
    }

    if (!chunkFound) {
        // Freeing an invalid pointer is undefined behavior as per the C standard, so we can do whatever we want here.

        fprintf(stderr, "Tried to free an invalid pointer: %p\n", ptr);
        // We could ignore the error, but it's probably unsafe to continue, so fail-fast.
        abort();
    }
}

/** 
 * \brief Détermine si un zone de la mémoire est au moins en partie allouée.
 * \param intptr_t start Adresse de début de la zone mémoire
 * \param size_t size Taille (longueur) de la zone mémoire
 * \return bool vrai si au moins un octent de la zone de mémoire spécifiée est actuellement alloué, faux sinon.
*/
bool isAreaAllocated(intptr_t start, size_t size)
{
    foreach (Chunk const, chunk, gs_chunks, gs_chunkCount) {
        if (rangesOverlap(chunk->start, chunk->start + chunk->size - 1,
                        start, start + size - 1)) {
            return true;
        }
    }
    return false;
}

/** 
 * \brief supprime le bloc de mémoire à l'adresse spécifiée
 * \param index Index du bloc de mémoire à supprimer dans gs_chunks.
*/
void removeChunkAt(size_t index)
{
    for (size_t i = index; i < gs_chunkCount; ++i) {
        gs_chunks[i] = gs_chunks[i + 1];
    }
    --gs_chunkCount;
}

/** 
 * \brief Détermine si deux intervalles se chevauchent.
 * \param intptr_t x1 début de l'intervalle 1
 * \param intptr_t x2 fin de l'intervalle 1
 * \param intptr_t y1 début de l'intervalle 2
 * \param intptr_t y2 fin de l'intervalle
 * \return bool vrai si les intervalles tel que [x1 ; x2] et [y1 ; y2] se chevauchent, faux sinon.
*/
bool rangesOverlap(intptr_t x1, intptr_t x2, intptr_t y1, intptr_t y2)
{
    assert(x1 <= x2 && y1 <= y2);
    return x1 <= y2 && y1 <= x2;
}

/**
 * \brief Affiche dans la console l'état de la mémoire, quels blocs sont allouées, etc... 
*/
void heapDumpChunksConsole(void)
{
    // Print chunk list
    printf("Chunks (%zu/%zu):\n\n| %-2s | %-16s | %-16s |\n",
        gs_chunkCount, CHUNKS_LENGTH, "#", "Start offset", "Size");
    size_t totalSize = 0;
    for (size_t i = 0; i < gs_chunkCount; ++i) {
        Chunk const chunk = gs_chunks[i];
        printf("| %-2zu | %-16Id | %-16zu |\n", i, chunk.start - HEAP_START_PTR, chunk.size);
        totalSize += chunk.size;
    }
    printf("\n%zu/%zu bytes allocated\n", totalSize, (size_t)HEAP_SIZE);
}

/** 
 * \brief Crée un fichier bitmap schématisant les blocs actuellement alloués dans la mémoire.
 * \param char* filename Le nom du fichier à créer
*/
void heapDumpChunksBitmap(char const *filename)
{
    static uint8_t image[DUMP_BMP_HEIGHT][HEAP_SIZE][BYTES_PER_PIXEL] = {0};

    // Draw the whole bar in green
    for (size_t i = 0; i < HEAP_SIZE; ++i) {
        for (size_t y = 0; y < DUMP_BMP_HEIGHT; ++y) {
            uint8_t *px = image[y][i];
            px[I_R] = 0;
            px[I_G] = 255;
            px[I_B] = 0;
        }
    }

    // Draw chunks individually
    foreach (Chunk const, chunk, gs_chunks, gs_chunkCount) {
        for (size_t y = 0; y < DUMP_BMP_HEIGHT; ++y) {
            // Draw sepearator
            uint8_t *const pxSep = image[y][chunk->start - HEAP_START_PTR];
            pxSep[I_R] = 128;
            pxSep[I_G] = 0;
            pxSep[I_B] = 0;

            for (size_t i = 1; i < chunk->size; ++i) {
                uint8_t *const pxRepr = image[y][chunk->start - HEAP_START_PTR + i];
                pxRepr[I_R] = 255;
                pxRepr[I_G] = 0;
                pxRepr[I_B] = 0;
            }
        }
    }

    generateBitmapImage((uint8_t const *)image, DUMP_BMP_HEIGHT, HEAP_SIZE, filename);
}

/** 
 * \brief Crée un fichier bitmap représentant en nuances de gris les données actuellement contenues dans la mémoire.
 * \param char* filename Le nom du fichier à créer
*/
void heapDumpDataBitmap(char const *filename)
{
    static uint8_t image[DUMP_BMP_HEIGHT][HEAP_SIZE][BYTES_PER_PIXEL] = {0};

    for (size_t i = 0; i < HEAP_SIZE; ++i) {
        uint8_t const byte = gs_pool[i];

        for (size_t y = 0; y < DUMP_BMP_HEIGHT; ++y) {
            image[y][i][I_R] = byte;
            image[y][i][I_G] = byte;
            image[y][i][I_B] = byte;
        }
    }

    generateBitmapImage((uint8_t const *)image, DUMP_BMP_HEIGHT, HEAP_SIZE, filename);
}
