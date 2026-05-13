
# Storpool inteview (Ivan Tsantsarov)

## Tasks:

### *1) Counting* (main.cpp)
As input to your program, there is a binary file containing 1 billion 32-bit integer numbers,
stored as uint32_t. The size of the file is 4,000,000,000 bytes.


#### а) count the unique numbers. For example: 
given input -> output
0x100 0x100 0xfff 0xfff -> 2 unique numbers
0x100 0x100 0x100 0x100 -> 1 unique number
0x100 0x100 0x800 0xfff -> 3 unique numbers
#### b) count how many numbers are seen ONLY once. For example:
given input -> output
0x100 0x100 0xfff 0xfff -> 0 numbers seen only once
0x100 0x100 0x100 0x100 -> 0 numbers seen only once
0x100 0x100 0x800 0xfff -> 2 numbers seen only once
### *2) Fizzbuzz* (fizzbuzz.s)
The standard fizzbuzz task, with a catch:
Write a program that outputs the numbers from 1 to 100. If the number is divisible by 3, instead
of it print “A”, if it’s divisible by 5 - “B”, if it’s divisible by 15 - “AB”. The catch is to write it without
any conditionals/ifs, including for/while loops.
### *3) Analysis* (main.cpp)
At https://quiz.storpool.com/bigf.json.bz2 there is a compressed JSON fine. In it are described
disks with their model and serial number. Count how many models there are and how many
times each one is present.
### *4) Reverse engineering* (explanation_a.txt & explanation_b.txt)

At https://quiz.storpool.com/binaries.tgz there’s an archive with two executable binary files that
crash in different ways. Can you explain why and what are they trying to do?

## Setup:

### 1) Install Build essensials (GCC, GDB)
### 2) Install VSCode
### 3) Copy extracted resource files in the external directory of the executable file 

#### defined in main.cpp
#define FILE_4BLN_PATH "../fourbln.bin"
#define BIG_JSON_FILE_PATH "../bigf.json"

* fourbln.bin will be automaticaly created if not existing