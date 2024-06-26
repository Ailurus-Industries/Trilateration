#include "link.h"
#include <vector>

struct MyLink *init_link() {
  struct MyLink *p = (struct MyLink *)malloc(sizeof(struct MyLink));
  p->next = NULL;
  p->anchor_addr = 0;
  p->range[0] = 0.0;
  p->range[1] = 0.0;
  p->range[2] = 0.0;
  return p;
}

void add_link(struct MyLink *p, uint16_t addr) {
  struct MyLink *temp = p;
  // Find struct MyLink end
  while (temp->next != NULL) {
    temp = temp->next;
  }
  // Create an anchor
  struct MyLink *a = (struct MyLink *)malloc(sizeof(struct MyLink));
  a->anchor_addr = addr;
  a->range[0] = 0.0;
  a->range[1] = 0.0;
  a->range[2] = 0.0;
  a->dbm = 0.0;
  a->next = NULL;

  // Add the anchor to end of struct MyLink
  temp->next = a;

  return;
}

struct MyLink *find_link(struct MyLink *p, uint16_t addr) {
  // Check inputs are valid
  if (addr == 0) {  return NULL; }
  if (p->next == NULL) { return NULL; }

  struct MyLink *temp = p;
  // Find target struct MyLink or struct MyLink end
  while (temp->next != NULL) {
    temp = temp->next;
    if (temp->anchor_addr == addr) {
      return temp;
    }
  }

  Serial.println("find_link:Can't find addr");
  return NULL;
}

void update_link(struct MyLink *p, uint16_t addr, float range, float dbm) {
  struct MyLink *temp = find_link(p, addr);
  if (temp != NULL) {
    temp->range[2] = temp->range[1];
    temp->range[1] = temp->range[0];
    temp->range[0] = (range + temp->range[1] + temp->range[2]) / 3;
    temp->dbm = dbm;
    return;
  }
  else {
    return;
  }
}

void print_link(struct MyLink *p) {
  struct MyLink *temp = p;
  while (temp->next != NULL) {
    // Serial.println("Dev %d:%d m", temp->next->anchor_addr, temp->next->range);
    // Serial.print(temp->next->anchor_addr, HEX);
    // Serial.print(" ");
    // Serial.print(temp->next->range[0]);
    // Serial.print(" ");
    // Serial.println(temp->next->dbm);
    temp = temp->next;
  }
  return;
}

void delete_link(struct MyLink *p, uint16_t addr) {
  if (addr == 0) { return; }

  struct MyLink *temp = p;
  // Loop over all known links
  while (temp->next != NULL) {
    // Once we've found the link before the link to be removed
    if(temp->next->anchor_addr == addr) {
      // Miss out the next link
      struct MyLink *del = temp->next;
      temp->next = del->next;
      // Remove the next link
      free(del);
      return;
    }
  temp = temp->next;
  }
  return;
}
