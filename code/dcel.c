//
// Created by mukul on 15/08/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "vect.h"
#include "dcel_struct.h"
#include "dcel.h"


#define WATCH_IDX 0
#define VERT_IDX  1
#define SPLIT_IDX 2
#define EDGE_IDX  3
#define FACE_IDX  4


void make_init_poly(edge_t **edge, face_t **face, int *len){
	int i, edge_num=0,is_last=0,vert_num=1;
	if(len[VERT_IDX]<2){
		fprintf(stderr,"Not enough vertex to create an edge\n");
		exit(EXIT_FAILURE);
	}
	//number of edges = number of vertices for initial convex polygon
	//    *edge = (edge_t*) malloc(sizeof (edge_t)*(len[VERT_IDX]));
	//    assert(*edge);
	//    len[EDGE_IDX] = len[VERT_IDX];

	//as we already have number of splits I will allocate space for those here
	// as well but will update total length of array later
	//each split edge will create 3 new edges,
	// so total edges = total vertices + 3*number of splits
	*edge = (edge_t*) malloc(sizeof (edge_t)*((3*len[SPLIT_IDX])+len[VERT_IDX]));
	assert(*edge);
	len[EDGE_IDX] = len[VERT_IDX];

	//as each split will create a new face we can say
	//total number of faces of final shape = number of splits + initial face
	len[FACE_IDX]=len[SPLIT_IDX]+1;
	*face = (face_t*) malloc(sizeof (face_t)*len[FACE_IDX]);
	for (i =0; i <len[FACE_IDX];i++){
		(*face)[i].halfedge=NULL;
	}

	for (edge_num=0;edge_num<len[EDGE_IDX];edge_num++){
		if(edge_num==(len[EDGE_IDX]-1)){
			is_last=1;
		}
		//  (int vert_idx, edge_t *edge, int edge_num,int is_last)
		insert_edge(vert_num,*edge,edge_num,is_last);
		vert_num++;
	}
	//allocating initial half_edge pointer to its face
	(*face)[0].halfedge= (*edge)[0].halfedge;
}
void free_poly(edge_t *edge, int len){
	for( int i =0; i<len;i++){
		free(edge[i].halfedge->twin_half_edge);
		free(edge[i].halfedge);
	}
	free(edge);
}

void traverse_init_poly(edge_t *edge){
	half_edge_t *start, *curr;
	curr = edge[0].halfedge;
	start = curr;
	while(curr->next_half_edge != start ){
		printf("current vertex:(%d,%d)\ncurrent edge:%d\ncurrent face:%d\n\n",
			   curr->start_vertex_idx,curr->end_vertex_idx,
			   curr->edge_index,curr->face_index);
		curr = curr->next_half_edge;
	}
	printf("current vertex:(%d,%d)\ncurrent edge:%d\ncurrent face:%d\n\n",
		   curr->start_vertex_idx,curr->end_vertex_idx,
		   curr->edge_index,curr->face_index);
	curr = curr->twin_half_edge;
	start = curr;
	while(curr->next_half_edge != start ){
		curr = curr->next_half_edge;
		printf("current vertex:(%d,%d)\ncurrent edge:%d\ncurrent face:%d\n\n",
			   curr->start_vertex_idx,curr->end_vertex_idx,
			   curr->edge_index,curr->face_index);
	}

}
void traverse_full(FILE *f3,edge_t *edge, face_t *face,loc_t **vertex,int *len){
	int cnt;
	half_edge_t *start;
	half_edge_t *curr;
	for (int i =0; i<len[FACE_IDX];i++){
		start = face[i].halfedge->prev_half_edge;
		curr = face[i].halfedge;
		cnt=0;
		printf("%i ,face: %d \n",i,curr->face_index);
		while(curr!= start ){
			if(cnt==0){
				start=start->next_half_edge;
				cnt++;
			}
			fprintf(f3,"%d, %d, %lf, %lf\n",
			curr->face_index,curr->edge_index, vertex[curr->start_vertex_idx]->x,
			vertex[curr->start_vertex_idx]->y);
			fprintf(f3,"%d, %d, %lf, %lf\n",
					i,curr->edge_index, vertex[curr->end_vertex_idx]->x,
					vertex[curr->end_vertex_idx]->y);
			curr = curr->next_half_edge;
		}
	}
}

void insert_edge(int vert_num, edge_t *edge, int edge_num,int is_last){
	half_edge_t *new;
	half_edge_t *pair;
	new = (half_edge_t*) malloc(sizeof (*new));
	assert(new);
	pair = (half_edge_t*) malloc(sizeof(*pair));
	assert(pair);
	new->edge_index = pair->edge_index = edge_num;
	new ->face_index = 0;
	pair->face_index=-1;
	new->twin_half_edge = pair;
	pair->twin_half_edge = new;
	new->start_vertex_idx = pair->end_vertex_idx = vert_num-1;
	if(is_last){
		new->end_vertex_idx = pair->start_vertex_idx = 0;
	}else{
		new->end_vertex_idx = pair->start_vertex_idx = vert_num;
	}
	if (edge_num==0){
		//  first time set everything to null
		new->next_half_edge = pair->prev_half_edge=NULL;
		new->prev_half_edge = pair->next_half_edge=NULL;
	}else{
		new->prev_half_edge  = edge[edge_num-1].halfedge;
		pair->next_half_edge = edge[edge_num-1].halfedge->twin_half_edge;
		edge[edge_num-1].halfedge->next_half_edge=new;
		edge[edge_num-1].halfedge->twin_half_edge->prev_half_edge=pair;

		if(is_last){
			//update edge 0 as well
			edge[0].halfedge->prev_half_edge = new;
			edge[0].halfedge->twin_half_edge->next_half_edge=pair;
			//set next to edge 0
			new->next_half_edge  = edge[0].halfedge;
			pair->prev_half_edge = edge[0].halfedge->twin_half_edge;
		}else{
			//set next ones to null
			new->next_half_edge = pair->prev_half_edge = NULL;
		}
	}
	edge[edge_num].halfedge=new;
}

//these functions are to perform split actions
void do_split(edge_t **edge, face_t *face,split_t *split, loc_t ***vertex,int *len){
	int old_vertex_len = len[VERT_IDX],
	old_edge_len = len[EDGE_IDX],curr_vert_num=0, curr_face_num = 0;
	int i,j=0;
	//updating total edge array length allocating before in make_init_poly()
	len[EDGE_IDX] = (3*len[SPLIT_IDX])+len[EDGE_IDX];

	// I have decided to realloc here as in read_second_file total length was
	//unknown anyway so there might have been more space allocated at the end
	len[VERT_IDX] = 2*len[SPLIT_IDX]+len[VERT_IDX];
	*vertex = (loc_t**) realloc(*vertex,len[VERT_IDX]*sizeof (loc_t*));
	assert(*vertex);

	for(i=0;i<len[SPLIT_IDX];i++){
		curr_vert_num = old_vertex_len+i+j+1;
		(*vertex)[curr_vert_num-1]= find_midpoint(*edge,split[i].edge_start,*vertex);
		(*vertex)[curr_vert_num]= find_midpoint(*edge,split[i].edge_end,*vertex);

		make_a_split_edge(*edge,split[i],face,old_edge_len,
						  curr_vert_num, &curr_face_num );
		old_edge_len+=3;
		j++;
	}
}

void print_split(split_t *split,int len){
	for(int i=0; i<len;i++){
		printf("%d, %d\n",split[i].edge_start,split[i].edge_end);
	}
}
int get_face_num(edge_t edge){
	return edge.halfedge->face_index;
}
void allocate_face_num(half_edge_t *half_edge){
	half_edge_t *start, *curr;
	start = half_edge;
	curr = start->next_half_edge;
	while(curr != start ){
		curr->face_index = half_edge->face_index;
		curr = curr->next_half_edge;
	}
}

loc_t* find_midpoint(edge_t *edge, int edge_num,loc_t **vertex){
	loc_t *midpoint = (loc_t *) malloc(sizeof (loc_t));
	assert(midpoint);
	loc_t start_vert;
	loc_t end_vert;
	start_vert.x= vertex[(edge[edge_num].halfedge->start_vertex_idx)]->x;
	start_vert.y= vertex[(edge[edge_num].halfedge->start_vertex_idx)]->y;
	end_vert.x= vertex[(edge[edge_num].halfedge->end_vertex_idx)]->x;
	end_vert.y= vertex[(edge[edge_num].halfedge->end_vertex_idx)]->y;
	midpoint->x = (start_vert.x+end_vert.x)/(2.0);
	midpoint->y = (start_vert.y+end_vert.y)/(2.0);
	return midpoint;
}

void make_a_split_edge(edge_t *edge,split_t split, face_t *face,
					   int edge_num, int vert_num, int *current_face_num){
	/*
     edge_num is number of edge introduced. this is the idx of split edge
     vert_num is the idx of end vertex of split edge,
      so vert_num-1 is idx of start vertex of split edge
     current_face_num  keeps track of total number of faces in the polygon at a
     given stage.
    */

	int count=1;
	//each split makes 3 new edges, number 0 is the split edge
	half_edge_t *new_half_edge[3];
	half_edge_t *pair_half_edge[3];

	edge_t old_edge1 = edge[split.edge_start];
	edge_t old_edge2 = edge[split.edge_end];

	for (int i=0; i<3;i++){
		new_half_edge[i] = (half_edge_t*) malloc(sizeof (half_edge_t));
		assert(new_half_edge[i]);
		pair_half_edge[i] = (half_edge_t*) malloc(sizeof(half_edge_t));
		assert(pair_half_edge[i]);

		new_half_edge[i]->edge_index = pair_half_edge[i]->edge_index = edge_num+i;
		new_half_edge[i]->twin_half_edge = pair_half_edge[i];
		pair_half_edge[i]->twin_half_edge = new_half_edge[i];
	}

	//make sure halfedges are facing same side and not outside (-1 means outside)
	while((get_face_num(old_edge1)!= get_face_num(old_edge2))
	||(get_face_num(old_edge1)==-1)||(get_face_num(old_edge2)==-1)){
		if((count)%2 ==0){
			old_edge2.halfedge=old_edge2.halfedge->twin_half_edge;
		}else{
			old_edge1.halfedge=old_edge1.halfedge->twin_half_edge;
		}
		count++;
	}
	//now they are on same side for sure
	//allocate vertex number for halfedge 0
	new_half_edge[0]->start_vertex_idx = vert_num-1;
	new_half_edge[0]->end_vertex_idx = vert_num;
	new_half_edge[0]->next_half_edge = old_edge2.halfedge;
	new_half_edge[0]->prev_half_edge = old_edge1.halfedge;
	//done
	pair_half_edge[0]->start_vertex_idx =vert_num;
	pair_half_edge[0]->end_vertex_idx = vert_num-1;
	pair_half_edge[0]->next_half_edge = new_half_edge[1];
	pair_half_edge[0]->prev_half_edge = new_half_edge[2];


	//The edge from old_edg1's new end vertex to its old end vertex (Edge edge_num+1)
	new_half_edge[1]->start_vertex_idx = vert_num-1;
	new_half_edge[1]->end_vertex_idx= old_edge1.halfedge->end_vertex_idx;
	new_half_edge[1]->next_half_edge = old_edge1.halfedge->next_half_edge;
	new_half_edge[1]->prev_half_edge =  pair_half_edge[0];

	pair_half_edge[1]->start_vertex_idx = old_edge1.halfedge->end_vertex_idx;
	pair_half_edge[1]->end_vertex_idx = vert_num-1;
	pair_half_edge[1]->next_half_edge = old_edge1.halfedge->twin_half_edge;
	pair_half_edge[1]->prev_half_edge = old_edge1.halfedge->twin_half_edge->prev_half_edge;

	//That was tiring :( Time to work on new_half_edge[2]
	//The edge from old_edge2's old start vertex to its new start vertex (Edge edge_num+2),
	new_half_edge[2]->start_vertex_idx = old_edge2.halfedge->start_vertex_idx;
	new_half_edge[2]->end_vertex_idx = vert_num ;
	new_half_edge[2]->next_half_edge = pair_half_edge[0];
	new_half_edge[2]->prev_half_edge = old_edge2.halfedge->prev_half_edge;

	pair_half_edge[2]->start_vertex_idx = vert_num;
	pair_half_edge[2]->end_vertex_idx =  old_edge2.halfedge->start_vertex_idx;
	pair_half_edge[2]->next_half_edge= old_edge2.halfedge->twin_half_edge->next_half_edge;
	pair_half_edge[2]->prev_half_edge = old_edge2.halfedge->twin_half_edge;

	//time to update old edges
	old_edge1.halfedge->end_vertex_idx = vert_num-1;
	old_edge1.halfedge->next_half_edge->prev_half_edge = new_half_edge[1];
	old_edge1.halfedge->next_half_edge = new_half_edge[0];

	old_edge1.halfedge->twin_half_edge->start_vertex_idx=vert_num-1;
	old_edge1.halfedge->twin_half_edge->prev_half_edge->next_half_edge = pair_half_edge[1];
	old_edge1.halfedge->twin_half_edge->prev_half_edge = pair_half_edge[1];

	old_edge2.halfedge->start_vertex_idx = vert_num;
	old_edge2.halfedge->prev_half_edge->next_half_edge = new_half_edge[2];
	old_edge2.halfedge->prev_half_edge = new_half_edge[0];

	old_edge2.halfedge->twin_half_edge->end_vertex_idx = vert_num;
	old_edge2.halfedge->twin_half_edge->next_half_edge->prev_half_edge = pair_half_edge[2];
	old_edge2.halfedge->twin_half_edge->next_half_edge = pair_half_edge[2];


	//updating face number
	(*current_face_num)++;
	printf("face: %d \n",(*current_face_num));
	new_half_edge[0]->face_index  = old_edge1.halfedge->face_index;
	pair_half_edge[0]->face_index = (*current_face_num);
	pair_half_edge[1]->face_index = old_edge1.halfedge->twin_half_edge->face_index;
	pair_half_edge[2]->face_index = old_edge2.halfedge->twin_half_edge->face_index;
	//storing pointer to new face half_edge in face array
	face[old_edge1.halfedge->face_index].halfedge = new_half_edge[0];
	face[(*current_face_num)].halfedge = pair_half_edge[0];
	//update face number on the side of new edge
	allocate_face_num(new_half_edge[0]);
	allocate_face_num(pair_half_edge[0]);


	//put pointers to new edges in edge
	for (int i=0; i<3;i++ ){
		edge[edge_num+i].halfedge   = new_half_edge[i];
	}

}
