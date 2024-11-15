#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <bitset>
#include <cstdlib>
#include <time.h>
#include <cmath>
#include <map>
#include <bitset>
#include <fstream>

// Alphabet: Binary i.e (A = {0,1}), length of alphabet q = 2

// Length: The length of the codeword will be equal to the number of generator polynomials corresponding to that particular k value (currently this is 5)

// Dimension: For convolutional encoders, the dimension generally refers to the number of input bits per unit time that the encoder processes 
// rather than the number of independent vectors in a linear subspace (as in block codes). R = k / n, k = # input bits, n = # output bits

// Distance: In convolutional codes, a related metric called the free distance 𝑑free is often used instead of the traditional minimum distance. 
// it is the minimum Hamming weight of the difference between any two output sequences generated by distinct input sequences
// A convolutional code has the potential to correct floor(dfree - 1 / 2) errors per code sequence


using namespace std;

map<int, vector<unsigned int>> generatorPolynomialsMap = {
    {4, {0x5, 0x5, 0x5, 0x5, 0x5}},
    {5, {0x9, 0x9, 0x9, 0x9, 0x9}},
    {6, {0x15, 0x15, 0x15, 0x15, 0x15}},
    {7, {0x23, 0x23, 0x23, 0x23, 0x23}},
    {8, {0x72, 0x72, 0x72, 0x72, 0x72}},
    {9, {0x9b, 0x9b, 0x9b, 0x9b, 0x9b}},
    {10, {0x13c, 0x13c, 0x13c, 0x13c, 0x13c}},
    {11, {0x29b, 0x29b, 0x29b, 0x29b, 0x29b}},
    {12, {0x4f5, 0x4f5, 0x4f5, 0x4f5, 0x4f5}},
    {13, {0xa4f, 0xa4f, 0xa4f, 0xa4f, 0xa4f}},
    {14, {0x10b7, 0x10b7, 0x10b7, 0x10b7, 0x10b7}},
    {15, {0x2371, 0x2371, 0x2371, 0x2371, 0x2371}},
    {16, {0x5a47, 0x5a47, 0x5a47, 0x5a47, 0x5a47}}
};

struct vNode {
    long long cumHammingDistance;
    bool inputArrivalBit;
    int state;

    bool operator<(const vNode& other) const {
        return state < other.state;
    }
};

int calculateHammingDistance(vector<bool>& code1, vector<bool>& code2);                         // DONE
vector<bool> generateOutput(vector<bool>& shiftregister, vector<unsigned int>& genPolynomials); // DONE
vector<bool> addNoise(vector<bool>& code, float prob_of_error);                                 // DONE
vector<bool> getOriginalCode(const vector<vector<vNode>> &trellis);                             // DONE
vector<vector<bool>> generateStates(int k);                                                     // DONE
vector<bool> calculatePotentialInput(vector<bool>& curState, bool next_input);                  // DONE
vector<bool> stringToVecBool(string& message);                                                  // DONE
string vecBoolToString(vector<bool>& binary);                                                   // DONE
void exportData(map<int, vector<float>>& k_data_points, string& filename);                      // DONE
void printTrellisStates(const vector<vector<vNode>> &trellis);                                  // DONE
string vecBoolToStringBinary(vector<bool>& binary);
int vecBoolToInt(vector<bool>& bits);                                               


int timeSteps = 0;
vector<vector<vNode>> trellis;

// // Encode Method
vector<bool> encode(vector<bool> code, int k, vector<unsigned int> genPolynomials) { // k is the constraint length i.e length of the shift register we want to use
    vector<bool> encodedVector = {};
    vector<bool> sliding_window = {};
    vector<bool> outputVector = {};
    // Follow this same procedure till every bit is processed
    for (int i = 0; i < code.size(); i++) {
        sliding_window.clear();
        for (int m = 0; m < k-i-1; m++) {
            sliding_window.push_back(0);
        }
        for (int j = 0; j < k - (max((k-i-1), 0));j++) {
            if (i >= k) {
                sliding_window.push_back(code[j-(k-i-1)]);
            }
            else {
                sliding_window.push_back(code[j]);
            }
            
        }
        vector<bool> outputVector = generateOutput(sliding_window, genPolynomials);
        encodedVector.insert(encodedVector.end(), outputVector.begin(), outputVector.end());

        timeSteps += 1;
    }
    return encodedVector;
}


vector<bool> viterbiDecode(vector<bool> noisy_encoded_code, int k, vector<vector<bool>> states, vector<unsigned int> genPolynomials) {

    vector<bool> decoded = {};
    
    int outputBits = genPolynomials.size();
    trellis.resize(timeSteps + 1);
    
    //initialize the only node at t = 0 (0,0)

    for (int t = 0; t < timeSteps+1; t++) {
        for (int s = 0; s < states.size(); s++) {
            vNode defaultNode;

            defaultNode.state = s;
            defaultNode.inputArrivalBit = 0;
            defaultNode.cumHammingDistance = INT_MAX;
            trellis[t].push_back(defaultNode);          
            
        }
    }

    trellis[0][0].cumHammingDistance = 0; // assuming out register will start at an intial state of State 0

    //printTrellisStates(trellis);



    for (int t = 1; t <= timeSteps; t++) {
        vector<bool> observedInput = vector<bool>(noisy_encoded_code.begin() + (outputBits*(t-1)), noisy_encoded_code.begin() + (outputBits*(t-1) + outputBits));
        for (int s = 0; s < states.size(); s++) {
            // cout << "------------------------------------------------------------------------" << endl;
            // cout << "State is: " << s << endl;
            bool transitionBit = s % 2;
            trellis[t][s].inputArrivalBit = transitionBit;
            
            int currentState = vecBoolToInt(states[s]);

    
            int firstPrevState = (currentState >> 1);
            if (trellis[t-1][firstPrevState].cumHammingDistance != INT_MAX) {
                // cout << "Previous state for first input is: " << to_string(firstPrevState) << endl;
                vector<bool> firstPotentialInput = calculatePotentialInput(states[firstPrevState], transitionBit);
                vector<bool> firstExpected = generateOutput(firstPotentialInput, genPolynomials);
                // cout << "Our expected is:    " << firstExpected << endl;
                // cout << "Our observation is: " << observedInput << endl;
                int firstHammingDistance = trellis[t-1][firstPrevState].cumHammingDistance + calculateHammingDistance(observedInput, firstExpected);
                // cout << "So the cumulative hamming distance would be: " << firstHammingDistance << endl;
                if (firstHammingDistance < trellis[t][s].cumHammingDistance) {
                    trellis[t][s].cumHammingDistance = firstHammingDistance;
                    trellis[t][s].inputArrivalBit = transitionBit;
                }
            }
            

            int secondPrevState = ((currentState) >> 1) | (1 << (k-2));
            if (trellis[t-1][secondPrevState].cumHammingDistance != INT_MAX) {
                // cout << "Previous state for second input is: " << to_string(secondPrevState) << endl;
                vector<bool> secondPotentialInput = calculatePotentialInput(states[secondPrevState], transitionBit);
                vector<bool> secondExpected = generateOutput(secondPotentialInput, genPolynomials);
                // cout << "Our expected is:    " << secondExpected << endl;
                // cout << "Our observation is: " << observedInput << endl;
                int secondHammingDistance = trellis[t-1][secondPrevState].cumHammingDistance + calculateHammingDistance(observedInput, secondExpected);
                // cout << "So the cumulative hamming distance would be: " << secondHammingDistance << endl;
                if (secondHammingDistance < trellis[t][s].cumHammingDistance) {
                    trellis[t][s].cumHammingDistance = secondHammingDistance;
                    trellis[t][s].inputArrivalBit = transitionBit;
                }
            }
        // cout << "------------------------------------------------------------------------" << endl;
        }
    }

    // printTrellisStates(trellis);

    return getOriginalCode(trellis);
}



int main() {
    float p;
    int lowerKlimit;
    int upperKlimit;
    string exportFile = "results.csv";

    // degree of any gen polynomial should always be less than or equal to k-1
    vector<vector<bool>> possibleStates;

    // used to get the random number between 0 and 1 when determining when to flip bits
    // unsigned int seed = 12345;  // Replace 12345 with any specific seed you want
    // srand(seed);
    srand( (unsigned)time( NULL ) );

    // the code that we want to encode
    string message;
    getline(cin, message);
    vector<bool> code = stringToVecBool(message);

    // string code = "1010";

    // string code = "1010";
    
    // the probability of ax single bit flipping after encoding the original code
    p = 0.1; // LOL
    p = 0.01; // Poor channel conditions, severe interference, or far-from-optimal signal quality.
    // p = 0.001; // Moderate noise, common in low-quality wireless connections or basic wired links with interference.
    p = 0.05;
    int numIterations = 1000;


    // if (code.length() < 50) {
    //     lowerKlimit = 4;
    //     upperKlimit = 5;
    // }
    // else if (code.length() < 100) {
    //     lowerKlimit = 5;
    //     upperKlimit = 7;
    // }
    // else {
    //     lowerKlimit = 7;
    //     upperKlimit = 8; // going above 8 will destroy your computer :)
    //}


    lowerKlimit = 4;
    upperKlimit = 10;
    
    map<int, vector<float>> k_averages;  // Adjusted to store averages for all possible_k values

    // Loop over possible values of k
    for (int possible_k = lowerKlimit; possible_k <= upperKlimit; possible_k++) {

        float average_ber = 0.0;
        float average_success_rate = 0.0;

        float ber;

        possibleStates = generateStates(possible_k);

        cout << "===============================================================" << endl;
        cout << "Processing for k = " << possible_k << " (" << numIterations << " iterations)" << endl;
        cout << "===============================================================" << endl;

        // Perform multiple iterations for each k value
        for (int i = 0; i < numIterations; i++) {
            // Reset the environment for each iteration
            timeSteps = 0;
            trellis.clear();
 
            // Encoding, adding noise, and decoding
            vector<bool> encoded = encode(code, possible_k, generatorPolynomialsMap[possible_k]);
            vector<bool> noisy_encoded = addNoise(encoded, p);
            vector<bool> originalCode = viterbiDecode(noisy_encoded, possible_k, possibleStates, generatorPolynomialsMap[possible_k]);
            string originalMessage = vecBoolToString(originalCode);
            // string originalMessage = originalCode;

            // Displaying results for each iteration
            cout << "---------------------------------------------------------------" << endl;
            cout << "Iteration #" << i + 1 << ":" << endl;
            cout << "K = " << possible_k << endl;

            // print a newline after all polynomials have been printed
            cout << endl;
            cout << "Original Message : " << message << endl;
            cout << "Original Code  : " << vecBoolToStringBinary(code) << endl;
            cout << "Encoded Code   : " << vecBoolToStringBinary(encoded) << endl;
            cout << "Noisy Code     : " << vecBoolToStringBinary(noisy_encoded) << endl;
            cout << "Noise Added?   : " << (noisy_encoded == encoded ? "No" : "Yes") << endl;
            cout << "# bits flipped: " << calculateHammingDistance(encoded,noisy_encoded) << endl;
            ber = ((float) calculateHammingDistance(code, originalCode) / (float) code.size());
            cout << "Bit Error Rate : " << ber * 100 << "%" << endl;
            average_ber += ber;
            cout << "Decoded Code   : " << vecBoolToStringBinary(originalCode) << endl;
            cout << "Decoded Message: " << originalMessage << endl;

            bool success = (code == originalCode);
            if (success) {
                cout << "Result         : SUCCESS" << endl;
                average_success_rate += 1;
            } else {
                cout << "Result         : FAIL" << endl;
            }
            cout << "---------------------------------------------------------------" << endl;
        }

        // Calculate and display the success rate for this k value
        average_ber /= numIterations;
        average_success_rate /= numIterations;
        k_averages[possible_k].push_back(average_ber);
        k_averages[possible_k].push_back(average_success_rate);

        cout << "===============================================================" << endl;
        cout << "Summary for k = " << possible_k << ":" << endl;
        cout << "Average Success Rate: " << average_success_rate * 100 << "% Success Rate" << endl;
        cout << "Average BER: " << average_ber * 100 << "% Bit Error Rate" << endl;
        cout << "===============================================================" << endl << endl;
    }

    // Final summary of all k values
    cout << "===================== Overall Results =====================" << endl;
    cout << "Noise was: " << p << endl;
    cout << "Original message was: " << message << endl;
    cout << "Message length was: " << code.size() << endl;
    // cout << "Output Bits per input: " << outputBits << endl;
    for (int i = lowerKlimit; i <= upperKlimit; i++) {
        cout << "For k = " << i << 
        " -> Average BER = " << std::fixed << std::setprecision(2) << k_averages[i][0] * 100 << "%" << 
        " -> Average Success Rate = " << k_averages[i][1] * 100 << "%" << endl;
    }
    cout << "===========================================================" << endl;

    exportData(k_averages, exportFile);
    return 0;
}



int calculateHammingDistance(vector<bool>& code1, vector<bool>& code2) {
    if (code1.size() != code2.size()) {
        return -1;
    }

    int hammingDistance = 0;
    for (int i = 0; i < code1.size(); i++) {
        if (code1[i] != code2[i]) {
            hammingDistance++;
        }
    }

    return hammingDistance;
}

vector<bool> generateOutput(vector<bool>& shiftregister, vector<unsigned int>& genPolynomials) {
    vector<bool> toReturn = {};
    int k = shiftregister.size();

    bool registerParity=0;

    for (unsigned int genPoly : genPolynomials) {
        genPoly = genPoly << 1 | 1; // just need to add another 1 at the end due to implicit +1 notation
        registerParity = 0;
        // cout << "Current genPoly: " << bitset<8>(genPoly) << endl; // Print binary of genPoly for clarity
        // cout << "Shift Register: " << shiftregister << endl;
        // gen poly = 1011
        // k = 4
        //j = 0
        for (int j = 0; j < k; j++) {
            if (((genPoly >> j) & 1) == 1) {
                // cout << "  - XOR with shiftregister[" << k - 1 - j << "] (" << shiftregister[k - 1 - j] << ")" << endl;
                registerParity ^= shiftregister[k - 1 - j];
            } 
            else {}
        }
        // cout << "Intermediate registerParity: " << registerParity << endl;
        toReturn.push_back(registerParity);
    }
// cout << "Final Output is: " << parityBits << endl;
    return toReturn; 
}

vector<bool> addNoise(vector<bool>& code, float prob_of_error) {

    vector<bool> noisyEncoded = {};
    
    // For every  bit in the  code
    for (int i = 0; i < code.size(); i++) {
        // turn the char back to an int
        // calculate a random number between 0 and 1 and if its less than the p value passed in, flip the bit
        if ((float) rand()/RAND_MAX < prob_of_error) {
            switch (code[i]) {
                case 0:
                    noisyEncoded.push_back(1);
                    break;
                case 1:
                    //cout << "Flipping 1 to 0" << endl;
                    noisyEncoded.push_back(1);
                    //cout << std::bitset<64>(raw_code_as_vector[i]).to_string() << endl;
                    break;
            }
        }
        // if the random number generated isn't less than p, dont flip the bit
        else {
            noisyEncoded.push_back(code[i]);
        }
        
    }
    return noisyEncoded;
}

void printTrellisStates(const vector<vector<vNode>> &trellis) {
    for (int i = 0; i < trellis.size(); i++) {
        cout << "For t = " << i << endl;
        for (int j = 0; j < trellis[i].size(); j++) {
            cout << "Node State: " << trellis[i][j].state 
                << " Cumulative Hamming Distance: " << trellis[i][j].cumHammingDistance << endl;
        }
    }
}


vector<bool> recursiveBackTrack(const vector<vector<vNode>>& trellis, int t, int state) {
    if (t == 0) {
        return {};
    }

    int bestPrevState = 0;
    int minDistance = INT_MAX;
    bool inputArrivalBit = 0;

    for (int prevState = 0; prevState < trellis[t-1].size(); prevState++) {
        if (trellis[t-1][prevState].cumHammingDistance < minDistance) {
            minDistance = trellis[t-1][prevState].cumHammingDistance;
            bestPrevState = prevState;
            inputArrivalBit = trellis[t][state].inputArrivalBit;
        }
    }

    vector<bool> result = recursiveBackTrack(trellis, t - 1, bestPrevState);

    result.push_back(inputArrivalBit);

    return result;
}


vector<bool> getOriginalCode(const vector<vector<vNode>> &trellis) {
    int finalState = 0;
    int minDistance = INT_MAX;


    for (int state = 0; state < trellis.back().size(); state++) {
        if (trellis.back()[state].cumHammingDistance < minDistance) {
            minDistance = trellis.back()[state].cumHammingDistance;
            finalState = state;
        }
    }

    return recursiveBackTrack(trellis, trellis.size()-1, finalState);
}



vector<vector<bool>> generateStates(int k) {
    vector<vector<bool>> statesVector = {};

    vector<bool> curState;
    //Ex. k = 4
    // this loop will run 8 times to generate all 8 possible states
    // i goes from 0-7
    // for i = 0
    for (int i = 0; i < pow(2, k-1); i++) {
        curState = {};
        // bit = 4 - 2 = 2
        // so bit goes from 2-0
        for (int bit = k - 2; bit >= 0; bit--) {
            // i = 0
            // bit = 2
            // 0 >> 2 = 0, & 1 = 0, 0 gets appended
            // bit = 1
            // 0 >> 1 = 0, & 1 = 0, 0 gets appended
            // bit = 0
            // 0 >> 0 = 0, & 1 = 0, 0 gets appended
            curState.push_back(((i >> bit) & 1));
        }
        statesVector.push_back(curState);
    }

    return statesVector;
}

vector<bool> calculatePotentialInput(vector<bool>& curState, bool next_input) {
    vector<bool> input = curState;
    input.push_back(next_input);
    return input;
}


vector<bool> stringToVecBool(string& message) {

    vector<bool> toReturn = {};

    bitset<8> curCharacter;

    // for every character in the string
    for (char c : message) {
        curCharacter = c;
        // for every bit in the character
        for (int i = 7; i >= 0; i--) {
            toReturn.push_back(curCharacter[i]);
        }
    }

    return toReturn;
}


string vecBoolToString(vector<bool>& binary) {
    string toReturn = "";

    char c = 0;

    for (int i = 0; i < binary.size() ; i+=8) {
        for (int j = 0; j < 8 ; j++) {
            c = c << 1 | binary[i+j];
        }
        toReturn += c;
    }

    return toReturn;
}

string vecBoolToStringBinary(vector<bool>& binary) {
    string stringBinary = "";

    for (int i = 0; i < binary.size(); i++) {
        stringBinary += to_string(binary[i]);
    }

    return stringBinary;
}

void exportData(map<int, vector<float>>& k_data_points, string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "K, Success Rate, BER\n";
        for (auto& entry : k_data_points) {
            file << entry.first << "," << entry.second[1] * 100 << "," << entry.second[0] * 100 << "\n";
        }
        file.close();
    }

}

int vecBoolToInt(vector<bool>& bits) {
    int value = 0;
    for (bool bit : bits) {
        value = (value << 1) | bit;
    }
    return value;
}