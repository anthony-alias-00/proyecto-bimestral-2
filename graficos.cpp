/**
 * MÓDULO DE VISUALIZACIÓN GRÁFICA
 * Implementa la representación visual del problema de optimización
 * usando la librería SFML (Simple and Fast Multimedia Library)
 *
 * INSTALACIÓN DE SFML:
 * - Windows: Descargar desde https://www.sfml-dev.org/
 * - Ubuntu/Debian: sudo apt-get install libsfml-dev
 * - macOS: brew install sfml
 *
 * COMPILACIÓN:
 * g++ -o optimizacion main.cpp optimizacion.cpp validaciones.cpp graficos.cpp -lsfml-graphics -lsfml-window -lsfml-system
 */

#include "optimizacion.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Comentar la siguiente línea si SFML no está disponible
#define SFML_DISPONIBLE

#ifdef SFML_DISPONIBLE
#include <SFML/Graphics.hpp>
using namespace sf;
#endif

using namespace std;

// Estructura para representar un punto en el gráfico
struct PuntoGrafico
{
    double x, y;
    PuntoGrafico(double x = 0, double y = 0) : x(x), y(y) {}
};

// Clase para manejar la visualización gráfica
class VisualizadorGrafico
{
private:
    static const int ANCHO_VENTANA = 800;
    static const int ALTO_VENTANA = 600;
    static const int MARGEN = 80;

    double escalaX, escalaY;
    double maxX, maxY;
    double origenX, origenY;

#ifdef SFML_DISPONIBLE
    RenderWindow ventana;
    Font fuente;
#endif

public:
    VisualizadorGrafico();
    bool inicializar();
    void configurarEscala(const vector<Restriccion> &restricciones, const SolucionOptima &solucion);
    void dibujarEjes();
    void dibujarRestricciones(const vector<Restriccion> &restricciones);
    void dibujarAreaFactible(const vector<Restriccion> &restricciones);
    void dibujarPuntoOptimo(const SolucionOptima &solucion);
    void dibujarFuncionObjetivo(double precioMesa, double precioSilla, double gananciaOptima);
    void mostrarLeyenda(const vector<Restriccion> &restricciones, const SolucionOptima &solucion);
    void ejecutarVisualizacion(const SistemaOptimizacion &sistema);

private:
    PuntoGrafico convertirAPantalla(double x, double y);
    vector<PuntoGrafico> calcularPuntosRecta(const Restriccion &restriccion, double xMin, double xMax);
    vector<PuntoGrafico> encontrarVerticesAreaFactible(const vector<Restriccion> &restricciones);
    string formatearNumero(double numero, int decimales = 2);
};

// Constructor
VisualizadorGrafico::VisualizadorGrafico() : escalaX(1.0), escalaY(1.0), maxX(100.0), maxY(100.0)
{
    origenX = MARGEN;
    origenY = ALTO_VENTANA - MARGEN;
}

// Inicializar SFML y cargar recursos
bool VisualizadorGrafico::inicializar()
{
#ifdef SFML_DISPONIBLE
    ventana.create(VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "Optimización de Producción - Visualización Gráfica");
    ventana.setFramerateLimit(60);

    // Intentar cargar una fuente por defecto del sistema
    if (!fuente.loadFromFile("arial.ttf"))
    {
        // Si no encuentra arial.ttf, usar fuente por defecto
        cout << "[INFO] No se pudo cargar arial.ttf, usando fuente por defecto." << endl;
        // En sistemas Linux/Unix, se puede intentar:
        if (!fuente.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
        {
            if (!fuente.loadFromFile("/System/Library/Fonts/Arial.ttf"))
            {
                cout << "[ADVERTENCIA] No se pudo cargar ninguna fuente del sistema." << endl;
                return false;
            }
        }
    }

    return true;
#else
    cout << "[ERROR] SFML no está disponible. La visualización gráfica no funcionará." << endl;
    return false;
#endif
}

// Configurar la escala del gráfico basada en los datos
void VisualizadorGrafico::configurarEscala(const vector<Restriccion> &restricciones, const SolucionOptima &solucion)
{
    maxX = 0;
    maxY = 0;

    // Encontrar los valores máximos basados en las restricciones
    for (const auto &restriccion : restricciones)
    {
        if (restriccion.operador == "<=" || restriccion.operador == "=")
        {
            if (restriccion.coeficienteX1 > 0)
            {
                maxX = max(maxX, restriccion.valorConstante / restriccion.coeficienteX1);
            }
            if (restriccion.coeficienteX2 > 0)
            {
                maxY = max(maxY, restriccion.valorConstante / restriccion.coeficienteX2);
            }
        }
    }

    // Considerar el punto óptimo
    if (solucion.solucionEncontrada)
    {
        maxX = max(maxX, solucion.x1 * 1.2);
        maxY = max(maxY, solucion.x2 * 1.2);
    }

    // Asegurar valores mínimos para la visualización
    maxX = max(maxX, 10.0);
    maxY = max(maxY, 10.0);

    // Redondear hacia arriba para números más limpios
    maxX = ceil(maxX / 10.0) * 10.0;
    maxY = ceil(maxY / 10.0) * 10.0;

    // Calcular escalas
    escalaX = (ANCHO_VENTANA - 2 * MARGEN) / maxX;
    escalaY = (ALTO_VENTANA - 2 * MARGEN) / maxY;
}

// Convertir coordenadas matemáticas a coordenadas de pantalla
PuntoGrafico VisualizadorGrafico::convertirAPantalla(double x, double y)
{
    return PuntoGrafico(origenX + x * escalaX, origenY - y * escalaY);
}

#ifdef SFML_DISPONIBLE

// Dibujar los ejes coordenados
void VisualizadorGrafico::dibujarEjes()
{
    // Eje X
    Vertex ejeX[] = {
        Vertex(Vector2f(origenX, origenY), Color::Black),
        Vertex(Vector2f(origenX + maxX * escalaX, origenY), Color::Black)};
    ventana.draw(ejeX, 2, Lines);

    // Eje Y
    Vertex ejeY[] = {
        Vertex(Vector2f(origenX, origenY), Color::Black),
        Vertex(Vector2f(origenX, origenY - maxY * escalaY), Color::Black)};
    ventana.draw(ejeY, 2, Lines);

    // Etiquetas de los ejes
    Text etiquetaX("x₁ (Mesas)", fuente, 14);
    etiquetaX.setFillColor(Color::Black);
    etiquetaX.setPosition(origenX + maxX * escalaX / 2, origenY + 20);
    ventana.draw(etiquetaX);

    Text etiquetaY("x₂ (Sillas)", fuente, 14);
    etiquetaY.setFillColor(Color::Black);
    etiquetaY.setPosition(10, origenY - maxY * escalaY / 2);
    etiquetaY.setRotation(-90);
    ventana.draw(etiquetaY);

    // Marcas en los ejes
    for (int i = 0; i <= static_cast<int>(maxX); i += static_cast<int>(maxX / 10))
    {
        if (i == 0)
            continue;

        PuntoGrafico punto = convertirAPantalla(i, 0);

        // Marca pequeña
        Vertex marca[] = {
            Vertex(Vector2f(punto.x, punto.y - 3), Color::Black),
            Vertex(Vector2f(punto.x, punto.y + 3), Color::Black)};
        ventana.draw(marca, 2, Lines);

        // Número
        Text numero(to_string(i), fuente, 10);
        numero.setFillColor(Color::Black);
        numero.setPosition(punto.x - 8, punto.y + 8);
        ventana.draw(numero);
    }

    for (int i = 0; i <= static_cast<int>(maxY); i += static_cast<int>(maxY / 10))
    {
        if (i == 0)
            continue;

        PuntoGrafico punto = convertirAPantalla(0, i);

        // Marca pequeña
        Vertex marca[] = {
            Vertex(Vector2f(punto.x - 3, punto.y), Color::Black),
            Vertex(Vector2f(punto.x + 3, punto.y), Color::Black)};
        ventana.draw(marca, 2, Lines);

        // Número
        Text numero(to_string(i), fuente, 10);
        numero.setFillColor(Color::Black);
        numero.setPosition(punto.x - 25, punto.y - 8);
        ventana.draw(numero);
    }
}

// Dibujar las líneas de restricciones
void VisualizadorGrafico::dibujarRestricciones(const vector<Restriccion> &restricciones)
{
    Color colores[] = {Color::Red, Color::Blue, Color::Green, Color::Magenta, Color::Cyan};
    int colorIndex = 0;

    for (const auto &restriccion : restricciones)
    {
        if (restriccion.operador == "<=" || restriccion.operador == "=")
        {
            // Saltear restricciones de no negatividad para el dibujo
            if ((restriccion.coeficienteX1 == 1.0 && restriccion.coeficienteX2 == 0.0 && restriccion.valorConstante == 0.0) ||
                (restriccion.coeficienteX1 == 0.0 && restriccion.coeficienteX2 == 1.0 && restriccion.valorConstante == 0.0))
            {
                continue;
            }

            vector<PuntoGrafico> puntosRecta = calcularPuntosRecta(restriccion, 0, maxX);

            if (puntosRecta.size() >= 2)
            {
                PuntoGrafico p1 = convertirAPantalla(puntosRecta[0].x, puntosRecta[0].y);
                PuntoGrafico p2 = convertirAPantalla(puntosRecta[1].x, puntosRecta[1].y);

                Vertex linea[] = {
                    Vertex(Vector2f(p1.x, p1.y), colores[colorIndex % 5]),
                    Vertex(Vector2f(p2.x, p2.y), colores[colorIndex % 5])};
                ventana.draw(linea, 2, Lines);

                // Etiqueta de la restricción
                string etiqueta = formatearNumero(restriccion.coeficienteX1) + "x₁";
                if (restriccion.coeficienteX2 != 0)
                {
                    etiqueta += (restriccion.coeficienteX2 > 0 ? " + " : " - ");
                    etiqueta += formatearNumero(abs(restriccion.coeficienteX2)) + "x₂";
                }
                etiqueta += " " + restriccion.operador + " " + formatearNumero(restriccion.valorConstante);

                Text textoEtiqueta(etiqueta, fuente, 10);
                textoEtiqueta.setFillColor(colores[colorIndex % 5]);
                textoEtiqueta.setPosition((p1.x + p2.x) / 2, (p1.y + p2.y) / 2 - 15);
                ventana.draw(textoEtiqueta);

                colorIndex++;
            }
        }
    }
}

// Dibujar el área factible sombreada
void VisualizadorGrafico::dibujarAreaFactible(const vector<Restriccion> &restricciones)
{
    vector<PuntoGrafico> vertices = encontrarVerticesAreaFactible(restricciones);

    if (vertices.size() >= 3)
    {
        // Crear un polígono con los vértices
        ConvexShape poligono;
        poligono.setPointCount(vertices.size());
        poligono.setFillColor(Color(100, 200, 100, 80)); // Verde semi-transparente
        poligono.setOutlineColor(Color(50, 150, 50));
        poligono.setOutlineThickness(2);

        for (size_t i = 0; i < vertices.size(); i++)
        {
            PuntoGrafico puntoP = convertirAPantalla(vertices[i].x, vertices[i].y);
            poligono.setPoint(i, Vector2f(puntoP.x, puntoP.y));
        }

        ventana.draw(poligono);
    }
}

// Dibujar el punto óptimo
void VisualizadorGrafico::dibujarPuntoOptimo(const SolucionOptima &solucion)
{
    if (!solucion.solucionEncontrada)
        return;

    PuntoGrafico puntoP = convertirAPantalla(solucion.x1, solucion.x2);

    // Dibujar punto como círculo
    CircleShape punto(8);
    punto.setFillColor(Color::Red);
    punto.setOutlineColor(Color::Black);
    punto.setOutlineThickness(2);
    punto.setPosition(puntoP.x - 8, puntoP.y - 8);
    ventana.draw(punto);

    // Etiqueta del punto óptimo
    string etiqueta = "Óptimo (" + formatearNumero(solucion.x1, 0) + ", " +
                      formatearNumero(solucion.x2, 0) + ")\nZ = $" +
                      formatearNumero(solucion.gananciaMaxima);

    Text textoEtiqueta(etiqueta, fuente, 12);
    textoEtiqueta.setFillColor(Color::Red);
    textoEtiqueta.setStyle(Text::Bold);
    textoEtiqueta.setPosition(puntoP.x + 15, puntoP.y - 20);
    ventana.draw(textoEtiqueta);
}

// Dibujar la función objetivo
void VisualizadorGrafico::dibujarFuncionObjetivo(double precioMesa, double precioSilla, double gananciaOptima)
{
    // Dibujar línea de isoganancias que pasa por el punto óptimo
    if (precioSilla != 0)
    {
        // Calcular dos puntos de la línea: precioMesa*x1 + precioSilla*x2 = gananciaOptima
        double x1_1 = 0;
        double x2_1 = gananciaOptima / precioSilla;

        double x1_2 = gananciaOptima / precioMesa;
        double x2_2 = 0;

        if (x1_2 <= maxX && x2_1 <= maxY)
        {
            PuntoGrafico p1 = convertirAPantalla(x1_1, x2_1);
            PuntoGrafico p2 = convertirAPantalla(x1_2, x2_2);

            Vertex lineaObjetivo[] = {
                Vertex(Vector2f(p1.x, p1.y), Color::Black),
                Vertex(Vector2f(p2.x, p2.y), Color::Black)};
            ventana.draw(lineaObjetivo, 2, Lines);

            // Etiqueta de la función objetivo
            string etiqueta = "Z = " + formatearNumero(precioMesa) + "x₁ + " +
                              formatearNumero(precioSilla) + "x₂";

            Text textoEtiqueta(etiqueta, fuente, 12);
            textoEtiqueta.setFillColor(Color::Black);
            textoEtiqueta.setStyle(Text::Bold);
            textoEtiqueta.setPosition((p1.x + p2.x) / 2, (p1.y + p2.y) / 2 + 15);
            ventana.draw(textoEtiqueta);
        }
    }
}

// Mostrar leyenda y información
void VisualizadorGrafico::mostrarLeyenda(const vector<Restriccion> &restricciones, const SolucionOptima &solucion)
{
    // Fondo para la leyenda
    RectangleShape fondoLeyenda(Vector2f(250, 200));
    fondoLeyenda.setFillColor(Color(255, 255, 255, 200));
    fondoLeyenda.setOutlineColor(Color::Black);
    fondoLeyenda.setOutlineThickness(1);
    fondoLeyenda.setPosition(ANCHO_VENTANA - 270, 20);
    ventana.draw(fondoLeyenda);

    // Título de la leyenda
    Text titulo("INFORMACIÓN", fuente, 14);
    titulo.setFillColor(Color::Black);
    titulo.setStyle(Text::Bold);
    titulo.setPosition(ANCHO_VENTANA - 250, 30);
    ventana.draw(titulo);

    // Información de la solución
    if (solucion.solucionEncontrada)
    {
        string info = "Solución Óptima:\n";
        info += "Mesas: " + formatearNumero(solucion.x1, 0) + "\n";
        info += "Sillas: " + formatearNumero(solucion.x2, 0) + "\n";
        info += "Ganancia: $" + formatearNumero(solucion.gananciaMaxima) + "\n\n";
        info += "Área verde: Región factible\n";
        info += "Punto rojo: Solución óptima\n";
        info += "Líneas de colores: Restricciones";

        Text textoInfo(info, fuente, 10);
        textoInfo.setFillColor(Color::Black);
        textoInfo.setPosition(ANCHO_VENTANA - 250, 55);
        ventana.draw(textoInfo);
    }
}

#endif

// Calcular puntos de una recta para dibujar
vector<PuntoGrafico> VisualizadorGrafico::calcularPuntosRecta(const Restriccion &restriccion, double xMin, double xMax)
{
    vector<PuntoGrafico> puntos;

    if (restriccion.coeficienteX2 != 0)
    {
        // Forma: x2 = (c - a*x1) / b
        double x2_min = (restriccion.valorConstante - restriccion.coeficienteX1 * xMin) / restriccion.coeficienteX2;
        double x2_max = (restriccion.valorConstante - restriccion.coeficienteX1 * xMax) / restriccion.coeficienteX2;

        if (x2_min >= 0 && x2_min <= maxY)
        {
            puntos.push_back(PuntoGrafico(xMin, x2_min));
        }
        if (x2_max >= 0 && x2_max <= maxY)
        {
            puntos.push_back(PuntoGrafico(xMax, x2_max));
        }

        // Intersección con eje X (x2 = 0)
        if (restriccion.coeficienteX1 != 0)
        {
            double x1_intercept = restriccion.valorConstante / restriccion.coeficienteX1;
            if (x1_intercept >= xMin && x1_intercept <= xMax)
            {
                puntos.push_back(PuntoGrafico(x1_intercept, 0));
            }
        }

        // Intersección con eje Y (x1 = 0)
        double x2_intercept = restriccion.valorConstante / restriccion.coeficienteX2;
        if (x2_intercept >= 0 && x2_intercept <= maxY)
        {
            puntos.push_back(PuntoGrafico(0, x2_intercept));
        }
    }
    else if (restriccion.coeficienteX1 != 0)
    {
        // Línea vertical: x1 = c/a
        double x1_val = restriccion.valorConstante / restriccion.coeficienteX1;
        if (x1_val >= xMin && x1_val <= xMax)
        {
            puntos.push_back(PuntoGrafico(x1_val, 0));
            puntos.push_back(PuntoGrafico(x1_val, maxY));
        }
    }

    return puntos;
}

// Encontrar vértices del área factible
vector<PuntoGrafico> VisualizadorGrafico::encontrarVerticesAreaFactible(const vector<Restriccion> &restricciones)
{
    vector<PuntoGrafico> vertices;

    // Agregar punto origen si es factible
    bool origenFactible = true;
    for (const auto &r : restricciones)
    {
        double valor = r.coeficienteX1 * 0 + r.coeficienteX2 * 0;
        if (r.operador == "<=" && valor > r.valorConstante + 1e-6)
            origenFactible = false;
        if (r.operador == ">=" && valor < r.valorConstante - 1e-6)
            origenFactible = false;
    }
    if (origenFactible)
    {
        vertices.push_back(PuntoGrafico(0, 0));
    }

    // Encontrar intersecciones de restricciones
    for (size_t i = 0; i < restricciones.size(); i++)
    {
        for (size_t j = i + 1; j < restricciones.size(); j++)
        {
            if (restricciones[i].operador == "<=" && restricciones[j].operador == "<=")
            {
                // Resolver sistema de ecuaciones
                double a1 = restricciones[i].coeficienteX1, b1 = restricciones[i].coeficienteX2, c1 = restricciones[i].valorConstante;
                double a2 = restricciones[j].coeficienteX1, b2 = restricciones[j].coeficienteX2, c2 = restricciones[j].valorConstante;

                double det = a1 * b2 - a2 * b1;
                if (abs(det) > 1e-10)
                {
                    double x1 = (c1 * b2 - c2 * b1) / det;
                    double x2 = (a1 * c2 - a2 * c1) / det;

                    if (x1 >= -1e-6 && x2 >= -1e-6 && x1 <= maxX + 1e-6 && x2 <= maxY + 1e-6)
                    {
                        // Verificar si el punto satisface todas las restricciones
                        bool factible = true;
                        for (const auto &r : restricciones)
                        {
                            double valor = r.coeficienteX1 * x1 + r.coeficienteX2 * x2;
                            if (r.operador == "<=" && valor > r.valorConstante + 1e-6)
                                factible = false;
                            if (r.operador == ">=" && valor < r.valorConstante - 1e-6)
                                factible = false;
                        }
                        if (factible)
                        {
                            vertices.push_back(PuntoGrafico(max(0.0, x1), max(0.0, x2)));
                        }
                    }
                }
            }
        }
    }

    // Intersecciones con los ejes
    for (const auto &r : restricciones)
    {
        if (r.operador == "<=")
        {
            // Intersección con eje X
            if (r.coeficienteX1 > 0)
            {
                double x1 = r.valorConstante / r.coeficienteX1;
                if (x1 >= 0 && x1 <= maxX)
                {
                    bool factible = true;
                    for (const auto &restriccion : restricciones)
                    {
                        double valor = restriccion.coeficienteX1 * x1;
                        if (restriccion.operador == "<=" && valor > restriccion.valorConstante + 1e-6)
                            factible = false;
                    }
                    if (factible)
                        vertices.push_back(PuntoGrafico(x1, 0));
                }
            }

            // Intersección con eje Y
            if (r.coeficienteX2 > 0)
            {
                double x2 = r.valorConstante / r.coeficienteX2;
                if (x2 >= 0 && x2 <= maxY)
                {
                    bool factible = true;
                    for (const auto &restriccion : restricciones)
                    {
                        double valor = restriccion.coeficienteX2 * x2;
                        if (restriccion.operador == "<=" && valor > restriccion.valorConstante + 1e-6)
                            factible = false;
                    }
                    if (factible)
                        vertices.push_back(PuntoGrafico(0, x2));
                }
            }
        }
    }

    // Eliminar duplicados y ordenar vertices
    sort(vertices.begin(), vertices.end(), [](const PuntoGrafico &a, const PuntoGrafico &b)
         {
        if (abs(a.x - b.x) < 1e-6) return a.y < b.y;
        return a.x < b.x; });

    vertices.erase(unique(vertices.begin(), vertices.end(), [](const PuntoGrafico &a, const PuntoGrafico &b)
                          { return abs(a.x - b.x) < 1e-6 && abs(a.y - b.y) < 1e-6; }),
                   vertices.end());

    return vertices;
}

// Función principal para ejecutar la visualización
void VisualizadorGrafico::ejecutarVisualizacion(const SistemaOptimizacion &sistema)
{
#ifdef SFML_DISPONIBLE
    if (!inicializar())
    {
        cout << "[ERROR] No se pudo inicializar SFML. Visualización no disponible." << endl;
        return;
    }

    configurarEscala(sistema.getRestricciones(), sistema.getSolucion());

    while (ventana.isOpen())
    {
        Event evento;
        while (ventana.pollEvent(evento))
        {
            if (evento.type == Event::Closed)
            {
                ventana.close();
            }
            if (evento.type == Event::KeyPressed)
            {
                if (evento.key.code == Keyboard::Escape)
                {
                    ventana.close();
                }
            }
        }

        ventana.clear(Color::White);

        // Dibujar todos los elementos
        dibujarEjes();
        dibujarAreaFactible(sistema.getRestricciones());
        dibujarRestricciones(sistema.getRestricciones());
        dibujarFuncionObjetivo(sistema.getPrecioMesa(), sistema.getPrecioSilla(),
                               sistema.getSolucion().gananciaMaxima);
        dibujarPuntoOptimo(sistema.getSolucion());
        mostrarLeyenda(sistema.getRestricciones(), sistema.getSolucion());

        // Instrucciones
        Text instrucciones("Presione ESC para cerrar", fuente, 12);
        instrucciones.setFillColor(Color::Black);
        instrucciones.setPosition(10, ALTO_VENTANA - 25);
        ventana.draw(instrucciones);

        ventana.display();
    }
#else
    cout << "[ERROR] SFML no está compilado. Mostrando información de la solución:" << endl;
    cout << "Punto óptimo: (" << sistema.getSolucion().x1 << ", " << sistema.getSolucion().x2 << ")" << endl;
    cout << "Ganancia máxima: $" << sistema.getSolucion().gananciaMaxima << endl;
    cout << "\nPara habilitar la visualización gráfica:" << endl;
    cout << "1. Instale SFML en su sistema" << endl;
    cout << "2. Descomente #define SFML_DISPONIBLE en graficos.cpp" << endl;
    cout << "3. Compile con: g++ ... -lsfml-graphics -lsfml-window -lsfml-system" << endl;
#endif
}

// Función auxiliar para formatear números
string VisualizadorGrafico::formatearNumero(double numero, int decimales)
{
    ostringstream stream;
    stream << fixed << setprecision(decimales) << numero;
    return stream.str();
}

// Función global para mostrar la visualización gráfica
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

    VisualizadorGrafico visualizador;
    visualizador.ejecutarVisualizacion(*this);
}