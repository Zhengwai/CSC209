#include <stdio.h>
#include <stdlib.h>
#include "dectree.h"

int main(int argc, char **argv) {
    Dataset* d = load_dataset("training_10.bin");
    int label;
    int freq;
    int ind[5] = {0,1,2,4,8};
    get_most_frequent(d, 5, ind, &label, &freq);
    printf("label: %d, freq: %d", label, freq);
    
}