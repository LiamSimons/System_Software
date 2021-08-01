/**
 * \author Bert Van Cauter
 */
#define _GNU_SOURCE
#include "dplist.h"
#include <check.h>  
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name);
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

void setup(void) {
    // Implement pre-test setup
}

void teardown(void) {
    // Implement post-test teardown
}
START_TEST(test_ListFree)
{
    // Test free NULL, don't use callback
    dplist_t *list = NULL;
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free NULL, use callback
    list = NULL;
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free empty list, don't use callback
    list = dpl_create(element_copy, element_free, element_compare);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free empty list, use callback
    list = dpl_create(element_copy, element_free, element_compare);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // TODO : Test free with one element, also test if inserted elements are set to NULL
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element1 = (my_element_t*)malloc(sizeof(my_element_t));
    element1->name = "Bert";
    element1->id = 1;
    list = dpl_insert_at_index(list, (void*)element1, 0, true);
    dpl_free(&list,true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
    free(element1);
    // TODO : Test free with multiple element, also test if inserted elements are set to NULL
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = (my_element_t*)malloc(sizeof(my_element_t));
    element->name = "Bert";
    element->id = 1;
    my_element_t *element2 = (my_element_t*)malloc(sizeof(my_element_t));
    element2->name = "Emma";
    element2->id = 2;
    list = dpl_insert_at_index(list, element, 0, false);
    list = dpl_insert_at_index(list, element2, 1, false);
    dpl_free(&list,false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
    free(element);
    free(element2);
}
END_TEST

START_TEST(test_getElementAtReference)
{
    //at List is NULL
    dplist_t *list = NULL; 
    my_element_t *element = (my_element_t*)malloc(sizeof(my_element_t));
    element->name = "Bert";
    element->id = 1;
    ck_assert_msg(dpl_get_element_at_reference(list,NULL)==NULL, "Failure: for list is NULL should return NULL");
    dpl_free(&list, true);
    free(element);
    //at list->head == NULL 
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element1 =(my_element_t*)malloc(sizeof(my_element_t));
    element1->name = "Bert";
    element1->id = 1;
    ck_assert_msg(dpl_get_element_at_reference(list, NULL)==NULL, "Failure: for list->head is NUL should return NULL");
    dpl_free(&list,true);
    free(element1);
    //at list one element
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element2 =(my_element_t*)malloc(sizeof(my_element_t));
    element2->name = "Bert";
    element2->id = 1;
    list = dpl_insert_at_index(list, element2,0, false);
    ck_assert_msg((((my_element_t*)dpl_get_element_at_reference(list, dpl_get_reference_at_index(list,0)))->id)==1, "Failure: for list with one element should return element2");
    dpl_free(&list,false);
    free(element2);
    //at list with multiple elements    */
}
END_TEST

START_TEST(test_getLastElement)
{
    //at list is NULL
    dplist_t *list = NULL;
    ck_assert_msg(dpl_get_last_reference(list)==NULL, "Failure: expected NULL for list is NULL");
    //with one element
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element1 =(my_element_t*)malloc(sizeof(my_element_t));
    element1->name = "Bert";
    element1->id = 1;
    list = dpl_insert_at_index(list,element1,0,false);
    dplist_node_t *testnode = dpl_get_last_reference(list);
    dplist_node_t *node = dpl_get_reference_at_index(list,2);
    ck_assert_msg(testnode == node, "Failure: expected element1 but got something else");
    dpl_free(&list,false);
    free(element1);
}
END_TEST

//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

int main(void) {
    Suite *s1 = suite_create("LIST_EX3");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListFree);
    tcase_add_test(tc1_1, test_getElementAtReference);
    tcase_add_test(tc1_1, test_getLastElement);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
