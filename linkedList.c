#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node {
  void *data;
  struct Node *next;
} Node;

typedef struct linked_list {
  struct Node *head;
  int length;
} LinkedList;

struct LinkedList *createList(void) {
  struct LinkedList *result = malloc(sizeof(*result));
  result->head = NULL;
  result->length = 0;
  return result;
}

struct Node *createNode(void *data) {
  struct Node *node = malloc(sizeof(*node));
  node->data = data;
  node->next = NULL;
  return node;
}

void map(LinkedList *list, void *function()) {
  struct Node *current = linkedList->head;
  for (int i = 0; i < position; i++) {
    current = current->next;
  }
}

void add(struct LinkedList *linkedList, struct Node *node) {
  if (linkedList->head == NULL) {
    linkedList->head = node;
  } else {
    struct Node *current = linkedList->head;
    while (current->next != NULL)
      current = current->next;
    current->next = node;
  }
  linkedList->length++;
}

struct Node get(struct LinkedList *linkedList, int position) {
  struct Node *current = linkedList->head;
  for (int i = 0; i < position; i++)
    current = current->next;
  return *current;
}

int SamsShittyTesting(int argc, char const *argv[]) {
  struct LinkedList *linkedList = createList();

  int anInt = 5;
  add(linkedList, createNode(&anInt));
  char *aString = "test 1";
  add(linkedList, createNode(aString));

  printf("%d\n", *(int*) get(linkedList, 0).data);

  printf("%s\n", (char*) get(linkedList, 1).data);

  return 0;
}
