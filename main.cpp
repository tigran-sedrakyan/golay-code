#include "golay.h"
#include <iostream>
using namespace std;

int main() {
    Golay g;
    int m[12] = {};
    cout << "Message (separate digits by spaces or new lines): ";
    for (int i = 0; i < 12; i++) {
        cin >> m[i];
    }
    cout << endl;
    g.encode(m);
    g.add_errors();
    g.decode();
}
