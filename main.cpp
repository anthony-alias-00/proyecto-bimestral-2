/**
 * PROYECTO SEGUNDO BIMESTRE - PROGRAMACIÓN I
 * Sistema de Optimización de Producción
 * Flair Furniture Company - Maximización de Beneficios
 *
 * Descripción: Aplicación de consola que resuelve problemas de optimización
 * de producción utilizando programación lineal para maximizar beneficios
 * en la fabricación de mesas y sillas.
 *
 * Autores: [Nombres de los integrantes del grupo]
 * Fecha: Julio 2025
 */

#include "optimizacion.h"
#include <iostream>
#include <limits>

using namespace std;

// Función para mostrar la información inicial del programa
void mostrarInformacionInicial()
{
    cout << "\n"
         << string(60, '=') << endl;
    cout << "    SISTEMA DE OPTIMIZACIÓN DE PRODUCCIÓN" << endl;
    cout << "         FLAIR FURNITURE COMPANY" << endl;
    cout << string(60, '=') << endl;
    cout << "\nEste sistema permite calcular la combinación óptima" << endl;
    cout << "de mesas y sillas para maximizar los beneficios," << endl;
    cout << "considerando las restricciones de producción." << endl;
    cout << "\nPresione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Función principal
int main()
{
    try
    {
        // Configurar la salida para mostrar números decimales correctamente
        cout << fixed << setprecision(2);

        // Mostrar información inicial
        mostrarInformacionInicial();

        // Crear instancia del sistema de optimización
        SistemaOptimizacion sistema;

        // Ejecutar el sistema principal
        sistema.ejecutarSistema();

        // Mensaje de despedida
        cout << "\n"
             << string(50, '-') << endl;
        cout << "Gracias por usar el sistema de optimización." << endl;
        cout << "¡Vuelva pronto!" << endl;
        cout << string(50, '-') << endl;
    }
    catch (const exception &e)
    {
        cerr << "\n[ERROR CRÍTICO] " << e.what() << endl;
        cerr << "El programa se cerrará. Contacte al administrador." << endl;

        cout << "\nPresione Enter para salir...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

        return 1;
    }
    catch (...)
    {
        cerr << "\n[ERROR CRÍTICO] Error desconocido en el sistema." << endl;
        cerr << "El programa se cerrará." << endl;

        cout << "\nPresione Enter para salir...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

        return 1;
    }

    return 0;
}