import random

def initialize_chessboard():
    #Initializes an 8x8 chessboard with random coin flips (heads or tails).
    #Returns a list representing the board state where 0 is heads and 1 is tails.
    return [random.choice([0, 1]) for _ in range(64)]

def encode_flipped_square(board):
    #Calculates the XOR of all coins that represent 1
    #XOR is an abbreviation for “Exclusively-OR.” 
    # The simplest XOR gate is a two-input digital circuit that outputs a logical “1” if the two input values differ.
    encoded_value = 0
    for i, coin in enumerate(board):
        if coin == 1:
            encoded_value ^= i
    return encoded_value

def simulate_flip(board, flip_index):
    #Flips the coin at the given index on the chessboard (0 for heads, 1 for tails and vice versa).
    board[flip_index] ^= 1

def find_flipped_square(initial_board, flipped_board):
    #Identifies the flipped square index by XORing initial and flipped states. 
    # The result will be the index of the flipped square.
    initial_xor = encode_flipped_square(initial_board)
    flipped_xor = encode_flipped_square(flipped_board)
    return initial_xor ^ flipped_xor

# Simulation of the Prisoner's Chessboard Problem
initial_board = initialize_chessboard()
flip_index = int(input("Which coin do you wanna flip?"))
flipped_board = initial_board.copy()
simulate_flip(flipped_board, flip_index)

# Find the flipped square using the encoded XOR difference
flipped_square = find_flipped_square(initial_board, flipped_board)

print("Actual flipped square:", flip_index)
print("Detected flipped square:", flipped_square)

