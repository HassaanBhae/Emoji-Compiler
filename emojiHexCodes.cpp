#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

void identifyEmoji(const string& emoji) {
  string c(1, emoji[0]);  
  string byte = "0xf0";
    if (c == "\xF0\x9F\x98\x81") {
        cout << "This is a Smiling Face emoji! " <<"😁"<<  endl;
    } else if (emoji == "😂") {
        cout << "This is a Face with Tears of Joy emoji! 😂" << endl;
    } else {
        cout << "Unknown emoji." << endl;
    }
    if (c == byte){
      cout << "0";
    }
    if (c == "0xF0"){
      cout << "0";
    }
    if (c == "0x9F"){
      cout << "1";
    }
    if (c == "0x98"){
      cout << "2";
    }
    if (c == "0x81"){
      cout << "3";
    }
}
void printUTF8Bytes(const string& emoji) {
    cout << "UTF-8 Bytes: ";
    cout << emoji << " ==> ";
    for (unsigned char byte : emoji) {
        cout << "0x" << hex << uppercase << setw(2) << setfill('0') << (int)byte << " ";
    }
    cout << endl;
}

int main() {
//   string arr[] = {
//     "🔢", "🌊", "🇩", "🔠", "🔤", "⁉️", "🗃️", "✍", "🖨️", "🔀", "🔒", "🚫", "🐫", 
//     "📌", "🏳️", "🤔", "😅", "➰", "➿", "🔁", "✅", "❌", "💔", "🧡", "🛠️", "🔙", 
//     "👆", "🍴", "🍽️", "👨‍👩‍👦", "🎯", "🏫", "🔑", "🏬", "🌐", "🛡"
// };

  string arr[] = {
    "✍","➰", "➿","✅", "❌","🔢", "🌊", "🇩", "🔠", "🔤","🔀", "🔒", "🚫", "🐫", "🔁","💔", "🧡",
  "📌", "🤔", "😅","🔙", "👆", "🍴","🎯", "🏫", "🔑", "🏬", "🌐", "🛡","⁉️", "🗃️","🖨️","🏳️","🛠️","🍽️","👨‍👩‍👦"
};
    int j = 0;
    for(int i = 0;i<36;i++ ){
      printUTF8Bytes(arr[j]);
      j++;
    }
    // printUTF8Bytes("✅");
    // printUTF8Bytes("❌");

    return 0;
}

