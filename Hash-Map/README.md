# Hash-Map
A C implementation of a generic hash map data structure.

# Description
a hash table (hash map) is a data structure that can map keys to values. A hash table uses a hash function to compute an index, also called a hash code, into an array of buckets or slots, from which the desired value can be found. During lookup, the key is hashed and the resulting hash indicates where the corresponding value is stored.

In my implementation, the capacity of the hash table is initially 16, and is defined to be a power of 2.
Also, the hush function chosen for this project is: h(x) = x mod capacity.
