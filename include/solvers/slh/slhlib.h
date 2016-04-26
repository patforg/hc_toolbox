
void slhlib_init(int* node_count, int* path, int** graph);

int get_prec_node(int* cur_node);
int get_next_node(int* cur_node);
//reverses nodes in a subpath
void swap_nodes(int node_a, int node_b);
//switches two subpaths
void switch_nodes(int a, int b, int c, int d);
int ladder_search(int start_node, int end_node, int target_node);
int count_gaps();
