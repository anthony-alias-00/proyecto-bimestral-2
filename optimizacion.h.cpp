#ifndef OPTIMIZACION_H
#define OPTIMIZACION_H

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <stdexcept>

// Estructura para representar una restricción lineal
struct Restriccion
{
    double coeficienteX1;  // Coeficiente de x1 (mesas)
    double coeficienteX2;  // Coeficiente de x2 (sillas)
    double valorConstante; // Valor del lado derecho de la restricción
    std::string operador;  // Operador (<=, >=, =)

    // Constructor
    Restriccion(double x1, double x2, double constante, std::string op = "<=")
        : coeficienteX1(x1), coeficienteX2(x2), valorConstante(constante), operador(op) {}
};

// Estructura para la solución óptima
struct SolucionOptima
{
    double x1;               // Número óptimo de mesas
    double x2;               // Número óptimo de sillas
    double gananciaMaxima;   // Ganancia máxima obtenida
    bool solucionEncontrada; // Indica si se encontró una solución válida

    // Constructor
    SolucionOptima() : x1(0), x2(0), gananciaMaxima(0), solucionEncontrada(false) {}
};

// Clase principal para el sistema de optimización
class SistemaOptimizacion
{
private:
    double precioMesa;                      // Precio de venta por mesa (p1)
    double precioSilla;                     // Precio de venta por silla (p2)
    std::vector<Restriccion> restricciones; // Vector de restricciones
    SolucionOptima solucion;                // Solución óptima calculada
    bool preciosIngresados;                 // Flag para verificar si se ingresaron precios
    bool restriccionesIngresadas;           // Flag para verificar si se ingresaron restricciones

public:
    // Constructor
    SistemaOptimizacion();

    // Funciones principales del menú
    void mostrarMenuPrincipal();
    void ejecutarOpcion(int opcion);
    void ejecutarSistema();

    // Opción 1: Ingreso de precios
    void ingresarPrecios();
    bool validarPrecio(double precio);

    // Opción 2: Ingreso de restricciones
    void ingresarRestricciones();
    void mostrarRestricciones();
    bool validarRestriccion(const Restriccion &restriccion);

    // Opción 3: Mostrar función de ganancia
    void mostrarFuncionGanancia();

    // Opción 4: Calcular solución óptima
    void calcularSolucionOptima();
    bool verificarDatosPrevios();

    // Opción 5: Mostrar solución gráfica
    void mostrarSolucionGrafica();

    // Funciones auxiliares para el cálculo
    std::vector<std::pair<double, double>> encontrarPuntosInterseccion();
    std::pair<double, double> interseccionRectas(const Restriccion &r1, const Restriccion &r2);
    bool puntoEsFactible(double x1, double x2);
    double evaluarFuncionObjetivo(double x1, double x2);

    // Funciones de validación y manejo de errores
    void manejarExcepcion(const std::exception &e);
    bool validarEntradaMenu(int &opcion);

    // Funciones utilitarias
    void limpiarPantalla();
    void pausarSistema();
    std::string formatearNumero(double numero, int decimales = 2);

    // Getters para acceso a datos
    double getPrecioMesa() const { return precioMesa; }
    double getPrecioSilla() const { return precioSilla; }
    const std::vector<Restriccion> &getRestricciones() const { return restricciones; }
    const SolucionOptima &getSolucion() const { return solucion; }
};

// Funciones globales para manejo de entrada
double solicitarNumeroReal(const std::string &mensaje);
int solicitarNumeroEntero(const std::string &mensaje);
void mostrarMensajeError(const std::string &mensaje);
void mostrarMensajeExito(const std::string &mensaje);

#endif // OPTIMIZACION_H