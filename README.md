# PlayfairCrack
Program to crack a Playfair cipher using Simulated Annealing

## Project Explanation
The playfair cipher is a block cipher that is quite effective in obfuscating text. Frequency analysis is not an effective cryptanalysis tool to crack playfair ciphers because each letter can be enciphered as a different letter depending on which letter it is paired with. Therefore, it is necessary to utitlize digraphs (and in the case of this program, quadgraphs) and their relative frequency to the english language to crack the cipher. This is done using Simulated annealing which calculates a 'fitness' for the english language. The closer to 0 the fitness is, the more identical to english language the text appears.

## How to use
* First, run 'make' to compile the program. 
* Next, store the ciphertext in a file accessible to the program.
* Run the program by typing './crack < [filepath]
* The cracking process can take anywhere from 5 seconds to 5 minutes depending on the length of the text and how representative the plaintext is of the english language (it is much harder to crack 'HexIsnxtzeetixng' than it is to crack 'He Isnt eating').
* Sometimes the process will not be able to crack the cipher. This is unfortunate, but definitely a possibility.
* By default, the cracker uses I/J combination, but you can substitute any letter you choose (such as Q or V) by using the commandline argument '-r'
  * Example: './crack -r Q < test.txt'

