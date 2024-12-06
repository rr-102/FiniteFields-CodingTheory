import numpy as np
import random

def golay24_generator():
    I12 = np.eye(12, dtype=int) #creates a 12x12 identity matrix
    P = np.array([
        [1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1],
        [0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0],
        [1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0],
        [1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0],
        [1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1],
        [0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0],
        [1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0],
        [0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1],
        [1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0],
        [0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1],
        [0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0],
        [1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1]
    ], dtype=int) #creates the parity check matrix
    G = np.hstack((I12, P)) #combines the left and right matrices to create 'G', the generator matrix consisting of basis codewords
    return G

def golay24_encode(message):
    G = golay24_generator()
    message = np.array(message, dtype=int) #converts inputted message to a numpy array
    codeword = np.dot(message, G) % 2 #multiplies vector by generator matrix to produce a codeword
    return codeword

def golay24_parity_check():
    P = golay24_generator()[:, 12:]
    H = np.hstack((P.T, np.eye(12, dtype=int)))
    return H #note that this matrix is the same as the generator matrix, except the left and right sides are swapped

def golay24_decode(received):
    H = golay24_parity_check()
    received = np.array(received, dtype=int) #converts recieved message to a numpy array
    syndrome = np.dot(received, H.T) % 2 #calculates the 'syndrome' of the code by multiplying the recieved message with the parity check matrix
    error_pattern = np.zeros(24, dtype=int)
    
    #case 1: no error
    if not np.any(syndrome):
        corrected = received
        message = corrected[:12]
        return corrected, message #syndrome was zero -> the word is a valid code

    #case 2: singular error
    for i in range(24):
        if np.array_equal(syndrome, H[:, i]): #checks if the syndrome equals a column of the parity check matrix
            error_pattern[i] = 1 #the error is a vector containing all zeros and a singular one
            corrected = (received + error_pattern) % 2 #adding this error pattern to the recieved message flips the bit that had the error
            message = corrected[:12]
            return corrected, message

    # case 3: two or three errors
    for i in range(24):
        for j in range(i + 1, 24):
            combined_error = H[:, i] ^ H[:, j] #finds combined error
            if np.array_equal(syndrome, combined_error):
                error_pattern[i] = 1
                error_pattern[j] = 1
                corrected = (received + error_pattern) % 2 #flips two error bits
                message = corrected[:12]
                return corrected, message

            # Check for three errors
            for k in range(j + 1, 24):
                combined_error = H[:, i] ^ H[:, j] ^ H[:, k] #finds combined error
                if np.array_equal(syndrome, combined_error):
                    error_pattern[i] = 1
                    error_pattern[j] = 1
                    error_pattern[k] = 1
                    corrected = (received + error_pattern) % 2 #flips three error bits
                    message = corrected[:12]
                    return corrected, message
    #the decoding stops here, since the Golay code cannot reliably correct more than 3 errors
    corrected = received
    message = corrected[:12]
    return corrected, message


#dictionary storing binary conversions of alphabet characters
alphabet_converter = {
    'a': [0, 0, 0, 0, 0, 1], 'b': [0, 0, 0, 0, 1, 0], 'c': [0, 0, 0, 0, 1, 1],
    'd': [0, 0, 0, 1, 0, 0], 'e': [0, 0, 0, 1, 0, 1], 'f': [0, 0, 0, 1, 1, 0],
    'g': [0, 0, 0, 1, 1, 1], 'h': [0, 0, 1, 0, 0, 0], 'i': [0, 0, 1, 0, 0, 1],
    'j': [0, 0, 1, 0, 1, 0], 'k': [0, 0, 1, 0, 1, 1], 'l': [0, 0, 1, 1, 0, 0],
    'm': [0, 0, 1, 1, 0, 1], 'n': [0, 0, 1, 1, 1, 0], 'o': [0, 0, 1, 1, 1, 1],
    'p': [0, 1, 0, 0, 0, 0], 'q': [0, 1, 0, 0, 0, 1], 'r': [0, 1, 0, 0, 1, 0],
    's': [0, 1, 0, 0, 1, 1], 't': [0, 1, 0, 1, 0, 0], 'u': [0, 1, 0, 1, 0, 1],
    'v': [0, 1, 0, 1, 1, 0], 'w': [0, 1, 0, 1, 1, 1], 'x': [0, 1, 1, 0, 0, 0],
    'y': [0, 1, 1, 0, 0, 1], 'z': [0, 1, 1, 0, 1, 0], ' ': [0, 0, 0, 0, 0, 0],
    '\'': [0, 1, 1, 1, 0, 0], '!': [0, 1, 1, 1, 0, 1], '.': [0, 1, 1, 1, 1, 0],
    ',': [0, 1, 1, 1, 1, 1], 'A': [1, 0, 0, 0, 0, 0], 'B': [1, 0, 0, 0, 0, 1],
    'C': [1, 0, 0, 0, 1, 0], 'D': [1, 0, 0, 0, 1, 1], 'E': [1, 0, 0, 1, 0, 0],
    'F': [1, 0, 0, 1, 0, 1], 'G': [1, 0, 0, 1, 1, 0], 'H': [1, 0, 0, 1, 1, 1],
    'I': [1, 0, 1, 0, 0, 0], 'J': [1, 0, 1, 0, 0, 1], 'K': [1, 0, 1, 0, 1, 0],
    'L': [1, 0, 1, 0, 1, 1], 'M': [1, 0, 1, 1, 0, 0], 'N': [1, 0, 1, 1, 0, 1],
    'O': [1, 0, 1, 1, 1, 0], 'P':[1, 0, 1, 1, 1, 1], 'Q': [1, 1, 0, 0, 0, 0],
    'R': [1, 1, 0, 0, 0, 1], 'S': [1, 1, 0, 0, 1, 0], 'T': [1, 1, 0, 0, 1, 1],
    'U': [1, 1, 0, 1, 0, 0], 'V': [1, 1, 0, 1, 0, 1], 'W': [1, 1, 0, 1, 1, 0],
    'X': [1, 1, 0, 1, 1, 1], 'Y': [1, 1, 1, 0, 0, 0], 'Z': [1, 1, 1, 0, 0, 1],
    '?': [1, 1, 1, 0, 1, 0], '#': [1, 1, 1, 0, 1, 1], '-': [1, 1, 1, 1, 0, 0],
    '[': [1, 1, 1, 1, 0, 1], ']': [1, 1, 1, 1, 1, 0], ';': [1, 1, 1, 1, 1, 1]
}


def encode_string(message):
    message_in_binary = []
    for char in message:
        if char in alphabet_converter:
            message_in_binary += alphabet_converter[char] #converts each character into 6 binary digits using the dictionary
        else:
            raise ValueError(f"Character '{char}' not found in alphabet_converter.")
    return message_in_binary

def get_key(value):#used to access keys in the dictionary by looking up values
    for key, binary in alphabet_converter.items():
        if binary == value:
            return key
    raise ValueError(f"Binary sequence {value} not found in alphabet_converter.")

def binary_converter(binary_message): #reverse of encode_string(), going from binary --> letters
    string = ''
    for i in range(0, len(binary_message), 6):
        chunk = binary_message[i:i + 6]
        string += get_key(chunk)
    return string

    
def add_noise(x, encoded): #adds errors to the message based on an input representing percentage of errors
    error_count = 0
    for i in range(len(encoded)): #iterates through binary digits
        if random.randint(0, 100) < (x * 100 / len(encoded)): #if a randomly generated number falls within the percent range, the bit is flipped
            encoded[i] = (encoded[i] + 1) % 2
            error_count += 1
            
    print(f"Introduced {error_count} errors.")
    return encoded


#combines functions for complete encoding and decoding process
def golay_encode_string(message):
    binary_message = encode_string(message)
    chunks = [binary_message[i:i + 12] for i in range(0, len(binary_message), 12)]
    if len(chunks[-1]) < 12:
        chunks[-1] += [0] * (12 - len(chunks[-1]))
    encoded_chunks = [golay24_encode(chunk) for chunk in chunks]
    return np.concatenate(encoded_chunks).tolist()

def golay_decode_string(encoded_message):
    chunks = [encoded_message[i:i + 24] for i in range(0, len(encoded_message), 24)]
    decoded_chunks = [golay24_decode(chunk)[1] for chunk in chunks]
    binary_message = np.concatenate(decoded_chunks).tolist()
    return binary_converter(binary_message)

message = input("Enter your message: ")
encoded = golay_encode_string(message)

noise_level = int(input("Enter the noise level (0-12): "))
noisy_encoded = add_noise(noise_level, encoded.copy())

decoded = golay_decode_string(noisy_encoded)
print("Decoded Message:", decoded)
