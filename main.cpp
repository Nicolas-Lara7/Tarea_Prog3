#include "funciones.hpp"
#include <vector>
#include <string>
#include <cassert>
#include <cmath>
#include <iostream>
using namespace std;




struct Coordenadas {
    double x = 0.0, y = 0.0;

    Coordenadas() = default;
    Coordenadas(double x_, double y_) : x(x_), y(y_) {}
    Coordenadas operator+(const Coordenadas& o) const {
        return {x + o.x, y + o.y};
    }
    Coordenadas operator-(const Coordenadas& o) const {
        return {x - o.x, y - o.y};
    }
    Coordenadas operator*(const Coordenadas& o) const {
        return {x * o.x, y * o.y};
    }
    Coordenadas operator/(size_t n) const {
        return {x / n, y / n};
    }

    // Comparacion por magnitud
    bool operator<(const Coordenadas& o) const {
        return (x * x + y * y) < (o.x * o.x + o.y * o.y);
    }
};

struct Vectores3D {
    double x = 0.0, y = 0.0, z = 0.0;
    Vectores3D() = default;
    Vectores3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    Vectores3D operator+(const Vectores3D& o) const {
        return {x + o.x, y + o.y, z + o.z};
    }
    Vectores3D operator-(const Vectores3D& o) const {
        return {x - o.x, y - o.y, z - o.z};
    }
    Vectores3D operator*(const Vectores3D& o) const {
        return {x * o.x, y * o.y, z * o.z};
    }
    Vectores3D operator/(size_t n) const {
        return {x / n, y / n, z / n};
    }

    bool operator<(const Vectores3D& o) const {
        return (x * x + y * y + z * z) < (o.x * o.x + o.y * o.y + o.z * o.z);
    }
};

// Esfera solo es Comparable (por volumen) no define
// operator+, para poder usarla como caso invalido con sum/mean/variance.
struct Esfera {
    double radio;
    explicit Esfera(double r) : radio(r) {}

    double volumen() const {
        return (4/ 3) * 3.1415 * radio * radio * radio;
    }

    bool operator<(const Esfera& o) const {
        return volumen() < o.volumen();
    }
    //o hay operator+ ni operator/ porque esfera no cumple addable ni divisible
};

int main()
{
    using namespace core_numeric;


    // Caso que compila: enteros
    {
        vector<int> v{1, 2, 3, 4};
        auto s = sum(v);
        assert(s == 10);
    }

    // Caso que compila: doubles
    {
        vector<double> v{1.5, 2.5, 3.0};
        auto s = sum(v);
        assert(abs(s - 7.0) < 1e-9);
    }

    // Caso que compila: objetos de clase (Coordenadas si es Addable)
    {
        vector<Coordenadas> v{{1, 2}, {3, 4}};
        auto s = sum(v);
        assert(s.x == 4 && s.y == 6);
    }

    // Caso que NO compila: vector<string>
    // string SI define operator+, pero el resultado de string + string no cumple Addable tal como esta definido aqui
    // sin ambiguedad de tipos (en realidad string SI compila con sum, asi que el ejemplo invalido real es con const char*, que no
    // tiene value_type utilizable de esta forma):
    // vector<const char*> v{"Hola", "Mundo"};
    // sum(v);
    // ERROR: const char* no cumple Addable (a + b entre punteros no esta definido como suma valida del mismo tipo).




    // Caso que NO compila: Esfera no define operator+
    // vector<Esfera> esferas{Esfera(1.0), Esfera(2.0)};
    // sum(esferas);
    // ERROR: Esfera no cumple el concept Addable (no existe operator+).







    // mean()  -- requiere Addable + Divisible


    // Caso que compila: enteros (el redondeo es via if constexpr)
    {
        vector<int> v{1, 2, 4};
        auto m = mean(v);           // (1+2+4)/3 = 2.33 -> redondea a 2
        assert(m == 2);
    }

    // Caso que compila: doubles
    {
        vector<double> v{1.0, 2.0, 3.0};
        auto m = mean(v);
        assert(abs(m - 2.0) < 1e-9);
    }

    // Caso que compila: Coordenadas (Addable y Divisible)
    {
        vector<Coordenadas> v{{2, 4}, {4, 8}};
        auto m = mean(v);
        assert(m.x == 3 && m.y == 6);
    }

    // Caso que NO compila: Esfera no define operator/ (no es Divisible)
    // vector<Esfera> esferas{Esfera(1.0), Esfera(2.0)};
    // mean(esferas);
    // ERROR: Esfera no cumple Divisible (no existe operator/(size_t)).





    // variance()  -- requiere Addable (internamente usa mean)



    // Caso que compila: doubles
    {
        vector<double> v{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
        auto var = variance(v);
        assert(var > 0);
    }

    // Caso que compila: Coordenadas
    {
        vector<Coordenadas> v{{1, 1}, {3, 3}, {5, 5}};
        auto var = variance(v);
        (void)var;
    }

    // Caso que NO compila: vector<string>
    // vector<string> palabras{"Hola", "Mundo"};
    // variance(palabras);
    // ERROR: string no cumple Addable de la forma requerida por variance (aunque existe operator+, aqui el uso posterior de
    // resta y multiplicacion sobre value_type no es valido para string, y ademas Divisible no se cumple).








    // max()  -- requiere el concept propio Comparable


    // Caso que compila: doubles
    {
        vector<double> v{1.0, 2.7, 0.3};
        auto mx = max(v);
        assert(mx == 2.7);
    }
    // Caso que compila: char
    {
        vector<char> v{'a', 'z', 'm'};
        auto mx = max(v);
        assert(mx == 'z');
    }
    // Caso que compila: Esfera (si cumple Comparable, por operator<)
    {
        vector<Esfera> esferas{Esfera(1.0), Esfera(3.0), Esfera(2.0)};
        auto mx = max(esferas);
        assert(abs(mx.radio - 3.0) < 1e-9);
    }



    // Caso que NO compila: vector<string> con max
    // pese a que string si es Comparable por operator<, el ejemplo invalido es real cuando es un tipo sin operator<, como
    // un struct vacio):
    // struct SinComparar { int valor; };
    // vector<SinComparar> v{{1}, {2}};
    // max(v);
    // ERROR: SinComparar no cumple el concept Comparable (no existe
    // operator< que devuelva algo convertible a bool).




    // transform_reduce()
    // Caso que compila: suma de cuadrados
    {
        vector<double> v{1.0, 2.0, 3.0};
        auto r = transform_reduce(v, [](double x) { return x * x; });
        assert(abs(r - 14.0) < 1e-9);
    }


    // Variadic + fold expressions


    // Casos que compilan
    {
        auto s1 = sum_variadic(1, 2, 33, 4);
        assert(s1 == 40);
        auto s2 = mean_variadic(0.0, 2.0, 4.0);
        assert(abs(s2 - 2.0) < 1e-9);
        auto s3 = variance_variadic(1.0, 2.0, 3.0, 4.0);
        (void)s3;
        auto s4 = max_variadic(1.0, 2.7, 3.0, 4.0);
        assert(s4 == 4.0);
        // Con objetos de clase (Vectores3D cumple Addable/Divisible/Comparable)
        auto s5 = sum_variadic(Vectores3D(1, 1, 1), Vectores3D(2, 2, 2));
        assert(s5.x == 3 && s5.y == 3 && s5.z == 3);
        auto s6 = max_variadic(Esfera(1.0), Esfera(5.0), Esfera(2.0));
        assert(abs(s6.radio - 5.0) < 1e-9);
    }




    // Caso que NO compila: sum_variadic con Esfera (Esfera no cumple Addable, no define operator+)
    // auto s = sum_variadic(Esfera(1.0), Esfera(2.0));
    // ERROR: no satisface el concept Addable en la lista de tipos.
    // Caso que NO compila: mean_variadic mezclando tipos que no son Divisible struct SoloSuma { int v; SoloSuma operator+(const SoloSuma& o) const { return {v+o.v}; } };
    // auto m = mean_variadic(SoloSuma{1}, SoloSuma{2});
    // ERROR: SoloSuma no cumple Divisible (no existe operator/(size_t)).

    cout << "Todos los casos validos compilaron y pasaron correctamente.\n";
    return 0;
}
