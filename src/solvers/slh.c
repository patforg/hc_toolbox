#include <stdio.h>
#include <stdlib.h>
#include "hclib.h"
#include "solvers/slh/slhlib.h"

static int* g_node_count;
static int* g_path;
static int* g_node_index;
static int** g_graph;

static void stage0();
static void stage1();
static void stage2();
static void stage3();
static void float2();
static void float5();

static int flo1(int* gap);
static int flo2(int* gap);
static int flo3(int* gap);
static int flo4(int* gap);
static int flo5(int* gap);


bool slh(int** graph, int* node_count, int* edge_count, int* path)
{
    int gap_count;

    /* set global vars */
    g_node_count = node_count;
    g_path = path;
    g_graph = graph;

    slhlib_init(node_count,path,graph);
    //stage0(graph, node_count, path);
    stage1(graph, node_count, path);
    /*
    init();
    stage1();
    */

    printf("Final path:");
    print_path(path, node_count);
    gap_count = count_gaps();
    printf("Left with  : %i gaps\n", gap_count);
    return gap_count == 0 ? true : false ;
}


/*************************************************************
 *                STAGE 0: Closing Transformations           *
 *************************************************************/
void stage0(int** graph, int* node_count, int* path) 
{
    int cur_node;
    int next_node;
    int prec_node;
    int inner_cur_node;
    int inner_next_node;
    int inner_prec_node;
    int inner_next_next_node;

    for (int i=0; i < *node_count; i++)
    {
        cur_node = path[i];
        next_node = get_next_node(&cur_node);
        prec_node = get_prec_node(&cur_node);

        // found a gap
        if (graph[cur_node][next_node] == 0) {

            //printf("Found gap between %i and %i\n", cur_node, next_node);

            for (int j=0; j < *node_count; j++) {
                inner_cur_node = j+1;

                // don't check nodes involved in current gap
                if (inner_cur_node == cur_node 
                    || inner_cur_node == next_node) {
                    continue;
                } //if

                inner_next_node = get_next_node( &inner_cur_node );
                inner_prec_node = get_prec_node( &inner_cur_node );

                // don't check nodes involved in current gap
                if (inner_next_node == cur_node 
                    || inner_next_node == next_node) {
                    continue;
                } //if

                //printf("Processing %i, next node is %i, preceding node is %i\n", inner_cur_node, inner_next_node, inner_prec_node);

                // check edge that can be used to close the gap
                // closing 2-opt type 1 transformation
                if (graph[next_node][inner_next_node] == 1 
                    && graph[inner_cur_node][inner_next_node] == 0) {

                    swap_nodes( inner_next_node, cur_node);
                    i = 0; // reset i
                    //printf("Closing 2-opt type 1 between nodes %i and %i\n", inner_cur_node, inner_next_node);
                    //print_path(path, node_count);
                    break;
                } //if

                // check edge that can be used to close the gap
                // closing 2-opt type 2 transformation
                if (graph[cur_node][inner_cur_node] == 1 
                    && graph[inner_next_node][next_node] == 1) {

                    swap_nodes( inner_next_node, cur_node);
                    i = 0; // reset i
                    //printf("Closing 2-opt type 2 between nodes %i and %i\n", cur_node, next_node);
                    //print_path(path, node_count);
                    break;

                } //if

                // check edge that can be used to close the gap
                // closing 3-opt type 2 transformation
                if (graph[next_node][inner_cur_node] == 1 ) {

                    inner_next_node = ladder_search(inner_cur_node, cur_node, cur_node);

                    while (inner_next_node != 0) {
                        inner_next_next_node = get_next_node( &inner_next_node );

                        if (inner_next_next_node != cur_node
                            && graph[inner_next_next_node][inner_prec_node]) {
                            
                            printf("Closing 3-opt between nodes %i and %i\n", cur_node, next_node);
                            printf("Swapped %i, %i and %i, %i\n", next_node, inner_prec_node, inner_next_next_node, cur_node);
                            print_path(path, node_count);
                            swap_nodes( next_node, inner_prec_node);
                            swap_nodes( inner_next_next_node, cur_node);

                            print_path(path, node_count);

                            inner_next_node = 0; // stop looking for next nodes
                            i = 0; // reset i
                        } else {
                            inner_next_node = ladder_search(inner_next_node, cur_node, cur_node);
                        } //if
                    } //while

                } //if

            } //for 

            //printf("Left with  : %i gaps\n", count_gaps());
        } //if
    } //for

}

void stage1(int** graph, int* node_count, int* path) 
{
		float5();
		/* commented out for testing
    
    int** gap_list;
    int** ordering_list;
    int min_gap_count = 0;
    int y,x;
    int found = 0;
    int* gap;
    int (*flo_functions[5])(int* gap);
    
    flo_functions[0] = flo1;
    flo_functions[1] = flo2;
    flo_functions[2] = flo3;
    flo_functions[3] = flo4;
    flo_functions[4] = flo5;
    
    gap = malloc( 2 * sizeof(int*));
    
    for (int i=0; i < *node_count; i++)
    {
        y = path[i];
        x = get_next_node(&y);

        // found a gap
        if (graph[y][x] == 0) {
            gap[0] = y;
            gap[1] = x;
            // TODO:  save g in gap list

            // try each flo function until one
            // can perform a transformation (returns 1)
            for (int j=0; j < 5; j++) {
                if ( (*flo_functions[j])(gap) == 1) {
                    found = 1;
                    printf("Found transformation in flo%i\n", j);
                    break;
                } //if
            } //for

            // TODO:
            // 1.1 a) check that new ordering has a new gap not in gap_list (goto 1.2)
            // 1.1 b) no new gap found revert to prev ordering and try another transformation
            // 1.1 c) no transformation can be done goto stage 2
            // 1.2 if transformation reduced number of gaps clear gaps and ordering. If gaps is 0 stop
            // 1.3 otherwise goto 1.1

            if (found == 1) {
                // exit loop (for now)
                // as soon as we found a
                // valid flo pattern
                break;
            }  //if
        } //if
    } //for

    free(gap);
	*/
}

void stage2(int** graph, int* node_count, int* path) 
{

}



void stage3(int** graph, int* node_count, int* path) 
{

}

int flo1(int* gap)
{
    int x,y,a,b,c,d;
    int prec_node;

    y = gap[0];
    x = gap[1];

    for (int i = 0; i < *g_node_count; i++) {

        a = g_path[i];
        c = get_prec_node(&a);

        // look for a edge between x and a
        if (g_graph[x][a] == 1) {        

            // from a find an edge b connected to a
            // or has a path to a and is connected to y
            // or if the following node d is connected to 
            // c node
            prec_node = a;
            b = get_next_node(&a);
            while (g_graph[prec_node][b] == 1 && b != y) {
                
                d = get_next_node(&b);
                if (g_graph[b][y] == 1
                    || g_graph[d][c] ==1) {
                    swap_nodes(d,y);
                    swap_nodes(x,c);
                    return 1;
                } //if

                // move to next node
                prec_node = b;
                b = get_next_node(&b);

            } //while
        } //if
        
    } //for

    return 0;
}

void float2()
{
		//I added a not_found flag to stop when a pattern has been found
		bool not_found = true;
    for (int i=0; i < *g_node_count && not_found; i++)
    {
		//the name of the nodes, not the index --maxime
        int y = g_path[i];
        int x = get_next_node(&y);

        // found a gap
        if (g_graph[y][x] == 1) continue;

		//printf("Found gap between %i and %i\n", cur_node, next_node);

		//can't be beside x because c must be in between
		//int a = get_next_node(&x);
		int a = get_next_node(&x);
		while(not_found) {
				a = ladder_search(a, y, x);
				if (a == 0) break;
				int c = get_prec_node(&a);
						int b = a;
						//d can't be beside a because b must be in between
						int d = get_next_node(&a);
						while(not_found){
								//b can't be beside y because d must be in between
								b = ladder_search(b, get_prec_node(&y), y);
								if (b == 0) break;
								d = get_next_node(&b);
								not_found = false;
						}
						while(not_found){
								//can't be beside y because d must be in between
								d = ladder_search(d, y, c);
								if (d == 0) break;
								b = get_prec_node(&d);
								not_found = false;
						}
						if(!not_found){
								swap_nodes(x, c);
								swap_nodes(d, y);
								swap_nodes(c,d);
								printf("y = %i, x = %i, a = %i, b = %i, c = %i, d = %i \n", y, x, a, b, c, d );
						}

		}
	} //for
}

int flo2(int* gap)
{
    return 0;
}

int flo3(int* gap)
{
    return 0;
}

int flo4(int* gap)
{
    return 0;
}

void float5()
{
		//I added a not_found flag to stop when a pattern has been found
		bool not_found = true;
    for (int i=0; i < *g_node_count && not_found; i++)
    {
			printf("test1");
		//the name of the nodes, not the index --maxime
        int y = g_path[i];
        int x = get_next_node(&y);

        // found a gap
        if (g_graph[y][x] == 1) continue;

		//printf("Found gap between %i and %i\n", cur_node, next_node);

		//can't be beside x because c must be in between
		//int a = get_next_node(&x);
		int a = get_next_node(&x);
		while(not_found) {
				a = ladder_search(a, y, x);
				if (a == 0) break;
				int f = get_next_node(&a);
				//c is temporary for now
				int c = get_prec_node(&a);
				int e = x;
				while(not_found){
						e = ladder_search(e, c, f);
						if (e == 0) break;
						int c = get_next_node(&e);
						//g b, h and j are temporary for now
						int g = get_next_node(&f);
						int b = get_next_node(&g);
						int j = get_next_node(&b);
						int h = get_prec_node(&y);

						int d = j;
						while(not_found){
								d = ladder_search(d, h, c);
								if (d == 0) break;
								h = get_next_node(&d);
								j = get_prec_node(&d);

								int b = g;
								while(not_found){
										b = ladder_search(b, j, y);
										if (b == 0) break;
										g = get_prec_node(&b);
										not_found = false;
										printf("y = %i, x = %i, e = %i, c = %i, a = %i, f = %i, g = %i, b = %i, j = %i, d = %i, h = %i \n", y, x, e, c, a, f, g, b, j, d, h);


								}
						}

				}

		}
	} //for
}

int flo5(int* gap)
{
    return 0;
}

