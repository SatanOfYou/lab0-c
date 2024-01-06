#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *result =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!result)
        return NULL;
    INIT_LIST_HEAD(result);
    return result;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *pos, *n;
    list_for_each_entry_safe (pos, n, l, list) {
        if (pos->value)
            free(pos->value);
        free(pos);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *elem = (element_t *) malloc(sizeof(element_t));
    if (!elem)
        return false;
    elem->value = (char *) malloc(strlen(s) + 1);
    if (!elem->value) {
        free(elem);
        return false;
    }
    strncpy(elem->value, s, strlen(s) + 1);
    elem->value[strlen(s)] = '\0';
    list_add(&elem->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *elem = (element_t *) malloc(sizeof(element_t));
    if (!elem)
        return false;
    elem->value = (char *) malloc(strlen(s) + 1);
    if (!elem->value) {
        free(elem);
        return false;
    }
    strncpy(elem->value, s, strlen(s) + 1);
    elem->value[strlen(s)] = '\0';
    list_add_tail(&elem->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *elem = list_first_entry(head, element_t, list);
    if (sp)
        memset(sp, 0, bufsize);
    if (sp && (bufsize > 1))
        strncpy(sp, elem->value, bufsize - 1);
    list_del_init(head->next);
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *elem = list_last_entry(head, element_t, list);
    if (sp)
        memset(sp, 0, bufsize);
    if (sp && (bufsize > 1))
        strncpy(sp, elem->value, bufsize - 1);
    list_del_init(head->prev);
    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *temp;
    list_for_each (temp, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next, *slow = head->next;
    for (; fast != head && fast->next != head;
         fast = fast->next->next, slow = slow->next)
        ;
    list_del(slow);
    element_t *elem = list_entry(slow, element_t, list);
    free(elem->value);
    free(elem);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    if (list_is_singular(head))
        return true;
    bool flag = false;
    struct list_head *target = head->next, *temp = target->next;
    for (; temp != head; temp = temp->next) {
        element_t *elem1 = list_entry(temp, element_t, list);
        element_t *elem2 = list_entry(target, element_t, list);
        bool equal = !strcmp(elem1->value, elem2->value);
        equal = !!equal;
        if (flag || equal) {
            list_del(target);
            free(elem2->value);
            free(elem2);
        }
        if (flag ^ equal)
            flag = !flag;
        target = temp;
    }
    if (flag) {
        list_del(target);
        free(list_entry(target, element_t, list)->value);
        free(list_entry(target, element_t, list));
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *temp = head->next;
    for (; temp != head && temp->next != head;) {
        struct list_head *entry1 = temp, *entry2 = temp->next;
        temp = temp->next->next;
        list_del(entry1);
        list_add(entry1, entry2);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *pos, *n, *last = head->prev;
    for (pos = head->next->next, n = pos->next;; pos = n, n = pos->next) {
        list_del(pos);
        list_add(pos, head);
        if (pos == last)
            break;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || list_is_singular(head) || !k || k == 1)
        return;
    if (k == 2) {
        q_reverse(head);
        return;
    }
    int count = 0, times = 0;
    struct list_head *temp;
    list_for_each (temp, head)
        count++;
    times = count / k;
    struct list_head new_head, *pos = head;
    temp = head->next;
    count = 0;
    INIT_LIST_HEAD(&new_head);
    while (true) {
        if (times == 0)
            break;
        struct list_head *iter = temp->next;
        list_del(temp);
        list_add(temp, &new_head);
        if (++count == k) {
            list_splice(&new_head, pos);
            INIT_LIST_HEAD(&new_head);
            times--;
            count = 0;
            pos = iter->prev;
        }
        temp = iter;
    }
}

struct list_head *mergeTwoSingleList(struct list_head *list1,
                                     struct list_head *list2,
                                     int descend)
{
    struct list_head head, *iter = &head;
    while (list1 && list2) {
        int diff = strcmp(list_entry(list1, element_t, list)->value,
                          list_entry(list2, element_t, list)->value);
        if ((descend && diff < 0) || (!descend && diff > 0)) {
            iter->next = list2;
            list2 = list2->next;
        } else {
            iter->next = list1;
            list1 = list1->next;
        }
        iter = iter->next;
    }
    iter->next = (list1 ? list1 : list2);
    return head.next;
}

struct list_head *merge_final(struct list_head *head,
                              struct list_head *list1,
                              struct list_head *list2,
                              int descend)
{
    struct list_head *target;
    INIT_LIST_HEAD(head);
    while (list1 && list2) {
        int diff = strcmp(list_entry(list1, element_t, list)->value,
                          list_entry(list2, element_t, list)->value);
        if ((descend && diff < 0) || (!descend && diff > 0)) {
            target = list2;
            list2 = list2->next;
        } else {
            target = list1;
            list1 = list1->next;
        }
        list_add_tail(target, head);
    }
    list1 = (list1 ? list1 : list2);
    while (list1) {
        target = list1;
        list1 = list1->next;
        list_add_tail(target, head);
    }
    return head;
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    int count = 0, temp;
    struct list_head *pending = NULL, *final_head = head;
    head->prev->next = NULL;
    head = head->next;

    do {
        struct list_head **tail;
        for (temp = count, tail = &pending; temp & 1; temp >>= 1)
            tail = &(*tail)->prev;
        if (temp) {
            struct list_head *list1 = *tail, *list2 = (*tail)->prev;
            list1 = mergeTwoSingleList(list2, list1, descend);
            list1->prev = list2->prev;
            *tail = list1;
        }
        head->prev = pending;
        pending = head;
        head = head->next;
        pending->next = NULL;
        count++;
    } while (head);
    struct list_head *list = pending;
    pending = pending->prev;
    while (true) {
        struct list_head *next = pending->prev;
        if (!next)
            break;
        list = mergeTwoSingleList(list, pending, descend);
        pending = next;
    }
    merge_final(final_head, pending, list, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    struct list_head *node = head->next;
    list_del(node);
    int result = q_ascend(head);
    element_t *elem1 = list_entry(node, element_t, list);
    if (list_empty(head) ||
        strcmp(list_first_entry(head, element_t, list)->value, elem1->value) >=
            0) {
        result++;
        list_add(node, head);
    } else {
        free(list_entry(node, element_t, list)->value);
        free(list_entry(node, element_t, list));
    }
    return result;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    struct list_head *node = head->next;
    list_del(node);
    int result = q_descend(head);
    element_t *elem1 = list_entry(node, element_t, list);
    if (list_empty(head) ||
        strcmp(list_first_entry(head, element_t, list)->value, elem1->value) <=
            0) {
        result++;
        list_add(node, head);
    } else {
        free(list_entry(node, element_t, list)->value);
        free(list_entry(node, element_t, list));
    }
    return result;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_first_entry(head, queue_contex_t, chain)->size;
    struct list_head *curr = head->next;
    list_del(curr);
    int result = q_merge(head, descend);
    queue_contex_t *queue1 = list_entry(curr, queue_contex_t, chain);
    queue_contex_t *queue2 = list_entry(head->next, queue_contex_t, chain);
    result += queue1->size;
    if (queue2->q && (!list_empty(queue2->q))) {
        queue1->q->prev->next = NULL;
        queue2->q->prev->next = NULL;
        queue2->size = 0;
        merge_final(queue1->q, queue1->q->next, queue2->q->next, descend);
        INIT_LIST_HEAD(queue2->q);
        queue1->size = result;
    }
    list_add(curr, head);
    return result;
}
