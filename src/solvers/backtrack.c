#include <stdlib.h>

int* backtrack(int** graph, int* node_count, int* edge_count)
{
    int* path = malloc( *node_count * sizeof(int*));
    int i;

    // initialize all positions to zero
    for (i=0; i <= *node_count; i++) {
        path[i] = 0;
    } //for
    

    return path;
}
