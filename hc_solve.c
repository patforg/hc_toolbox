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
    int* solution;
    int i;
    graph = read_graph(graph_file, &node_count, &edge_count);

    printf("Graph has %d nodes and %d edges\n", node_count, edge_count);

    if (strcmp("backtrack", solver) == 0) {
        solution = backtrack(graph, &node_count, &edge_count);
    } else {
        printf("Unknown solver: %s", solver);
    }

    if (solution) {
        printf("Suggested path:");
        for (i = 1; i <= node_count; i++) {
            if (solution[i] > 0) {
                printf("%i", solution[i]);
            } //if
        } //for
        printf("\n");

        free(solution);
    } //if
    
    free_graph(graph, &node_count);

    return 0;
}

static void print_usage()
{
    printf("Checks if tour is a valid Hamiltonian Cycle for a gprah\n\n");

    printf("Usage:\n");
    printf("hc_check graphfile.hcp tourfile.tour\n");
    
}
