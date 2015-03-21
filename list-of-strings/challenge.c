#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LL_FOREACH(list, node) LL_FOREACH2(list, node, next)
#define LL_FOREACH2(list, node, next)                           \
for (node = list; node != NULL; node = node->next)

/*
  Safe foreach approach: you may free the node variable, without
  affecting the traversing of the list.
 */
#define LL_FOREACH_SAFE(list, node, tmp) LL_FOREACH_SAFE2(list, node, tmp, next)
#define LL_FOREACH_SAFE2(list, node, tmp, next)                 \
  for ((node) = (list); (node) && (tmp = (node)->next, 1); (node) = tmp)

#define DLL_APPEND(list, add) DLL_APPEND2(list, add, next, prev)
#define DLL_APPEND2(list, add, next, prev)                      \
do {                                                            \
  if ((list) == NULL) {                                         \
    (list) = (add);                                             \
    (list)->prev = (list);                                      \
    (list)->next = NULL;                                        \
  } else {                                                      \
    (add)->prev = (list)->prev;                                 \
    (list)->prev->next = (add);                                 \
    (list)->prev = (add);                                       \
    (add)->next = NULL;                                         \
  }                                                             \
} while (0);

/*
 Doubly linked list.
 */
typedef struct node {
  char* value;
  struct node *prev, *next;
} node_t;

void node_create(node_t** node, char* str, int len) {
  (*node) = malloc(sizeof(node_t));
  (*node)->value = malloc(sizeof(char) * len);
  (*node)->prev = NULL;
  (*node)->next = NULL;
  strcpy((*node)->value, str);
}

int node_search(node_t* root, char* node) {
  node_t* it;
  for (it = root; it != NULL; it = it->next) {
    if (strcmp(it->value, node) == 0) {
      return 1;
    }
  }

  return 0;
}

/**
 * Basic naïve solution.
 * How fast can YOU make it?
 */
int node_insert(node_t** root, node_t* node) {
  if (node_search(root, str) != 0) {
    return 0;
  }

  DLL_APPEND(*root, node);
  return 1;
}

void node_free(node_t* node) {
  free(node->value);
  free(node);
}

void list_clear(node_t** root, int* count) {
  node_t* it;
  node_t* tmp;

  (*count) = 0;
  LL_FOREACH_SAFE(*root, it, tmp) {
    node_free(it);
    (*count)++;
  }

  *root = NULL;
}

int main() {
  node_t* root = NULL;

  FILE* fp = fopen("wordlist.dat", "r");

  int lines = 0;
  char ch;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if (ch == '\n') {
      lines++;
    }
  }
  rewind(fp);

  // you may use "lines" to print a progress bar, or something.

  uint32_t i = 0;
  size_t maxlen = 32;
  char str[maxlen];
  while ((fgets(str, maxlen, fp)) != NULL) {
    node_t* node = NULL;
    node_create(&node, str, maxlen);

    if (node_insert(&root, node) == 0) {
      node_free(node);
    }
    i++;

    printf("\r%d of %d %.2f %%", i, lines, (i / (float)lines) * 100);
  }

  fclose(fp);

  int count;
  list_clear(&root, &count);
  printf("\r%d elements, tried %d words (%d)\n", count, i, i - count);
}