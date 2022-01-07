#include "dectree.h"

int main(int argc, char **argv) {
    Dataset* d = load_dataset("testing_data.bin");
    for(int i=0; i< NUM_PIXELS;i++){
        printf("%d", d->images[0].data[i]);
    }
    free_dataset(d);
    
}