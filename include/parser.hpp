#pragma once

// Возможна работа с различными serial, поддерживающими функции available и read
// Идет статическое связывание через шаблоны, а не через динамическое наследование,
// чтобы избежать лишних виртуальных вызовов


template<typename SerialT>
void skipSpaces(SerialT &serial) {
    for (char c; serial.available(); c = serial.read()) {
        if (!(c == ' ' || c == '\n')) {
            break;
        }
    }
}

template<typename SerialT>
Optional<uint16_t> readInt(SerialT &serial) {
    skipSpaces(serial);

    if (serial.available()) {
        // нету ни одного симвова
        return Optional<uint16_t>(serial.read());
    }

    uint8_t num[5] = {0, 0, 0, 0, 0};
    static uint8_t maxValue[5] = {6, 5, 5, 3, 6};
    // больше 5 десятичных значений не поместится в uint16_t, 65536 предел;
    for (uint8_t i = 0; i != 5 && serial.available(); ++i) {
        char c = serial.read();

        // до первого пробела или знака новой строки
        if (c == ' ' || c == '\n') {
            break;
        }

        // переносим все разряды левее
        for (int8_t j = i; j != -1; --j) {
            num[3 - j] = num[4 - j];
        }

        // записываем значение в последний разряд
        num[4] = c - '0';
    }

    uint16_t val = 0;
    // перебираем число с конца
    for (int8_t i = 4; i != -1; --i) {
        // соединяем все разряды числа. pow возвращается double, поэтому переполнения не будет
        if (num[i] > maxValue[i]) {
            // ошибка переполнения
            return Optional<uint16_t>(val);
        }
        val += num[i] * pow(10, i);
    }

    return Optional<uint16_t>(val);
}

template<typename SerialT>
Optional<char> readChar(SerialT &serial) {
    skipSpaces(serial);
    if (serial.available()) {
        // нету ни одного симвова
        return Optional<char>(serial.read());
    }
    return {};
}

