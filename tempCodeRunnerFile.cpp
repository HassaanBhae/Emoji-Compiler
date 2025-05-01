
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
