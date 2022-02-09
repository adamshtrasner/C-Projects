# Doubly-Linked-List
A C implementation of the doubly linked list data structure.

# Description
A doubly linked list is a linked data structure that consists of a set of sequentially linked records called nodes.
Each node contains three fields: two link fields (references to the previous and to the next node in the sequence of nodes) and one data field.
The beginning and ending nodes' previous and next links, respectively, point to some kind of terminator, typically a sentinel node or null,
to facily one sentinel node, then the list is circularly linked via the sentinel node. It can be conclitate traversal of the list.
If there is oneptualized as two singly linked lists formed from the same data items, but in opposite sequential orders.

In my implementation, every node consists of a list of integers.

# Input
The input is a text file of the format:
* The first letter is always either 's' or 'e', which will indicate whether to insert
  the node at the head of the list ('s' - left most node) or at the the end of the list ('e' - right most node).
* After the letter there will be a comma, and after that integers, seperated by commas, which are incoded into a list by
  the order they were given.
* No spaces on each line.
* There will be atleast 1 line (The file is not empty)

# Output
A doubly linked list (struct LinkedList) according to the txt file.
