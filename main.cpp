//
// Created by 8752355675 on 3/6/23.
// Результат можно сравнить например с openssl:
// echo -en "\x01\x23\x45\x67\x89\xAB\xCD\xEF" | openssl enc -des-ecb -nosalt -nopad -K 133457799BBCDFF1 | hexdump -C
//

#include<string>
#include "des.h"

int main() {
    DES C;
    string s = "\x01\x23\x45\x67\x89\xAB\xCD\xEF";
//  string s = "qqqqqqqq";
    string k = "\x13\x34\x57\x79\x9B\xBC\xDF\xF1";
//  string k = "01234567";
    C.get_s(s);
    C.get_key(k);
    C.show_encryp(); // Зашифровать, сгенерировать зашифрованный текст и записать его в 1.txt
    return 0;
}
