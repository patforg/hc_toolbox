/* 
 * creates a dot file for graphviz lib
 * input is an edge list file 
 * out is a dot file
 */

#include <stdio.h>

#include "hclib.h"

static void print_usage();

int main(int argc, char *argv[])
{

    char *graph_file;
    char *out_file;
    FILE * fp;
    if (argc > 1)
    {
        graph_file = argv[1];
        out_file = argv[2];
    } else
    {
        print_usage();
        return 1;
    }


    int **graph;
    int node_count;
    int edge_count;
    graph = read_graph(graph_file, &node_count, &edge_count);

    printf("Graph has %d nodes and %d edges\n", node_count, edge_count);

    int i;
    int j;

    fp = fopen(out_file,"w+");
    if (!fp) {
        printf("Could not open file for writting %s\n", out_file);
        return 1;

    }

    fputs("graph G {\n",fp);
    for (i = 1; i <= node_count; i++) {
        for (j = i+1; j <= node_count; j++) {
            if (graph[i][j] == 1) {
                fprintf(fp,"    %i -- %i;\n",i,j);
            }
        }
    }
    fputs("}",fp);

    if (fp) {
        fclose(fp);
    }

    free_graph(graph, &node_count);

    return 0;
}

static void print_usage()
{
    printf("Converts an hcp file to a doc file\n\n");

    printf("Usage:\n");
    printf("hc_to_dot graphfile.hcp dotfile.opt\n");
    
}
