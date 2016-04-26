#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "hclib.h"

static int* g_node_count;
static int* g_path;
static int* g_node_index;
static int** g_graph;

static void init();
static void stage0();
static void stage1();
static void stage2();
static void stage3();
static void float2();
static void float5();

static int get_prec_node(int* cur_node);
static int get_next_node(int* cur_node);
static void swap_nodes(int node_a, int node_b);
static int ladder_search(int start_node, int end_node, int target_node);
static int count_gaps();

bool slh(int** graph, int* node_count, int* edge_count, int* path)
{
    int gap_count;

    g_node_count = node_count;
    g_path = path;
    g_graph = graph;

    init();
    stage1();


    printf("Final path:");
    print_path(path, node_count);
    gap_count = count_gaps();
    printf("Left with  : %i gaps\n", gap_count);
    return gap_count == 0 ? true : false ;
}

static int get_prec_node(int* cur_node) {
    int index = g_node_index[ *cur_node ];
    return ( index == 0 ? g_path[ *g_node_count-1 ] : g_path[ index-1 ] );
}

static int get_next_node(int* cur_node) {
    int index = g_node_index[ *cur_node ];
    return ( index == *g_node_count-1 ? g_path[0] : g_path[ index+1 ] );
}

static void swap_nodes(int node_a, int node_b) {
    int node_a_index;
    int node_b_index;
    int tmp_node;
    int tmp_index;
    int item_to_swap;

    node_a_index = g_node_index[ node_a ]; // eg 4
    node_b_index = g_node_index[ node_b ]; // eg 2
    
    if (node_a_index > node_b_index) {
        item_to_swap = (*g_node_count - node_a_index) + node_b_index + 1;
    } else {
        item_to_swap = node_b_index - node_a_index + 1;
    } //if

    item_to_swap = floor( item_to_swap / 2);

    printf("Count: %i Start: %i, end: %i swaps: %i\n", *g_node_count, node_a_index, node_b_index, item_to_swap);

    while (item_to_swap > 0) {

        // printf("Swapping %i with %i\n", *node_a, *node_b);
        tmp_node = g_path[node_b_index];

        // swap path values
        g_path[node_b_index] = g_path[node_a_index];
        g_path[node_a_index] = tmp_node;

        // update index
        g_node_index[ node_a ] = node_b_index;
        g_node_index[ node_b ] = node_a_index;

        node_a_index = (node_a_index == *g_node_count-1) ? 0 : node_a_index+1;
        node_b_index = (node_b_index == 0) ? *g_node_count-1 : node_b_index-1;
        
        printf("New a index: %i new b index: %i\n",node_a_index, node_b_index);

        node_a = g_path[node_a_index];
        node_b = g_path[node_b_index];

        item_to_swap--;
    } //while

}


void init() 
{
    int cur_node;
    int next_node;

    g_node_index  = malloc( (*g_node_count+1) * sizeof(int*));

    // initialize all path positions to zero
    for (int i=0; i < *g_node_count; i++)
    {
        cur_node = i+1;

        g_path[i] = cur_node;
        g_node_index[cur_node] = i;

    } //for

    g_node_index[ *g_node_count ] = *g_node_count - 1;

    printf("Started with  : %i gaps\n", count_gaps());
    print_path(g_path, g_node_count);
}

/*************************************************************
 *                STAGE 0: Closing Transformations           *
 *************************************************************/
void stage0() 
{
    int cur_node;
    int next_node;
    int prec_node;
    int inner_cur_node;
    int inner_next_node;
    int inner_prec_node;
    int inner_next_next_node;

    for (int i=0; i < *g_node_count; i++)
    {
        cur_node = g_path[i];
        next_node = get_next_node(&cur_node);
        prec_node = get_prec_node(&cur_node);

        // found a gap
        if (g_graph[cur_node][next_node] == 0) {

            //printf("Found gap between %i and %i\n", cur_node, next_node);

            for (int j=0; j < *g_node_count; j++) {
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
                if (g_graph[next_node][inner_next_node] == 1 
                    && g_graph[inner_cur_node][inner_next_node] == 0) {

                    swap_nodes( inner_next_node, cur_node);
                    i = 0; // reset i
                    //printf("Closing 2-opt type 1 between nodes %i and %i\n", inner_cur_node, inner_next_node);
                    //print_path(path, node_count);
                    break;
                } //if

                // check edge that can be used to close the gap
                // closing 2-opt type 2 transformation
                if (g_graph[cur_node][inner_cur_node] == 1 
                    && g_graph[inner_next_node][next_node] == 1) {

                    swap_nodes( inner_next_node, cur_node);
                    i = 0; // reset i
                    //printf("Closing 2-opt type 2 between nodes %i and %i\n", cur_node, next_node);
                    //print_path(path, node_count);
                    break;

                } //if

                // check edge that can be used to close the gap
                // closing 3-opt type 2 transformation
                if (g_graph[next_node][inner_cur_node] == 1 ) {

                    inner_next_node = ladder_search(inner_cur_node, cur_node, cur_node);

                    while (inner_next_node != 0) {
                        inner_next_next_node = get_next_node( &inner_next_node );

                        if (inner_next_next_node != cur_node
                            && g_graph[inner_next_next_node][inner_prec_node]) {
                            
                            printf("Closing 3-opt between nodes %i and %i\n", cur_node, next_node);
                            printf("Swapped %i, %i and %i, %i\n", next_node, inner_prec_node, inner_next_next_node, cur_node);
                            print_path(g_path, g_node_count);
                            swap_nodes( next_node, inner_prec_node);
                            swap_nodes( inner_next_next_node, cur_node);

                            print_path(g_path, g_node_count);

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

void stage1() {
		float5();
		return;
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

void float5()
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
				int f = get_next_node(&a);
				//d can't be beside a because b must be in between
				int e = x;
				while(not_found){
						//e can't be beside a because c must be in between
						e = ladder_search(e, get_prec_node(&a), a);
						if (e == 0) break;
						int c = get_next_node(&e);
						//g b, h and j are temporary for now
						int g = get_next_node(&f);
						int b = get_next_node(&g);
						int j = get_next_node(&b);
						int h = get_prec_node(&y);

						int d = get_next_node(&j);
						while(not_found){
								d = ladder_search(d, y, c);
								if (d == 0) break;
								h = get_next_node(&d);
								j = get_prec_node(&d);

								int b = g;
								while(not_found){
								b = ladder_search(b, j, y);
								if (b == 0) break;
								g = get_prec_node(&b);
								not_found = false;
								printf("y = %i, x = %i, a = %i, b = %i, c = %i, d = %i \n", y, x, a, b, c, d );
								}
						}

						not_found = false;
				}

		}
	} //for
}
void stage2() 
{

}

void stage3() 
{

}

int count_gaps() {

    int gap_count = 0;
    int cur_node;
    int next_node;
    for (int i=0; i < *g_node_count; i++)
    {
        cur_node = g_path[i];
        next_node = get_next_node(&cur_node);
       
       if (g_graph[cur_node][next_node] == 0) {
           gap_count++; 
       }
    }

    return gap_count;
}

static int ladder_search(int start_node, int end_node, int target_node)
{
   int found_node = 0;

   while (start_node != end_node) {
        start_node = get_next_node(&start_node);
        if (g_graph[start_node][target_node] == 1) {
            found_node = start_node;
            break;
        } //if
   }

   return found_node;
}
