#include "golay.h"
#include <iostream>

using namespace std;

// binary addition
int bin_add(int a, int b) { return (a + b) % 2; }

// binary multiplication
int bin_mult(int a, int b) { return (a * b) % 2; }

// calculates weight of given vector
int weight(int *v) {
    int weight = 0;
    for (int i = 0; i < 12; i++) {
        weight += v[i];
    }
    return weight;
}

// constructor
Golay::Golay() { get_matrices(); }

// construct generator and transposed parity check matrices from P matrix
void Golay::get_matrices() {
    for (int i = 0; i < 12; ++i) {
        G[i] = new int[24];
    }

    for (int i = 0; i < 24; ++i) {
        HT[i] = new int[12];
    }

    int I[12][12] = {};
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            if (i == j) {
                I[i][j] = 1;
            } else {
                I[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 24; j++) {
            if (j < 12) {
                HT[j][i] = I[i][j];
                G[i][j] = P[i][j];
            } else {
                G[i][j] = I[i][j - 12];
                HT[j][i] = P[i][j - 12];
            }
        }
    }
}

// encode given message
void Golay::encode(int *m) {
    cout << "Encoded codeword: ";
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 12; j++) {
            u[i] = bin_add(u[i], bin_mult(m[j], G[j][i]));
            r[i] = u[i];
        }
        cout << u[i];
    }
    cout << endl;
}

// add errors to the encoded message while sending through noisy channel
void Golay::add_errors() {
    cout << endl << "Sending through a noisy channel..." << endl;
    int *positions = new int;
    cout << "How many errors? (in case of odd number >= 5, decoding error will "
            "occur):"
         << endl;
    int num;
    cin >> num;
    for (int i = 0; i < num; i++) {
        cout << "Position " << i + 1 << ": ";
        cin >> positions[i];
    }

    cout << "Adding errors in positions: ";
    for (int i = 0; i < num; i++) {
        r[positions[i]] = bin_add(r[positions[i]], 1);
        cout << positions[i] << " ";
    }
    cout << "\n\n";

    cout << "Received message: ";
    for (int i = 0; i < 24; i++) {
        cout << r[i];
    }
    cout << endl;
}

// calculate syndrome of the received message
void Golay::get_syndrome() {
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 12; j++) {
            s[j] = bin_add(s[j], bin_mult(r[i], HT[i][j]));
        }
    }

    cout << "Syndrome: ";
    for (int i = 0; i < 12; i++) {
        cout << s[i];
    }
    cout << endl;
}

// decode message
void Golay::decode() {
    get_syndrome();
    int e[24];
    bool decodable = true;
    int sp[12] = {};
    cout << "Trying to decode..." << endl;
    // if w(s) <= 3 then set e = (s, 0)
    if (weight(s) <= 3) {
        for (int i = 0; i < 24; i++) {
            if (i < 12) {
                e[i] = s[i];
            } else {
                e[i] = 0;
            }
        }
        cout << "w(s) = " << weight(s) << " <= 3" << endl;
        goto result;
    }

    // if w(s + pi) <= 2 for some pi then set e = (s + pi, u(i))
    else {
        for (int i = 0; i < 12; i++) {
            int spi[12] = {};
            for (int j = 0; j < 12; j++) {
                spi[j] = s[j];
            }
            for (int j = 0; j < 12; j++) {
                spi[j] = bin_add(spi[j], P[i][j]);
            }
            if (weight(spi) <= 2) {
                for (int k = 0; k < 24; k++) {
                    if (k < 12) {
                        e[k] = spi[k];
                    } else {
                        e[k] = (i == k - 12);
                    }
                }
                cout << "w(s + p" << i << ") = " << weight(spi) << " <= 2" << endl;
                goto result;
            }
        }
    }

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            sp[j] = bin_add(sp[j], bin_mult(s[i], P[i][j]));
        }
    }

    // if w(s*P) = 2 or w(s*P) = 3 then set e = (0, s*P)
    if (weight(sp) == 2 || weight(sp) == 3) {
        for (int i = 0; i < 24; i++) {
            if (i < 12) {
                e[i] = 0;
            } else {
                e[i] = sp[i - 12];
            }
        }
        cout << "w(s*P) = " << weight(sp) << endl;
        goto result;
    }

    // if w(s*P + pi) = 2 for some pi then set e = (u(i), s*P + pi)
    else {
        for (int i = 0; i < 12; i++) {
            int sppi[12] = {};
            for (int j = 0; j < 12; j++) {
                sppi[j] = sp[j];
            }
            for (int j = 0; j < 12; j++) {
                sppi[j] = bin_add(sppi[j], P[i][j]);
            }
            if (weight(sppi) == 2) {
                for (int k = 0; k < 24; k++) {
                    if (k < 12) {
                        e[k] = (i == k);
                    } else {
                        e[k] = sppi[k - 12];
                    }
                }
                cout << "w(s*P + p" << i << ") = " << weight(sppi) << endl;
                goto result;
            }
        }
    }

    // if pattern is not correctable, request retransmission
    decodable = false;

result:
    if (decodable) {
        cout << "Error pattern: ";
        for (int i = 0; i < 24; i++) {
            cout << e[i];
        }
        cout << endl;
        cout << "Decoded codeword: ";
        for (int i = 0; i < 24; i++) {
            cout << bin_add(r[i], e[i]);
        }
    } else {
        cout << "ERROR: Message Undecodable. Requesting retransmission...";
    }
    cout << endl;
}
