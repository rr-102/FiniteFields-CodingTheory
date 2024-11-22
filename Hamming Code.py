
# Online Python - IDE, Editor, Compiler, Interpreter
import numpy as np
import random

#Gives array of binary representation of x with bits number of bits
def fun(x,bits):
    return np.array([int(i) for i in bin(x)[2:].zfill(bits)])
    
#Turns binary rep into integer
def unfun(arr):
    binaryString=""
    for digit in arr:
        binaryString += str(digit)
    return int(binaryString,base=2)


#Parity check matrix
H = np.transpose(np.array([fun(i,4) for i in range(1,9+1)]))
#print(H)

#Example stuff
#word=np.transpose(np.array([random.randint(0,1) for i in range (1,2**3)]))


#Error correction
def correct(codeword):
    syn = np.mod(np.matmul(H,codeword),2)
    index = unfun(syn)-1
    if index==-1 or index>8:
        return codeword
    else:
        codeword[index] = 1 ^ codeword[index]
        return codeword
    #print(word)

#Generator matrix
G = np.transpose(np.array([
    [1,1,1,0,0,0,0,0,0],
    [1,0,0,1,1,0,0,0,0],
    [0,1,0,1,0,1,0,0,0],
    [1,1,0,1,0,0,1,0,0],
    [1,0,0,0,0,0,0,1,1]
    ]))
#print(G)

def encode (letternum):
    return np.mod(np.matmul(G,letternum),2)
    
def error(codeword):
    index=random.randint(1,9)-1
    codeword[index] = 1^codeword[index]
    return codeword
 
def decode(codeword):
    infobits=np.array([3,5,6,7,9])
    decoded = np.array([0,0,0,0,0])
    currentindex=0
    for i in range(0,len(infobits)):
        decoded[i]=codeword[infobits[i]-1]
    return decoded
    
letters=['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
'p','q','r','s','t','u','v','w','x','y','z',' ',',','.','?','!','"']
def numeric(letter):
    #return ord(letter)-96
    return letters.index(letter)


def LetterToCode(letter):
    return encode(fun(numeric(letter),5))
def CodeToLetter(codeword):
    return letters[unfun(decode(codeword))]
    
def WordToCode(word):
    return np.array([LetterToCode(word[i]) for i in range(len(word))])

errorprob=0.04
correctprob=round(9*errorprob*(1-errorprob)**8/(1-(1-errorprob)**9),3)
worderrorprob=round(1-(1-errorprob)**9,3)
print("Probability of correcting an error: "+str(correctprob))
print("Probability of getting an error: " + str(worderrorprob))
print("Probability of correcting wrongly: "+str(round(
    worderrorprob*(1-correctprob),3)))

def WordError(codedword):
    newcodedword=np.copy(codedword)
    for i in range(len(codedword)):
        for j in range(len(codedword[0])):
            if random.uniform(0,1) < errorprob:
                newcodedword[i][j] = 1^codedword[i][j]
    return newcodedword

def WordCorrect(codedword):
    return np.array([ correct(codedword[i]) for i in 
    range(len(codedword))])

def CodeToWord(codedword):
    return np.array([CodeToLetter(codedword[i]) for 
    i in range(len(codedword))])
    
def WordToString(word):
    string=""
    for i in range(len(word)):
        string+= word[i]
    return string
#number = input("Choose a letter")
#print(CodeToLetter(error(LetterToCode(number))))
#print(CodeToLetter(correct(error(LetterToCode(number)))))

word = list(input("Input any phrase madeup of lowercase letters and basic punctuation"))
wordcode=WordToCode(word)
#print(wordcode)
wrongword=WordError(wordcode)
#print(wrongword)
print(WordToString(CodeToWord(wrongword)))
correctwordcode=WordCorrect(wrongword)
correctword=CodeToWord(correctwordcode)
print(WordToString(correctword))

count=0
for i in range(len(word)):
    if word[i]==correctword[i]:
        count=count+1
#print("Error rate: "+str(1-count/len(word)))

