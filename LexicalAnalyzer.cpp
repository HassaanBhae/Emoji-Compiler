    #include <iostream>
    #include <windows.h>
    #include <vector>
    #include <regex>
    #include <unordered_set>
    #include <string>
    #include <fstream>

    using namespace std;
        
    // Token types
    enum class myTokenType { KEYWORD, IDENTIFIER, LITERAL, OPERATOR, SEPARATOR, COMMENT, WHITESPACE ,ACCESSMODIFIER ,DT ,ID,
        IntConst, FloatConst, CharConst, StringConst, BoolConst, 
        If, Else, Elseif,
        Do, While, For,
        Return,
        Class, Interface,Abstract,
        AccessModifier, Static,Final ,
        Extends, Implements,
        This, Super, New,
        Try, Catch, Finally, Throw,
        Comma, Colon, SemiColon, DBQoute, Qoute,
        OpenRoundBrkt, CloseRoundBrkt, OpenSqrBrkt, CloseSqrBrkt, OpenCurlyBrkt, CloseCurlyBrkt,
        Dot,
        AddSub, Increase_Decrease, MulDivMod,
        RO1, RO2,
        Equal, CompoundAssign,
        AND, OR, NOT,
        InvalidInput,
        $, Break, Continue,
        Void , Main,Const,
        object };

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
        //EMOJI CHECKER
// ----------------------------------------------------------------------------------------------
        if (firstByte == 0xF0) { 
            string emoji = input.substr(i, 4);
            cout << "c is 4 byte Emoji:" << emoji << "\n";
            tokens.push_back({myTokenType::KEYWORD, emoji, line}); 
            i += 3;
        }
        else if (firstByte == 0xE2) { 
            string emoji = input.substr(i, 3);

            if (emoji == "\xE2\x9C\x85") {
                cout << "its a True" << "\xE2\x9C\x85" << "\n";
                token = "true";
                tokens.push_back({myTokenType::LITERAL, token, line}); 
            }
            else if (emoji == "\xE2\x9D\x8C") {
                cout << "its a False" << "\xE2\x9D\x8C" << "\n";
                token = "false";
                tokens.push_back({myTokenType::LITERAL, token, line}); 
            }
            else {
                cout << "c is 3 byte Emoji:" << emoji << "\n";
                token = "keyword";
                tokens.push_back({myTokenType::KEYWORD, token, line}); 
            }
            i += 2;
        }
// ---------------------------------------------------------------------------------------------------

        if (c[0] == '\n') { 
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
    cout << "Testing Token Index1:" <<tokens[1].value << endl;
    cout << "Testing Token Index2:" <<tokens[2].value << endl;
    cout << "Testing Token Index9:" <<tokens[9].value << endl;
    cout << "Total tokens: " << tokens.size() << endl;

    SA parser(tokens);
    return 0;
}
///SYNTAX ANALYZER
class SA {
    int index = 0; // Current token index
    vector<Token> tokens;
    public:
    // Constructor for SA to accept tokens
    SA(const vector<Token>& t) : tokens(t) {}
    // <program> -> <dec> <mainTerminal> <dec>
    bool program() {
        if (tokens[index].type == myTokenType::ACCESSMODIFIER || tokens[index].type == myTokenType::ACCESSMODIFIER ||
            tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID) {
            if (dec()) {
                if (mainTerminal()) {
                    if (dec()) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    // <dec> ->  <struct_dec> | <struct_children> | <interface> | ε
    bool dec() {
        if (tokens[index].type == myTokenType::ACCESSMODIFIER || tokens[index].type == myTokenType::ACCESSMODIFIER) {
            if (struct_dec()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID) {
            if (struct_children()) {
                return true;
            }
        }
        else if (tokens[index].value == "interface") { // Checking token value
            if (interfaceFunc()) {
                return true;
            }
        }
        // else if (/* ε case: check if current token is FIRST of next rule */) {
        //     return true;
        // }
    
        return false;
    }

    // <mainTerminal> -> main ( ) { <mst> }
    bool mainTerminal() { 

        if (tokens[index].type == myTokenType::Main) {
            if (tokens[index].type == myTokenType::Main) {
                index++; 
                if (tokens[index].type ==  myTokenType::OpenRoundBrkt) {
                    index++; 
                    if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                        index++; 
                        if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
                            index++; 
                            if (MST()) {
                                if(tokens[index].type == myTokenType::CloseCurlyBrkt){
                                    index++;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false; 
    }
    // <struct_dec> -> <access_modifier> struct ID <extends> { <struct_body>}
    bool struct_dec() { 
        if (tokens[index].type == myTokenType::ACCESSMODIFIER) {  // Empty if to check FIRST set
            if (tokens[index].type == myTokenType::ACCESSMODIFIER) {
                index++; 
                if (tokens[index].type == myTokenType::Class) {
                    index++; 
                    if (tokens[index].type == myTokenType::ID) {
                        index++; 
                        if (tokens[index].type == myTokenType::Extends || tokens[index].type == myTokenType::OpenCurlyBrkt) {
                            index++; 
                            if (struct_body()) {
                                if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                                    index++;
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false; 
    }
    // <access_modifier> -> public | private
    bool access_modifier() {
        if (tokens[index].type == myTokenType::ACCESSMODIFIER) {
            if (tokens[index].type == myTokenType::ACCESSMODIFIER) {
                index++; 
                return true;
            } 
        }
        if (tokens[index].type == myTokenType::ACCESSMODIFIER) {
            if (tokens[index].type == myTokenType::ACCESSMODIFIER) {
                index++; 
                return true;
            } 
        }
        return false;
    }
    // <extends> -> extends:ID | ε
    bool extends() {
        if (tokens[index].type == myTokenType::Extends) {
            if (tokens[index].type == myTokenType::Extends) {
                index++; 
                if (tokens[index].type == myTokenType::ID) {
                    index++; 
                    return true;
                }
            }
        }
        else if(tokens[index].type == myTokenType::OpenCurlyBrkt){
            index++; 
            return true;
        }   
        return false; 
    }
    
    // <struct_body> -> <struct_children> <struct_body_tail>
    bool struct_body() {
        if (tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID ) {  // Empty if to check FIRST set
            if (struct_children()) {
                if (struct_body_tail()) {
                    return true;
                }
            }
        }
        return false; 
    }

    // <struct_children> -> DT ID <dt_decORfunc_dec> | <array_dec> | <Constructor>
    bool struct_children() {
        if (tokens[index].type == myTokenType::DT) {
            if (tokens[index].type == myTokenType::DT) {
                if (tokens[index + 1].type == myTokenType::ID) {
                    if (dt_decORfunc_dec()) {
                        return true;
                    }
                }
            }
        }
        else if (tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID) {  // Empty if to check FIRST set for array_dec
            if (array_dec()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::ID) {  // Empty if to check FIRST set for Constructor
            if (Constructor()) {
                return true;
            }
        }
        return false;
    }
    
    // <struct_body_tail> -> <struct_body> | ε
    bool struct_body_tail() {
        if (tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID) {  // Empty if to check FIRST set
            if (struct_body()) {
                return true;
            }
        }
        else if(tokens[index].type == myTokenType::CloseCurlyBrkt){
            return true;
        }
        return false;  
    }
    // <interface> -> interface ID <interface_inherit>  { <interface_body> }
    bool interfaceFunc() {
        if (tokens[index].type == myTokenType::Interface) {
            if (tokens[index].type == myTokenType::Interface) {
                index++;
                if (tokens[index].type == myTokenType::ID) {
                    index++;
                    if (interface_inherit()) {
                        if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
                            index++;
                            if (interface_body()) {
                                if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    // <interface_inherit> -> extends ID <interface_inherit2> | null
    bool interface_inherit() {
        if (tokens[index].type == myTokenType::Extends) {  // Empty if to check FIRST set
            if (tokens[index].type == myTokenType::Extends) {
                index++;
                if (tokens[index].type == myTokenType::ID) {
                    index++;
                    if (interface_inherit2()) {
                        return true;
                    }
                }
            }
        } 
        else if (tokens[index].type == myTokenType::$) {  // Empty if for null case
            return true;  // null case
        }
        return false;
    }
    
    // <interface_inherit2> -> , ID <interface_inherit2> | null
    bool interface_inherit2() {
        if (tokens[index].type == myTokenType::Comma) {  // Empty if to check FIRST set
            if (tokens[index].type == myTokenType::Comma) {  
                index++;  
                if (tokens[index].type == myTokenType::ID) {  
                    index++;  // Move past the ID
                    if (interface_inherit2()) {  //ERROR
                        return true;
                    }
                }
            } 
        } 
        else if (tokens[index].type == myTokenType::CloseCurlyBrkt) {  // Empty case for null or end
            return true;  // Null case, as per the production
        }
        return false;  // Return false if none of the conditions are met
    }
    
    // <interface_body> -> <dt_dec> | DT ID ( <param_list> );
    bool interface_body() {
        if (tokens[index].type == myTokenType::Equal || tokens[index].type == myTokenType::SemiColon || tokens[index].type == myTokenType::CloseCurlyBrkt) {  // Empty if to check FIRST set
            if (dt_dec()) {  // Check for <dt_dec>
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::DT) {  // Check for <dt_dec>
            if (tokens[index].type == myTokenType::DT) {  // Check for DT to start the second rule
                index++;
                if (tokens[index].type == myTokenType::ID) {  // Check for ID after DT
                    index++;
                    if (tokens[index].type == myTokenType::OpenRoundBrkt) {  // Check for open round bracket
                        index++;
                        if (param_list()) {  // Check for <param_list>
                            if (tokens[index].type == myTokenType::CloseRoundBrkt) {  // Check for semicolon
                                index++;
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;  // Return false if none of the conditions are met
    }
    // <dt_decORfunc_dec> -> <func_dec> | <dt_dec>
    bool dt_decORfunc_dec() {
        if (tokens[index].type == myTokenType::OpenRoundBrkt) { // Empty check for FIRST set
            if (func_dec()) {
                return true;
            }
        }
        if (tokens[index].type == myTokenType::Equal || tokens[index].type == myTokenType::SemiColon || tokens[index].type == myTokenType::CloseCurlyBrkt) {  // Empty if to check FIRST set
            if (dt_dec()) {
                return true;
            }
        }
        return false;
    }
    // <var_init> → = <Const_or_ID> | ε					
    bool var_init() {
        // if (tokens[index].type == myTokenType::IntConst ||tokens[index].type == myTokenType::FloatConst || tokens[index].type == myTokenType::CharConst || tokens[index].type == myTokenType::BoolConst || tokens[index].type == myTokenType::StringConst  ) { // Empty check for FIRST set of the rule
        if (tokens[index].type == myTokenType::Equal   ) {
            if (tokens[index].type == myTokenType::Equal   ) { // Empty check for FIRST set of the rule
                index++;
                if (Const_or_ID()) {
                    return true;
                }
            }
        } 
        else if (tokens[index].type == myTokenType::Comma || tokens[index].type == myTokenType::Colon) { // Empty check for ε case
            return true; 
        }
        return false;
    }
    // <var_init_tail> → , ID <var_init> <var_init_tail> | ε			
    bool var_init_tail() {
        if (tokens[index].type == myTokenType::Comma) { // Empty check for FIRST set of the rule
            if (tokens[index].type == myTokenType::Comma) { // Checking for ','
                index++;
                if (tokens[index].type == myTokenType::ID) { // Checking for 'ID'
                    index++;
                    if (var_init()) {
                        if (var_init_tail()) {
                            return true;
                        }
                    }
                }
            }
        }
        else if (tokens[index].type == myTokenType::SemiColon) { // Empty check for ε case
            return true; // ε case: no tokens to consume
        }
        return false;
    }
    // <Const_or_ID> -> Const | ID
    bool Const_or_ID() {
        if (tokens[index].type == myTokenType::IntConst ||tokens[index].type == myTokenType::FloatConst || tokens[index].type == myTokenType::CharConst || tokens[index].type == myTokenType::BoolConst || tokens[index].type == myTokenType::StringConst  ) { // Empty check for FIRST set of the rule
            if (tokens[index].type == myTokenType::IntConst ||tokens[index].type == myTokenType::FloatConst || tokens[index].type == myTokenType::CharConst || tokens[index].type == myTokenType::BoolConst || tokens[index].type == myTokenType::StringConst  ) { // Empty check for FIRST set of the rule
                index++;
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::ID) { // Empty check for ε case
            if (tokens[index].type == myTokenType::ID) { // Checking for 'ID'
                index++;
                return true;
            }
        }
        return false;
    }
    // <array_dec> -> <arr_type> ID [] = <arr_items> ;
    bool array_dec() {
        if (tokens[index].type == myTokenType::Const || tokens[index].type == myTokenType::ID) { // Empty check for FIRST set of <arr_type>
            if (arr_type()) { // Checking for <arr_type>
                index++;
                if (tokens[index].type == myTokenType::ID) { // Checking for ID
                    index++;
                    if (tokens[index].type == myTokenType::OpenSqrBrkt) { // Checking for '['
                        index++;
                        if (tokens[index].type == myTokenType::CloseSqrBrkt) { // Checking for ']'
                            index++;
                            if (tokens[index].type == myTokenType::Equal) { // Checking for '='
                                index++;
                                if (arr_items()) { // Checking for <arr_items>
                                    if (tokens[index].type == myTokenType::SemiColon) { // Checking for ';'
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    // <arr_items> -> <arr_item> <arr_items_tail> ;
    bool arr_items() {
        if (tokens[index].type == myTokenType::Const || tokens[index].type == myTokenType::ID || tokens[index].type == myTokenType::OpenCurlyBrkt) { // Empty check for FIRST set of <arr_type>
            if (arr_item()) { // Checking for <arr_item>
                if (arr_items_tail()) { // Checking for <arr_items_tail>
                    if (tokens[index].type == myTokenType::SemiColon) { // Checking for ';'
                        return true;
                    }
                }
            }
        }
        return false;
    }
    // <arr_items_tail> -> <arr_items_tail_prime> | ε ;
    bool arr_items_tail() {
        if (tokens[index].type == myTokenType::Comma) { // Empty check for FIRST set of <arr_items_tail_prime>
            if (arr_items_tail_prime()) { // Checking for <arr_items_tail_prime>
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::SemiColon){
            return true;
        }
        return false; 
    }
    // <arr_items_tail_prime> -> , <arr_item> <arr_items_tail_prime> ;
    bool arr_items_tail_prime() {
        if (tokens[index].type == myTokenType::Comma) {  // Empty if to check FIRST set
            if (tokens[index].type == myTokenType::Comma) {
                index++; 
                if (arr_item()) {
                    if (arr_items_tail_prime()) {
                        if (tokens[index].type == myTokenType::SemiColon) {
                            return true;
                        }
                    }
                }
            }
        }
    
        return false; 
    }
    // <arr_item> -> <Const_or_ID>  | <arr_row>
    bool arr_item() {
        if (tokens[index].type == myTokenType::Const || tokens[index].type == myTokenType::ID) {  // Empty if to check FIRST set
            if (Const_or_ID()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::OpenCurlyBrkt) {  // Empty if for checking the FIRST set of <arr_row>
            if (arr_row()) {
                return true;
            }
        }
    
        return false;
    }
    // <arr_row> -> { <arr_items> }
    bool arr_row() {
        if (tokens[index].type == myTokenType::OpenCurlyBrkt) {  // Empty if to check FIRST set
            if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
                index++;
                if (arr_items()) {
                    if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                        index++;
                        return true;
                    }
                }
            }
        }
    
        return false;
    }
    // <arr_type> -> DT | ID
    bool arr_type() {
        if (tokens[index].type == myTokenType::DT) {  // Empty if to check FIRST set
            if (tokens[index].type == myTokenType::DT) {
                index++;
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::ID) {
            if (tokens[index].type == myTokenType::ID){
                index++;
                return true;
            }
        }
    
        return false;
    }
    // <SST> -> <while_loop> | <for_loop> | <if> | <do_while> | <Expr> | <try> | <throw> | <return> | <continue> | <break> | <dt_dec> | <func_dec> | <func_call> 
    bool SST() {
        if (tokens[index].type == myTokenType::While) {  // Empty if to check FIRST set for <while_loop>
            if (while_loop()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::For) {  // Empty if to check FIRST set for <for_loop>
            if (for_loop()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::If) {  // Empty if to check FIRST set for <if_else>
            if (if_statement()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Do) {  // Empty if to check FIRST set for <do_while>
            if (do_while()) {
                return true;
            }
        }
        // else if (tokens[index].type == myTokenType::) {  // Empty if to check FIRST set for <Expr>
        //     if (Expr()) {
        //         return true;
        //     }
        // }
        else if (tokens[index].type == myTokenType::Try) {  // Empty if to check FIRST set for <try>
            if (try_statement()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Throw) {  // Empty if to check FIRST set for <throw>
            if (throw_statement()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Return) {  // Empty if to check FIRST set for <return>
            if (return_statement()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Continue) {  // Empty if to check FIRST set for <continue>
            if (continue_stmt()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Break) {  // Empty if to check FIRST set for <break>
            if (break_stmt()) {
                return true;
            }
        }
        if (tokens[index].type == myTokenType::Equal || tokens[index].type == myTokenType::SemiColon || tokens[index].type == myTokenType::CloseCurlyBrkt) {  // Empty if to check FIRST set
            if (dt_dec()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::OpenCurlyBrkt) {  // Empty if to check FIRST set for <func_dec>
            if (func_dec()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::ID) {  // Empty if to check FIRST set for <func_call>
            if (func_call()) {
                return true;
            }
        }
    
        return false;
    }
    // <MST> -> <SST><MST> | ε
    bool MST() {
        if (tokens[index].type == myTokenType::While || tokens[index].type == myTokenType::For ||
            tokens[index].type == myTokenType::If || tokens[index].type == myTokenType::Do ||
            tokens[index].type == myTokenType::ID || tokens[index].type == myTokenType::Try ||
            tokens[index].type == myTokenType::Throw || tokens[index].type == myTokenType::Return ||
            tokens[index].type == myTokenType::Continue || tokens[index].type == myTokenType::Break ||
            tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::OpenRoundBrkt) {
            
            if (SST()) {
                if (MST()) {
                    return true;
                }
                return false;
            }
            return false;
        }
        else if(tokens[index].type == myTokenType::CloseRoundBrkt){
            return true;
        }
        return false;  // ε case
    }
    // <while_loop> -> while (<cond>)<loop_body>
    bool while_loop() {
        if (tokens[index].type == myTokenType::While) {
            if (tokens[index].type == myTokenType::While) {
                index++;
                if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                    index++;
                    if (cond()) {
                        if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                            index++;
                            if (loop_body()) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    // <cond> ->  <Const_or_ID> | <Const_or_ID>  <ROP>  <Const_or_ID> | <Expr> 
    bool cond() {
        if (tokens[index].type == myTokenType::Const || tokens[index].type == myTokenType::ID) {
            if (Const_or_ID()) {
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Const || tokens[index].type == myTokenType::ID) {
            if (Const_or_ID()) {
                if (ROP()) {
                    if (Const_or_ID()) {
                        return true;
                    }
                } 
            }
        }
        // else if (Expr()){
        //     if(Expr()){

        //     }
        // }
    
        return false;
}
    // <ROP> -> RO1 | RO2
    bool ROP() {
        if (tokens[index].type == myTokenType::RO1) {
            if (tokens[index].type == myTokenType::RO1) {
                index++;
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::RO2) {
            if (tokens[index].type == myTokenType::RO2) {
                index++;
                return true;
            }
        }
        return false;
    }
    // <loop_body> -> ; | <SST> | {<MST>}
    bool loop_body() {
        if (tokens[index].type == myTokenType::SemiColon) {
            if (tokens[index].type == myTokenType::SemiColon) {
                index++;
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::While || tokens[index].type == myTokenType::For ||
            tokens[index].type == myTokenType::If || tokens[index].type == myTokenType::Do ||
            tokens[index].type == myTokenType::ID || tokens[index].type == myTokenType::Try ||
            tokens[index].type == myTokenType::Throw || tokens[index].type == myTokenType::Return ||
            tokens[index].type == myTokenType::Continue || tokens[index].type == myTokenType::Break ||
            tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::OpenRoundBrkt) {            
                if (SST()) {
                    index++;
                    return true;
            }
        }
        else if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
            index++;
            if (MST()) {
                if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                    index++;
                    return true;
                }
            }
        }
        return false;
    }
    // <for_loop> -> for (<F1><F2>;<F3>) <loop_body>
    bool for_loop() {
        if (tokens[index].type == myTokenType::For) {
            if (tokens[index].type == myTokenType::For) {
                index++;
                if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                    index++;
                    if (F1()) {
                        if (F2()) {
                            if (tokens[index].type == myTokenType::SemiColon) {
                                index++;
                                if (F3()) {
                                    if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                                        index++;
                                        if (loop_body()) {
                                            return true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    // <F1> -> <dt_dec> | <assign_st> | ;
    bool F1() {
        if (tokens[index].type == myTokenType::Equal || tokens[index].type == myTokenType::SemiColon || tokens[index].type == myTokenType::CloseCurlyBrkt) {  // Empty if to check FIRST set
            if (dt_dec()) {
                return true;
            }
        } else if (tokens[index].type == myTokenType::ID) {  // Empty if to check FIRST set
            if (assign_st()) {
                return true;
            }
        } else if (tokens[index].type == myTokenType::Colon) {  // Empty if to check FIRST set
            if (tokens[index].type == myTokenType::Colon) {
                return true;
            }
        }
        return false;
    }
    // <F2> -> <cond> | ε 
    bool F2() {
        if (tokens[index].type == myTokenType::Const || tokens[index].type == myTokenType::ID ||tokens[index].type == myTokenType::OpenRoundBrkt) {
            if (cond()) {
                return true;
            }
        } else if (tokens[index].type == myTokenType::SemiColon) {
            return true;
        }
        return false;
    }
    // <F3> -> ExprList> | ε
    bool F3() {
        if (Expr()) {
            if (ExprList()) {
                return true;
            }
        } else if (tokens[index].type == myTokenType::OpenRoundBrkt) {
            return true;
        }
        return false;
    }

    // <inc_dec> -> ID increase_decrease 
    bool increase_decrease() {
        if (tokens[index].type == myTokenType::ID) {
            if (tokens[index].type == myTokenType::ID) {
                index++;
                if (tokens[index].type == myTokenType::Increase_Decrease) {
                    index++;
                    return true;
                }
            }
        }
            return false;
    }
    // <assign_st> ->  ID  = <assign_options> ;
    bool assign_st() {
        if (tokens[index].type == myTokenType::ID) {
            if (tokens[index].type == myTokenType::ID) {
                index++;
                if (tokens[index].type == myTokenType::Equal) {
                    index++;
                    if (assign_options()) {
                        if (tokens[index].type == myTokenType::SemiColon) {
                            index++;
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
    // <assign_options> -> <Const_or_ID> | <Expr>
    bool assign_options() {
        if (tokens[index].type == myTokenType::Const || tokens[index].type == myTokenType::ID) {
            if (Const_or_ID()) {
                index++;
                return true;
            }
        }
        else if (Expr()) {
            if (Expr()) {
                return true;
            }
        }
        return false;
    }
    // <if> -> if (<cond>) <loop_body> <else>
    bool if_statement() {
        if (tokens[index].type == myTokenType::If) {
            if (tokens[index].type == myTokenType::If) {
                index++;
                if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                    index++;
                    if (cond()) {
                        if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                            index++;
                            if (loop_body()) {
                                if (else_statement()) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    // <else> -> else <loop_body> | null
    bool else_statement() {
        if (tokens[index].type == myTokenType::Else) {
            if (tokens[index].type == myTokenType::Else) {
                index++;
                if (loop_body()) {
                    return true;
                }
            }
        }else if (tokens[index].type == myTokenType::While || 
            tokens[index].type == myTokenType::For || 
            tokens[index].type == myTokenType::If || 
            tokens[index].type == myTokenType::Do || 
            tokens[index].type == myTokenType::ID || 
            tokens[index].type == myTokenType::Try || 
            tokens[index].type == myTokenType::Throw || 
            tokens[index].type == myTokenType::Return || 
            tokens[index].type == myTokenType::Continue || 
            tokens[index].type == myTokenType::Break || 
            tokens[index].type == myTokenType::DT || 
            tokens[index].type == myTokenType::OpenRoundBrkt || 
            tokens[index].type == myTokenType::CloseCurlyBrkt) {
            return true;
        }
        return false;
        
    }
    // <do_while> -> do <loop_body> while (<cond>);
    bool do_while() {
        if (tokens[index].type == myTokenType::Do) {
            if (tokens[index].type == myTokenType::Do) {
                index++; 
                if (loop_body()) {
                    if (tokens[index].type == myTokenType::While) {
                        index++; 
                        if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                            index++; 
                            if (cond()) {
                                if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                                    index++; 
                                    if (tokens[index].type == myTokenType::SemiColon) {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    
    // <this_rule> -> this ID id_or_func_call ;
    bool this_rule() {
        if (tokens[index].type == myTokenType::This) {
            if (tokens[index].type == myTokenType::This) {  // Empty if to check FIRST set
                index++;
                if (tokens[index].type == myTokenType::ID) {
                    index++;
                    if (id_or_func_call()) {
                        if (tokens[index].type == myTokenType::SemiColon) {
                            index++;
                        return true;
                    }
                }
            }
        }
        }  
        return false;
    }
    // <id_or_func_call> -> ε | ( <arg_list> )
    bool id_or_func_call() {
        if (tokens[index].type == myTokenType::OpenRoundBrkt) {
            if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                index++;
                if (arg_list()) {
                    if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                        index++;
                        return true;
                    }
                }
            }
        }else if (tokens[index].type == myTokenType::CloseRoundBrkt || tokens[index].type == myTokenType::SemiColon){

            return true;  // ε case returns true
        }
        return false;
    }
    // <func_call>-> ID ( <arg_list> )
    bool func_call() {
        if (tokens[index].type == myTokenType::ID) {
            if (tokens[index].type == myTokenType::ID) {  // Check if it's an ID
                index++;
                if (tokens[index].type == myTokenType::OpenRoundBrkt) {  // Check for '('
                    index++;
                    if (arg_list()) {  // Call arg_list to check arguments
                        if (tokens[index].type == myTokenType::CloseRoundBrkt) {  // Check for ')'
                            index++;
                            return true;
                        }
                    }
                }
            }
        }  
        return false;  // Return false if any condition fails
    }
    // <arg_list> -> <arg> <arg_list_tail> | ε
    bool arg_list() {
        if (tokens[index].type == myTokenType::ID || tokens[index].type == myTokenType::Const) {  // Check if it's the start of <arg>
            if (arg()) {  // Call the <arg> function
                if (arg_list_tail()) {  // Call <arg_list_tail> to handle the tail part of the argument list
                    return true;
                }
            }
        } 
        else if(tokens[index].type == myTokenType::CloseRoundBrkt){

        }        
        return true;  
    }
    // <arg_list_tail> -> , <arg> <arg_list_tail> | ε
    bool arg_list_tail() {
        if (tokens[index].type == myTokenType::Comma) {
            if (tokens[index].type == myTokenType::Comma) {  // Check for the ',' indicating the continuation of argument list
                index++;  // Move to the next token
                if (arg()) {  // Process the next argument
                    if (arg_list_tail()) {  // Recursively process the rest of the argument list
                        return true;
                    }
                }
            } 
        }
        else if(tokens[index].type == myTokenType::CloseRoundBrkt){
        }
        return false;
    }
    // <arg> -> Const | ID
    bool arg() {
        if (tokens[index].type == myTokenType::Const) {
            if (tokens[index].type == myTokenType::Const) {  // Check if the token is a constant
                index++;  // Move to the next token
                return true;
            } 
        }  // Check if the token is a constant
        else if (tokens[index].type == myTokenType::ID) {
             if (tokens[index].type == myTokenType::ID) {  // Check if the token is an identifier
                index++;  // Move to the next token
                return true;
            }
        }  // Check if the token is an identifier
        return false;  // If neither Const nor ID, return false
    }
    // <return> ->  return <return_options> ;
    bool return_statement() {
        if (tokens[index].type == myTokenType::Return) {
            if (tokens[index].type == myTokenType::Return) {  // Check if the token is 'return'
                index++;  // Move to the next token
                if (return_options()) {  // Call the return_options function
                    if (tokens[index].type == myTokenType::SemiColon) {  // Check for the semicolon
                        index++;  // Move to the next token
                        return true;  // Successfully parsed the return statement
                    }
                }
            }
        }  // Check if the token is 'return'
        return false;  // Return false if the rule doesn't match
    }
    // <return_options> -> ID | const | <Expr> |null
    bool return_options() {
        if (tokens[index].type == myTokenType::ID) {
            if (tokens[index].type == myTokenType::ID) {
                index++;
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Const) {
            if (tokens[index].type == myTokenType::Const) {
                index++;
                return true;
            }
        }
        // else if (Expr()) {
        //     return true;
        // }
        else if (tokens[index].type == myTokenType::SemiColon) {
             if (tokens[index].type == myTokenType::SemiColon) {
                return true;
            }
        }
        return false;
    }
    // <continue> -> continue;
    bool continue_stmt() {
        if (tokens[index].type == myTokenType::Continue) {
            if (tokens[index].type == myTokenType::Continue) {
                index++;
                if (tokens[index].type == myTokenType::SemiColon) {
                    index++;
                    return true;
                }
            }
        }
        return false;
    }
    // <break> -> break;
    bool break_stmt() {
        if (tokens[index].type == myTokenType::Break) {
            if (tokens[index].type == myTokenType::Break) {
                index++;
                if (tokens[index].type == myTokenType::SemiColon) {
                    index++;
                    return true;
                }
            }
        }
    return false;
    }
    // <try> -> try { <MST> } <catch_list>
    bool try_statement() {
        if (tokens[index].type == myTokenType::Try) {
            if (tokens[index].type == myTokenType::Try) {
                index++;
                if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
                    index++;
                    if (MST()) {
                        if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                            index++;
                            if (catch_list()) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    // <catch_list> -> catch ( ID )  { <MST> }  <catch_list_tail>
    bool catch_list() {
        if (tokens[index].type == myTokenType::Catch) {
            if (tokens[index].type == myTokenType::Catch) {
                index++;
                if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                    index++;
                    if (tokens[index].type == myTokenType::ID) {
                        index++;
                        if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                            index++;
                            if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
                                index++;
                                if (MST()) {
                                    if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                                        index++;
                                    if (catch_list_tail()) {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
        return false;
    }
    // <catch_list_tail> -> catch ( ID ) { <MST> }  <catch_list_tail> | ε
    bool catch_list_tail() {
        if (tokens[index].type == myTokenType::Catch) {
            if (tokens[index].type == myTokenType::Catch) {
                index++;
                if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                    index++;
                    if (tokens[index].type == myTokenType::ID) {
                        index++;
                        if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                            index++;
                            if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
                                index++;
                                if (MST()) {
                                    if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                                        index++;
                                        if (catch_list_tail()) {
                                            return true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (tokens[index].type == myTokenType::While ||
            tokens[index].type == myTokenType::For ||
            tokens[index].type == myTokenType::If ||
            tokens[index].type == myTokenType::Do ||
            tokens[index].type == myTokenType::ID ||
            tokens[index].type == myTokenType::Throw ||
            tokens[index].type == myTokenType::Return ||
            tokens[index].type == myTokenType::Continue ||
            tokens[index].type == myTokenType::Break ||
            tokens[index].type == myTokenType::DT ||
            tokens[index].type == myTokenType::OpenRoundBrkt){
                return true;
        }
        return false;
    }
    // <throw> -> throw <throw_options>: function with a n extra empty if
    bool throw_statement() {
        if (tokens[index].type == myTokenType::Throw) {
            if (tokens[index].type == myTokenType::Throw) {  // Check if it's the "throw" keyword
                index++;
                if (throw_options()) {  // Check if the throw options match
                    return true;
                }
            }
        }  // Check if it's the "throw" keyword
        return false;
    }
    // <throw_options> -> ID | Const | new ID ( <param_list> ) 
    bool throw_options() {
        if (tokens[index].type == myTokenType::ID) {
            if (tokens[index].type == myTokenType::ID) {  // Check for ID type
                index++;
                return true;
            }
        }  // Check for ID type
            
        else if (tokens[index].type == myTokenType::Const) {

            if (tokens[index].type == myTokenType::Const) {  // Check for Const type
                index++;
                return true;
            }
        }  // Check for Const type
    
        else if (tokens[index].type == myTokenType::New) {
             if (tokens[index].type == myTokenType::New) {  // Check for "new" keyword
                index++;
                if (tokens[index].type == myTokenType::ID) {  // Check for ID after "new"
                    index++;
                    if (tokens[index].type == myTokenType::OpenRoundBrkt) {  // Check for opening parentheses
                        index++;
                        if (param_list()) {  // Check for parameters
                            if (tokens[index].type == myTokenType::CloseRoundBrkt) {  // Check for closing parentheses
                                index++;
                                return true;
                            }
                        }
                    }
                }
            }
        }  // Check for "new" keyword
    
        return false;
    }
    
    // <func_dec> -> ( <param_list> ) <Body>
    bool func_dec() {
        if (tokens[index].type == myTokenType::OpenRoundBrkt) {
            if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                index++;
                if (param_list()) {
                    if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                        index++;
                        if (Body()) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
    // <Body> ->  { <MST> }
    bool Body() {
        if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
            if (tokens[index].type == myTokenType::OpenCurlyBrkt) {
                index++;
                if (MST()) {
                    if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                        index++;
                        return true;
                    }
                }
            }
        }
        return false;
    }
    // <param_list> -> <param> <param_list_tail> | ε
    bool param_list() {
        if (tokens[index].type == myTokenType::DT) {
            if (param()) {
                if (param_list_tail()) {
                    return true;
                }
            }
        } else if (tokens[index].type == myTokenType::CloseRoundBrkt) {

            return true;
        }
        return false;
    }
    // <param_list_tail> -> , <param> <param_list_tail> | ε
    bool param_list_tail() {
        if (tokens[index].type == myTokenType::Comma) {
            if (tokens[index].type == myTokenType::Comma) {
                index++;
                if (param()) {
                    if (param_list_tail()) {
                        return true;
                    }
                }
            }
        } else if(tokens[index].type == myTokenType::OpenRoundBrkt){
            return true;
        }
        return false;
    }
    // <param> -> DT ID
    bool param() {
        if (tokens[index].type == myTokenType::DT) {
            if (tokens[index].type == myTokenType::DT) {
                index++;
                if (tokens[index].type == myTokenType::ID) {
                    index++;
                    return true;
                }
            }
        }
        return false;
    }
    // <Constructor> -> ID ( <param_list> ) <Body> 
    bool Constructor() {
        if (tokens[index].type == myTokenType::ID) {
            if (tokens[index].type == myTokenType::ID) {
                index++;
                if (tokens[index].type == myTokenType::OpenRoundBrkt) {
                    index++;
                    if (param_list()) {
                        if (tokens[index].type == myTokenType::CloseRoundBrkt) {
                            index++;
                            if (Body()) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }
    // <dt_dec> →  <var_init> <var_init_tail>  ;
    bool dt_dec() {
        if (tokens[index].type == myTokenType::Equal || 
            tokens[index].type == myTokenType::While ||
            tokens[index].type == myTokenType::For ||
            tokens[index].type == myTokenType::If ||
            tokens[index].type == myTokenType::Do ||
            tokens[index].type == myTokenType::ID ||
            tokens[index].type == myTokenType::Try ||
            tokens[index].type == myTokenType::Throw ||
            tokens[index].type == myTokenType::Return ||
            tokens[index].type == myTokenType::Continue ||
            tokens[index].type == myTokenType::Break ||
            tokens[index].type == myTokenType::DT ||
            tokens[index].type == myTokenType::OpenRoundBrkt 
        ) {
            if (var_init()) {
                if (var_init_tail()) {
                    if (tokens[index].type == myTokenType::SemiColon) {
                        index++;
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    bool Expr(){
        return false;
    }
    // <ExprList> -> <Expr> <ExprListTail>
    bool ExprList() {
        if (Expr()) {
            if (ExprListTail()) {
                return true;
            }
        }
        return false;
    }
    bool ExprListTail(){
        return false;
    }



};
