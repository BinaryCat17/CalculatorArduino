#include <Arduino.h>
#include "timer.hpp"
#include "optional.hpp"
#include "parser.hpp"

unsigned const firstPin = 3;

void displayBin(uint16_t val) {
    bool bits[8];
    for (uint8_t i = 0; i != 8; ++i) {
        // получаем последний бит в числе
        bits[7 - i] = val % 2;
        // убираем один разряд из числа
        val /= 2;
    }

    // выводим все биты
    for (uint8_t i = 0; i != 8; ++i) {
        Serial.print(bits[i]);
        digitalWrite(i + firstPin, bits[i]);
    }
}

void displayHex(uint16_t val) {
    static const char *digits = "0123456789ABCDEF";
    // Хватит, чтобы вместитить любой uint16_t. Класс String слишком дорогой
    // по скорости работы для такой задачи из-за динасмического выделения памяти.
    char num[4];

    // такой же алгоритм, как в функции displayBinary
    for (uint8_t i = 0; i != 4; ++i) {
        num[4 - i] = digits[val % 16];
        val /= 16;
    }

    for (uint8_t i = 0; i != 4; ++i) {
        Serial.println(i);
    }
}


// костыль для эмуляции исключений
// если бы ардуино не заставлял пользоваться его функцицей loop, а позволял использовать свою,
// то можно было бы полноценно эмулировать исключения
static bool error = false;

template<typename T>
T unwrap(Optional<T> const &v) {
    if (auto val = v.val()) {
        return *val;
    } else {
        error = true;
        return {};
    }
}

static Optional<uint16_t> firstVal;
static Optional<char> op;
static Optional<uint16_t> secondVal;

void reset() {
    firstVal.reset();
    op.reset();
    secondVal.reset();
}

bool read() {
    // пользуюсь особенностью оператора && в С++ - проверка выполняется последовательно и
    // прекращается при первой неудаче
    // Если все значения инициализированны
    return !firstVal.tryInit([] { return unwrap(readInt(Serial)); }) &&
        !op.tryInit([] { return unwrap(readChar(Serial)); }) &&
        !secondVal.tryInit([] { return unwrap(readInt(Serial)); });
}

Optional<uint16_t> calculate(uint16_t first, char c, uint16_t second) {
    if (first > second) {
        return {};
    }

    // не знаю как ловить ошибку переполнения тут
    switch (c) {
        case '+':
            return  Optional<uint16_t>(first + second);
        case '-':
            return  Optional<uint16_t>(first - second);
        case '*':
            return  Optional<uint16_t>(first * second);
        case '/':
            if(second == 0) {
                return {};
            }
            return  Optional<uint16_t>(first / second);
        default:
            return {};
    }
}

void setup() {
    Serial.begin(9600);
    for (int i = firstPin; i != firstPin + 8 ; ++i) {
        pinMode(i, OUTPUT);
    }
}

void loop() {
    // к сожалению arduino не поддерживает мультипоточность, поэтому, чтобы программа не спала
    // используется такой костыль

    // каждые 100мс опрашиваем ввод, чтобы не негружать ардуинку
    timer<100>([] {
        bool ready = read();
        if(error) {
            Serial.println("Parse error!");
            reset();
            return;
        }
        if(ready) {
            if(auto pval = calculate(*firstVal.val(), *op.val(), *secondVal.val()).val()) {
                uint16_t val = *pval;
                Serial.println(val);
                displayBin(val);
                displayHex(val);
            } else {
                Serial.println("Calculation error!");
                reset();
                return;
            }
        }
    });
}