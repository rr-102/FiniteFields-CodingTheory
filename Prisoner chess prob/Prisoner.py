import random

def initialize_chessboard():
    """
    Initializes an 8x8 chessboard with random coin flips (heads or tails).
    Returns a list representing the board state where 0 is heads and 1 is tails.
    """
    return [random.choice([0, 1]) for _ in range(64)]

def encode_flipped_square(board):
    """
    Calculates the XOR of all positions on the board that represent 1.
    XOR combines bits to detect a flipped square logically.
    """
    encoded_value = 0
    for i, coin in enumerate(board):
        if coin == 1:
            encoded_value ^= i
    return encoded_value

def simulate_flip(board, flip_index):
    """
    Flips the coin at the given index on the chessboard (0 for heads, 1 for tails and vice versa).
    """
    board[flip_index] ^= 1

def compute_parity(board):
    """
    Computes parity for groups of bits in the chessboard.
    For an 8x8 board, computes parity of rows and columns.
    Returns a dictionary with row and column parity.
    """
    row_parity = [sum(board[i * 8:(i + 1) * 8]) % 2 for i in range(8)]
    col_parity = [sum(board[i::8]) % 2 for i in range(8)]
    return {"rows": row_parity, "columns": col_parity}

# Game begins
print("Welcome to the Prisoner's Chess Problem Game! You are the second prisoner. The program will simulate the first prisoner and the warden's question.")

# Create the board
initial_board = initialize_chessboard()

# Compute and show the initial parity
initial_parity = compute_parity(initial_board)
print("\nInitial Parity Information:")
print("Row Parity (before flip):", initial_parity["rows"])
print("Column Parity (before flip):", initial_parity["columns"])

# The warden selects a random square to ask about
warden_question = random.randint(0, 63)
print(f"\nThe warden has chosen a question related to square {warden_question}. The first prisoner must flip one square to encode the answer.")

# The first prisoner calculates which square to flip
flip_index = encode_flipped_square(initial_board) ^ warden_question
flipped_board = initial_board.copy()
simulate_flip(flipped_board, flip_index)

# Compute and show the new parity
flipped_parity = compute_parity(flipped_board)
print("\nFlipped Parity Information:")
print("Row Parity (after flip):", flipped_parity["rows"])
print("Column Parity (after flip):", flipped_parity["columns"])

# Clue
row_diff = [i for i in range(8) if initial_parity["rows"][i] != flipped_parity["rows"][i]]
col_diff = [i for i in range(8) if initial_parity["columns"][i] != flipped_parity["columns"][i]]
print("\nClues:")
print("The flipped square is in one of these rows:", row_diff)
print("The flipped square is in one of these columns:", col_diff)

# Guesses the warden's question
try:
    user_guess = int(input("\nEnter your guess for the warden's question (a number between 0 and 63): "))
    if user_guess < 0 or user_guess > 63:
        print("Invalid guess! The number must be between 0 and 63.")
    else:
        # Check if the user's guess is correct
        if user_guess == warden_question:
            print(f"Correct! The warden's question was {warden_question}.")
        else:
            print(f"Incorrect. The correct answer was {warden_question}.")
except ValueError:
    print("Invalid input! Please enter a number between 0 and 63.")
