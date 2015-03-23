#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define ALPHABET_SIZE 26 // a-z
#define MAX_WORD_SIZE 64

typedef struct node {
  int leaf; /* used to indicate that this node represents the last character in a string. 
    leaf == 0 => no terminating node
    leaf == 1 => terminating node
    leaf > 1  => terminating node, where leaf is the number of identical strings inserted
  */
  struct node* children[ALPHABET_SIZE];
} node_t;

/*
  removes any characters that is not a-z
 */
inline void strfix(char key[], char** out, int* outlen) {
  int len = strlen(key);
  char* s = malloc(sizeof(char) * (len + 1)); // make room for null-byte

  int i, j = 0;
  for (i = 0; i < len; ++i) {
    char c = key[i];
    if (c < 'a' || c > 'z') {
      break;
    }

    s[j++] = c;
  }

  s[j] = '\0';
  *outlen = j;
  *out = s;
}

void node_init(node_t* node) {
  node->leaf = 0;

  int i;
  for (i = 0; i < ALPHABET_SIZE; ++i) {
    node->children[i] = NULL;
  }
}

node_t* node_create() {
  node_t* node = malloc(sizeof(node_t));
  node_init(node);
  return node;
}

static int trie_size = 0;

/*
  key must be null-byte terminated
 */
int node_insert(node_t* root, char key[], int length) {
  assert(length > 0);

  node_t* it = root;

  int i = 0, j;
  char ch = key[i++];

  while (ch) {
    j = ch - 'a';

    if (it->children[j] == NULL) {
      it->children[j] = node_create();
    }

    it = it->children[j];
    ch = key[i++];
  }

  assert(ch == '\0');

  /* if leaf == 1 it means we tried to insert a string
    that already exists in the trie */
  it->leaf++;

  if (it->leaf > 1) {
    return 0;
  }

  /* set special value for leaf node */
  trie_size++;
  return 1;
}

void node_traverse_rec(FILE* stream, node_t* root, char key[], int length) {
  int i;

  if (root->leaf > 0) {
    for (i = 0; i < length; ++i) {
      fprintf(stream, "%c", key[i]);
    }
    if (root->leaf > 1) {
      fprintf(stream, " (%d)", root->leaf);
    }
    fprintf(stream, "\n");
  }

  for (i = 0; i < ALPHABET_SIZE; ++i) {
    if (root->children[i] != NULL) {
      key[length] = i + 'a';
      node_traverse_rec(stream, root->children[i], key, length + 1);
    }
  }
}

void node_traverse(node_t* root, FILE* stream) {
  int i;
  char key[MAX_WORD_SIZE];
  int length = 0;

  for (i = 0; i < ALPHABET_SIZE; ++i) {
    if (root->children[i] != NULL) {
      key[length] = i + 'a';
      node_traverse_rec(stream, root->children[i], key, length + 1);
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s INPUT_FILE OUTPUT_FILE\n", argv[0]);
    return 1;
  }

  char* input_file = argv[1];
  char* output_file = argv[2];

  node_t root;
  node_init(&root);

  char str[MAX_WORD_SIZE];

  FILE* fp = fopen(input_file, "r");
  while ((fgets(str, MAX_WORD_SIZE, fp)) != NULL) {
    char* s = NULL;
    int len = 0;
    strfix(str, &s, &len);

    node_insert(&root, s, len);
    free(s);
  }

  fclose(fp);
  printf("%d\n", trie_size);

  fp = fopen(output_file, "w");
  node_traverse(&root, fp);
  fclose(fp);

  // XXX: free all nodes here. :)
}