#include <iostream>
#include <regex>  // For pattern matching
#include <string> // For handling string input

using namespace std;

// Function to classify the input word based on regex patterns
void wordChecker(const string& testWord) {
    // Regular expression pattern for Integer (allows optional + or - signs)
    regex intPattern(R"(^[+-]?(0|[1-9][0-9]*)$)");

    // Regular expression pattern for Float (handles decimals with optional signs)
    regex floatPattern(R"(^[+-]?([0-9]*\.[0-9]+|\.[0-9]+)$)");

    // Regular expression pattern for String (matches text enclosed in double quotes)
    regex stringPattern(R"(^\"(\\.|[^\"])*\"$)");

    // Regular expression pattern for Boolean (supports "real", "cooked", "0", or "1")
    regex boolPattern(R"(^real|cooked|0|1$)");

    // Regular expression pattern for Character (single character enclosed in single quotes)
    regex charPattern(R"(^'(\\[ntrb\"\\']|[^\\'])'$)");

    // Regular expression pattern for Identifier (starts with a letter, contains underscore)
    regex idenPattern(R"(^[a-zA-Z][a-zA-Z0-9]*(_[a-zA-Z0-9]+)?$)");

    // Check if the input word matches the integer pattern
    if (regex_match(testWord, intPattern)) {
        cout << "Word belongs to classification: Integer" << endl;
        return; // Exit function after finding a match
    }

    // Check if the input word matches the float pattern
    if (regex_match(testWord, floatPattern)) {
        cout << "Word belongs to classification: Float" << endl;
        return;
    }

    // Check if the input word matches the string pattern
    if (regex_match(testWord, stringPattern)) {
        cout << "Word belongs to classification: String" << endl;
        return;
    }

    // Check if the input word matches the boolean pattern
    if (regex_match(testWord, boolPattern)) {
        cout << "Word belongs to classification: Boolean" << endl;
        return;
    }

    // Check if the input word matches the character pattern
    if (regex_match(testWord, charPattern)) {
        cout << "Word belongs to classification: Character" << endl;
        return;
    }

    // Check if the input word matches the identifier pattern
    if (regex_match(testWord, idenPattern)) {
        cout << "Word belongs to classification: Identifier" << endl;
        return;
    }

    // If the input word does not match any pattern, print "Invalid word"
    cout << "Invalid word" << endl;
}

int main() {
    // Create a string to store user input
    string input;
    while(true){

        // Ask the user to enter a word
        cout << "Enter a word to check its class: ";
        
        // Read the user input
        cin >> input;
    
        // Call the wordChecker function to classify the input word
        wordChecker(input);
    }

    return 0;
}
