/**
 * IMPLEMENTACIÓN PRINCIPAL DEL SISTEMA DE OPTIMIZACIÓN
 * Contiene toda la lógica de negocio y cálculos del sistema
 */

#include "optimizacion.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cmath>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <cstdlib>
#include <unistd.h>
#endif

using namespace std;

// Constructor de la clase SistemaOptimizacion
SistemaOptimizacion::SistemaOptimizacion()
    : precioMesa(0.0), precioSilla(0.0), preciosIngresados(false), restriccionesIngresadas(false)
{
    // Inicializar restricciones predeterminadas del caso Flair Furniture
    // Estas se pueden modificar en la Opción 2
    restricciones.clear();
}

// Función principal que ejecuta el sistema
void SistemaOptimizacion::ejecutarSistema()
{
    int opcion;
    bool continuar = true;

    while (continuar)
    {
        try
        {
            mostrarMenuPrincipal();

            if (validarEntradaMenu(opcion))
            {
                if (opcion == 6)
                {
                    continuar = false;
                }
                else
                {
                    ejecutarOpcion(opcion);
                }
            }
            else
            {
                mostrarMensajeError("Opción inválida. Seleccione un número del 1 al 6.");
            }

            if (continuar && opcion != 5)
            { // No pausar después de la opción gráfica
                pausarSistema();
            }
        }
        catch (const exception &e)
        {
            manejarExcepcion(e);
            pausarSistema();
        }
    }
}

// Mostrar el menú principal
void SistemaOptimizacion::mostrarMenuPrincipal()
{
    limpiarPantalla();

    cout << "\n"
         << string(60, '=') << endl;
    cout << "           MENÚ PRINCIPAL - OPTIMIZACIÓN" << endl;
    cout << string(60, '=') << endl;
    cout << "\n1. Ingreso de precios de venta" << endl;
    cout << "2. Ingreso de restricciones de producción" << endl;
    cout << "3. Mostrar función de ganancia" << endl;
    cout << "4. Calcular solución óptima" << endl;
    cout << "5. Visualizar solución gráfica" << endl;
    cout << "6. Salir del programa" << endl;
    cout << "\n"
         << string(60, '-') << endl;

    // Mostrar estado actual del sistema
    cout << "Estado actual:" << endl;
    cout << "  • Precios: " << (preciosIngresados ? "✓ Configurados" : "✗ No configurados") << endl;
    cout << "  • Restricciones: " << (restriccionesIngresadas ? "✓ Configuradas (" + to_string(restricciones.size()) + ")" : "✗ No configuradas") << endl;
    cout << "  • Solución: " << (solucion.solucionEncontrada ? "✓ Calculada" : "✗ No calculada") << endl;

    cout << "\n"
         << string(60, '-') << endl;
    cout << "Seleccione una opción (1-6): ";
}

// Ejecutar la opción seleccionada
void SistemaOptimizacion::ejecutarOpcion(int opcion)
{
    switch (opcion)
    {
    case 1:
        ingresarPrecios();
        break;
    case 2:
        ingresarRestricciones();
        break;
    case 3:
        mostrarFuncionGanancia();
        break;
    case 4:
        calcularSolucionOptima();
        break;
    case 5:
        mostrarSolucionGrafica();
        break;
    default:
        throw runtime_error("Opción no implementada: " + to_string(opcion));
    }
}

// OPCIÓN 1: Ingreso de precios de venta
void SistemaOptimizacion::ingresarPrecios()
{
    limpiarPantalla();
    cout << "\n"
         << string(50, '=') << endl;
    cout << "         OPCIÓN 1: INGRESO DE PRECIOS" << endl;
    cout << string(50, '=') << endl;

    try
    {
        cout << "\nIngrese los precios de venta:" << endl;

        // Solicitar precio de mesas
        double precioMesaTemp = solicitarNumeroReal("Precio de venta por mesa (USD): $");
        if (!validarPrecio(precioMesaTemp))
        {
            throw invalid_argument("El precio de las mesas debe ser positivo.");
        }

        // Solicitar precio de sillas
        double precioSillaTemp = solicitarNumeroReal("Precio de venta por silla (USD): $");
        if (!validarPrecio(precioSillaTemp))
        {
            throw invalid_argument("El precio de las sillas debe ser positivo.");
        }

        // Guardar precios si son válidos
        precioMesa = precioMesaTemp;
        precioSilla = precioSillaTemp;
        preciosIngresados = true;

        // Resetear solución anterior si existía
        solucion.solucionEncontrada = false;

        // Mostrar confirmación
        cout << "\n"
             << string(50, '-') << endl;
        mostrarMensajeExito("Los precios fueron registrados exitosamente:");
        cout << "  • Mesas: $" << formatearNumero(precioMesa) << " USD" << endl;
        cout << "  • Sillas: $" << formatearNumero(precioSilla) << " USD" << endl;
    }
    catch (const exception &e)
    {
        mostrarMensajeError("Error al ingresar precios: " + string(e.what()));
        preciosIngresados = false;
    }
}

// OPCIÓN 2: Ingreso de restricciones de producción
void SistemaOptimizacion::ingresarRestricciones()
{
    limpiarPantalla();
    cout << "\n"
         << string(50, '=') << endl;
    cout << "      OPCIÓN 2: RESTRICCIONES DE PRODUCCIÓN" << endl;
    cout << string(50, '=') << endl;

    try
    {
        cout << "\n¿Desea usar las restricciones del caso Flair Furniture? (s/n): ";
        char opcion;
        cin >> opcion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        restricciones.clear();

        if (opcion == 's' || opcion == 'S')
        {
            // Cargar restricciones predeterminadas del caso Flair
            restricciones.push_back(Restriccion(4.0, 3.0, 240.0));     // Carpintería: 4x₁ + 3x₂ ≤ 240
            restricciones.push_back(Restriccion(2.0, 1.0, 100.0));     // Pintura: 2x₁ + x₂ ≤ 100
            restricciones.push_back(Restriccion(0.0, 1.0, 60.0));      // Límite sillas: x₂ ≤ 60
            restricciones.push_back(Restriccion(1.0, 0.0, 0.0, ">=")); // No negatividad x₁: x₁ ≥ 0
            restricciones.push_back(Restriccion(0.0, 1.0, 0.0, ">=")); // No negatividad x₂: x₂ ≥ 0

            mostrarMensajeExito("Restricciones del caso Flair Furniture cargadas.");
        }
        else
        {
            // Ingreso manual de restricciones
            int numRestricciones = solicitarNumeroEntero("Número de restricciones a ingresar: ");

            if (numRestricciones <= 0 || numRestricciones > 20)
            {
                throw invalid_argument("El número de restricciones debe estar entre 1 y 20.");
            }

            for (int i = 0; i < numRestricciones; i++)
            {
                cout << "\n--- Restricción " << (i + 1) << " ---" << endl;
                cout << "Formato: ax₁ + bx₂ ≤ c" << endl;

                double coefX1 = solicitarNumeroReal("Coeficiente de x₁ (mesas): ");
                double coefX2 = solicitarNumeroReal("Coeficiente de x₂ (sillas): ");
                double constante = solicitarNumeroReal("Valor constante (lado derecho): ");

                cout << "Operador (<=, >=, =) [por defecto <=]: ";
                string operador;
                getline(cin, operador);
                if (operador.empty())
                    operador = "<=";

                Restriccion nuevaRestriccion(coefX1, coefX2, constante, operador);

                if (validarRestriccion(nuevaRestriccion))
                {
                    restricciones.push_back(nuevaRestriccion);
                }
                else
                {
                    mostrarMensajeError("Restricción inválida. Se omitirá.");
                    i--; // Repetir esta iteración
                }
            }
        }

        restriccionesIngresadas = !restricciones.empty();
        solucion.solucionEncontrada = false; // Resetear solución

        cout << "\n"
             << string(50, '-') << endl;
        mostrarRestricciones();
    }
    catch (const exception &e)
    {
        mostrarMensajeError("Error al ingresar restricciones: " + string(e.what()));
        restriccionesIngresadas = false;
    }
}

// OPCIÓN 3: Mostrar función de ganancia
void SistemaOptimizacion::mostrarFuncionGanancia()
{
    limpiarPantalla();
    cout << "\n"
         << string(50, '=') << endl;
    cout << "        OPCIÓN 3: FUNCIÓN DE GANANCIA" << endl;
    cout << string(50, '=') << endl;

    if (!preciosIngresados)
    {
        mostrarMensajeError("Debe ingresar los precios primero (Opción 1).");
        return;
    }

    cout << "\nLa función objetivo a maximizar es:" << endl;
    cout << "\n  Maximizar Z = " << formatearNumero(precioMesa) << "x₁ + "
         << formatearNumero(precioSilla) << "x₂" << endl;

    cout << "\nDonde:" << endl;
    cout << "  • x₁ = Número de mesas a producir" << endl;
    cout << "  • x₂ = Número de sillas a producir" << endl;
    cout << "  • Z = Ganancia total en USD" << endl;

    if (restriccionesIngresadas)
    {
        cout << "\n"
             << string(50, '-') << endl;
        cout << "Restricciones actuales:" << endl;
        mostrarRestricciones();
    }
}

// OPCIÓN 4: Calcular solución óptima
void SistemaOptimizacion::calcularSolucionOptima()
{
    limpiarPantalla();
    cout << "\n"
         << string(50, '=') << endl;
    cout << "        OPCIÓN 4: CÁLCULO DE SOLUCIÓN ÓPTIMA" << endl;
    cout << string(50, '=') << endl;

    if (!verificarDatosPrevios())
    {
        return;
    }

    try
    {
        cout << "\nCalculando solución óptima..." << endl;
        cout << "Método: Evaluación de puntos extremos" << endl;

        // Encontrar puntos de intersección (vértices del área factible)
        vector<pair<double, double>> puntosInterseccion = encontrarPuntosInterseccion();

        if (puntosInterseccion.empty())
        {
            throw runtime_error("No se encontraron puntos factibles. Verifique las restricciones.");
        }

        // Evaluar función objetivo en cada punto factible
        double mejorGanancia = -numeric_limits<double>::infinity();
        double mejorX1 = 0, mejorX2 = 0;

        cout << "\nEvaluando puntos candidatos:" << endl;
        cout << string(40, '-') << endl;

        for (const auto &punto : puntosInterseccion)
        {
            double x1 = punto.first;
            double x2 = punto.second;

            if (puntoEsFactible(x1, x2))
            {
                double ganancia = evaluarFuncionObjetivo(x1, x2);

                cout << "Punto (" << formatearNumero(x1) << ", " << formatearNumero(x2)
                     << ") → Z = $" << formatearNumero(ganancia) << endl;

                if (ganancia > mejorGanancia)
                {
                    mejorGanancia = ganancia;
                    mejorX1 = x1;
                    mejorX2 = x2;
                }
            }
        }

        // Guardar solución óptima
        solucion.x1 = mejorX1;
        solucion.x2 = mejorX2;
        solucion.gananciaMaxima = mejorGanancia;
        solucion.solucionEncontrada = true;

        // Mostrar resultado
        cout << "\n"
             << string(50, '=') << endl;
        mostrarMensajeExito("SOLUCIÓN ÓPTIMA ENCONTRADA:");
        cout << string(50, '=') << endl;
        cout << "  • Número de mesas (x₁): " << formatearNumero(solucion.x1, 0) << " unidades" << endl;
        cout << "  • Número de sillas (x₂): " << formatearNumero(solucion.x2, 0) << " unidades" << endl;
        cout << "  • Ganancia máxima: $" << formatearNumero(solucion.gananciaMaxima) << " USD" << endl;
        cout << string(50, '=') << endl;
    }
    catch (const exception &e)
    {
        mostrarMensajeError("Error en el cálculo: " + string(e.what()));
        solucion.solucionEncontrada = false;
    }
}

// OPCIÓN 5: Mostrar solución gráfica (placeholder - se implementará en graficos.cpp)
void SistemaOptimizacion::mostrarSolucionGrafica()
{
    limpiarPantalla();
    cout << "\n"
         << string(50, '=') << endl;
    cout << "        OPCIÓN 5: VISUALIZACIÓN GRÁFICA" << endl;
    cout << string(50, '=') << endl;

    if (!verificarDatosPrevios() || !solucion.solucionEncontrada)
    {
        mostrarMensajeError("Debe calcular la solución óptima primero (Opción 4).");
        return;
    }

    cout << "\nPreparando visualización gráfica..." << endl;
    cout << "Esto abrirá una ventana con el gráfico de la solución." << endl;
    cout << "\nPresione Enter para continuar...";
    cin.get();

    // Esta función se implementará en graficos.cpp
    // Por ahora mostramos un mensaje informativo
    cout << "\n[INFORMACIÓN] La visualización gráfica se implementará" << endl;
    cout << "usando la librería SFML en el archivo graficos.cpp" << endl;
    cout << "\nDatos para el gráfico:" << endl;
    cout << "• Punto óptimo: (" << formatearNumero(solucion.x1) << ", "
         << formatearNumero(solucion.x2) << ")" << endl;
    cout << "• Ganancia: $" << formatearNumero(solucion.gananciaMaxima) << endl;
}

// Implementación completa de funciones auxiliares para cálculos
vector<pair<double, double>> SistemaOptimizacion::encontrarPuntosInterseccion()
{
    vector<pair<double, double>> puntos;

    // Agregar punto origen (0,0)
    puntos.push_back(make_pair(0.0, 0.0));

    // Encontrar intersecciones con los ejes
    for (const auto &restriccion : restricciones)
    {
        if (restriccion.operador == "<=" || restriccion.operador == "=")
        {
            // Intersección con eje X (x2 = 0)
            if (restriccion.coeficienteX1 != 0)
            {
                double x1 = restriccion.valorConstante / restriccion.coeficienteX1;
                if (x1 >= 0)
                {
                    puntos.push_back(make_pair(x1, 0.0));
                }
            }

            // Intersección con eje Y (x1 = 0)
            if (restriccion.coeficienteX2 != 0)
            {
                double x2 = restriccion.valorConstante / restriccion.coeficienteX2;
                if (x2 >= 0)
                {
                    puntos.push_back(make_pair(0.0, x2));
                }
            }
        }
    }

    // Encontrar intersecciones entre pares de restricciones
    for (size_t i = 0; i < restricciones.size(); i++)
    {
        for (size_t j = i + 1; j < restricciones.size(); j++)
        {
            if (restricciones[i].operador == "<=" || restricciones[i].operador == "=")
            {
                if (restricciones[j].operador == "<=" || restricciones[j].operador == "=")
                {
                    pair<double, double> interseccion = interseccionRectas(restricciones[i], restricciones[j]);
                    if (interseccion.first >= -1e-6 && interseccion.second >= -1e-6)
                    {
                        puntos.push_back(interseccion);
                    }
                }
            }
        }
    }

    // Eliminar puntos duplicados
    sort(puntos.begin(), puntos.end());
    puntos.erase(unique(puntos.begin(), puntos.end(),
                        [](const pair<double, double> &a, const pair<double, double> &b)
                        {
                            return abs(a.first - b.first) < 1e-6 && abs(a.second - b.second) < 1e-6;
                        }),
                 puntos.end());

    return puntos;
}

pair<double, double> SistemaOptimizacion::interseccionRectas(const Restriccion &r1, const Restriccion &r2)
{
    // Resolver sistema: a1*x1 + b1*x2 = c1, a2*x1 + b2*x2 = c2
    double a1 = r1.coeficienteX1, b1 = r1.coeficienteX2, c1 = r1.valorConstante;
    double a2 = r2.coeficienteX1, b2 = r2.coeficienteX2, c2 = r2.valorConstante;

    double determinante = a1 * b2 - a2 * b1;

    if (abs(determinante) < 1e-10)
    {
        // Rectas paralelas o coincidentes
        return make_pair(-1e9, -1e9);
    }

    double x1 = (c1 * b2 - c2 * b1) / determinante;
    double x2 = (a1 * c2 - a2 * c1) / determinante;

    return make_pair(x1, x2);
}

// Funciones auxiliares para validación y cálculos
bool SistemaOptimizacion::validarPrecio(double precio)
{
    return precio > 0.0;
}

bool SistemaOptimizacion::validarRestriccion(const Restriccion &restriccion)
{
    // Validar que no todos los coeficientes sean cero
    if (restriccion.coeficienteX1 == 0.0 && restriccion.coeficienteX2 == 0.0)
    {
        return false;
    }

    // Validar operador
    string op = restriccion.operador;
    return (op == "<=" || op == ">=" || op == "=");
}

bool SistemaOptimizacion::verificarDatosPrevios()
{
    if (!preciosIngresados)
    {
        mostrarMensajeError("Debe ingresar los precios primero (Opción 1).");
        return false;
    }

    if (!restriccionesIngresadas)
    {
        mostrarMensajeError("Debe ingresar las restricciones primero (Opción 2).");
        return false;
    }

    return true;
}

void SistemaOptimizacion::mostrarRestricciones()
{
    if (restricciones.empty())
    {
        cout << "No hay restricciones registradas." << endl;
        return;
    }

    cout << "\nRestricciones registradas:" << endl;
    for (size_t i = 0; i < restricciones.size(); i++)
    {
        const auto &r = restricciones[i];
        cout << "  " << (i + 1) << ". ";

        if (r.coeficienteX1 != 0)
        {
            cout << formatearNumero(r.coeficienteX1) << "x₁";
        }

        if (r.coeficienteX2 != 0)
        {
            if (r.coeficienteX1 != 0)
            {
                cout << (r.coeficienteX2 > 0 ? " + " : " - ");
                cout << formatearNumero(abs(r.coeficienteX2)) << "x₂";
            }
            else
            {
                cout << formatearNumero(r.coeficienteX2) << "x₂";
            }
        }

        cout << " " << r.operador << " " << formatearNumero(r.valorConstante) << endl;
    }
}

double SistemaOptimizacion::evaluarFuncionObjetivo(double x1, double x2)
{
    return precioMesa * x1 + precioSilla * x2;
}

bool SistemaOptimizacion::puntoEsFactible(double x1, double x2)
{
    for (const auto &restriccion : restricciones)
    {
        double valorIzquierdo = restriccion.coeficienteX1 * x1 + restriccion.coeficienteX2 * x2;

        if (restriccion.operador == "<=")
        {
            if (valorIzquierdo > restriccion.valorConstante + 1e-6)
                return false;
        }
        else if (restriccion.operador == ">=")
        {
            if (valorIzquierdo < restriccion.valorConstante - 1e-6)
                return false;
        }
        else if (restriccion.operador == "=")
        {
            if (abs(valorIzquierdo - restriccion.valorConstante) > 1e-6)
                return false;
        }
    }
    return true;
}

// Funciones utilitarias
void SistemaOptimizacion::limpiarPantalla()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void SistemaOptimizacion::pausarSistema()
{
    cout << "\nPresione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string SistemaOptimizacion::formatearNumero(double numero, int decimales)
{
    ostringstream stream;
    stream << fixed << setprecision(decimales) << numero;
    return stream.str();
}

bool SistemaOptimizacion::validarEntradaMenu(int &opcion)
{
    if (!(cin >> opcion))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return opcion >= 1 && opcion <= 6;
}

void SistemaOptimizacion::manejarExcepcion(const exception &e)
{
    mostrarMensajeError("Excepción capturada: " + string(e.what()));
}