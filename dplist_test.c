/**
* \author Liam Simons
*/

#define _GNU_SOURCE
#include "dplist.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// CALLBACK - PRE - POST - TEST ---------------------------------------------------------//

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


//--------------------------------------------------------------------------------------//
// TEST 1 - dpl_create();
// Case 1
START_TEST(T1C1){
	dplist_t *list = dpl_create();
	ck_assert_msg(list != NULL, "Failure: expected result to not be NULL");
	dpl_free(&list);
}END_TEST

//--------------------------------------------------------------------------------------//
// TEST 2 - dpl_free();
// Case 1
START_TEST(T2C1){
	dplist_t *list = NULL;
	dpl_free(&list);
	ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
}END_TEST
// Case 2
START_TEST(T2C2){
	dplist_t* list = dpl_create();
	dpl_free(&list);
	printf("LIST == %p", list);
	//ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
}END_TEST
// Case 3
START_TEST(T2C3){
	dplist_t* list = dpl_create();
	char x;
	x = 'a';
	list = dpl_insert_at_index(list, &x, 0);
	dpl_free(&list);
	ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
}END_TEST
// Case 3
START_TEST(T2C4){
	dplist_t* list = dpl_create();
	char x, y;
	x = 'a';
	y = 'b';
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	dpl_free(&list);
	ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
}END_TEST

//--------------------------------------------------------------------------------------//
// TEST 3 - dpl_size();
// Case 1 - list == NULL
START_TEST(T3C1){
	dplist_t *list = NULL;
	int size = dpl_size(list);
	ck_assert_msg(size == -1, "Failure: expected -1 on null list");
	dpl_free(&list);
}END_TEST
// CASE 2 - empty list
START_TEST(T3C2){
	dplist_t* list = dpl_create();
	int size = dpl_size(list);
	ck_assert_msg(size == 0, "Failure: expected 0 on empty list");
	dpl_free(&list);
}END_TEST
// CASE 3 - 1 element in list
START_TEST(T3C3){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	int size = dpl_size(list);
	ck_assert_msg(size == 1, "Failure: expected 1 on list with one element");
	dpl_free(&list);
}END_TEST
// CASE 4 - multiple element list
START_TEST(T3C4){
	dplist_t* list = dpl_create();
	char  x, y;
	x = 1;
	y = 2;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	int size = dpl_size(list);
	ck_assert_msg(size == 2, "Failure: expected 1 on list with one element");	
	dpl_free(&list);
}END_TEST

//--------------------------------------------------------------------------------------//
// TEST 4 - dpl_insert_at_index(list, element, index);
// Case 1 - list == NULL
START_TEST(T4C1){
	dplist_t* list = NULL;
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	ck_assert_msg(list == NULL, "Failure: expected list to be null");
	dpl_free(&list);
}END_TEST
// Case 2 - list == empty
START_TEST(T4C2){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	int size = dpl_size(list);
	ck_assert_msg(size == 1, "Failure: expected size to be 1");
	dpl_free(&list);
}END_TEST
// Case 3 - insert at index 0
// tested above
// Case 4 - insert in between elements
START_TEST(T4C4){
	dplist_t* list = dpl_create();
	char x, y, z, a;
	x = 1;
	y = 2;
	z = 3;
	a = 4;
	list = dpl_insert_at_index(list, &x, 0);	// 1
	list = dpl_insert_at_index(list, &y, 0);	// 2-1
	list = dpl_insert_at_index(list, &z, 1); // 2-3-1
	list = dpl_insert_at_index(list, &a, 3); // 2-3-1-4
	int size = dpl_size(list);
	ck_assert_msg(size == 4, "Failure: expected size to be 4");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 1) == 3, "Failure: expected element to be 3");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 0) == 2, "Failure: expected element to be 2");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 3) == 4, "Failure: expected element to be 4");
	dpl_free(&list);
}END_TEST
// Case 5 - last index
// tested above
// Case 6 - negative index
START_TEST(T4C6){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, -1);
	int size = dpl_size(list);
	ck_assert_msg(size == 1, "Failure: expected size to be 1");
	dpl_free(&list);
}END_TEST
// Case 7 - insert at bigger index 
START_TEST(T4C7){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 99);
	int size = dpl_size(list);
	ck_assert_msg(size == 1, "Failure: expected size to be 1");
	dpl_free(&list);
}END_TEST

//--------------------------------------------------------------------------------------//
// TEST 5 - dpl_remove_at_index(list, index);
// Case 1 - list == NULL
START_TEST(T5C1){
	dplist_t* list = NULL;
	list = dpl_remove_at_index(list, 0);
	ck_assert_msg(list == NULL, "Failure: expected list to be NULL");
	dpl_free(&list);
}END_TEST
//Case 2 - list == empty
START_TEST(T5C2){
	dplist_t* list = dpl_create();
	dplist_t* temp = dpl_remove_at_index(list, 0);
	//printf("LIST = %p \tTEMP = %p\n", list, temp);
	ck_assert_msg(list == temp, "Failure: expected list to be unchanged");
	int size = dpl_size(list);
	ck_assert_msg(size == 0, "Failure: expected size to be 0");
	dpl_free(&list);
}END_TEST
//Case 3 - list has 1 element
START_TEST(T5C3){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	ck_assert_msg(dpl_size(list) == 1, "Failure: element not inserted in list");
	list = dpl_remove_at_index(list, 0);
	ck_assert_msg(dpl_size(list) == 0, "Failure: element not deleted from list");
	dpl_free(&list);
}END_TEST
//Case 4 - list has multiple elements
START_TEST(T5C4){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	list = dpl_remove_at_index(list, 1);
	ck_assert_msg(dpl_size(list) == 2, "Failure: expected size to be 2");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 1) == 3, "Failure: expected second element in list to be 3");
	list = dpl_remove_at_index(list, 0);
	ck_assert_msg(dpl_size(list) == 1, "Failure: expected size to be 1");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 0) == 3, "Failure: expected remaining element to be 3");
	dpl_free(&list);
}END_TEST
// Case 5 - index == 0
// tested above
// Case 6 - index in between elements
// tested above
// Case 7 - index is last index
START_TEST(T5C7){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	list = dpl_remove_at_index(list, 2);
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, dpl_size(list) - 1) == 2, "Failure: expected remaining element to be 2 but got: %d", dpl_size(list));
	dpl_free(&list);
}END_TEST
// Case 8 - index is negative
START_TEST(T5C8){
	dplist_t* list = dpl_create();
	char x, y;
	x = 1;
	y = 2;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_remove_at_index(list, -10);
	ck_assert_msg(dpl_size(list) == 1, "Failure: expected size of list to be 1");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 0) == 2, "Failure: expected remaining element to be 2");
	dpl_free(&list); 
}END_TEST
// Case 9 - index >= dpl_size(list)
START_TEST(T5C9){
	dplist_t* list = dpl_create();
	char x, y;
	x = 1;
	y = 2;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_remove_at_index(list, 10);
	ck_assert_msg(dpl_size(list) == 1, "Failure: expected size of list to be 1");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 0) == 1, "Failure: expected remaining element to be 1");
	dpl_free(&list); 
}END_TEST

//--------------------------------------------------------------------------------------//
// TEST 6 - dpl_get_reference_at_index(list, index);
// Case 1 - list == NULL
START_TEST(T6C1){
	dplist_t* list = NULL;
	ck_assert_msg(dpl_get_reference_at_index(list, 0) == NULL, "Failure: expected to be NULL");
	dpl_free(&list);
}END_TEST
// Case 2 - list == empty
START_TEST(T6C2){
	dplist_t* list = dpl_create();
	ck_assert_msg(dpl_get_reference_at_index(list, 0) == NULL, "Failure: expected to be NULL");
	dpl_free(&list);
}END_TEST
// Case 3 - list has 1 element
START_TEST(T6C3){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	dplist_node_t* node = dpl_get_reference_at_index(list, 0);
	ck_assert_msg(node != NULL/*(node->element) == 1*/, "Failure: expected reference to not be NULL");
	dpl_free(&list);
}END_TEST
// Case 4 - list has multiple elements
START_TEST(T6C4){
	dplist_t* list = dpl_create();
	char x, y;
	x = 1;
	y = 2;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	dplist_node_t* node = dpl_get_reference_at_index(list, 1);
	ck_assert_msg(node != NULL/*(node->element) == 2*/, "Failure: expected reference to not be NULL");
	dpl_free(&list);
}END_TEST
// Case 5 - index == 0 
// tested above
// Case 6 - index in between elements
START_TEST(T6C6){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	dplist_node_t* node = dpl_get_reference_at_index(list, 1);
	ck_assert_msg(node != NULL/*(node->element) == 2*/, "Failure: expected reference to not be NULL");
	dpl_free(&list);
}END_TEST
// Case 7 - index == dpl_size(list) - 1
START_TEST(T6C7){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	dplist_node_t* node = dpl_get_reference_at_index(list, dpl_size(list) - 1);
	ck_assert_msg(node != NULL, "Failure: expected reference to not be NULL");
	dpl_free(&list);
}END_TEST
// Case 8 - index is negative
START_TEST(T6C8){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	dplist_node_t* node = dpl_get_reference_at_index(list, -10);
	ck_assert_msg(node != NULL, "Failure: expected reference to not be NULL");
	dpl_free(&list);
}END_TEST
// Case 9 - index >= dpl_size(list)
START_TEST(T6C9){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	dplist_node_t* node = dpl_get_reference_at_index(list, 99);
	ck_assert_msg(node != NULL, "Failure: expected reference to not be NULL");
	dpl_free(&list);
}END_TEST

//--------------------------------------------------------------------------------------//
// TEST 7 - dpl_get_element_at_index(list, index)
// Case 1 - list == NULL
START_TEST(T7C1){
	dplist_t* list = NULL;
	ck_assert_msg(dpl_get_element_at_index(list, 0) == NULL, "Failure: expected to be 0");
	dpl_free(&list);
}END_TEST
// Case 2 - list == empty
START_TEST(T7C2){
	dplist_t* list = dpl_create();
	ck_assert_msg(dpl_get_element_at_index(list, 0) == NULL, "Failure: expected to be 0");
	dpl_free(&list);
}END_TEST
// Case 3 - list has 1 element
START_TEST(T7C3){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 0) == 1, "Failure: expected to be 1");
	dpl_free(&list);
}END_TEST
// Case 4 - list has multiple elements
START_TEST(T7C4){
	dplist_t* list = dpl_create();
	char x, y;
	x = 1;
	y = 2;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 0) == 1, "Failure: expected to be 1");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 1) == 2, "Failure: expected to be 2");
	dpl_free(&list);
}END_TEST
// Case 5 - index == 0
// tested above
// Case 6 - index in between elements
START_TEST(T7C6){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 0) == 1, "Failure: expected to be 1");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 1) == 2, "Failure: expected to be 2");
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 2) == 3, "Failure: expected to be 3");
	dpl_free(&list);
}END_TEST
// Case 7 - index == dpl_size(list) - 1
START_TEST(T7C7){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, dpl_size(list)-1) == 3, "Failure: expected to be 3");
	dpl_free(&list);
}END_TEST
// Case 8 - index is negative
START_TEST(T7C8){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, -10) == 1, "Failure: expected to be 1");
	dpl_free(&list);
}END_TEST
// Case 9 - index >= dpl_size(list)
START_TEST(T7C9){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	ck_assert_msg(*(char*)dpl_get_element_at_index(list, 99) == 3, "Failure: expected to be 3");
	dpl_free(&list);
}END_TEST

//--------------------------------------------------------------------------------------//
// TEST 8 - dpl_get_index_of_element(list, element);
// Case 1 - list == NULL
START_TEST(T8C1){
	dplist_t* list = NULL;
	char x;
	int index = dpl_get_index_of_element(list, &x);
	ck_assert_msg(index == -1, "Failure: expected to be -1 but got: %d", index);
	dpl_free(&list);
}END_TEST
// Case 2 - list == empty
START_TEST(T8C2){
	dplist_t* list = dpl_create();
	char x;
	int index = dpl_get_index_of_element(list, &x);
	ck_assert_msg(index == -1, "Failure: expected to be -1 but got: %d", index);
	dpl_free(&list);
}END_TEST
// Case 3 - list has 1 element
START_TEST(T8C3){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	int index = dpl_get_index_of_element(list, &x);
	ck_assert_msg(index == 0, "Failure: expected to be 0 but got: %d", index);
	dpl_free(&list);
}END_TEST
// Case 4 - list has multiple elements
START_TEST(T8C4){
	dplist_t* list = dpl_create();
	char x, y, z;
	x = 1;
	y = 2;
	z = 3;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 1);
	list = dpl_insert_at_index(list, &z, 2);
	int index = dpl_get_index_of_element(list, &z);
	ck_assert_msg(index == 2, "Failure: expected to be 2 but got: %d", index);
	dpl_free(&list);
}END_TEST
// Case 5 - element is in the list 1 time
// tested above
// Case 6 - element == NULL
START_TEST(T8C6){
	dplist_t* list = dpl_create();
	char x;
	x = 1;
	list = dpl_insert_at_index(list, &x, 0);
	int index = dpl_get_index_of_element(list, NULL);
	ck_assert_msg(index == -1, "Failure: expected to be -1 but got: %d", index);
	dpl_free(&list);
}END_TEST
// Case 7 - element is in list multiple times
START_TEST(T8C7){
	dplist_t* list = dpl_create();
	char x, y;
	x = 1;
	y = 2;
	list = dpl_insert_at_index(list, &x, 0);
	list = dpl_insert_at_index(list, &y, 0);
	list = dpl_insert_at_index(list, &x, 0);
	int index = dpl_get_index_of_element(list, &x);
	ck_assert_msg(index == 0, "Failure: expected to be 0 but got: %d", index);
	dpl_free(&list);
}END_TEST

// MAIN --------------------------------------------------------------------------------//

int main(void){
	Suite *s1 = suite_create("LIST_EX1");
	TCase *tc1_1 = tcase_create("Core");
	SRunner *sr = srunner_create(s1);
	int nf;

	suite_add_tcase(s1, tc1_1);
	tcase_add_checked_fixture(tc1_1, setup, teardown);

	// ALL TESTS
	tcase_add_test(tc1_1, T1C1);
	tcase_add_test(tc1_1, T2C1);
	tcase_add_test(tc1_1, T2C2);
	tcase_add_test(tc1_1, T2C3);
	tcase_add_test(tc1_1, T2C4);
	tcase_add_test(tc1_1, T3C1);
	tcase_add_test(tc1_1, T3C2);
	tcase_add_test(tc1_1, T3C3);
	tcase_add_test(tc1_1, T3C4);
	tcase_add_test(tc1_1, T4C1);
	tcase_add_test(tc1_1, T4C2);
	tcase_add_test(tc1_1, T4C4);
	tcase_add_test(tc1_1, T4C6);
	tcase_add_test(tc1_1, T4C7);
	tcase_add_test(tc1_1, T5C1);
	tcase_add_test(tc1_1, T5C2);
	tcase_add_test(tc1_1, T5C3);
	tcase_add_test(tc1_1, T5C4);
	tcase_add_test(tc1_1, T5C7);
	tcase_add_test(tc1_1, T5C8);
	tcase_add_test(tc1_1, T5C9);
	tcase_add_test(tc1_1, T6C1);
	tcase_add_test(tc1_1, T6C2);
	tcase_add_test(tc1_1, T6C3);
	tcase_add_test(tc1_1, T6C4);
	tcase_add_test(tc1_1, T6C6);
	tcase_add_test(tc1_1, T6C7);
	tcase_add_test(tc1_1, T6C8);
	tcase_add_test(tc1_1, T6C9);
	tcase_add_test(tc1_1, T7C1);
	tcase_add_test(tc1_1, T7C2);
	tcase_add_test(tc1_1, T7C3);
	tcase_add_test(tc1_1, T7C4);
	tcase_add_test(tc1_1, T7C6);
	tcase_add_test(tc1_1, T7C7);
	tcase_add_test(tc1_1, T7C8);
	tcase_add_test(tc1_1, T7C9);
	tcase_add_test(tc1_1, T8C1);
	tcase_add_test(tc1_1, T8C2);
	tcase_add_test(tc1_1, T8C3);
	tcase_add_test(tc1_1, T8C4);
	tcase_add_test(tc1_1, T8C6);
	tcase_add_test(tc1_1, T8C7);

	srunner_run_all(sr, CK_VERBOSE);

	nf = srunner_ntests_failed(sr);
	srunner_free(sr);

	return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}