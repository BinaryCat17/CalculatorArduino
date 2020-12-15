#pragma once

// Реализайия таймера без динамического выделения памяти и динамической передачи функций

// заглушка для остановки callAll
void callAll() {}

// принимат произвольное количество функций и вызывает их
template<typename F, typename... Fn>
void callAll(F && f, Fn &&... fn) {
    f();
    callAll(fn...);
}

// вызывает функцию f через определенный интервал времени. Должна вызываться из loop
// interval передан в шаблон, чтобы для каждого таймера с разными интервалами были разные lastTime
template<unsigned interval, typename Fn>
void timer(Fn && f) {
    unsigned time = millis();
    static unsigned lastTime = 0;

    if(time - lastTime > interval) {
        lastTime = time;
        f();
    }
}

// тот-же timer, но с произвольным количеством функций, а не одной
template<unsigned interval, typename... Fn>
void timer(Fn &&... f) {
    timer<interval>([&] { callAll(f...); });
}
