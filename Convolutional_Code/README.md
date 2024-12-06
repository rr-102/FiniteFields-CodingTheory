# Convoloutional Code with Viterbi Decoder Implementation in C++

## Overview
This project implements a convolutional encoder and decoder simulation using Viterbi decoding. The code simulates error-correcting capabilities of convolutional codes, allowing analysis of bit error rates (BER) and decoding success rates under various noise conditions


## Features
- **Encoding**: Converts binary data into encoded codewords using convolutional generator polynomials.
- **Error Introduction**: Simulates channel noise with configurable probabilities for bit flipping.
- **Error Detection and Correction**: Implements the Viterbi algorithm for decoding received noisy codewords back to the original message.
- **Performance Metrics**: Calculates bit error rates (BER) and success rates for different configurations of generator polynomials and constraint lengths.

## Project Structure
- **Code File**: ConvCode.cpp -> Implements encoding, noise addition, and Viterbi decoding algorithms.
- **Makefile**: Automates the compilation of the program.
- **Output**: The simulation outputs detailed results to the console

## Requirements
- C++11 compatible compiler
- Standard C++ libraries

## Compilation
Use the provided Makefile to compile the program:

```bash
make
```

## Usage
This will create an executible with the name ./conv, which can be run with:

```bash
./conv
```
After this, just type in the message that you want to simulate and hit enter and the simulation will begin.



Example Output:

```plaintext 
Iteration #550:
K = 8

Original Message : hellp
Original Code  : 0110100001100101011011000110110001110000
Encoded Code   : 00000111111111111111000000000000000000000000011111000000000011111111111111100000000001111100000000000000000000111111111100000111111111111111111111111100000111110000011111111110000000000111111111100000
Noisy Code     : 00000111111111111111000000000000100000001000111111000000000011111111111111100010010001111110000000000000001000111111111100001111111111111111111111111100100111110000011111111110000000000111111111100000
Noise Added?   : Yes
# bits flipped: 9
Bit Error Rate : 0%
Decoded Code   : 0110100001100101011011000110110001110000
Decoded Message: hellp
Result         : SUCCESS
```