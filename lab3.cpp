#include <stdio.h>
#include <iostream>
#include <unistd.h>

using namespace std;

typedef unsigned char Byte;
//Have a global array of one MB
unsigned char myheap[1048576];
//Have a pagesize
#define PAGESIZE 1024 //or 2048, or 4096 …

typedef struct chunkhead{
    int size;
    // occupied means info = 1, info = 0 is free
    unsigned int info = 0;
    //FIXME changed pointers to chunkheads (used to be unsigned char)
    unsigned char *next, *prev = NULL;
}chunkhead;



unsigned char* mymalloc( int size);
void myfree(unsigned char* address);
void analyse();

int main(){

    chunkhead* first = (chunkhead*) myheap;
    first -> info = 0;
    first -> size = (1048576 );

unsigned char *a,*b,*c; 
a = mymalloc(1000); 
b = mymalloc(1000); 
c = mymalloc(1000); 


// myfree(b);
// mymalloc(1000);

analyse(); 

}


// fix return pointer value
unsigned char* mymalloc( int size){

    chunkhead *p = (chunkhead*) myheap;

    for(; p != NULL; p = (chunkhead*) p -> next){
        
        //FIXME size check is wrong
        if(p->info == 0 && (p -> size  > (size + sizeof(chunkhead)) )){

            // printf("p -> size = %d and size + chunk = %d\n", p ->size, size);

            //page is now occupied
            p->info = 1;
            int alloc_size = 0;

            // printf("alloc size - chunk head = %d\nsize = %d\n", (alloc_size -sizeof(chunkhead)), size);

            // allocating the proper amount of space for "size"
            while( size + sizeof(chunkhead)  > (alloc_size ) ){
                // printf("making alloc size bigger %d", alloc_size);
                alloc_size += PAGESIZE;
            }
            

            // printf("alloc bytes ended up at %d\n", alloc_size);
           
           // amount of memory remaining = (rest mem) - (size we're allocating) - (new chunkhead size) 
            int remaining_mem = p -> size  - alloc_size;//- sizeof(chunkhead);
            
            // set allocated size for p
            p -> size = alloc_size;



            chunkhead* nextHead = (chunkhead*) ( (unsigned char *) p + sizeof(chunkhead) + p -> size);
            nextHead -> next = p-> next;
            nextHead -> prev = (unsigned char*) p;
            nextHead -> size = remaining_mem ;
            p -> next = (unsigned char*) nextHead;


             return (unsigned char*) p + sizeof(chunkhead);
        }
    }

    // printf("got to the end of for loop without answer for size = %d!\n", size);
    
    return NULL;

}


void myfree(unsigned char* address){

    chunkhead* iter = (chunkhead*) myheap;

    for(; iter != NULL; iter = (chunkhead*) iter -> next ){

        chunkhead* lastChunk = (chunkhead *) iter -> prev;
        chunkhead* nextChunk = (chunkhead *) iter -> next;

        //TODO combine neighboring free blocks
        if( (unsigned char*)iter == (unsigned char *) address - sizeof(chunkhead)){

           

            // free 
            iter -> info = 0;
            // printf("found the address %p\n" , (void *)  address);
        }

        

        if (iter -> info == 0 && nextChunk && nextChunk -> info == 0){
            // cout << "went to first if statement" << endl;
            // curr is first, next is second
            chunkhead * kill = nextChunk;
            chunkhead * keep = iter;
            keep -> size += kill -> size;
            keep -> next = kill -> next;
            chunkhead *newNext =  (chunkhead *)kill -> next;
            if(newNext){
                newNext -> prev = (unsigned char *) keep;
            }
            
        }
        if(iter-> info == 0 && lastChunk && lastChunk -> info  == 0){
            // cout << "went to second if statement" << endl;
            // last is first, curr is second
            chunkhead * kill = iter;
            chunkhead * keep = lastChunk;
            keep -> size += kill -> size;
            keep -> next = kill -> next;
            chunkhead *newNext =  (chunkhead *)kill -> next;
            if(newNext){
                newNext -> prev = (unsigned char *) keep;
            }
            

        }

        
        
    }

}



void analyse(){
    chunkhead* iter = (chunkhead*) myheap;
    int index = 1;

    for(; iter  != NULL; iter = (chunkhead*) iter -> next){
        cout << "Chunk #" << index << ":" <<endl;
        printf("address: %p\n", (void *) iter);
        printf("Size = %d bytes\n", iter->size);
        if(iter -> info == 0){
            cout << "Free" << endl;
        } else {
            cout << "Occupied" << endl;
        }
        printf("Next = %p\n", (void *) (iter -> next) );
        printf("Prev = %p\n\n", (void *)  (iter -> prev ) );


        index += 1;
    }
}
