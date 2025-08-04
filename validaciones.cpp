/**
 * MÓDULO DE VALIDACIONES Y ENTRADA DE DATOS
 * Contiene todas las funciones auxiliares para validar entradas del usuario
 * y mostrar mensajes del sistema de forma consistente
 */

#include "optimizacion.h"
#include <iostream>
#include <sstream>
#include <limits>
#include <string>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

/**
 * Solicita un número real al usuario con validación robusta
 * @param mensaje Mensaje a mostrar al usuario
 * @return Número real válido ingresado por el usuario
 */
double solicitarNumeroReal(const string &mensaje)
{
    double numero;
    string entrada;
    bool entradaValida = false;

    while (!entradaValida)
    {
        cout << mensaje;
        getline(cin, entrada);

        // Remover espacios en blanco
        entrada.erase(remove_if(entrada.begin(), entrada.end(), ::isspace), entrada.end());

        if (entrada.empty())
        {
            mostrarMensajeError("Entrada vacía. Por favor ingrese un número.");
            continue;
        }

        // Intentar convertir a número
        try
        {
            istringstream stream(entrada);
            stream >> numero;

            if (stream.fail() || !stream.eof())
            {
                throw invalid_argument("Formato inválido");
            }

            // Validar que no sea infinito o NaN
            if (!isfinite(numero))
            {
                throw invalid_argument("Número fuera de rango válido");
            }

            entradaValida = true;
        }
        catch (const exception &e)
        {
            mostrarMensajeError("Entrada inválida. Ingrese un número válido (ej: 7.5, 123, -4.2)");
        }
    }

    return numero;
}

/**
 * Solicita un número entero al usuario con validación
 * @param mensaje Mensaje a mostrar al usuario
 * @return Número entero válido ingresado por el usuario
 */
int solicitarNumeroEntero(const string &mensaje)
{
    int numero;
    string entrada;
    bool entradaValida = false;

    while (!entradaValida)
    {
        cout << mensaje;
        getline(cin, entrada);

        // Remover espacios en blanco
        entrada.erase(remove_if(entrada.begin(), entrada.end(), ::isspace), entrada.end());

        if (entrada.empty())
        {
            mostrarMensajeError("Entrada vacía. Por favor ingrese un número entero.");
            continue;
        }

        // Verificar que solo contenga dígitos (y opcionalmente signo negativo al inicio)
        bool formatoValido = true;
        size_t inicio = (entrada[0] == '-') ? 1 : 0;

        if (inicio == entrada.length())
        {
            formatoValido = false; // Solo signo negativo
        }

        for (size_t i = inicio; i < entrada.length() && formatoValido; i++)
        {
            if (!isdigit(entrada[i]))
            {
                formatoValido = false;
            }
        }

        if (!formatoValido)
        {
            mostrarMensajeError("Entrada inválida. Ingrese un número entero válido (ej: 5, -3, 100)");
            continue;
        }

        // Intentar convertir
        try
        {
            numero = stoi(entrada);
            entradaValida = true;
        }
        catch (const out_of_range &e)
        {
            mostrarMensajeError("Número fuera de rango. Ingrese un valor entre " +
                                to_string(numeric_limits<int>::min()) + " y " +
                                to_string(numeric_limits<int>::max()));
        }
        catch (const exception &e)
        {
            mostrarMensajeError("Error al procesar el número. Intente nuevamente.");
        }
    }

    return numero;
}

/**
 * Solicita una cadena de texto al usuario con validación básica
 * @param mensaje Mensaje a mostrar al usuario
 * @param permitirVacia Si se permite entrada vacía
 * @return Cadena de texto válida
 */
string solicitarCadena(const string &mensaje, bool permitirVacia)
{
    string entrada;
    bool entradaValida = false;

    while (!entradaValida)
    {
        cout << mensaje;
        getline(cin, entrada);

        if (entrada.empty() && !permitirVacia)
        {
            mostrarMensajeError("Entrada vacía no permitida. Por favor ingrese texto.");
            continue;
        }

        entradaValida = true;
    }

    return entrada;
}

/**
 * Valida si una cadena representa un operador matemático válido
 * @param operador Cadena a validar
 * @return true si es un operador válido (<=, >=, =)
 */
bool validarOperador(const string &operador)
{
    return (operador == "<=" || operador == ">=" || operador == "=");
}

/**
 * Solicita un operador matemático válido al usuario
 * @param mensaje Mensaje a mostrar al usuario
 * @return Operador válido como string
 */
string solicitarOperador(const string &mensaje)
{
    string operador;
    bool entradaValida = false;

    while (!entradaValida)
    {
        cout << mensaje;
        getline(cin, operador);

        // Remover espacios
        operador.erase(remove_if(operador.begin(), operador.end(), ::isspace), operador.end());

        if (operador.empty())
        {
            operador = "<="; // Valor por defecto
            entradaValida = true;
        }
        else if (validarOperador(operador))
        {
            entradaValida = true;
        }
        else
        {
            mostrarMensajeError("Operador inválido. Use: <= (menor o igual), >= (mayor o igual), = (igual)");
            cout << "Operadores disponibles: <=, >=, =" << endl;
        }
    }

    return operador;
}

/**
 * Valida si un número está en un rango específico
 * @param numero Número a validar
 * @param minimo Valor mínimo permitido
 * @param maximo Valor máximo permitido
 * @return true si está en el rango
 */
bool validarRango(double numero, double minimo, double maximo)
{
    return (numero >= minimo && numero <= maximo);
}

/**
 * Solicita confirmación del usuario (s/n)
 * @param mensaje Mensaje de confirmación
 * @return true si el usuario confirma (s/S), false en caso contrario
 */
bool solicitarConfirmacion(const string &mensaje)
{
    char respuesta;
    bool entradaValida = false;

    while (!entradaValida)
    {
        cout << mensaje << " (s/n): ";
        cin >> respuesta;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        respuesta = tolower(respuesta);

        if (respuesta == 's' || respuesta == 'n')
        {
            entradaValida = true;
        }
        else
        {
            mostrarMensajeError("Respuesta inválida. Ingrese 's' para sí o 'n' para no.");
        }
    }

    return (respuesta == 's');
}

/**
 * Muestra un mensaje de error con formato consistente
 * @param mensaje Mensaje de error a mostrar
 */
void mostrarMensajeError(const string &mensaje)
{
// Cambiar color a rojo en Windows
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD originalColor = consoleInfo.wAttributes;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
#endif

    cout << "\n[ERROR] " << mensaje << endl;

// Restaurar color original en Windows
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, originalColor);
#endif
}

/**
 * Muestra un mensaje de éxito con formato consistente
 * @param mensaje Mensaje de éxito a mostrar
 */
void mostrarMensajeExito(const string &mensaje)
{
// Cambiar color a verde en Windows
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD originalColor = consoleInfo.wAttributes;
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif

    cout << "\n[ÉXITO] " << mensaje << endl;

// Restaurar color original en Windows
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, originalColor);
#endif
}

/**
 * Muestra un mensaje informativo con formato consistente
 * @param mensaje Mensaje informativo a mostrar
 */
void mostrarMensajeInfo(const string &mensaje)
{
// Cambiar color a azul en Windows
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD originalColor = consoleInfo.wAttributes;
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif

    cout << "\n[INFO] " << mensaje << endl;

// Restaurar color original en Windows
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, originalColor);
#endif
}

/**
 * Muestra un mensaje de advertencia con formato consistente
 * @param mensaje Mensaje de advertencia a mostrar
 */
void mostrarMensajeAdvertencia(const string &mensaje)
{
// Cambiar color a amarillo en Windows
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    WORD originalColor = consoleInfo.wAttributes;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#endif

    cout << "\n[ADVERTENCIA] " << mensaje << endl;

// Restaurar color original en Windows
#ifdef _WIN32
    SetConsoleTextAttribute(hConsole, originalColor);
#endif
}

/**
 * Valida una dirección de email básica (para futuras extensiones)
 * @param email Email a validar
 * @return true si tiene formato básico válido
 */
bool validarEmail(const string &email)
{
    if (email.empty())
        return false;

    size_t arrobaPos = email.find('@');
    if (arrobaPos == string::npos || arrobaPos == 0 || arrobaPos == email.length() - 1)
    {
        return false;
    }

    size_t puntoPos = email.find('.', arrobaPos);
    if (puntoPos == string::npos || puntoPos == email.length() - 1)
    {
        return false;
    }

    return true;
}

/**
 * Convierte una cadena a mayúsculas
 * @param cadena Cadena a convertir
 * @return Cadena en mayúsculas
 */
string convertirAMayusculas(const string &cadena)
{
    string resultado = cadena;
    transform(resultado.begin(), resultado.end(), resultado.begin(), ::toupper);
    return resultado;
}

/**
 * Convierte una cadena a minúsculas
 * @param cadena Cadena a convertir
 * @return Cadena en minúsculas
 */
string convertirAMinusculas(const string &cadena)
{
    string resultado = cadena;
    transform(resultado.begin(), resultado.end(), resultado.begin(), ::tolower);
    return resultado;
}

/**
 * Elimina espacios en blanco al inicio y final de una cadena
 * @param cadena Cadena a procesar
 * @return Cadena sin espacios al inicio y final
 */
string eliminarEspaciosExtremos(const string &cadena)
{
    size_t inicio = cadena.find_first_not_of(" \t\n\r\f\v");
    if (inicio == string::npos)
        return "";

    size_t final = cadena.find_last_not_of(" \t\n\r\f\v");
    return cadena.substr(inicio, final - inicio + 1);
}

/**
 * Valida si una cadena contiene solo números
 * @param cadena Cadena a validar
 * @return true si contiene solo dígitos
 */
bool esNumerico(const string &cadena)
{
    if (cadena.empty())
        return false;

    size_t inicio = (cadena[0] == '-' || cadena[0] == '+') ? 1 : 0;
    if (inicio == cadena.length())
        return false;

    bool tienePunto = false;
    for (size_t i = inicio; i < cadena.length(); i++)
    {
        if (cadena[i] == '.')
        {
            if (tienePunto)
                return false; // Más de un punto
            tienePunto = true;
        }
        else if (!isdigit(cadena[i]))
        {
            return false;
        }
    }

    return true;
}

/**
 * Genera una línea separadora para la interfaz
 * @param caracter Carácter a usar para la línea
 * @param longitud Longitud de la línea
 * @return String con la línea separadora
 */
string generarLineaSeparadora(char caracter, int longitud)
{
    return string(longitud, caracter);
}

/**
 * Centra un texto en una línea de longitud específica
 * @param texto Texto a centrar
 * @param longitud Longitud total de la línea
 * @return Texto centrado con espacios
 */
string centrarTexto(const string &texto, int longitud)
{
    if (static_cast<int>(texto.length()) >= longitud)
    {
        return texto;
    }

    int espacios = longitud - static_cast<int>(texto.length());
    int espaciosIzquierda = espacios / 2;
    int espaciosDerecha = espacios - espaciosIzquierda;

    return string(espaciosIzquierda, ' ') + texto + string(espaciosDerecha, ' ');
}