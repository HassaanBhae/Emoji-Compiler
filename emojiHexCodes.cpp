#include <iostream>
#include <string>
#include <iomanip>
#include <windows.h>

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
      cout << "0xF0";
    }
    if (c == "0x9F"){
      cout << "0x9F";
    }
    if (c == "0x98"){
      cout << "0x98";
    }
    if (c == "0x81"){
      cout << "0x81";
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
    SetConsoleOutputCP(CP_UTF8);

//   string arr[] = {
//     "🔢", "🌊", "🇩", "🔠", "🔤", "⁉️", "🗃️", "✍", "🖨️", "🔀", "🔒", "🚫", "🐫", 
//     "📌", "🏳️", "🤔", "😅", "➰", "➿", "🔁", "✅", "❌", "💔", "🧡", "🛠️", "🔙", 
//     "👆", "🍴", "🍽️", "👨‍👩‍👦", "🎯", "🏫", "🔑", "🏬", "🌐", "🛡"
// };

  string arr[] = {
    "➰", "➿","✅", "❌","✍","⁉️","🔢", "🌊", "🇩", "🔠", "🔤","🔀", "🔒", "🚫", "🐫", "🔁","💔", "🧡",
  "📌", "🤔", "😅","🔙", "👆", "🍴","🎯", "🏫", "🔑", "🏬", "🌐",  "🗃️","🖨️","🏳️","🍽️","🖥️","👨‍👩‍👦"
};
    int j = 0;
    int arrSize = sizeof(arr)/sizeof(arr[0]);
    for(int i = 0;i< arrSize;i++ ){
      printUTF8Bytes(arr[j]);
      j++;
    }
    // printUTF8Bytes("✅");
    // printUTF8Bytes("❌");

    return 0;
}

