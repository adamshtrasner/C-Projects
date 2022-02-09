## Caesar-cipher
A C implementation of a program which deciphers and encodes a text using a method called the Caesar cipher.

## Description
In cryptography, a Caesar cipher, is one of the simplest and most widely known encryption techniques.
It is a type of substitution cipher in which each letter in the plaintext is replaced by a letter some fixed number of positions down the alphabet.
For example, with a left shift of 3, D would be replaced by A, E would become B, and so on.
The method is named after Julius Caesar, who used it in his private correspondence.

## Input
The program recieves 4 arguments through the command line:
* Command: a string. Can be "encode" or "decode".
* k - an integer, number of shifts.
* Input file path.
* Output file path.

## Output
The output will be written in the output file (given by the user), according to the command
and the number of shifts given.
