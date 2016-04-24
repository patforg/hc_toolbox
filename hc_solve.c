/* 
 *  Try to find a Hamiltonian Cycle 
 *  using one of the available solvers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hclib.h"
#include "solvers/backtrack.h"
#include "solvers/slh.h"

static void print_usage();

int main(int argc, char *argv[])
{

    char *solver;
    char *graph_file;
    char *out_file;
    FILE* fp;
    if (argc > 1)
    {
        solver = argv[1];
        graph_file = argv[2];
        if (argc > 2) {
            out_file = argv[3];
        } //if
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
    } else if (strcmp("slh", solver) == 0) {
        found_solution = slh(graph, &node_count, &edge_count, path);
    } else {
        printf("Unknown solver: %s", solver);
    }

    if (found_solution) {
        // print solution
        printf("Suggested path:");
        print_path(path, &node_count);

        // if valid write solution to file
        if (hc_validate(path, node_count, graph, node_count, true) && 
            out_file) {

            fp = fopen(out_file,"w+");
            if (!fp) {
                printf("Could not open file for writting %s\n", out_file);
            }  else {

                /*
                 * NAME : Envelope tour file
                 * COMMENT : Tour found
                 * TYPE : TOUR
                 * DIMENSION : 6
                 * TOUR_SECTION
                 */
                fprintf(fp, "NAME : Solution for %s\n",graph_file);
                fprintf(fp, "COMMENT : Tour found\n");
                fprintf(fp, "TYPE : TOUR\n");
                fprintf(fp, "DIMENSION : %i\n", node_count);
                fprintf(fp, "TOUR_SECTION");
                
                for (i = 0; i < node_count; i++) {
                    fprintf(fp,i % 10 == 0 ? "\n" : " ");
                    fprintf(fp,"%i", path[i]);
                } //for
                fprintf(fp,"\n-1\n");
                fprintf(fp,"EOF\n");

                if (fp) {
                    fclose(fp);
                }
            } //if
        } //if
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
    printf("Try to find Hamiltonian Tours in a graph using a solving algorithm\n\n");

    printf("Usage:\n");
    printf("hc_solve [ALG] [GRAPH_FILE] [TOUR_FILE]\n\n");
    printf("    [ALG] Alorithm to use for solving can be one of:\n");
    printf("        backtrack : simple backtracking algorithm\n");
    printf("        slh : Snakes and ladders heurisitic\n");
    printf("\n");
    printf("    [GRAPH_FILE] HCP graph file\n");
    printf("    [TOUR_FILE] HCP Tour file to write if a solution is found\n\n");
    
}
