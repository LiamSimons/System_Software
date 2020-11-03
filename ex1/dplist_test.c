/**
 * \author Liam Simons
 */

#include "dplist.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>

void setup(void) {
    // Implement pre-test setup
}

void teardown(void) {
    // Implement post-test teardown
}
START_TEST(test_ListCreate)
    {
        dplist_t *list = dpl_create();
        ck_assert_msg(list != NULL, "Failure: expected result to not be NULL");
        dpl_free(&list);
        //ck_assert_msg(list->head == NULL, "Failure: expected result to be NULL");
    }
END_TEST

START_TEST(test_ListFree)
    {
        // Test free NULL
        dplist_t *list = NULL;
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list
        list = dpl_create();
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
        // TODO : Test free with one element
        list = dpl_create();
        dpl_insert_at_index(list, 1, 0);
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
        // TODO : Test free with multiple element
        list = dpl_create();
        dpl_insert_at_index(list, 1, 0);
        dpl_insert_at_index(list, 2, 0);
        dpl_insert_at_index(list, 3, 0);
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
    }
END_TEST

START_TEST(test_ListInsertAtIndexListNULL)
    {
        // Test inserting at index -1
        dplist_t *result = dpl_insert_at_index(NULL, 'A', -1);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        // TODO : Test inserting at index 0
        result = dpl_insert_at_index(NULL, 'A', 0);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        // TODO : Test inserting at index 99
        result = dpl_insert_at_index(NULL, 'A', 99);
        ck_assert_msg(result == NULL, "Failure: expected list ot be NULL");
        dpl_free(&result);
    }
END_TEST

START_TEST(test_ListInsertAtIndexListEmpty)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dpl_insert_at_index(list, 'A', -1);
    ck_assert_msg(dpl_size(list) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(list));
    
    // TODO : Test inserting at index 0
    dpl_insert_at_index(list, 'B', 0);
    ck_assert_msg(dpl_size(list) == 2, "Failure: expected list to have size of 2, got a size of %d",
                                         dpl_size(list));
    // TODO : Test inserting at index 99
    dpl_insert_at_index(list, 'C', 99);
    ck_assert_msg(dpl_size(list) == 3, "Failure: expected list to have size of 3, got a size of %d",
                                         dpl_size(list));
    dpl_free(&list);
}
END_TEST

START_TEST(test_RemoveAtIndexONE)
{
    //remove at index = 0
    dplist_t *list = dpl_create();
    dpl_insert_at_index(list, 'A', 0);
    dpl_remove_at_index(list, 0);
    ck_assert_msg(dpl_size(list) == 0, "Failure: expected list to have size of 0, got a size of %d",
                                         dpl_size(list));
    ck_assert_msg(dpl_check_head(list) == 0, "Failure: expected list head to be NULL");

    //remove at index = -1
    dpl_insert_at_index(list, 'B', 0);
    dpl_remove_at_index(list, -1);
    ck_assert_msg(dpl_size(list) == 0, "Failure: expected list to have size of 0, got a size of %d",
                                         dpl_size(list));
    ck_assert_msg(dpl_check_head(list) == 0, "Failure: expected list head to be NULL");

    //remove at index = 99;
    /*dpl_insert_at_index(list, 'C', 0);
    dpl_remove_at_index(list, 99);
    ck_assert_msg(dpl_size(list) == 0, "Failure: expected list to have size of 0, got a size of %d",
                                         dpl_size(list));
    ck_assert_msg(dpl_check_head(list) == 0, "Failure: expected list head to be NULL");

    //free*/
    dpl_free(&list);
}
END_TEST

START_TEST(test_RemoveAtIndexMUL)
{
    //remove at index = 0
    dplist_t *list = dpl_create();
    dpl_insert_at_index(list, 'C', 0);
    dpl_insert_at_index(list, 'B', 0);
    dpl_insert_at_index(list, 'A', 0);
    ck_assert_msg(dpl_size(list) == 3, "Failure: expected list to have size of 3, got a size of %d",
                                         dpl_size(list));
    dpl_remove_at_index(list, 0);
    ck_assert_msg(dpl_size(list) == 2, "Failure: expected list to have size of 2, got a size of %d",
                                         dpl_size(list));
    ck_assert_msg(dpl_get_element_at_index(list, 0) == 'B', "Failure: expected element to be 'B'");
    ck_assert_msg(dpl_get_element_at_index(list, 1) == 'C', "Failure: expected element to be 'C'");

    //remove at index = -1
    dpl_remove_at_index(list, -1);
    ck_assert_msg(dpl_size(list) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(list));
    ck_assert_msg(dpl_get_element_at_index(list, 0) == 'C', "Failure: expected element to be 'C'");

    //remove at index = 99
    dpl_insert_at_index(list, 'B', 0);
    dpl_remove_at_index(list, 99);
    ck_assert_msg(dpl_size(list) == 1, "Failure: expected list to have size of 1, got a size of %d",
                                         dpl_size(list));
    ck_assert_msg(dpl_get_element_at_index(list, 0) == 'B', "Failure: expected element to be 'B'");


    dpl_free(&list);
}
END_TEST

START_TEST(test_ElementAtIndex)
{
    dplist_t *list = dpl_create();
    dpl_insert_at_index(list, 'A', 0);
    ck_assert_msg(dpl_get_element_at_index(list, 0) == 'A', "Failure: expected element to be 'A'");
    dpl_insert_at_index(list, 'Z', -20);
    ck_assert_msg(dpl_get_element_at_index(list, 0) == 'Z', "Failure: expected element to be 'Z'");
    dpl_insert_at_index(list, 'E', 99);
    ck_assert_msg(dpl_get_element_at_index(list, 99) == 'E', "Failure: expected element to be 'Z'");


    dpl_free(&list);
}
END_TEST

START_TEST(test_Size)
{
    dplist_t *list = dpl_create();
    ck_assert_msg(dpl_size(list) == 0, "Failure: expected size of list to be 0 but got: %i", dpl_size(list));
}
END_TEST

START_TEST(test_GetReferenceAtIndex)
{
    dplist_t *list = dpl_create();
    dpl_insert_at_index(list, 'A', 0);
    dpl_insert_at_index(list, 'Z', -20);
    dpl_insert_at_index(list, 'E', 99);
    //ZAE

    ck_assert_msg(dpl_get_reference_at_index(list, -1) == dpl_get_reference_at_index(list, 0), 
        "Failure: expected reference of -1 and 0 to be the same");
    ck_assert_msg(dpl_get_reference_at_index(list, 2) == dpl_get_reference_at_index(list, 99), 
        "Failure: expected reference of 2 and 99 to be the same");
}
END_TEST

START_TEST(test_IndexOfElement)
{
    dplist_t *list = dpl_create();
    dpl_insert_at_index(list, 'A', 0);
    dpl_insert_at_index(list, 'Z', -20);
    dpl_insert_at_index(list, 'E', 99);
    //ZAE
    ck_assert_msg(dpl_size(list) == 3, "Failure: expected size of list to be 3 but got: %i", dpl_size(list));
    ck_assert_msg(dpl_get_index_of_element(list, 'A') == 1, "Failure: expected index of A to be 1 but got: %i",
        dpl_get_index_of_element(list, 'A'));
    ck_assert_msg(dpl_get_index_of_element(list, 'Z') == 0, "Failure: expected index of Z to be 0 but got: %i",
        dpl_get_index_of_element(list, 'Z'));
    ck_assert_msg(dpl_get_index_of_element(list, 'E') == 2, "Failure: expected index of E to be 2 but got: %i",
        dpl_get_index_of_element(list, 'E'));
    dpl_free(&list);
}
END_TEST
//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

int main(void) {
    Suite *s1 = suite_create("LIST_EX1");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListCreate);                   //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_ListFree);                     //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_ListInsertAtIndexListNULL);    //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_ListInsertAtIndexListEmpty);   //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_RemoveAtIndexONE);             //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_RemoveAtIndexMUL);             //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_ElementAtIndex);               //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_Size);                         //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_IndexOfElement);               //no memory leaks possible - Valgrind checked
    tcase_add_test(tc1_1, test_GetReferenceAtIndex);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
