#pragma once

// простенький класс для определения инициализации значения
template<typename T>
class Optional {
public:
    Optional() = default;

    explicit Optional(T val) {
        init(val);
    }

    explicit operator bool() const {
        return initialized;
    }

    // передача по значению, а не ссылке, потому что практически всё на ардуино примитивные объекты
    void init(T v) {
        mVal = v;
        initialized = true;
    }

    void reset() {
        initialized = false;
    }

    // исключений на ардуинке нету, поэтому нулевые указатели, хоть это небезопасно довольно
    T* val() {
        if(initialized) {
            return &mVal;
        } else {
            return nullptr;
        }
    }

    T const* val() const {
        if(initialized) {
            return &mVal;
        } else {
            return nullptr;
        }
    }

    // очень удобная штука, инициализирующая объект вызовом функции, если он не инициализирован
    template<typename Fn>
    bool tryInit(Fn && f)  {
        if(!initialized) {
            init(f());
            return true;
        }
        return false;
    }

private:
    T mVal = {};
    bool initialized = false;
};