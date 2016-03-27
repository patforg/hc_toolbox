/* 
 *  Try to find a Hamiltonian Cycle 
 *  using one of the available solvers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hclib.h"
#include "solvers/backtrack.h"

static void print_usage();

int main(int argc, char *argv[])
{

    char *solver;
    char *graph_file;
    if (argc > 1)
    {
        solver = argv[1];
        graph_file = argv[2];
    } else
    {
        print_usage();
        return 1;
    }

    int **graph;
    int node_count;
    int edge_count;
    bool found_solution = false;
    int* path;
    int i;
    graph = read_graph(graph_file, &node_count, &edge_count);

    printf("Graph has %d nodes and %d edges\n", node_count, edge_count);
    path = malloc( node_count * sizeof(int*));

    if (strcmp("backtrack", solver) == 0) {
        found_solution = backtrack(graph, &node_count, &edge_count, path);
    } else {
        printf("Unknown solver: %s", solver);
    }

    if (found_solution) {
        printf("Suggested path:");
        for (i = 0; i < node_count; i++) {
            if (path[i] > 0) {
                if (i > 0) {
                    printf("-");
                } //if
                printf("%i", path[i]);
            } //if
        } //for
        printf("\n");

        hc_validate(path, node_count, graph, node_count, true); 
    } else
    {
        printf("No solution could be found \n");
    } //if
    
    free(path);
    free_graph(graph, &node_count);

    return 0;
}

static void print_usage()
{
    printf("Checks if tour is a valid Hamiltonian Cycle for a gprah\n\n");

    printf("Usage:\n");
    printf("hc_check graphfile.hcp tourfile.tour\n");
    
}
