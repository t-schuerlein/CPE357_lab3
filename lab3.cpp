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
    unsigned int size;
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
    first -> size = (1048576 - sizeof(chunkhead));

unsigned char *a,*b,*c;
    a = mymalloc(1000);
b = mymalloc(1000);
c = mymalloc(1000);
printf("trying to find %x\n", b);
myfree(b);
// myfree(a);
analyse();

}


// fix return pointer value
unsigned char* mymalloc( int size){
    chunkhead *p = (chunkhead*) myheap;

    for(; p != NULL; p = (chunkhead*) p -> next){
        
        //FIXME size check is wrong
        if(p->info == 0 && (p -> size - sizeof(chunkhead)) > size ){
            //page is now occupied
            p->info = 1;
            int alloc_size = 0;

            // printf("alloc size - chunk head = %d\nsize = %d\n", (alloc_size -sizeof(chunkhead)), size);

            // FIXME error check to make sure we 
            while( size + sizeof(chunkhead)  > (alloc_size ) ){
                // printf("making alloc size bigger %d", alloc_size);
                alloc_size += PAGESIZE;
            }
            // printf("alloc bytes ended up at %d\n", alloc_size);
           
           int remaining_mem = p -> size - alloc_size;
            p -> size = alloc_size;

            unsigned char* returnThis =  ((Byte*) p + sizeof(chunkhead));

            chunkhead* nextHead = (chunkhead*) (returnThis + (p -> size));
            nextHead -> next = p-> next;
            nextHead -> prev = (unsigned char*) p;
            nextHead -> size = remaining_mem -sizeof(chunkhead);
            p -> next = (unsigned char*) nextHead;


             return returnThis;
        }
    }

    printf("got to the end of for loop without answer!\n");

}


void myfree(unsigned char* address){

    chunkhead* iter = (chunkhead*) myheap;
    
    

    for(; iter != NULL; iter = (chunkhead*) iter -> next ){

        chunkhead* lastChunk = (chunkhead *) iter -> prev;
        chunkhead* nextChunk = (chunkhead *) iter -> next;

        //TODO combine neighboring free blocks
        if( iter == (chunkhead *) address){
            // free 
            iter -> info = 0;
            printf("found the address %x" ,address);
        }
        if (iter -> info == 0 && nextChunk && nextChunk -> info == 0){
            // curr is first, next is second
            chunkhead * kill = nextChunk;
            chunkhead * keep = iter;
            keep -> size += kill -> size;
            keep -> next = kill -> next;
            chunkhead *newNext =  (chunkhead *)kill -> next;
            newNext -> prev = (unsigned char *) keep;
        }
        if(iter-> info == 0 && lastChunk && lastChunk -> info  == 0){
            // last is first, curr is second
            chunkhead * kill = iter;
            chunkhead * keep = iter;
            keep -> size += kill -> size;
            keep -> next = kill -> next;
            chunkhead *newNext =  (chunkhead *)kill -> next;
            newNext -> prev = (unsigned char *) keep;

        }
        
    }

}



void analyse(){
    chunkhead* iter = (chunkhead*) myheap;
    int index = 1;

    for(; iter  != NULL; iter = (chunkhead*) iter -> next){
        cout << "Chunk #" << index << ":" <<endl;
        printf("address: %x\n", iter);
        printf("Size = %d bytes\n", iter->size);
        if(iter -> info == 0){
            cout << "Free" << endl;
        } else {
            cout << "Occupied" << endl;
        }
        printf("Next = %x\n", iter -> next);
        printf("Prev = %x\n\n", iter -> prev);


        index += 1;
    }
}
