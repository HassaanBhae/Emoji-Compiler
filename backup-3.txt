    #include <iostream>
    #include <windows.h>
    #include <vector>
    #include <regex>
    #include <unordered_set>
    #include <string>
    #include <fstream>

    using namespace std;
        
    // Token types
    enum class myTokenType { KEYWORD, IDENTIFIER, LITERAL, OPERATOR, SEPARATOR, COMMENT, WHITESPACE };

    // Token structure
    struct Token {
        myTokenType type;
        string value;
        int line;
    };

    // Keywords (supports Unicode emoji-based keywords)
    unordered_set<string> keywords = {"🔢", "🌊", "🔠", "🔤", "🤔", "😅", "🔙", "🔁", "➰", "✅", "❌"};

    // Regular expressions for token classification
    regex intPattern(R"(^[+-]?(0|[1-9][0-9]*)$)");
    regex floatPattern(R"(^[+-]?([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)$)");
    regex stringPattern(R"(^\"(\\.|[^\"])*\"$)");
    regex boolPattern(R"(^(✅|❌)$)");
    // regex idenPattern(R"(^[a-z]+$)");
    // regex idenPattern(R"(^[a-zA-Z]+$)");
    regex idenPattern(R"(^[a-zA-Z][a-zA-Z_]*$)");
    regex operatorPattern(R"([\+\-\*/%=<>!&|^~]+)");
    regex separatorPattern(R"([\(\)\{\}\[\];,.])");

    // Function to convert myTokenType to string
    string tokenTypeToString(myTokenType type) {
        switch (type) {
            case myTokenType::KEYWORD: return "KEYWORD";
            case myTokenType::IDENTIFIER: return "IDENTIFIER";
            case myTokenType::LITERAL: return "LITERAL";
            case myTokenType::OPERATOR: return "OPERATOR";
            case myTokenType::SEPARATOR: return "SEPARATOR";
            case myTokenType::COMMENT: return "COMMENT";
            case myTokenType::WHITESPACE: return "WHITESPACE";
            default: return "UNKNOWN";
        }
    }

    // Tokenization function
vector<Token> tokenize(const string &input) {
    cout << "Tokenization Started!\n";
    vector<Token> tokens;
    string token;
    size_t i = 0;
    int line = 1; // 🔥 Track the current line

    while (i < input.size()) {
        string c(1, input[i]);
        unsigned char firstByte = static_cast<unsigned char>(input[i]);

        if (firstByte == 0xF0) { 
            string emoji = input.substr(i, 4);
            cout << "c is 4 byte Emoji:" << emoji << "\n";
            tokens.push_back({myTokenType::KEYWORD, "keyword", line}); // 🔥
            i += 3;
        }
        else if (firstByte == 0xE2) { 
            string emoji = input.substr(i, 3);

            if (emoji == "\xE2\x9C\x85") {
                cout << "its a True" << "\xE2\x9C\x85" << "\n";
                token = "true";
                tokens.push_back({myTokenType::LITERAL, token, line}); // 🔥
            }
            else if (emoji == "\xE2\x9D\x8C") {
                cout << "its a False" << "\xE2\x9D\x8C" << "\n";
                token = "false";
                tokens.push_back({myTokenType::LITERAL, token, line}); // 🔥
            }
            else {
                cout << "c is 3 byte Emoji:" << emoji << "\n";
                token = "keyword";
                tokens.push_back({myTokenType::KEYWORD, token, line}); // 🔥
            }
            i += 2;
        }

        if (c[0] == '\n') { // 🔥 Handle newline early
            line++;
            i++;
            continue;
        }

        if (isspace(c[0])) {
            i++;
            continue;
        }

        // Comments
        if (input.substr(i, 2) == "//") {
            cout << "Is a Comment\n";
            while (i < input.size() && input[i] != '\n') i++;
            continue;
        }
        if (input.substr(i, 2) == "/*") {
            cout << "Is a Comment\n";
            i += 2;
            while (i + 1 < input.size() && input.substr(i, 2) != "*/") {
                if (input[i] == '\n') line++; // 🔥 Count lines inside block comment
                i++;
            }
            i += 2;
            continue;
        }

        // String literals
        if (c[0] == '"') {
            cout << "Is a String literal\n";
            size_t start = i;
            token += c;
            i++;
            bool isValidString = false;

            int numbToKeepTrackOfLinesInStringInCaseOfReset = 0;

            while (i < input.size()) {
                token += input[i];
                if (input[i] == '"') {
                    isValidString = true;
                    break;
                }
                if (input[i] == '\n') {
                    line++; // 🔥 String across lines
                    numbToKeepTrackOfLinesInStringInCaseOfReset++;
                }
                i++;
            }

            if (isValidString) {
                tokens.push_back({myTokenType::LITERAL, token, line}); // 🔥
            } else {
                cout << "Error: Unmatched double quote found. Skipping invalid string literal.\n";
                i = start; // Reset i to the original position before "
                line = line - numbToKeepTrackOfLinesInStringInCaseOfReset;
            }

            token.clear();
            i++;
            continue;
        }

        // Floating point and integer literals
        if (isdigit(c[0])) {
            cout << "Is a Float or Integer\n";
            token += c;
            bool isFloat = false;
            while (i + 1 < input.size() && (isdigit(input[i + 1]) || input[i + 1] == '.')) {
                if (input[i + 1] == '.') isFloat = true;
                token += input[++i];
            }
            tokens.push_back({myTokenType::LITERAL, token, line}); // 🔥
            token.clear();
            i++;
            continue;
        }

        // Identifiers
        if (isalpha(c[0])) { // First char must be a letter (a-z or A-Z)
            cout << "Is an Identifier\n";
            token += c[0];
            i++;
            while (i < input.size() && (isalpha(input[i]) || input[i] == '_')) {
                token += input[i++];
            }
            if (regex_match(token, idenPattern)) {
                tokens.push_back({myTokenType::IDENTIFIER, token, line}); // 🔥
            }
            token.clear();
            continue;
        }


        // Operators
        if (regex_match(c, operatorPattern)) {
            cout << "Is a Operator\n";
            token += c;
            if (i + 1 < input.size() && regex_match(string(1, input[i + 1]), operatorPattern)) {
                token += input[++i];
            }
            tokens.push_back({myTokenType::OPERATOR, token, line}); // 🔥
            token.clear();
            i++;
            continue;
        }

        // Separators
        if (regex_match(c, separatorPattern)) {
            cout << "Is a Separator\n";
            tokens.push_back({myTokenType::SEPARATOR, c, line}); // 🔥
            i++;
            continue;
        }

        i++;
    }
    return tokens;
}

    // Main function
    // int main() {
    //     SetConsoleOutputCP(CP_UTF8);
    //     // ❌✅
    //     std::string content = R"(
    //                             🏫 A_B_ _C::56BCA
    //                             🔁(a<<==b!!&&=56bc
    //                             🔤s str"abc+=56xy+=75\\"+=
    //                             a+'++'\n'\\'\\'++'++'r
    //                             a== 1 " 1 abc//xyz
    //                             🔙 a.77b.7bc.88.99.5\\a=b=c
    //                             🤔😅(( /*a==b+=***//a++
    //                         )";
        
                                                                                                    
    //     std::cout << "Input Content:\n" << content << "\n\n";
    //     // Tokenize the input
    //     vector<Token> tokens = tokenize(content);

    //     // Print tokens
    //     cout << "---------------------------------Printing Tokens---------------------------------\n";
    //     for (const Token &t : tokens) {
    //         std::cout << "Line " << t.line << " -> " << tokenTypeToString(t.type) << ": " << t.value << std::endl;
    //     }

    //     return 0;
    // }

int main() {
    SetConsoleOutputCP(CP_UTF8);

    string content;
    string line;
    ifstream inputFile("codeInput.cpp");

    if (!inputFile) {
        cerr << "Error opening file.\n";
        return 1;
    }

    // Read the file line by line
    //  while (getline(inputFile, line, '\n')) { //delimiter
    while (getline(inputFile, line)) {
        // Process each line
        cout << "Line: " << line << endl;
        content += line + '\n';

    }

    inputFile.close();  // Don't forget to close the file!

    std::cout << "Input Content:\n" << content << "\n\n";

    // Tokenize the input
    vector<Token> tokens = tokenize(content);

    // Print tokens
    cout << "---------------------------------Printing Tokens---------------------------------\n";
    for (const Token &t : tokens) {
        std::cout << "Line " << t.line << " -> " << tokenTypeToString(t.type) << ": " << t.value << std::endl;
    }

    return 0;
}
