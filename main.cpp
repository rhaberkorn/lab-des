//
// Created by 8752355675 on 3/6/23.
//

#include<string>
#include "des.h"

    int main() {
        DES C;
        string s = "qqqqqqqq";
        string k = "01234567";
        C.get_s(s);
        C.get_key(k);
        C.show_encryp(); // Зашифровать, сгенерировать зашифрованный текст и записать его в 1.txt
        C.show_decrypt(); // Считываем зашифрованный текст из 1.txt, расшифровываем, генерируем открытый текст в 2.txt
        return 0;
    }
