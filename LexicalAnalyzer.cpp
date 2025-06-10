#include <iostream>
#include <windows.h>
#include <vector>
#include <regex>
#include <unordered_set>
#include <string>
#include <fstream>

// using namespace std;
// using namespace std::;
    
// Token types
enum class myTokenType { KEYWORD, IDENTIFIER, LITERAL, OPERATOR, SEPARATOR, COMMENT, WHITESPACE  ,DT ,ID,
    If, Else, Elseif,Switch,Case,Default,
    Do, While, For,
    Return,
    Class,Struct, Interface,Abstract,
    AccessModifier, Static,Final ,Global,
    Extends, 
    This,  New,
    Try, Catch, Finally, Throw,
    Comma, Colon, SemiColon, DBQoute, Qoute,
    OpenRoundBrkt, CloseRoundBrkt, OpenSqrBrkt, CloseSqrBrkt, OpenCurlyBrkt, CloseCurlyBrkt,
    Dot,
    AddSub, Increase_Decrease, MulDivMod,
    RO1, RO2,
    Equal, CompoundAssign,
    AND, OR, NOT,Input,Print,
    InvalidInput,
    $, Break, Continue,
    Void , Main,Const,
    object };

// Token structure
struct Token {
    myTokenType type;
    std::string value;
    int line;
};

// Keywords (supports Unicode emoji-based keywords)
std::unordered_set<std::string> keywords = {"🔢", "🌊", "🔠", "🔤", "🤔", "😅", "🔙", "🔁", "➰", "✅", "❌"};

// Regular expressions for token classification
std::regex intPattern(R"(^[+-]?(0|[1-9][0-9]*)$)");
std::regex floatPattern(R"(^[+-]?([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)$)");
std::regex stringPattern(R"(^\"(\\.|[^\"])*\"$)");
std::regex boolPattern(R"(^(✅|❌)$)");
// regex idenPattern(R"(^[a-z]+$)");
// regex idenPattern(R"(^[a-zA-Z]+$)");
// std::regex idenPattern(R"(^[a-zA-Z][a-zA-Z_]*$)");
std::regex idenPattern(R"(^[a-zA-Z][a-zA-Z_]*$)");
std::regex operatorPattern(R"([\+\-\*/%=<>!&|^~]+)");
std::regex separatorPattern(R"([\(\)\{\}\[\];,.])");

// Function to convert myTokenType to string
std::string tokenTypeToString(myTokenType type) {
    switch (type) {
        case myTokenType::KEYWORD: return "KEYWORD";
        case myTokenType::IDENTIFIER: return "IDENTIFIER";
        case myTokenType::LITERAL: return "LITERAL";
        case myTokenType::OPERATOR: return "OPERATOR";
        case myTokenType::SEPARATOR: return "SEPARATOR";
        case myTokenType::COMMENT: return "COMMENT";
        case myTokenType::WHITESPACE: return "WHITESPACE";
        case myTokenType::DT: return "DT";
        case myTokenType::ID: return "ID";
        case myTokenType::If: return "If";
        case myTokenType::Else: return "Else";
        case myTokenType::Elseif: return "Elseif";
        case myTokenType::Switch: return "Switch";
        case myTokenType::Case: return "Case";
        case myTokenType::Default: return "Default";
        case myTokenType::Do: return "Do";
        case myTokenType::While: return "While";
        case myTokenType::For: return "For";
        case myTokenType::Return: return "Return";
        case myTokenType::Class: return "Class";
        case myTokenType::Struct: return "Struct";
        case myTokenType::Interface: return "Interface";
        case myTokenType::Abstract: return "Abstract";
        case myTokenType::AccessModifier: return "AccessModifier";
        case myTokenType::Static: return "Static";
        case myTokenType::Final: return "Final";
        case myTokenType::Global: return "Global";
        case myTokenType::Extends: return "Extends";
        case myTokenType::This: return "This";
        case myTokenType::New: return "New";
        case myTokenType::Try: return "Try";
        case myTokenType::Catch: return "Catch";
        case myTokenType::Finally: return "Finally";
        case myTokenType::Throw: return "Throw";
        case myTokenType::Comma: return "Comma";
        case myTokenType::Colon: return "Colon";
        case myTokenType::SemiColon: return "SemiColon";
        case myTokenType::DBQoute: return "DBQoute";
        case myTokenType::Qoute: return "Qoute";
        case myTokenType::OpenRoundBrkt: return "OpenRoundBrkt";
        case myTokenType::CloseRoundBrkt: return "CloseRoundBrkt";
        case myTokenType::OpenSqrBrkt: return "OpenSqrBrkt";
        case myTokenType::CloseSqrBrkt: return "CloseSqrBrkt";
        case myTokenType::OpenCurlyBrkt: return "OpenCurlyBrkt";
        case myTokenType::CloseCurlyBrkt: return "CloseCurlyBrkt";
        case myTokenType::Dot: return "Dot";
        case myTokenType::AddSub: return "AddSub";
        case myTokenType::Increase_Decrease: return "Increase_Decrease";
        case myTokenType::MulDivMod: return "MulDivMod";
        case myTokenType::RO1: return "RO1";
        case myTokenType::RO2: return "RO2";
        case myTokenType::Equal: return "Equal";
        case myTokenType::CompoundAssign: return "CompoundAssign";
        case myTokenType::AND: return "AND";
        case myTokenType::OR: return "OR";
        case myTokenType::NOT: return "NOT";
        case myTokenType::Input: return "Input";
        case myTokenType::Print: return "Print";
        case myTokenType::InvalidInput: return "InvalidInput";
        case myTokenType::$: return "$";
        case myTokenType::Break: return "Break";
        case myTokenType::Continue: return "Continue";
        case myTokenType::Void: return "Void";
        case myTokenType::Main: return "Main";
        case myTokenType::Const: return "Const";
        case myTokenType::object: return "object";
        default: return "UNKNOWN";
    }
}


// Tokenization function
std::vector<Token> tokenize(const std::string &input) {
    std::cout << "Tokenization Started!\n";
    std::vector<Token> tokens;
    std::string token;
    size_t i = 0;
    int line = 1; // 🔥 Track the current line

    while (i < input.size()) {
        std::string c(1, input[i]);
        unsigned char firstByte = static_cast<unsigned char>(input[i]);
        //EMOJI CHECKER
    // ----------------------------------------------------------------------------------------------
        //IF Starts with E2 BYTE
        if (firstByte == 0xE2) { 
            std::string emoji = input.substr(i, 3);
            if (emoji == "\xE2\x9C\x85") {           //✅
                std::cout << "its a True" << "\xE2\x9C\x85" << "\n";
                token = "\xE2\x9C\x85";
                tokens.push_back({myTokenType::Const, token, line}); 
                i += 2;
            }
            else if (emoji == "\xE2\x9D\x8C") {      //❌
                std::cout << "its a False" << "\xE2\x9D\x8C" << "\n";
                token = "\xE2\x9D\x8C";
                tokens.push_back({myTokenType::Const, token, line}); 
                i += 2;
            }
            else if (emoji == "\xE2\x9E\xB0") {     //➰
                std::cout << "its a For" << "\xE2\x9E\xB0" << "\n";
                token = "\xE2\x9E\xB0";
                tokens.push_back({myTokenType::For, token, line}); 
                i += 2;
            }
            else if (emoji == "\xE2\x9E\xBF") {     //➿
                std::cout << "its a Do" << "\xE2\x9E\xBF" << "\n";
                token = "\xE2\x9E\xBF";
                tokens.push_back({myTokenType::Do, token, line}); 
                i += 2;
            }
            else if (emoji == "\xE2\x9C\x8D") {     //✍
                std::cout << "its a input" << "\xE2\x9C\x8D" << "\n";
                token = "\xE2\x9C\x8D";
                tokens.push_back({myTokenType::Input, token, line}); 
                i += 2;
            }
            else if (emoji == "\xE2\x81\x89") {     //⁉️
                std::string emoji = input.substr(i, 6);
                std::cout << "its a bool" << "\xE2\x81\x89\xEF\xB8\x8F" << "\n";
                token = "\xE2\x81\x89\xEF\xB8\x8F";
                tokens.push_back({myTokenType::DT, token, line}); 
                i += 5;
            }
            token.clear();
        }
        //IF Starts with F0 BYTE
        if (firstByte == 0xF0) { 
            std::string emoji = input.substr(i, 4);
            if (emoji == "\xF0\x9F\x94\xA2") {  //🔢
                std::cout << "its a int" << "\xF0\x9F\x94\xA2" << "\n";
                token = "\xF0\x9F\x94\xA2";
                tokens.push_back({myTokenType::DT, token, line}); 
                i += 3;
            } else if (emoji == "\xF0\x9F\x8C\x8A") {  // 🌊
                std::cout << "its a float" << "\xF0\x9F\x8C\x8A" << "\n";
                token = "\xF0\x9F\x8C\x8A";
                tokens.push_back({myTokenType::DT, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x8F\x81") {  // 🏁
                std::cout << "its a main" << "\xF0\x9F\x8C\x8A" << "\n";
                token = "\xF0\x9F\x8F\x81";
                tokens.push_back({myTokenType::Main, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x87\xA9") {  // 🇩
                std::cout << "its a double" << "\xF0\x9F\x87\xA9" << "\n";
                token = "\xF0\x9F\x87\xA9";
                tokens.push_back({myTokenType::DT, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x94\xA0") {  // 🔠
                std::cout << "its a char" << "\xF0\x9F\x94\xA0" << "\n";
                token = "\xF0\x9F\x94\xA0";
                tokens.push_back({myTokenType::DT, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x94\xA4") {  // 🔤
                std::cout << "its a string" << "\xF0\x9F\x94\xA4" << "\n";
                token = "\xF0\x9F\x94\xA4";
                tokens.push_back({myTokenType::DT, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x94\x80") {  // 🔀
                std::cout << "its a switch" << "\xF0\x9F\x94\x80" << "\n";
                token = "\xF0\x9F\x94\x80";
                tokens.push_back({myTokenType::Switch, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x94\x92") {  // 🔒
                std::cout << "its a const" << "\xF0\x9F\x94\x92" << "\n";
                token = "\xF0\x9F\x94\x92";
                tokens.push_back({myTokenType::Const, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x9A\xAB") {  // 🚫
                std::cout << "its a void" << "\xF0\x9F\x9A\xAB" << "\n";
                token = "\xF0\x9F\x9A\xAB";
                tokens.push_back({myTokenType::Void, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x90\xAB") {  // 🐫
                std::cout << "its a case" << "\xF0\x9F\x90\xAB" << "\n";
                token = "\xF0\x9F\x90\xAB";
                tokens.push_back({myTokenType::Case, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x94\x81") {  // 🔁
                std::cout << "its a while" << "\xF0\x9F\x94\x81" << "\n";
                token = "\xF0\x9F\x94\x81";
                tokens.push_back({myTokenType::While, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x92\x94") {  // 💔
                std::cout << "its a break" << "\xF0\x9F\x92\x94" << "\n";
                token = "\xF0\x9F\x92\x94";
                tokens.push_back({myTokenType::Break, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\xA7\xA1") {  // 🧡
                std::cout << "its a continue" << "\xF0\x9F\xA7\xA1" << "\n";
                token = "\xF0\x9F\xA7\xA1";
                tokens.push_back({myTokenType::Continue, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x93\x8C") {  // 📌
                std::cout << "its a static" << "\xF0\x9F\x93\x8C" << "\n";
                token = "\xF0\x9F\x93\x8C";
                tokens.push_back({myTokenType::Static, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\xA4\x94") {  // 🤔
                std::cout << "its a if" << "\xF0\x9F\xA4\x94" << "\n";
                token = "\xF0\x9F\xA4\x94";
                tokens.push_back({myTokenType::If, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x98\x85") {  // 😅
                std::cout << "its a else" << "\xF0\x9F\x98\x85" << "\n";
                token = "\xF0\x9F\x98\x85";
                tokens.push_back({myTokenType::Else, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x94\x99") {  // 🔙
                std::cout << "its a return" << "\xF0\x9F\x94\x99" << "\n";
                token = "\xF0\x9F\x94\x99";
                tokens.push_back({myTokenType::Return, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x91\x86") {  // 👆
                std::cout << "its a this" << "\xF0\x9F\x91\x86" << "\n";
                token = "\xF0\x9F\x91\x86";
                tokens.push_back({myTokenType::This, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x8D\xB4") {  // 🍴
                std::cout << "its a try" << "\xF0\x9F\x8D\xB4" << "\n";
                token = "\xF0\x9F\x8D\xB4";
                tokens.push_back({myTokenType::Try, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x8E\xAF") {  // 🎯
                std::cout << "its a throw" << "\xF0\x9F\x8E\xAF" << "\n";
                token = "\xF0\x9F\x8E\xAF";
                tokens.push_back({myTokenType::Throw, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x8F\xAB") {  // 🏫
                std::cout << "its a class" << "\xF0\x9F\x8F\xAB" << "\n";
                token = "\xF0\x9F\x8F\xAB";
                tokens.push_back({myTokenType::Class, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x94\x91") {  // 🔑
                std::cout << "its a private" << "\xF0\x9F\x94\x91" << "\n";
                token = "\xF0\x9F\x94\x91";
                tokens.push_back({myTokenType::AccessModifier, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x8F\xAC") {  // 🏬
                std::cout << "its a struct" << "\xF0\x9F\x8F\xAC" << "\n";
                token = "\xF0\x9F\x8F\xAC";
                tokens.push_back({myTokenType::Struct, token, line});
                i += 3;
            } else if (emoji == "\xF0\x9F\x8C\x90") {  // 🌐
                std::cout << "its a public" << "\xF0\x9F\x8C\x90" << "\n";
                token = "\xF0\x9F\x8C\x90";
                tokens.push_back({myTokenType::AccessModifier, token, line});
                i += 3;
            } else {  
                std::string emoji = input.substr(i, 7);
                if (emoji == "\xF0\x9F\x97\x83\xEF\xB8\x8F") {  //🗃️
                    std::cout << "its a array" << "\xF0\x9F\x97\x83\xEF\xB8\x8F" << "\n";
                    token = "\xF0\x9F\x97\x83\xEF\xB8\x8F";
                    tokens.push_back({myTokenType::DT, token, line}); 
                    i += 6;
                } else if (emoji == "\xF0\x9F\x96\xA8\xEF\xB8\x8F") {  // 🖨️
                    std::cout << "its a print" << "\xF0\x9F\x96\xA8\xEF\xB8\x8F" << "\n";
                    token = "\xF0\x9F\x96\xA8\xEF\xB8\x8F";
                    tokens.push_back({myTokenType::Print, token, line});
                    i += 6;
                }
                else if (emoji == "\xF0\x9F\x8F\xB3\xEF\xB8\x8F") {  // 🏳️
                    std::cout << "its a default" << "\xF0\x9F\x8F\xB3\xEF\xB8\x8F" << "\n";
                    token = "\xF0\x9F\x8F\xB3\xEF\xB8\x8F";
                    tokens.push_back({myTokenType::Default, token, line});
                    i += 6;
                }
                else if (emoji == "\xF0\x9F\x8D\xBD\xEF\xB8\x8F") {  // 🍽️
                    std::cout << "its a catch" << "\xF0\x9F\x8D\xBD\xEF\xB8\x8F" << "\n";
                    token = "\xF0\x9F\x8D\xBD\xEF\xB8\x8F";
                    tokens.push_back({myTokenType::Catch, token, line});
                    i += 6;
                }
                else if (emoji == "\xF0\x9F\x96\xA5\xEF\xB8\x8F") {  // 🖥️
                    std::cout << "its a interface" << "\xF0\x9F\x96\xA5\xEF\xB8\x8F" << "\n";
                    token = "\xF0\x9F\x96\xA5\xEF\xB8\x8F";
                    tokens.push_back({myTokenType::Interface, token, line});
                    i += 6;
                }else {  
                    std::string emoji = input.substr(i, 18);
                    std::cout << "its a extends" << "\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA6" << "\n";
                    token = "\xF0\x9F\x91\xA8\xE2\x80\x8D\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x91\xA6";
                    tokens.push_back({myTokenType::Extends, token, line});
                    i += 17;
                }
            }
            
            token.clear();
        }
    // ---------------------------------------------------------------------------------------------------
        if(c[0] == '$'){
            std::cout << "Is an End Marker\n";
            token = '$';
            tokens.push_back({myTokenType::$, token, line}); // 🔥
            token.clear();
            i++;
            continue;
            continue;
        }
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
            std::cout << "Is a Comment\n";
            while (i < input.size() && input[i] != '\n') i++;
            continue;
        }
        if (input.substr(i, 2) == "/*") {
            std::cout << "Is a Comment\n";
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
            std::cout << "Is a String literal\n";
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
                tokens.push_back({myTokenType::Const, token, line}); // 🔥
            } else {
                std::cout << "Error: Unmatched double quote found. Skipping invalid string literal.\n";
                i = start; // Reset i to the original position before "
                line = line - numbToKeepTrackOfLinesInStringInCaseOfReset;
            }

            token.clear();
            i++;
            continue;
        }

        // Floating point and integer literals
        if (isdigit(c[0])) {
            std::cout << "Is a Float or Integer\n";
            token += c;
            bool isFloat = false;
            while (i + 1 < input.size() && (isdigit(input[i + 1]) || input[i + 1] == '.' && !isFloat)) {
                if (input[i + 1] == '.') isFloat = true;
                token += input[++i];
            }
            tokens.push_back({myTokenType::Const, token, line}); // 🔥
            token.clear();
            i++;
            continue;
        }

        // Identifiers
        if (isalpha(c[0])) { // First char must be a letter (a-z or A-Z)
            std::cout << "Is an Identifier\n";
            token += c[0];
            i++;
            while (i < input.size() && (isalpha(input[i]) || input[i] == '_')) {
                token += input[i++];
            }
            // if (regex_match(token, idenPattern)) {
                std::cout << "identifier pushed\n";
                tokens.push_back({myTokenType::ID, token, line}); // 
            // }else{
            //     std::cout << "Regex Failed\n";
            // }
            token.clear();
            continue;
        }


        // Operators
        if (regex_match(c, operatorPattern)) {
            std::cout << "Is a Operator\n";
            token += c;
            if (i + 1 < input.size() && std::regex_match(std::string(1, input[i + 1]), operatorPattern)) {
                token += input[++i];
            }
            tokens.push_back({myTokenType::OPERATOR, token, line}); // 🔥
            token.clear();
            i++;
            continue;
        }

        // Separators
        if (regex_match(c, separatorPattern)) {
            std::cout << "Is a Separator\n";
            // tokens.push_back({myTokenType::SEPARATOR, c, line}); // 🔥
            switch(c[0]){
                case '(': tokens.push_back({myTokenType::OpenRoundBrkt, "(", line}); break;
                case ')': tokens.push_back({myTokenType::CloseRoundBrkt, ")", line}); break;
                case '{': tokens.push_back({myTokenType::OpenCurlyBrkt, "{", line}); break;
                case '}': tokens.push_back({myTokenType::CloseCurlyBrkt, "}", line}); break;
                case '[': tokens.push_back({myTokenType::OpenSqrBrkt, "[", line}); break;
                case ']': tokens.push_back({myTokenType::CloseSqrBrkt, "]", line}); break;
                case ';': tokens.push_back({myTokenType::SemiColon, ";", line}); break;
                case ',': tokens.push_back({myTokenType::Comma, ",", line}); break;
                case '.': tokens.push_back({myTokenType::Dot, ".", line}); break;
                default: break; // not a separator
            }
        }
        i++;
        continue;
    }
    return tokens;
}
///------------------------------------------------------------------------------------------------------------------------
///SYNTAX ANALYZER
class SA {
    int index = 0; // Current token index
    std::vector<Token> tokens;
    public:
    // Constructor for SA to accept tokens
    SA(const std::vector<Token>& t) {
        tokens = t;
        index = 0;
    }
    // <program> -> <dec> <mainTerminal> <dec>
    bool program() {
        if (dec()) {
            if (mainTerminal()) {
                if (dec()) {
                    // std::cout << "\n program true";
                    return true;
                }else{
                    return false;
                }
            }
        }
        std::cerr << "Syntax error at token index: " << index << " line: " << tokens[index].line
          << " Value: " << tokens[index].value << " Type: " << tokenTypeToString(tokens[index].type)  << std::endl;
        return false;
    }

    // <dec> ->  <struct_dec> | <struct_children> | <interface> | ε
    bool dec() {
        if (index >= tokens.size()) {
            return true; // ε-production 
        }
        if (tokens[index].type == myTokenType::AccessModifier) {
            if (struct_dec()) {
                // std::cout << "\n dec true\n";
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID) {
            if (struct_children()) {
                // std::cout << "\n dec true\n";
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Interface) {
            if (interfaceFunc()) {
                // std::cout << "\n dec true\n";
                return true;
            }
        }
        else if (tokens[index].type == myTokenType::Main || tokens[index].type == myTokenType::$) {
            // std::cout << "\n dec true (epsilon)\n";
            return true; // ε-production
        }

        // std::cout << "\n dec false\n";
        return false;
    }


    // <mainTerminal> -> main ( ) { <mst> }
    bool mainTerminal() { 
        // std::cout<< "\nmainTerminal called\n";
        
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
                                    // std::cout<< "\n mainTerminal true\n";
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
        // std::cout<< "\n struct_dec called\n";

        if (tokens[index].type == myTokenType::AccessModifier) {  // Empty if to check FIRST set
            if (tokens[index].type == myTokenType::AccessModifier) {
                index++; 
                if (tokens[index].type == myTokenType::Struct) {
                    index++; 
                    if (tokens[index].type == myTokenType::ID) {
                        index++; 
                        if (tokens[index].type == myTokenType::Extends || tokens[index].type == myTokenType::OpenCurlyBrkt) {
                            index++; 
                            if (struct_body()) {
                                if (tokens[index].type == myTokenType::CloseCurlyBrkt) {
                                    index++;
                                    // std::cout<< "\n struct_dec true \n";
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
        // std::cout<< "\naccess_modifier called\n";

        if (tokens[index].type == myTokenType::AccessModifier) {
            if (tokens[index].type == myTokenType::AccessModifier) {
                index++; 
                return true;
            } 
        }
        if (tokens[index].type == myTokenType::AccessModifier) {
            if (tokens[index].type == myTokenType::AccessModifier) {
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
        // std::cout << "\nstruct_body called\n";
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
        // std::cout<< "\nstruct_children called\n";
        if (tokens[index].type == myTokenType::DT) {
            if (tokens[index].type == myTokenType::DT) {
                index++;
                if (tokens[index].type == myTokenType::ID) {
                    index++;
                    if (dt_decORfunc_dec()) {
                        // std::cout<< "\nstruct_children true\n";
                        return true;
                    }
                }
            }
        }
        // else if (tokens[index].type == myTokenType::DT || tokens[index].type == myTokenType::ID) {  // Empty if to check FIRST set for array_dec
        //     if (array_dec()) {
        //         return true;
        //     }
        // }
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
        // std::cout << "\n dt_decORfunc_dec called \n";
        if (tokens[index].type == myTokenType::OpenRoundBrkt) { // Empty check for FIRST set
            if (func_dec()) {
                return true;
            }
        }
        if (tokens[index].type == myTokenType::Equal || tokens[index].type == myTokenType::SemiColon || tokens[index].type == myTokenType::CloseCurlyBrkt) {  // Empty if to check FIRST set
            if (dt_dec()) {
                    // std::cout << "\n dt_decORfunc_dec true\n";
                return true;
            }
        }
        return false;
    }
    // <var_init> → = <Const_or_ID> | ε					
    bool var_init() {
        // std::cout << "\n var_init called\n";
        // if (tokens[index].type == myTokenType::Const ||tokens[index].type == myTokenType::FloatConst || tokens[index].type == myTokenType::CharConst || tokens[index].type == myTokenType::BoolConst || tokens[index].type == myTokenType::StringConst  ) { // Empty check for FIRST set of the rule
        if (tokens[index].type == myTokenType::Equal   ) {
            if (tokens[index].type == myTokenType::Equal   ) { // Empty check for FIRST set of the rule
                index++;
                if (Const_or_ID()) {
                    return true;
                }
            }
        } 
        else if (tokens[index].type == myTokenType::Comma || tokens[index].type == myTokenType::SemiColon) { // Empty check for ε case
                // std::cout << "\n var_init true\n";
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
        if (tokens[index].type == myTokenType::Const ) { // Empty check for FIRST set of the rule
            if (tokens[index].type == myTokenType::Const  ) { // Empty check for FIRST set of the rule
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
        // std::cout << "\n SST called\n";
        if (tokens[index].type == myTokenType::While) return while_loop();
        if (tokens[index].type == myTokenType::For) return for_loop();
        if (tokens[index].type == myTokenType::If) return if_statement();
        if (tokens[index].type == myTokenType::Do) return do_while();
        if (tokens[index].type == myTokenType::Try) return try_statement();
        if (tokens[index].type == myTokenType::Throw) return throw_statement();
        if (tokens[index].type == myTokenType::Return) return return_statement();
        if (tokens[index].type == myTokenType::Continue) return continue_stmt();
        if (tokens[index].type == myTokenType::Break) return break_stmt();
        if (tokens[index].type == myTokenType::DT) return struct_children();
        if (tokens[index].type == myTokenType::OpenRoundBrkt || tokens[index].type == myTokenType::ID || tokens[index].type == myTokenType::Const) return Expr(); // or exp()
        if (tokens[index].type == myTokenType::ID) return func_call();
        if (tokens[index].type == myTokenType::OpenCurlyBrkt) return func_dec();

        return false;
    }

    // <MST> -> <SST><MST> | ε
    bool MST() {
        // std::cout<< "\n MST called\n";
        if (SST()) {
            if (MST()) {
                return true;
            }
            return false;
        }
        else if(tokens[index].type == myTokenType::CloseCurlyBrkt){
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
        // std::cout << "dt_dec called \n";
            if (var_init()) {
                if (var_init_tail()) {
                    if (tokens[index].type == myTokenType::SemiColon) {
                        index++;
                        // std::cout <<"\n dt_dec true\n";
                        return true;
                    }
                }
            }
        return false;
    }
    // <Expr> -> <AssignExpr>
        boolean Expr() {
    if (tokens[index].type == myTokenType::Increase_Decrease ||
        tokens[index].type == myTokenType::NOT ||
        tokens[index].type == myTokenType::ID ||
        tokens[index].type == myTokenType::This ||
        tokens[index].type == myTokenType::Const) {
        
        if (AssignExpr()) {
            return true;
        }
    }

        return false;
    }
    // <AssignExpr> → <OrExpr> <AssignExpr'>
    bool AssignExpr() {
        myTokenType type = tokens[index].type;

        if (type == myTokenType::Increase_Decrease ||
            type == myTokenType::NOT ||
            type == myTokenType::ID ||
            type == myTokenType::This ||
            type == myTokenType::Const) {

            if (OrExpr()) {
                if (AssignExprPrime()) {
                    return true;
                }
            }
        }

        return false;
    }
    bool AssignExprPrime() {
        if (tokens[index].type == myTokenType::CompoundAssign) {
            index++;
            return AssignExprPrime(); // Left-recursive
        }

        myTokenType type = tokens[index].type;
        if (type == myTokenType::CloseRoundBrkt || type == myTokenType::SemiColon ||
            type == myTokenType::CloseSqrBrkt || type == myTokenType::Comma) {
            return true; // ε-production
        }

        return false;
    }
    bool OrExpr() {
        myTokenType type = tokens[index].type;

        if (type == myTokenType::Increase_Decrease || type == myTokenType::NOT || type == myTokenType::ID ||
            type == myTokenType::This ||  type == myTokenType::Const) {

            if (AndExpr()) {
                return OrExprPrime();
            }
        }

        return false;
    }

    bool OrExprPrime() {
        if (tokens[index].type == myTokenType::OR) {
            index++;
            return AndExpr() && OrExprPrime();
        }
        myTokenType type = tokens[index].type;
        if (type == myTokenType::CompoundAssign || type == myTokenType::SemiColon ||
            type == myTokenType::CloseSqrBrkt || type == myTokenType::CloseRoundBrkt ||
            type == myTokenType::Comma) {
            return true;
        }
        return false;
    }

    bool AndExpr() {
        myTokenType type = tokens[index].type;
        if (type == myTokenType::Increase_Decrease || type == myTokenType::NOT || type == myTokenType::ID ||
            type == myTokenType::This || 
            type == myTokenType::Const ) {

            if (EqualityExpr() && AndExprPrime()) {
                return true;
            }
        }
        return false;
    }

    bool AndExprPrime() {
        if (tokens[index].type == myTokenType::AND) {
            index++;
            return EqualityExpr() && AndExprPrime();
        }
        myTokenType type = tokens[index].type;
        if (type == myTokenType::OR || type == myTokenType::CompoundAssign ||
            type == myTokenType::SemiColon || type == myTokenType::CloseSqrBrkt ||
            type == myTokenType::CloseRoundBrkt || type == myTokenType::Comma) {
            return true;
        }
        return false;
    }

    bool EqualityExpr() {
        myTokenType type = tokens[index].type;
        if (type == myTokenType::Increase_Decrease || type == myTokenType::NOT || type == myTokenType::ID ||
            type == myTokenType::This || 
            type == myTokenType::Const ) {

            if (RelationalExpr() && EqualityExprPrime()) {
                return true;
            }
        }
        return false;
    }

    bool EqualityExprPrime() {
        if (tokens[index].type == myTokenType::RO2) {
            index++;
            return RelationalExpr() && EqualityExprPrime();
        }
        myTokenType type = tokens[index].type;
        if (type == myTokenType::AND || type == myTokenType::OR ||
            type == myTokenType::CompoundAssign || type == myTokenType::SemiColon ||
            type == myTokenType::CloseSqrBrkt || type == myTokenType::CloseRoundBrkt ||
            type == myTokenType::Comma) {
            return true;
        }
        return false;
    }

    bool RelationalExpr() {
        myTokenType type = tokens[index].type;
        if (type == myTokenType::Increase_Decrease || type == myTokenType::NOT || type == myTokenType::ID ||
            type == myTokenType::This || 
            type == myTokenType::Const ) {

            if (AdditiveExpr() && RelationalExprPrime()) {
                return true;
            }
        }
        return false;
    }

    bool RelationalExprPrime() {
        if (tokens[index].type == myTokenType::RO1) {
            index++;
            return AdditiveExpr() && RelationalExprPrime();
        }
        myTokenType type = tokens[index].type;
        if (type == myTokenType::RO2 || type == myTokenType::AND ||
            type == myTokenType::OR || type == myTokenType::CompoundAssign ||
            type == myTokenType::SemiColon || type == myTokenType::CloseSqrBrkt ||
            type == myTokenType::CloseRoundBrkt || type == myTokenType::Comma) {
            return true;
        }
        return false;
    }

    bool AdditiveExpr() {
        myTokenType type = tokens[index].type;
        if (type == myTokenType::Increase_Decrease || type == myTokenType::NOT || type == myTokenType::ID ||
            type == myTokenType::This || 
            type == myTokenType::Const ) {

            if (MultiplicativeExpr() && AdditiveExprPrime()) {
                return true;
            }
        }
        return false;
    }

    bool AdditiveExprPrime() {
        if (tokens[index].type == myTokenType::AddSub) {
            index++;
            return MultiplicativeExpr() && AdditiveExprPrime();
        }
        myTokenType type = tokens[index].type;
        if (type == myTokenType::RO1 || type == myTokenType::RO2 ||
            type == myTokenType::AND || type == myTokenType::OR ||
            type == myTokenType::CompoundAssign || type == myTokenType::SemiColon ||
            type == myTokenType::CloseSqrBrkt || type == myTokenType::CloseRoundBrkt ||
            type == myTokenType::Comma) {
            return true;
        }
        return false;
    }

    bool MultiplicativeExpr() {
        myTokenType type = tokens[index].type;
        if (type == myTokenType::Increase_Decrease || type == myTokenType::NOT || type == myTokenType::ID ||
            type == myTokenType::This || 
            type == myTokenType::Const) {

            if (UnaryExpr() && MultiplicativeExprPrime()) {
                return true;
            }
        }
        return false;
    }

    bool MultiplicativeExprPrime() {
        if (tokens[index].type == myTokenType::MulDivMod) {
            index++;
            return UnaryExpr() && MultiplicativeExprPrime();
        }
        myTokenType type = tokens[index].type;
        if (type == myTokenType::AddSub || type == myTokenType::RO1 ||
            type == myTokenType::RO2 || type == myTokenType::AND ||
            type == myTokenType::OR || type == myTokenType::CompoundAssign ||
            type == myTokenType::SemiColon || type == myTokenType::CloseSqrBrkt ||
            type == myTokenType::CloseRoundBrkt || type == myTokenType::Comma) {
            return true;
        }
        return false;
    }

    bool UnaryExpr() {
        myTokenType type = tokens[index].type;
        if (type == myTokenType::Increase_Decrease) {
            index++;
            return Primary();
        } else if (type == myTokenType::NOT) {
            index++;
            return UnaryExpr();
        } else if (type == myTokenType::ID || type == myTokenType::This || 
                type == myTokenType::Const  || type == myTokenType::OpenRoundBrkt) {
            return Primary();
        }
        return false;
    }

    bool Primary() {
        myTokenType type = tokens[index].type;
        if (type == myTokenType::ID || type == myTokenType::This) {
            index++;
            return true;
        } else if (type == myTokenType::Const ) {
            index++;
            return true;
        } else if (type == myTokenType::OpenRoundBrkt) {
            index++;
            if (Expr() && tokens[index].type == myTokenType::CloseRoundBrkt) {
                index++;
                return true;
            }
        }
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

    std::string content;
    std::string line;
    // std::ifstream inputFile("codeInput.cpp");
    std::ifstream inputFile("codeInput2.cpp");

    if (!inputFile) {
        std::cerr << "Error opening file.\n";
        return 1;
    }

    // Read the file line by line
    //  while (getline(inputFile, line, '\n')) { //delimiter
    while (std::getline(inputFile, line)) {
        // Process each line
        std::cout << "Line: " << line << std::endl;
        content += line + '\n';

    }

    inputFile.close();  // Don't forget to close the file!

    std::cout << "Input Content:\n" << content << "\n\n";

    // Tokenize the input
    std::vector<Token> tokens = tokenize(content);

    // Print tokens
    std::cout << "---------------------------------Printing Tokens---------------------------------\n";
    for (const Token &t : tokens) {
        std::cout << "Line " << t.line << " -> " << tokenTypeToString(t.type) << ": " << t.value << std::endl;
    }
    // std::cout << "Testing Token Index1:" <<tokens[0].value << std::endl;
    // std::cout << "Testing Token Index1:" <<tokens[1].value << std::endl;
    // std::cout << "Testing Token Index2:" <<tokens[2].value << std::endl;
    // std::cout << "Testing Token Index9:" <<tokens[9].value << std::endl;
    std::cout << "Total tokens: " << tokens.size() << std::endl;

    SA parser(tokens);
    if (parser.program()) {
        std::cout << "Parsing successful.\n";
    } else {
        std::cout << "Parsing failed.\n";
    }
    return 0;
}
