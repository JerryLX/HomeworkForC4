#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#define OPT

#ifdef OPT
#define H manhattan2
#else
#define H manhattan
#endif

/** 
 * READ THIS DESCRIPTION
 *
 * node data structure: containing state, g, f
 * you can extend it with more information if needed
 */
typedef struct node{
	int state[16];
	int g;
	int f;
    int blank_pos;
} node;

/**
 * Global Variables
 */

// used to track the position of the blank in a state,
// so it doesn't have to be searched every time we check if an operator is applicable
// When we apply an operator, blank_pos is updated
//int blank_pos;

// Initial node of the problem
node initial_node;

// Statistics about the number of generated and expendad nodes
unsigned long generated;
unsigned long expanded;


/**
 * The id of the four available actions for moving the blank (empty slot). e.x.
 * Left: moves the blank to the left, etc. 
 */

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

/*
 * Helper arrays for the applicable function
 * applicability of operators: 0 = left, 1 = right, 2 = up, 3 = down 
 */
int ap_opLeft[]  = { 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 };
int ap_opRight[]  = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };
int ap_opUp[]  = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int ap_opDown[]  = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
int *ap_ops[] = { ap_opLeft, ap_opRight, ap_opUp, ap_opDown };


inline node * copy_node(node* old){
    generated++;
    node* new_node = malloc(sizeof(node));
    memcpy(new_node, old, sizeof(node));
    return new_node;
}

inline int x(int i){
    return i%4;
}

inline int y(int i){
    return i/4;
}

/* print state */
void print_state( int* s )
{
	int i;
	
	for( i = 0; i < 16; i++ )
		printf( "%2d%c", s[i], ((i+1) % 4 == 0 ? '\n' : ' ') );
}
      
void printf_comma (long unsigned int n) {
    if (n < 0) {
        printf ("-");
        printf_comma (-n);
        return;
    }
    if (n < 1000) {
        printf ("%lu", n);
        return;
    }
    printf_comma (n/1000);
    printf (",%03lu", n%1000);
}

/* return the sum of manhattan distances from state to goal */
int manhattan( int* state )
{
	int sum = 0;
    int i = 0;
	/**
	 * FILL WITH YOUR CODE
	 */

    for(; i<16; i++){
        if(!state[i])
            continue;
        int posx = x(i);
        int posy = y(i);
        int posx2 = x(state[i]);
        int posy2 = y(state[i]);
    
        sum += abs(posx - posx2) + abs(posy - posy2);
    }
	return( sum );
}

int LIS(int *vec, int count){
    int dp[4], i, j;
    dp[0] = 1;

    for(i=1;i<count;i++){
        dp[i] = 0;
        for(j=0;j<i;j++){
            if(vec[i]>vec[j]){
                dp[i] = dp[j]+1>dp[i]?dp[j]+1:dp[i];
            }
        }
    }
    return dp[count-1];
}

int manhattan2( int* state )
{
	int sum = 0;
    int i = 0;
	/**
	 * FILL WITH YOUR CODE
	 */

    int one_in_first_row = 0;
    int one_in_first_col = 0;
    int four_in_first_row = 0;
    int four_in_first_col = 0;

    int row[4][4];
    int col[4][4];
    int count_row[4];
    int count_col[4];
    for(i=0;i<4;i++){
        count_row[i] = count_col[i] = 0;
    }

    for(i=0; i<16; i++){
        if(!state[i])
            continue;
        int posx = x(i);
        int posy = y(i);
        int posx2 = x(state[i]);
        int posy2 = y(state[i]);

        if(posx == posx2){
            col[posx][count_col[posx]] = state[i];
            count_col[posx]++;
        }
        if(posy == posy2){
            row[posy][count_row[posy]] = state[i];
            count_row[posy]++;
        }
    
        if(state[i] == 1){
            if(posx == 0)
                one_in_first_col = 1;
            if(posy == 0)
                one_in_first_row = 1;

        }
        if(state[i] == 4){
            if(posx == 0)
                four_in_first_col = 1;
            if(posy == 0)
                four_in_first_row = 1;
        }

        sum += abs(posx - posx2) + abs(posy - posy2);
    }
    if(!one_in_first_col && !four_in_first_row &&!one_in_first_row && !four_in_first_col){
        sum+=2;
    }
    for(i=0;i<4;i++){
        if(count_row[i]){
            int lis = LIS(row[i],count_row[i]);
            sum += 2*(count_row[i]-lis);
        }
        if(count_col[i]){
            int lis = LIS(col[i],count_col[i]);
            sum += 2*(count_col[i]-lis);
        }
    }
	return( sum );
}

/* return 1 if op is applicable in state, otherwise return 0 */
int applicable( int op, node* n)
{
       	return( ap_ops[op][n->blank_pos] );
}


/* apply operator */
void apply( node* n, int op )
{
	int t;
    int blank_pos = n->blank_pos;
	//find tile that has to be moved given the op and blank_pos
	t = blank_pos + (op == 0 ? -1 : (op == 1 ? 1 : (op == 2 ? -4 : 4)));

	//apply op
	n->state[blank_pos] = n->state[t];
	n->state[t] = 0;
	
	//update blank pos
	n->blank_pos = t;
}

/* Recursive IDA */
node* ida( node* node, int threshold, int* newThreshold )
{

	/**
	 * FILL WITH YOUR CODE
	 *
	 * Algorithm in Figure 2 of handout
	 */
    expanded++;
    int i;
    for( i=0; i<4; i++ ){
        if( !applicable(i, node) ) 
            continue;
        struct node* new_node = copy_node(node);
        apply(new_node, i);
        new_node->g++;
        new_node->f = new_node->g + H(new_node->state);
        
        if(new_node->f > threshold){
            if(new_node->f < *newThreshold)
                *newThreshold = new_node->f;
            free(new_node);
        }else{
            if(H(new_node->state) == 0)
                return new_node;
            struct node* r = ida(new_node, threshold, newThreshold);
            free(new_node);
            if(r)
                return r;
        }
    }
	return( NULL );
}


/* main IDA control loop */
int IDA_control_loop(  ){
	node* r = NULL;
	
	int threshold;
	
	/* initialize statistics */
	generated = 0;
	expanded = 0;

	/* compute initial threshold B */
	initial_node.f = threshold = H( initial_node.state );
	printf( "Initial Estimate = %d\nThreshold =", threshold );
	/**
	 * FILL WITH YOUR CODE
	 *
	 * Algorithm in Figure 1 of handout
	 */
    while(!r){
        printf(" %d", threshold);
        int newThreshold = INT_MAX;
        initial_node.g = 0;
        r = ida(&initial_node, threshold, &newThreshold);
        if(!r)
            threshold = newThreshold;
    }
	if(r)
		return r->g;
	else
		return -1;
}


static inline float compute_current_time()
{
	struct rusage r_usage;
	
	getrusage( RUSAGE_SELF, &r_usage );	
	float diff_time = (float) r_usage.ru_utime.tv_sec;
	diff_time += (float) r_usage.ru_stime.tv_sec;
	diff_time += (float) r_usage.ru_utime.tv_usec / (float)1000000;
	diff_time += (float) r_usage.ru_stime.tv_usec / (float)1000000;
	return diff_time;
}

int main( int argc, char **argv )
{
	int i, solution_length;

	/* check we have a initial state as parameter */
	if( argc != 2 )
	{
		fprintf( stderr, "usage: %s \"<initial-state-file>\"\n", argv[0] );
		return( -1 );
	}


	/* read initial state */
	FILE* initFile = fopen( argv[1], "r" );
	char buffer[256];

	if( fgets(buffer, sizeof(buffer), initFile) != NULL ){
		char* tile = strtok( buffer, " " );
		for( i = 0; tile != NULL; ++i )
			{
				initial_node.state[i] = atoi( tile );
				initial_node.blank_pos = (initial_node.state[i] == 0 ? i : initial_node.blank_pos);
				tile = strtok( NULL, " " );
			}		
	}
	else{
		fprintf( stderr, "Filename empty\"\n" );
		return( -2 );

	}
       
	if( i != 16 )
	{
		fprintf( stderr, "invalid initial state\n" );
		return( -1 );
	}

	/* initialize the initial node */
	initial_node.g=0;
	initial_node.f=0;

	print_state( initial_node.state );


	/* solve */
	float t0 = compute_current_time();
	
	solution_length = IDA_control_loop();				

	float tf = compute_current_time();

	/* report results */
	printf( "\nSolution = %d\n", solution_length);
	printf( "Generated = ");
	printf_comma(generated);		
	printf("\nExpanded = ");
	printf_comma(expanded);		
	printf( "\nTime (seconds) = %.2f\nExpanded/Second = ", tf-t0 );
	printf_comma((unsigned long int) expanded/(tf+0.00000001-t0));
	printf("\n\n");

	/* aggregate all executions in a file named report.dat, for marking purposes */
	FILE* report = fopen( "report.dat", "a" );

	fprintf( report, "%s", argv[1] );
	fprintf( report, "\n\tSoulution = %d, Generated = %lu, Expanded = %lu", solution_length, generated, expanded);
	fprintf( report, ", Time = %f, Expanded/Second = %f\n\n", tf-t0, (float)expanded/(tf-t0));
	fclose(report);
	
	return( 0 );
}


