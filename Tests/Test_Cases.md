## Sysprog - Lab4 - Linked List - Test cases

# All functions to test:
	- dpl_create();									1
	- dpl_free(&list);								2
	- dpl_size(list);								3
	- dpl_insert_at_index(list, element, index);	4
	- dpl_remove_at_index(list, index);				5
	- dpl_get_reference_at_index(list, index);		6
	- dpl_get_element_at_index(list, index);		7
	- dpl_get_index_of_element(list, element);		8

# TEST 1 - dpl_create():
	Case 1:		create new							// no other cases

# TEST 2 - dpl_free(&list):
	Case 1:		list == NULL
	Case 2:		list == empty
	Case 3:		list has 1 element
	Case 4:		list has multiple elements

# TEST 3 - dpl_size(list):
	Case 1:		list == NULL
	Case 2:		list == empty
	Case 3:		list has 1 element
	Case 4:		list has multiple elements

# TEST 4 - dpl_insert_at_index(list, element, index):
	Case 1:		list == NULL
	Case 2:		list == empty
	Case 3:		insert at index 0
	Case 4: 	insert at index in between elements
	Case 5:		insert at last index
	Case 6:		insert at negative index
	Case 7:		insert at index bigger than last index

# TEST 5 - dpl_remove_at_index(list, index):
	Case 1:		list == NULL
	Case 2:		list == empty
	Case 3:		list has 1 element
	Case 4: 	list has multiple elements
	Case 5:		index == 0
	Case 6:		index in between elements
	Case 7:		index == dpl_size(list) - 1
	Case 8:		index is negative
	Case 9:		index >= dpl_size(list)

# TEST 6 - dpl_get_reference_at_index(list, index):
	Case 1:		list == NULL
	Case 2:		list == empty
	Case 3:		list has 1 element
	Case 4: 	list has multiple elements
	Case 5:		index == 0
	Case 6:		index in between elements
	Case 7:		index == dpl_size(list) - 1
	Case 8:		index is negative
	Case 9:		index >= dpl_size(list)

# TEST 7 - dpl_get_element_at_index(list, index):
	Case 1:		list == NULL
	Case 2:		list == empty
	Case 3:		list has 1 element
	Case 4: 	list has multiple elements
	Case 5:		index == 0
	Case 6:		index in between elements
	Case 7:		index == dpl_size(list) - 1
	Case 8:		index is negative
	Case 9:		index >= dpl_size(list)

# TEST 8 - dpl_get_index_of_element(list, element):
	Case 1:		list == NULL
	Case 2:		list == empty
	Case 3:		list has 1 element
	Case 4: 	list has multiple elements
	Case 5:		element == null
	Case 6:		element is in list 1 time
	Case 7:		element is not in list
	Case 8:		element is in list multiple times

