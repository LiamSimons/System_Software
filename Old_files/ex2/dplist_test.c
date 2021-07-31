/**
 * \author Bert Van Cauter
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
START_TEST(test_ListFree)
    {
        // Test free NULL
        dplist_t *list = NULL;
        dpl_free(&list);    
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL for no list (NULL)");
        // Test free empty list
        list = dpl_create();
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL for freeing empty list");
        // TODO : Test free with one element
        list = dpl_create();
        element_t element = (element_t)malloc(sizeof(char));
        *element = 'A';
        list = dpl_insert_at_index(list,element,0);
        ck_assert_msg(*(dpl_get_element_at_index(list, 0)) == 'A', "Failure: expected result to be 'A' but got: %c", dpl_get_element_at_index(list, 0));
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL for freeing list with one element");
        
        // TODO : Test free with multiple element
        list = dpl_create();
        element_t element2 = (element_t)malloc(sizeof(char));
        *element2 = 'B';
        list = dpl_insert_at_index(list,element,0);
        // printf("Element at index 0 before B added: %c\n", *(dpl_get_element_at_index(list, 0)));
        list = dpl_insert_at_index(list,element2,0);
        // printf("Element at index 0: %c\n", *(dpl_get_element_at_index(list, 0)));
        // printf("Element at index 1: %c\n", *(dpl_get_element_at_index(list, 1)));
        ck_assert_msg(*(dpl_get_element_at_index(list, 1)) == 'A', "Failure: expected result to be 'A' but got: %c|", dpl_get_element_at_index(list, 1));
        ck_assert_msg(*(dpl_get_element_at_index(list, 0)) == 'B', "Failure: expected result to be 'B' but got: %c|", dpl_get_element_at_index(list, 0));
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL for freeing list with multiple element");

        //free the elements
        free(element);
        free(element2);
    }
END_TEST

START_TEST(test_ListInsertAtIndexListNULL)
    {
        //making elements
        element_t element = (element_t)malloc(sizeof(char));
        *element = 'A';
        element_t element2 = (element_t)malloc(sizeof(char));
        *element2 = 'B';
        element_t element3 = (element_t)malloc(sizeof(char));
        *element3 = 'C';
        //Test inserting at index -1
        dplist_t *result1 = dpl_insert_at_index(NULL, element, -1);
        ck_assert_msg(result1 == NULL, "Failure: expected list to be NULL");
        // TODO : Test inserting at index 0
        dplist_t *result2 = dpl_insert_at_index(NULL, element2, 0);
        ck_assert_msg(result2 == NULL, "Failure: expected list to be NULL");
        // TODO : Test inserting at index 99
        dplist_t *result3 = dpl_insert_at_index(NULL, element3, 99);
        ck_assert_msg(result3 == NULL, "Failure: expected list to be NULL");
        //freeing elements
        free(element);
        free(element2);
        free(element3);
    }
END_TEST

START_TEST(test_ListInsertAtIndexListEmpty)
{
    //making elements
    element_t element = (element_t)malloc(sizeof(char));
    *element = 'A';
    element_t element2 = (element_t)malloc(sizeof(char));
    *element2 = 'B';
    element_t element3 = (element_t)malloc(sizeof(char));
    *element3 = 'C';
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, element, -1);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d for index -1",
                                         dpl_size(result));
    dpl_free(&list);
    // TODO : Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, element2, 0);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d for index 0",
                                         dpl_size(result));
    dpl_free(&list);
    // TODO : Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, element3, 99);
    ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d for index 99",
                                         dpl_size(result));
    dpl_free(&list);
    //freeing elements
    free(element);
    free(element2);
    free(element3);
}
END_TEST

/*START_TEST(test_ListSize)
{
    //Test size for list == NULL
    dplist_t *list = NULL;
    int size = dpl_size(list);
    ck_assert_msg(size == -1, "Failure: expected -1 due to no excisting list (NULL)");
    dpl_free(&list);
    //TODO : Test size for list with 0 elements
    list = dpl_create();
    size = dpl_size(list);
    ck_assert_msg(size == 0, "Failure: expected 0 due to empty list)");
    dpl_free(&list);
    //TODO : Test size for list with 1 element
    list = dpl_create();
    list = dpl_insert_at_index(list,'A',0);
    size = dpl_size(list);
    ck_assert_msg(size == 1, "Failure: expected size of list is 1)");
    dpl_free(&list);
    //TODO : Test size for list with multiple elements
    list = dpl_create();
    list = dpl_insert_at_index(list,'A',0);
    list = dpl_insert_at_index(list,'B',2);
    list = dpl_insert_at_index(list,'C',1);
    list = dpl_insert_at_index(list,'D',-1);
    size = dpl_size(list);
    ck_assert_msg(size == 4, "Failure: expected size of list is 4, got a size of %d",dpl_size(list));
    dpl_free(&list);
}
END_TEST

START_TEST(test_ListInsertAtIndexListMultipleElements)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', -1);
    ck_assert_msg(dpl_get_element_at_index(list,0)=='C', "Failure: expected list to element 'C', got an element %c for index -1",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
    // TODO : Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 0);
    ck_assert_msg(dpl_get_element_at_index(list,0)=='C', "Failure: expected list to element 'C', got an element %c for index 0",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
    // TODO : Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 99);
    ck_assert_msg(dpl_get_element_at_index(list,2)=='C', "Failure: expected list to element 'C', got an element %c for index 99",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
}
END_TEST

START_TEST(test_RemoveElementAtIndexListNULL)
{
    // Test inserting at index -1
    dplist_t *result1 = dpl_remove_at_index(NULL,-1);
        ck_assert_msg(result1 == NULL, "Failure: expected list to be NULL");
    // TODO : Test inserting at index 0
    dplist_t *result2 = dpl_remove_at_index(NULL,0);
        ck_assert_msg(result2 == NULL, "Failure: expected list to be NULL");
    // TODO : Test inserting at index 99
    dplist_t *result3 = dpl_remove_at_index(NULL,99);
        ck_assert_msg(result3 == NULL, "Failure: expected list to be NULL");
}
END_TEST

START_TEST(test_RemoveElementAtIndexListMultipleElements)
{
    // Test removing at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    result = dpl_remove_at_index(list,-1);
    ck_assert_msg(dpl_get_element_at_index(result,0)=='B', "Failure: expected list to element 'B', got an element %c for index -1",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
    // TODO : Test removing at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    result = dpl_remove_at_index(list,0);
    ck_assert_msg(dpl_get_element_at_index(result,0)=='B', "Failure: expected list to element 'B', got an element %c for index 0",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
    // TODO : Test removing at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    result = dpl_remove_at_index(list,99);
    ck_assert_msg(dpl_get_element_at_index(result,1)=='B', "Failure: expected list to element 'B', got an element %c for index 0",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
}
END_TEST

START_TEST(test_GetElementAtIndexAtNullList)
{
    //Test inserting at index -1
    dplist_t *result1 = dpl_insert_at_index(NULL, 'A', -1);
    ck_assert_msg(dpl_get_element_at_index(result1,0) == 0, "Failure: expected list to be NULL");
    // TODO : Test inserting at index 0
    dplist_t *result2 = dpl_insert_at_index(NULL, 'A', 0);
    ck_assert_msg(dpl_get_element_at_index(result2,0)== 0, "Failure: expected list to be NULL");
    // TODO : Test inserting at index 99
    dplist_t *result3 = dpl_insert_at_index(NULL, 'A', 99);
    ck_assert_msg(dpl_get_element_at_index(result3,0)== 0, "Failure: expected list to be NULL");
}
END_TEST

START_TEST(test_GetElementAtIndexAtListWithMultipleElements)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    ck_assert_msg(dpl_get_element_at_index(list,-1)=='A', "Failure: expected list to element 'A', got an element %c for index -1",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
    // TODO : Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    ck_assert_msg(dpl_get_element_at_index(list,0)=='A', "Failure: expected list to element 'A', got an element %c for index 0",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
    // TODO : Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    ck_assert_msg(dpl_get_element_at_index(list,99)=='C', "Failure: expected list to element 'C', got an element %c for index 99",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
}
END_TEST

START_TEST(test_GetRefrenceAtIndexWithMultipleElements)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    ck_assert_msg(dpl_get_element_at_index(list,-1)=='A', "Failure: expected list to element 'A', got an element %c for index -1",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
    // TODO : Test inserting at index 0
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    ck_assert_msg(dpl_get_element_at_index(list,0)=='A', "Failure: expected list to element 'A', got an element %c for index 0",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
    // TODO : Test inserting at index 99
    list = dpl_create();
    result = dpl_insert_at_index(list, 'A', 0);
    result = dpl_insert_at_index(list, 'B', 1);
    result = dpl_insert_at_index(list, 'C', 2);
    ck_assert_msg(dpl_get_element_at_index(list,99)=='C', "Failure: expected list to element 'C', got an element %c for index 99",
                                         dpl_get_element_at_index(list,0));
    dpl_free(&list);
}
END_TEST

START_TEST(test_GetIndexOfElement)
{
    //when element in List
    dplist_t *list = dpl_create();
    dplist_t *result = dpl_insert_at_index(list,'A',-1);
    result = dpl_insert_at_index(list, 'B',1);
    result = dpl_insert_at_index(list, 'C',2);
    result = dpl_insert_at_index(list, 'D',99);
    ck_assert_msg(dpl_get_index_of_element(list, 'C') == 2, "Failure: expected index 2 for element 'C' but got %d as index", dpl_get_index_of_element(list,'C'));
    dpl_free(&list);
    //When element not in List
    list = dpl_create();
    result = dpl_insert_at_index(list,'A',-1);
    result = dpl_insert_at_index(list, 'B',1);
    result = dpl_insert_at_index(list, 'C',2);
    result = dpl_insert_at_index(list, 'D',99);
    ck_assert_msg(dpl_get_index_of_element(list, 'E') == -1, "Failure: expected index -1 for element 'C' but got %d as index because element is not present", dpl_get_index_of_element(list,'C'));
    dpl_free(&list);
}
END_TEST*/

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
    tcase_add_test(tc1_1, test_ListFree);
    // tcase_add_test(tc1_1, test_ListInsertAtIndexListNULL);
    // tcase_add_test(tc1_1, test_ListInsertAtIndexListEmpty);
    // tcase_add_test(tc1_1, test_ListSize); 
    // tcase_add_test(tc1_1, test_ListInsertAtIndexListMultipleElements); 
    // tcase_add_test(tc1_1, test_RemoveElementAtIndexListNULL);
    // tcase_add_test(tc1_1, test_RemoveElementAtIndexListMultipleElements);
    // tcase_add_test(tc1_1, test_GetElementAtIndexAtNullList);
    // tcase_add_test(tc1_1, test_GetElementAtIndexAtListWithMultipleElements);
    // tcase_add_test(tc1_1, test_GetIndexOfElement);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
