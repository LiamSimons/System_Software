/**
 * \author Liam Simons
 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include "dplist.h"
#include <assert.h>
#include <check.h>
#include <stdlib.h>
#include <stdio.h>


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
    //dplist_t *list = NULL;
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

    // NO CALLBACK
    // TODO : Test free with one element, also test if inserted elements are set to NULL
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = (my_element_t*)malloc(sizeof(my_element_t));
    element->name = "Liam";
    element->id = 1;
    list = dpl_insert_at_index(list, element, 0, false);
    dpl_free(&list, false);
    ck_assert_msg(element != NULL, "Failure: expected element to not be NULL");
    ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
    // inserted NULL element
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, NULL, 0, false);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
    // TODO : Test free with multiple element, also test if inserted elements are set to NULL
    list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 0, false);
    my_element_t *element2 = (my_element_t*)malloc(sizeof(my_element_t));
    element2->name = "Bert";
    element2->id = 2;
    list = dpl_insert_at_index(list, element2, 0, false);
    dpl_free(&list, false);
    ck_assert_msg(element != NULL, "Failure: expected element to not be NULL");
    ck_assert_msg(list == NULL, "Failure: expected list to be NULL");

    // CALLBACK
     // TODO : Test free with one element, also test if inserted elements are set to NULL
    list = dpl_create(element_copy, element_free, element_compare);
    ck_assert_msg(element != NULL, "Failure: expected element to not be NULL");
    list = dpl_insert_at_index(list, element, 0, false);

    dpl_free(&list, true);
    ck_assert_msg(element == NULL, "Failure: expected element to be NULL but got: %p",element);
    ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
    free(element2);
    // inserted NULL element
    
    // list = dpl_create(element_copy, element_free, element_compare);
    // list = dpl_insert_at_index(list, NULL, 0, false);
    // dpl_free(&list, true);
    // ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
    // // TODO : Test free with multiple element, also test if inserted elements are set to NULL
    // list = dpl_create(element_copy, element_free, element_compare);
    // list = dpl_insert_at_index(list, element, 0, false);
    // list = dpl_insert_at_index(list, element2, 0, false);
    // dpl_free(&list, true);
    // ck_assert_msg(element != NULL, "Failure: expected element to not be NULL");
    // ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
}
END_TEST

START_TEST(test_Size)
{
    //NULL list
    dplist_t *list = NULL;
    ck_assert_msg(dpl_size(list) == -1, "Failure: expected size of list to be -1");
    //list with no nodes
    list = dpl_create(element_copy, element_free, element_compare);
    ck_assert_msg(dpl_size(list) == 0, "Failure: expected size of list to be 0");
    //1 node
    my_element_t *element = (my_element_t*)malloc(sizeof(my_element_t));
    element->name = "Liam";
    element->id = 1;
    list = dpl_insert_at_index(list, element, 0, false);
    ck_assert_msg(dpl_size(list) == 1, "Failure: expected size of list to be 1");
    //
    my_element_t *element2 = (my_element_t*)malloc(sizeof(my_element_t));
    element2->name = "Bert";
    element2->id = 2;
    list = dpl_insert_at_index(list, element2, 1, false);
    ck_assert_msg(dpl_size(list) == 2, "Failure: expected size of list to be 1");
    my_element_t *element3 = (my_element_t*)malloc(sizeof(my_element_t));
    element3->name = "Ernie";
    element3->id = 3;
    list = dpl_insert_at_index(list, element3, 2, false);
    ck_assert_msg(dpl_size(list) == 3, "Failure: expected size of list to be 1");
    //
    //dpl_free(&list, true);
}
END_TEST

START_TEST(test_Insert)
{
    my_element_t *element = (my_element_t*)malloc(sizeof(my_element_t));
    element->name = "A";
    element->id = 1;
    my_element_t *element2 = (my_element_t*)malloc(sizeof(my_element_t));
    element2->name = "B";
    element2->id = 2;
    my_element_t *element3 = (my_element_t*)malloc(sizeof(my_element_t));
    element3->name = "C";
    element3->id = 3;
    // NULL LIST
    //copy false
    //Test inserting at index -1
    dplist_t *result1 = dpl_insert_at_index(NULL, element, -1, false);
    ck_assert_msg(result1 == NULL, "Failure: expected list to be NULL");
    // TODO : Test inserting at index 0
    dplist_t *result2 = dpl_insert_at_index(NULL, element2, 0, false);
    ck_assert_msg(result2 == NULL, "Failure: expected list to be NULL");
    // TODO : Test inserting at index 99
    dplist_t *result3 = dpl_insert_at_index(NULL, element3, 99, false);
    ck_assert_msg(result3 == NULL, "Failure: expected list to be NULL");
    // element = NULL
    //dplist_t *result4 = dpl_insert_at_index(NULL, NULL, 0, false);
    ck_assert_msg(result3 == NULL, "Failure: expected list to be NULL");
    
    //copy true
    result1 = dpl_insert_at_index(NULL, element2, -1, true);
    ck_assert_msg(result1 == NULL, "Failure: expected list to be NULL");
    
    // EMPTY LIST
    //copy false
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    list = dpl_insert_at_index(list, element, 99, false);
    ck_assert_msg(dpl_size(list) == 1, "Failure: expected size of list to be 1 but got: %i", dpl_size(list));
    list = dpl_insert_at_index(list, element2, 99, false);
    ck_assert_msg(dpl_size(list) == 2, "Failure: expected size of list to be 1 but got: %i", dpl_size(list));
    ck_assert_msg(dpl_get_element_at_index(list, 1) == element2, "Failure: expected list[0] to be element2");
    ck_assert_msg(dpl_get_element_at_index(list, 0) == element, "Failure: expected list[1] to be element");
    list = dpl_insert_at_index(list, NULL, 99, false);
    ck_assert_msg(dpl_size(list) == 2, "Failure: expected size to be 3");
    // ck_assert_msg(dpl_get_element_at_index(list, 2) == element3, "Failure: expected list[2] to be element3");

    //copy true
     dplist_t *list2 = dpl_create(element_copy, element_free, element_compare);
    list2 = dpl_insert_at_index(list2, element, 99, true);
    ck_assert_msg(dpl_size(list2) == 1, "Failure: expected size of list to be 1 but got: %i", dpl_size(list));
    list2 = dpl_insert_at_index(list2, element2, 99, true);
    ck_assert_msg(dpl_size(list2) == 2, "Failure: expected size of list to be 1 but got: %i", dpl_size(list));
    ck_assert_msg(dpl_get_element_at_index(list2, 1) != element2, "Failure: expected list[0] to be element2");
    ck_assert_msg(dpl_get_element_at_index(list2, 0) != element, "Failure: expected list[1] to be element");
    //ck_assert_msg(((my_element_t *)dpl_get_reference_at_index(list2, 0))->name == "A", "Failure: expected name of first element to be A but got: %s", (my_element_t *)dpl_get_reference_at_index(list2, 0));
    list2 = dpl_insert_at_index(list2, NULL, 99, true);
    ck_assert_msg(dpl_size(list2) == 2, "Failure: expected size to be 3");

    //freeing elements
    // element_free(&element);
    // element_free(&element2);
    // element_free(&element3);
}
END_TEST

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
    tcase_add_test(tc1_1, test_Size);
    tcase_add_test(tc1_1, test_Insert);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
