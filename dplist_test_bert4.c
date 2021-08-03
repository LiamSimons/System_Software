/**
 * \author Liam Van Cauter
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
    element1->name = "Liam";
    element1->id = 1;
    list = dpl_insert_at_index(list, (void*)element1, 0, true);
    dpl_free(&list,true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
    free(element1);
    // TODO : Test free with multiple element, also test if inserted elements are set to NULL
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = (my_element_t*)malloc(sizeof(my_element_t));
    element->name = "Liam";
    element->id = 1;
    my_element_t *element2 = (my_element_t*)malloc(sizeof(my_element_t));
    element2->name = "Max";
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
    element->name = "Liam";
    element->id = 1;
    ck_assert_msg(dpl_get_element_at_reference(list,NULL)==NULL, "Failure: for list is NULL should return NULL");
    dpl_free(&list, true);
    free(element);
    //at list->head == NULL 
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element1 =(my_element_t*)malloc(sizeof(my_element_t));
    element1->name = "Liam";
    element1->id = 1;
    ck_assert_msg(dpl_get_element_at_reference(list, NULL)==NULL, "Failure: for list->head is NUL should return NULL");
    dpl_free(&list,true);
    free(element1);
    //at list one element
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element2 =(my_element_t*)malloc(sizeof(my_element_t));
    element2->name = "Liam";
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
    element1->name = "Liam";
    element1->id = 1;
    list = dpl_insert_at_index(list,element1,0,false);
    dplist_node_t *testnode = dpl_get_last_reference(list);
    dplist_node_t *node = dpl_get_reference_at_index(list,2);
    ck_assert_msg(testnode == node, "Failure: expected element1 but got something else");
    dpl_free(&list,false);
    free(element1);
}
END_TEST

START_TEST(test_get_element_at_reference){
	dplist_t* list = NULL;
	ck_assert_msg(dpl_get_element_at_reference(list, NULL) == NULL, "Failure: expected NULL");
	list = dpl_create(element_copy, element_free, element_compare);
	my_element_t *element = (my_element_t*)malloc(sizeof(my_element_t));
    element->name = "Liam";
    element->id = 1;
	list = dpl_insert_at_index(list, element, 0, false);
	ck_assert_msg(dpl_size(list) == 1, "Failure: expected size to be 1 but got %d", dpl_size(list));
	dplist_node_t* node = dpl_get_reference_at_index(list, 0);
	my_element_t* returned_element = (my_element_t*)dpl_get_element_at_reference(list, node);
	ck_assert_msg(returned_element->id == 1, "Failure: expected returned element id to be 1");
	ck_assert_msg(strcmp(returned_element->name, "Liam") == 0, "Failure: expected returned element name to be Liam");
	dpl_free(&list, false);
	ck_assert_msg(list == NULL, "Failure: expected list to be NuLL");
	free(element);
}END_TEST

START_TEST(test_get_first_last_reference){
	dplist_t* list = NULL;
	ck_assert_msg(dpl_get_first_reference(list) == NULL, "Failure: expected NULL");
	ck_assert_msg(dpl_get_last_reference(list) == NULL, "Failure: expected NULL");
	list = dpl_create(element_copy, element_free, element_compare);
	my_element_t *element = (my_element_t*)malloc(sizeof(my_element_t));
    element->name = "Liam";
    element->id = 1;
	my_element_t *element2 = (my_element_t*)malloc(sizeof(my_element_t));
    element2->name = "Simons";
    element2->id = 2;
	list = dpl_insert_at_index(list, element, 0, false);
	list = dpl_insert_at_index(list, element2, 1, false);
	my_element_t* dummy = dpl_get_element_at_index(list, 0);
	ck_assert_msg(dummy->id == 1, "Failure: expected dummy id to be 1 but got %d", dummy->id);

	dplist_node_t* node = dpl_get_first_reference(list);
	my_element_t* returned_element = (my_element_t*)dpl_get_element_at_reference(list, node);
	ck_assert_msg(returned_element->id == 1, "Failure: expected returned element id to be 1 but got %d", returned_element->id);
	ck_assert_msg(strcmp(returned_element->name, "Liam") == 0, "Failure: expected returned element name to be Liam");
	dplist_node_t* node2 = dpl_get_last_reference(list);
	my_element_t* returned_element2 = (my_element_t*)dpl_get_element_at_reference(list, node2);
	ck_assert_msg(returned_element2->id == 2, "Failure: expected returned element id to be 2 but got %d", returned_element->id);
	ck_assert_msg(strcmp(returned_element2->name, "Simons") == 0, "Failure: expected returned element name to be Simons");

	// NEXT reference
	dplist_node_t* next_node = dpl_get_next_reference(list, node);
	my_element_t* returned_element3 = (my_element_t*)dpl_get_element_at_reference(list, next_node);
	ck_assert_msg(returned_element3->id == 2, "Failure: expected returned element id to be 2 but got %d", returned_element->id);
	ck_assert_msg(strcmp(returned_element3->name, "Simons") == 0, "Failure: expected returned element name to be Simons");
	// NEXT reference without any next
	dplist_node_t* no_node = dpl_get_next_reference(list, node2);
	my_element_t* returned_element4 = (my_element_t*)dpl_get_element_at_reference(list, no_node);
	ck_assert_msg(returned_element4 == NULL, "Failure: expected returned element id to be 2 but got %d", returned_element->id);
	// NEXT with reference NULL
	dplist_node_t* null_node = dpl_get_next_reference(list, NULL);
	ck_assert_msg(null_node == NULL, "Failure: expected returned element id to be 2 but got %d", returned_element->id);

	// PREVIOUS reference
	dplist_node_t* prev_node = dpl_get_previous_reference(list, node2);
	my_element_t* returned_element5 = (my_element_t*)dpl_get_element_at_reference(list, prev_node);
	ck_assert_msg(returned_element5->id == 1, "Failure: expected returned element id to be 2 but got %d", returned_element->id);
	ck_assert_msg(strcmp(returned_element5->name, "Liam") == 0, "Failure: expected returned element name to be Simons");
	
	// GET REFERENCE OF ELEMENT
	dplist_node_t* ref_of_el = dpl_get_reference_of_element(list, returned_element5);
	ck_assert_msg(ref_of_el == prev_node, "Failure: expected ref_of_el and prev_node to be equal");
	
	dpl_free(&list, false);
	free(element);
	free(element2);
}END_TEST



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
	tcase_add_test(tc1_1, test_get_element_at_reference);
	tcase_add_test(tc1_1, test_get_first_last_reference);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
