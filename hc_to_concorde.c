/* 
 * creates a concord matrix style for concorde 
 * input is an edge list file output a weighted 
 * adjacency matrix 0 if edge exists and 1 
 * if no edge exists
 */

#include <stdio.h>

#include "hclib.h"

static void print_usage();

int main(int argc, char *argv[])
{

    /* read command line arguments */
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


    /* read graph file */
    int **graph;
    int node_count;
    int edge_count;
    graph = read_graph(graph_file, &node_count, &edge_count);

    printf("Graph has %d nodes and %d edges\n", node_count, edge_count);


    int i;
    int j;

    /* setup the output file */
    fp = fopen(out_file,"w+");
    if (!fp) {
        printf("Could not open file for writting %s\n", out_file);
        return 1;

    }

    /* convert 0 to 1 in adjacency matrix */
    for (i = 1; i <= node_count; i++) {
        for (j = 1; j <= node_count; j++) {
            if (j > 1) {
                fprintf(fp," ");
            } //if
            fprintf(fp,"%i", (graph[i][j] ? 0 : 1) );
        }
        fputs("\n",fp);
    }

    /* cleanup */
    if (fp) {
        fclose(fp);
    }

    free_graph(graph, &node_count);

    return 0;
}

static void print_usage()
{
    printf("Converts graphfile into concorde compatible matrix");

    printf("Usage:\n");
    printf("hc_to_dot graphfile.hcp dotfile.opt\n");
    
}
