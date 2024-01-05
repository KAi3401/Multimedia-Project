#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256

struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

int compare(const void *a, const void *b) {
    struct MinHeapNode **n1 = (struct MinHeapNode **)a;
    struct MinHeapNode **n2 = (struct MinHeapNode **)b;
    return (*n1)->freq - (*n2)->freq;
}

struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = size;
    minHeap->capacity = size;
    minHeap->array = (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    qsort(minHeap->array, minHeap->size, sizeof(struct MinHeapNode*), compare);

    return minHeap;
}

struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    while (minHeap->size > 1) {
        left = minHeap->array[0];
        minHeap->array[0] = minHeap->array[minHeap->size - 1];
        minHeap->size--;
        qsort(minHeap->array, minHeap->size, sizeof(struct MinHeapNode*), compare);

        right = minHeap->array[0];
        minHeap->array[0] = minHeap->array[minHeap->size - 1];
        minHeap->size--;
        qsort(minHeap->array, minHeap->size, sizeof(struct MinHeapNode*), compare);

        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        minHeap->array[minHeap->size] = top;
        minHeap->size++;
        qsort(minHeap->array, minHeap->size, sizeof(struct MinHeapNode*), compare);
    }

    return minHeap->array[0];
}

int isLeaf(struct MinHeapNode* root) {
    return !(root->left) && !(root->right);
}
void printArr(int arr[], int n) {
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
    printf("\n");
}
void printCodes(struct MinHeapNode* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    if (isLeaf(root)) {
        printf("%c: ", root->data);
        printArr(arr, top);
    }
}
void storeCodes(struct MinHeapNode* root, int arr[], int top, char codes[MAX_TREE_HT][MAX_TREE_HT]) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1, codes);
    }

    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1, codes);
    }

    if (isLeaf(root)) {
        int i;
        for (i = 0; i < top; ++i) {
            codes[(int)root->data][i] = arr[i] + '0';
        }
        codes[(int)root->data][i] = '\0';
    }
}

void HuffmanCodes(char *str, char encodedStr[]) {
    int freq[MAX_TREE_HT] = {0};
    char data[MAX_TREE_HT];
    int size = 0;
    char codes[MAX_TREE_HT][MAX_TREE_HT];

    for (int i = 0; str[i] != '\0'; i++) {
        int index = -1;
        for (int j = 0; j < size; j++) {
            if (data[j] == str[i]) {
                index = j;
                break;
            }
        }

        if (index == -1) {
            data[size] = str[i];
            freq[size] = 1;
            size++;
        } else {
            freq[index]++;
        }
    }

    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
    int arr[MAX_TREE_HT], top = 0;
    printf("The Huffman table:\n");
    printCodes(root, arr, top);
    printf("Encode text:\n");
    storeCodes(root, arr, top, codes);
    

    // Print and store encoded string
    for (int i = 0; str[i] != '\0'; i++) {
        strcat(encodedStr, codes[(int)str[i]]);
    }
}

int main(int argc,char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <string> <outputFile>\n", argv[0]);
        return 1;
    }
    char *str = argv[1];
    char *out = argv[2];
    FILE *fn_out = fopen(out, "w");

    printf("Input : '%s'\n", str);

    char encodedStr[MAX_TREE_HT] = "";
    HuffmanCodes(str, encodedStr);
    printf("Encoded String: %s\n", encodedStr);
    fprintf(fn_out,encodedStr);
    fclose(fn_out);
    return 0;
}
