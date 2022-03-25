#include <iostream>
#include "audio/AudioChat.h"

using std::cin;
using std::cout;
using std::endl;

int main() {
    cout << "HELLO" << endl;
    AudioChat chat;
    std::string s;
    cin >> s;
    chat.JoinRoom(QString::fromStdString(s), 100);
    cin >> s;
    return 0;
}
