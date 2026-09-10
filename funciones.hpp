

#include <concepts>
#include <iterator>
#include <type_traits>
#include <cstddef>
using namespace std;
namespace core_numeric {





// 1. CONCEPTS:
// Un tipo es Iterable si soporta begin()/end() (rango recorrible)
template<typename C>
concept Iterable = requires(C c) {
    begin(c);
    end(c);
};



// Un tipo es Addable si a + b esta definido y devuelve el mismo tipo
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> same_as<T>;
};






// Un tipo es Divisible si a / n (n = size_t) esta definido y el resultado es convertible a T. (Se usa convertible_to en lugar de
// same_as porque para tipos aritmeticos como int, "int / size_t"
// produce un size_t por promocion, no un int).
template<typename T>
concept Divisible = requires(T a, size_t n) {
    { a / n } -> convertible_to<T>;
};






// Concept propio (creado desde cero): Comparable.
// Un tipo es Comparable si se puede ordenar con operator< y el resultado es convertible a bool. Es la base para max().
template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> convertible_to<bool>;
};









// 2. sum: suma todos los elementos de un contenedor
//    Requiere Iterable<C> y Addable<value_type>

template<Iterable C>
requires Addable<typename C::value_type>
auto sum(const C& container)
{
    using T = typename C::value_type;

    T result{};  // Para tipos numericos (int, double, float) se
                 // inicializa en 0. Para clases se llama al
                 // constructor por defecto.

    for (const auto& value : container)
        result = result + value;

    return result;
}








// 3. mean: promedio del contenedor. Reutiliza sum().
//    Requiere Divisible ademas de Addable (via sum).
//    Usa if constexpr para diferenciar enteros de flotantes:
//    enteros: division con redondeo al entero mas cercano
//    flotantes: division exacta
template<Iterable C>
requires Addable<typename C::value_type> && Divisible<typename C::value_type>
auto mean(const C& container)
{
    using T = typename C::value_type;

    T total = sum(container);
    size_t n = static_cast<size_t>(
        distance(begin(container), end(container)));

    if constexpr (is_integral_v<T>) {
        // Redondeo al entero mas cercano (comportamiento distinto
        // al de la division entera truncada)
        return static_cast<T>((total + static_cast<T>(n) / 2) / n);
    } else {
        // Flotantes: division exacta, sin redondeo especial
        return total / n;
    }
}






// 4. variance: varianza del contenedor. Reutiliza mean().
//    Restringida por Iterable y Addable (variance(string) no compila porque string no cumple Addable de la forma
//    requerida por esta funcion).
template<Iterable C>
requires Addable<typename C::value_type>
auto variance(const C& container)
{
    using T = typename C::value_type;

    auto m = mean(container);
    T total{};

    for (const auto& value : container) {
        auto diff = value - m;               // desviacion
        total = total + static_cast<T>(diff * diff);
    }

    size_t n = static_cast<size_t>(
        distance(begin(container), end(container)));

    return total / n;
}







// 5. max: elemento maximo del contenedor.
//    Restringida por Iterable y por el concept propio Comparable.
template<Iterable C>
requires Comparable<typename C::value_type>
auto max(const C& container)
{
    using T = typename C::value_type;

    auto it = begin(container);
    T best = *it;

    for (++it; it != end(container); ++it) {
        if (best < *it)
            best = *it;
    }

    return best;
}





// 6. transform_reduce: aplica una funcion a cada elemento y reduce (suma) el resultado. La funcion se recibe como parametro
//    template (tipo generico F deducido en la llamada).
template<Iterable C, typename F>
auto transform_reduce(const C& container, F func)
{
    using T = typename C::value_type;
    using R = invoke_result_t<F, T>;

    R result{};
    for (const auto& value : container)
        result = result + func(value);

    return result;
}




// 7. Variadic templates + fold expressions
// sum_variadic: suma N argumentos usando un fold expression unario
template<Addable... Ts>
requires (sizeof...(Ts) > 0)
auto sum_variadic(Ts... args)
{
    return (args + ...);
}



// mean_variadic: promedio de N argumentos. Reutiliza el fold de suma y usa if constexpr para diferenciar enteros de flotantes.
template<typename... Ts>
requires (Addable<Ts> && ...) && (Divisible<Ts> && ...)
auto mean_variadic(Ts... args)
{
    auto total = (args + ...);
    using T = decltype(total);
    constexpr size_t n = sizeof...(Ts);

    if constexpr (is_integral_v<T>) {
        return static_cast<T>((total + static_cast<T>(n) / 2) / n);
    } else {
        return total / static_cast<T>(n);
    }
}






// variance_variadic: varianza de N argumentos. Reutiliza mean_variadic.
template<typename... Ts>
requires (Addable<Ts> && ...)
auto variance_variadic(Ts... args)
{
    auto m = mean_variadic(args...);
    using T = decltype(m);
    constexpr size_t n = sizeof...(Ts);

    T total{};
    // Fold expression (coma) que acumula la suma de las desviaciones
    // al cuadrado
    ((total = total + static_cast<T>((args - m) * (args - m))), ...);

    return total / n;
}



// max_variadic: maximo de N argumentos. Usa fold expression (coma)
// y el concept propio Comparable.
template<Comparable T, Comparable... Ts>
auto max_variadic(T first, Ts... rest)
{
    T best = first;
    ((best = (best < rest) ? static_cast<T>(rest) : best), ...);
    return best;
}

}
