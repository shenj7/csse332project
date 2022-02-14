#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"

void init_list_head(struct list_head *list)
{
  list->next = list;
  list->prev = list;
}

// for internal use only
// insert new between prev and next
static inline void __list_add(struct list_head *new, struct list_head *prev,
                              struct list_head *next)
{
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
  next->prev = prev;
  prev->next = next;
}

void list_add(struct list_head *head, struct list_head *new)
{
  __list_add(new, head, head->next);
}

void list_add_tail(struct list_head *head, struct list_head *new)
{
  __list_add(new, head->prev, head);
}

void list_del(struct list_head *entry)
{
  __list_del(entry->prev, entry->next);
  entry->prev = entry->next = entry;
}

void list_del_init(struct list_head *entry)
{
  __list_del(entry->prev, entry->next);
  init_list_head(entry);
}

