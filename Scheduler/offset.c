#include <stdio.h>
#include <stdlib.h>
 
#define offset_of(_type, _member) ((size_t)&(((_type*)0)->_member))
#define baseObj(_type, _obj, _member) ((_type*)(((void*)(_obj)) - offset_of(_type,_member)))

 
typedef struct list_elem {
    struct list_elem *prev;
    struct list_elem *next;
} list_elem;

typedef list_elem list_t; 

#define INIT_LIST(var) { &var, &var }  // points to itself hence empty
#define INIT_LIST_ELEM(var) do { var.next = &var; var.prev = &var; } while (0)

int list_empty(list_t *list)
{
    return (list->next == (list_elem*)list);
}

int list_add_tail(list_t *list, list_elem *elem)
{
    list->prev->next = elem;
    elem->prev = list->prev;
    elem->next = list;
    list->prev = elem;
}

int list_del(list_elem *t)
{
    // will show some other time
}

#define list_add_obj(_lst, _obj, _member) \
                list_add_tail(_lst, (_obj)->_member)

#define for_list(_lst, _var, _member ) \
          for ((_var) = ((typeof(_var)) baseObj(typeof(*(_var)),(_lst)->next, _member)); \
               ((void*)&(_var)->_member != (void*)(_lst)); \
                (_var) = baseObj(typeof(*(_var)),(_var)->_member.next, _member) )

typedef struct Process {
    int pid;
    list_elem list1;
    list_elem list2;
    list_elem list3;
} Process; 

Process *newProc(int pid)
{
     Process *proc = malloc(sizeof(Process));
     proc->pid = pid;
     INIT_LIST_ELEM(proc->list1);
     INIT_LIST_ELEM(proc->list2);
     return proc;
}

list_t mylist1 = INIT_LIST(mylist1); 
list_t mylist2 = INIT_LIST(mylist2); 
list_t mylist3 = INIT_LIST(mylist3); 

int main()
{
   Process *proc1, *proc2 , *proc3, *proc4;
   Process *p;

   printf("offset of list1: %lu\n", offset_of(Process, list1));
   printf("offset of list2: %lu\n", offset_of(Process, list2));

   proc1 = newProc(1);
   proc2 = newProc(2);
   proc3 = newProc(3);

   // create list 1 out of [1,2]
   list_add_tail(&mylist1, (list_elem*) &proc1->list1);
   list_add_tail(&mylist1, (list_elem*) &proc2->list1);

   // create list 2 out of [2,3]
   list_add_tail(&mylist2, (list_elem*) &proc2->list2);
   list_add_tail(&mylist2, (list_elem*) &proc3->list2);

   // create list 3 out of [1,3]
   list_add_tail(&mylist3, (list_elem*) &proc1->list3);
   list_add_tail(&mylist3, (list_elem*) &proc3->list3);

   proc4 = baseObj(Process, mylist3.next , list3);
   printf("%p %p %p %p\n", proc1, proc2, proc3, proc4);
   printf("%d %d %d\n", (proc4 == proc1), (proc4 == proc2), (proc4 == proc3));

   proc4 = baseObj(Process, mylist2.next , list2);
   printf("%p %p %p %p\n", proc1, proc2, proc3, proc4);
   printf("%d %d %d\n", (proc4 == proc1), (proc4 == proc2), (proc4 == proc3));

   printf("list1: ");
   for_list(&mylist1, p, list1) 
	printf("%d ", p->pid);
   printf("\n");

   printf("list2: ");
   for_list(&mylist2, p, list2) 
	printf("%d ", p->pid);
   printf("\n");

   printf("list3: ");
   for_list(&mylist3, p, list3) 
	printf("%d ", p->pid);
   printf("\n");

   return 0;
}
