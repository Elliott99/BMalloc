//Author: Elliott Goldstein
//December 18 2018
//CMSC 226
//BMalloc() and Bfree()
//I worked with alone on this assignment with assistance from The Linux Programming Interface
#include <stdio.h>
#include <stdlib.h>
#define align4(x) (((((x) - 1) >> 2) << 2) + 4)
#define MAX_HEAP_SIZE (2 << 24)
/*My block struct that will be used to create space
 * for memory on the heap. Contains a pointer to the next and 
 * previous blocks, the size, and the flag that
 * determines if the block has been freed
 * */
typedef struct block {
size_t size;
struct block* next;
struct block* prev;
int free;
}block_t;

/* *A simple node struct to use for the linked list
 * I will implement. Contains a pointer
 * to the next node and the value the
 * node contains
 * */
typedef struct node{
	struct node * next;
	int val;
}node_t;

static size_t index=0;


static char heap[MAX_HEAP_SIZE];
static block_t* freelist = 0;
static block_t* curr= 0;
/*My bmalloc() function. Takes a size_t parameter that
 * will determine how large the block_t struct will be.
 * To put the blocks on the heap, I loop through
 * while the current block is not free, setting the
 * flags equal to 1 which will signal the block
 * is currently allocated on the heap
 * */
void *bmalloc(size_t size){
	void * mem=0;
	size_t alloc=size;
		if (freelist==0){
			freelist=&heap[0];
			freelist->free=0;
			freelist->prev=0;
			freelist->size=MAX_HEAP_SIZE;
			freelist->next=0;
			//printf("is this happening?");
		}
		for (curr=freelist;curr!=NULL;curr=curr->next){
			if (alloc<=curr->size && curr->free==0){
				block_t *tmp=curr->next;
				curr->free=1;
				char * bcurr=(char*) curr;
				curr->next=(void*)(bcurr+alloc+sizeof(block_t));
				curr->next->size=curr->size-alloc;
				curr->size=alloc;
				curr->next->free=0;
				curr->next->prev=curr;
				mem=bcurr+sizeof(block_t);
				return mem;
			}
		}
	return 0;
}
/*Method for pushing node_t struct onto a linked
 * list. Uses bmalloc() to allocate a block
 * of memory for node_t struct
 * */
void push(struct node * head, int val){
	node_t *top=head;
	while (top->next!=NULL){
		top=top->next;
	}
	
	top->next=bmalloc(sizeof(node_t));
	top->next->val=val;
	top->next->next=NULL;
}
/*My bfree() method. Takes a void* type as an argument
 * and loops through the blocks on the heap. If
 * the address of curr+1 (curr+sizeof(block_t) scaled
 * for pointer arithmetic) matched the address of the void
 * ptr, check the condition under which it is freed
 * and change the flag to 0, signaling the block
 * is no longer allocated and cannot be accessed
 **/
void *bfree (void *ptr){
	for (curr=freelist;curr!=NULL;curr=curr->next){
		if (curr+1==ptr){
			if (curr->prev==0 && curr->next!=0){
				printf("Case beginning happened\n");
				curr->free=0;
				curr->next->prev=curr->prev;
				
			}
			else if (curr->next->free==0 && curr->next->next==0 && curr->prev->free==1){
					curr->free=0;
					curr->prev->next=curr->next;
					printf("Case end happened\n");
				}

			else if (curr->next->free==0 && curr->prev->free==1){
				printf("Case 1 happened\n");
				curr->free=0;
				curr->size=curr->size+curr->next->size;
				curr->next->next->prev=curr->next->prev;
				curr->next=curr->next->next;
				}
			 else if (curr->prev->free==0 && curr->next->free==1){
				 printf("Case 2 happened\n");
				curr->free=0;
				curr->size=curr->size+curr->prev->size;
				curr->prev->prev->next=curr->prev->next;
				curr->prev=curr->prev->prev;
				//curr->prev->prev->next=curr;
			}
			 else if (curr->prev->free==1 && curr->next->free==1){
				 printf("Case 3 happened\n");
				curr->free=0;
			}
			 else if (curr->prev->free==0 && curr->next->free==0){
				 printf("Case 4 happened\n");
				curr->free=0;
				curr->size=curr->size+curr->prev->size+curr->next->size;
				curr->next->next->prev=curr->next->prev;
				curr->prev->prev->next=curr->prev->next;
				curr->prev=curr->prev->prev;
				curr->next=curr->next->next;
			}
		}
	}
}


/*Method for deleting a given node of the linked list.
 * Traverses linked list, and deletes node based on parameter
 * value "k". Uses bfree to change flag of block that
 * the node_t struct exists on
 * */
void deletenode(node_t **head, int k){
	node_t*tmp=head, *prev;
	if (tmp!=NULL && tmp->val==k){			
		head=tmp->next;
		printf("%d is head value\n", tmp->val);
		bfree(tmp);
		tmp->val=NULL;
		return;
	}	
	while(tmp!=NULL && tmp->val!=k){
		prev=tmp;
		tmp=tmp->next;
		}
	if (tmp==NULL){
		printf("Can't find this index");
	}
	prev->next=tmp->next;
	bfree(tmp);
}
	
		
	
	



/*A method for printing the linked list that I create through bmallocing
 * node_t struct*/
void printlist(node_t * head){
	int p=0;
	node_t*print=head;
	while(print!=NULL){
		p++;
		printf("%d\n", print->val);
		print=print->next;
	}
	if (p==0){
		printf("Nothing in the linked list\n");
	}
}	
/*A method to keep track of each of bmalloc'ed block_t structs.
 * The method lists the size, next and previous pointers,
 * free flag and address of the block
 * */
void printcurr(void){
	for(curr=freelist;curr!=NULL;curr=curr->next){
		printf("Curr is %d\n", curr);
		printf("Curr prev is %d\n", curr->prev);
		printf ("Curr next is %d\n", curr->next);
		printf("Curr free is %d\n", curr->free);
		printf("Curr size is %d\n", curr->size);
	}
}
		

	



	
//Main checks the case where I'm freeing a block between two already freed blocks
//Size is merged into 24 because 8(3)=24
int main(int argc, char*argv[]){
	node_t *tn=bmalloc(sizeof(node_t));
	tn->val=7;
	push(tn,8);
	push(tn,9);
	push(tn,10);
	push(tn,11);
	deletenode(tn,8);
	deletenode(tn,10);
	deletenode(tn,9);
	printlist(tn);
	printcurr();
}
