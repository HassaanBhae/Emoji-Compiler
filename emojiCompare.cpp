#include <iostream>
#include <string>
using namespace std;

void identifyEmoji(const std::string& emoji) {
    if (emoji == "😊") {
        std::cout << "This is a Smiling Face emoji! 😊" << std::endl;
    } else if (emoji == "😂") {
        std::cout << "This is a Face with Tears of Joy emoji! 😂" << std::endl;
    } else if (emoji == "❤️") {
        std::cout << "This is a Red Heart emoji! ❤️" << std::endl;
    } else {
        std::cout << "Unknown emoji." << std::endl;
    }
}

int main() {
    std::string input_emoji = "😊";  // You can change this to test different emojis
    identifyEmoji(input_emoji);
    string emojiCout1 = "✅";
    // string emojiCout2 = ✅;
    cout <<"EMoji COUt:"<< emojiCout1 << "\n";
    // cout << emojiCout2;
    return 0;
}
