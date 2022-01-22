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


    printf("sizeof chunkhead = %d\n", sizeof(chunkhead));
    // analyse();

    mymalloc(50);
    mymalloc(2000);

    analyse();

}

unsigned char* mymalloc( int size){
    chunkhead *p = (chunkhead*) myheap;

    for(; p != NULL; p = (chunkhead*) p -> next){
        
        //FIXME size check is wrong
        if(p->info == 0 && (p -> size - sizeof(chunkhead)) > size ){
            //page is now occupied
            p->info = 1;
            int alloc_size = 0;

            printf("alloc size - chunk head = %d\nsize = %d\n", (alloc_size -sizeof(chunkhead)), size);

            // FIXME error check to make sure we 
            while( size + sizeof(chunkhead)  > (alloc_size ) ){
                printf("making alloc size bigger %d", alloc_size);
                alloc_size += PAGESIZE;
            }
            printf("alloc bytes ended up at %d\n", alloc_size);
           
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

}



void analyse(){
    chunkhead* iter = (chunkhead*) myheap;
    int index = 1;

    for(; iter  != NULL; iter = (chunkhead*) iter -> next){
        cout << "Chunk #" << index << ":" <<endl;
        printf("Size = %d bytes\n", iter->size);
        printf("Next = %x\n", iter -> next);
        printf("Prev = %x\n", iter -> prev);


        index += 1;
    }
}
