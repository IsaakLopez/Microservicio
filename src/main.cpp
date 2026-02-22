#define _WIN32_WINNT 0x0A00
#include "httplib.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace httplib;
using namespace std;

// Estructura de ejemplo para datos
struct Usuario {
    int id;
    string nombre;
    string email;
};

// Base de datos simulada
vector<Usuario> usuarios = {
    {1, "Juan", "juan@example.com"},
    {2, "Maria", "maria@example.com"},
    {3, "Carlos", "carlos@example.com"}
};

// Función para generar JSON simple (sin librería externa)
string generarJSON(const Usuario& u) {
    return "{\"id\":" + to_string(u.id) + 
           ",\"nombre\":\"" + u.nombre + 
           "\",\"email\":\"" + u.email + "\"}";
}

string generarListaJSON() {
    string resultado = "[";
    for (size_t i = 0; i < usuarios.size(); i++) {
        resultado += generarJSON(usuarios[i]);
        if (i < usuarios.size() - 1) resultado += ",";
    }
    resultado += "]";
    return resultado;
}

int main() {
    Server svr;
    
    // 1. Ruta básica GET - Hola Mundo
    svr.Get("/", [](const Request& req, Response& res) {
        res.set_content("<h1>Hola Mundo desde mi microservicio!</h1>", "text/html");
    });
    
    // 2. GET con parámetros en la URL (query string)
    //    Ejemplo: /saludo?nombre=Ana&edad=25
    svr.Get("/saludo", [](const Request& req, Response& res) {
        string nombre = req.get_param_value("nombre");
        string edad = req.get_param_value("edad");
        
        if (nombre.empty()) nombre = "Invitado";
        if (edad.empty()) edad = "desconocida";
        
        string html = "<h1>Hola " + nombre + "!</h1>";
        html += "<p>Tienes " + edad + " años.</p>";
        html += "<p><a href='/'>Volver</a></p>";
        
        res.set_content(html, "text/html");
    });
    
    // 3. GET con parámetros en la ruta (path params)
    //    Ejemplo: /usuarios/2
    svr.Get(R"(/usuarios/(\d+))", [](const Request& req, Response& res) {
        int id = stoi(req.matches[1]);
        
        for (const auto& u : usuarios) {
            if (u.id == id) {
                res.set_content(generarJSON(u), "application/json");
                return;
            }
        }
        
        res.status = 404;
        res.set_content("{\"error\":\"Usuario no encontrado\"}", "application/json");
    });
    
    // 4. GET para listar todos los usuarios
    svr.Get("/usuarios", [](const Request& req, Response& res) {
        res.set_content(generarListaJSON(), "application/json");
    });
    
    // 5. POST para crear un nuevo usuario (recibe JSON)
    svr.Post("/usuarios", [](const Request& req, Response& res) {
        // Parseo manual de JSON simple (para no depender de librerías)
        string body = req.body;
        
        // Buscar campos (versión muy simplificada)
        size_t pos_nombre = body.find("\"nombre\"");
        size_t pos_email = body.find("\"email\"");
        
        if (pos_nombre == string::npos || pos_email == string::npos) {
            res.status = 400;
            res.set_content("{\"error\":\"Formato inválido\"}", "application/json");
            return;
        }
        
        // Extraer valores (simplificado - asume formato específico)
        string nombre, email;
        
        size_t inicio_nombre = body.find(":", pos_nombre) + 1;
        size_t fin_nombre = body.find(",", inicio_nombre);
        nombre = body.substr(inicio_nombre, fin_nombre - inicio_nombre);
        // Limpiar comillas y espacios
        nombre.erase(remove(nombre.begin(), nombre.end(), '"'), nombre.end());
        nombre.erase(remove(nombre.begin(), nombre.end(), ' '), nombre.end());
        
        size_t inicio_email = body.find(":", pos_email) + 1;
        size_t fin_email = body.find("}", inicio_email);
        email = body.substr(inicio_email, fin_email - inicio_email);
        email.erase(remove(email.begin(), email.end(), '"'), email.end());
        email.erase(remove(email.begin(), email.end(), ' '), email.end());
        
        // Crear nuevo usuario
        int nuevoId = usuarios.size() + 1;
        usuarios.push_back({nuevoId, nombre, email});
        
        res.status = 201;
        res.set_content(generarJSON(usuarios.back()), "application/json");
    });
    
    // 6. PUT para actualizar un usuario
    svr.Put(R"(/usuarios/(\d+))", [](const Request& req, Response& res) {
        int id = stoi(req.matches[1]);
        
        for (auto& u : usuarios) {
            if (u.id == id) {
                // Parsear body (simplificado)
                string body = req.body;
                
                size_t pos_nombre = body.find("\"nombre\"");
                if (pos_nombre != string::npos) {
                    size_t inicio = body.find(":", pos_nombre) + 1;
                    size_t fin = body.find(",", inicio);
                    string nombre = body.substr(inicio, fin - inicio);
                    nombre.erase(remove(nombre.begin(), nombre.end(), '"'), nombre.end());
                    nombre.erase(remove(nombre.begin(), nombre.end(), ' '), nombre.end());
                    u.nombre = nombre;
                }
                
                size_t pos_email = body.find("\"email\"");
                if (pos_email != string::npos) {
                    size_t inicio = body.find(":", pos_email) + 1;
                    size_t fin = body.find("}", inicio);
                    string email = body.substr(inicio, fin - inicio);
                    email.erase(remove(email.begin(), email.end(), '"'), email.end());
                    email.erase(remove(email.begin(), email.end(), ' '), email.end());
                    u.email = email;
                }
                
                res.set_content(generarJSON(u), "application/json");
                return;
            }
        }
        
        res.status = 404;
        res.set_content("{\"error\":\"Usuario no encontrado\"}", "application/json");
    });
    
    // 7. DELETE para eliminar un usuario
    svr.Delete(R"(/usuarios/(\d+))", [](const Request& req, Response& res) {
        int id = stoi(req.matches[1]);
        
        for (auto it = usuarios.begin(); it != usuarios.end(); ++it) {
            if (it->id == id) {
                usuarios.erase(it);
                res.set_content("{\"mensaje\":\"Usuario eliminado\"}", "application/json");
                return;
            }
        }
        
        res.status = 404;
        res.set_content("{\"error\":\"Usuario no encontrado\"}", "application/json");
    });
    
    // 8. Ruta para servir un archivo estático (HTML simple)
    svr.Get("/formulario", [](const Request& req, Response& res) {
        string html = R"(
            <!DOCTYPE html>
            <html>
            <head>
                <title>Formulario de Usuario</title>
                <style>
                    body { font-family: Arial; margin: 50px; }
                    input { margin: 10px; padding: 5px; width: 200px; }
                    button { padding: 10px 20px; background: #4CAF50; color: white; border: none; cursor: pointer; }
                </style>
            </head>
            <body>
                <h1>Crear Nuevo Usuario</h1>
                <form id="userForm">
                    <div>Nombre: <input type="text" id="nombre" required></div>
                    <div>Email: <input type="email" id="email" required></div>
                    <button type="submit">Enviar</button>
                </form>
                
                <div id="resultado"></div>
                
                <script>
                    document.getElementById('userForm').addEventListener('submit', async (e) => {
                        e.preventDefault();
                        
                        const usuario = {
                            nombre: document.getElementById('nombre').value,
                            email: document.getElementById('email').value
                        };
                        
                        const response = await fetch('/usuarios', {
                            method: 'POST',
                            headers: {'Content-Type': 'application/json'},
                            body: JSON.stringify(usuario)
                        });
                        
                        const resultado = await response.json();
                        document.getElementById('resultado').innerHTML = 
                            '<h3>Usuario creado:</h3>' + 
                            '<pre>' + JSON.stringify(resultado, null, 2) + '</pre>';
                    });
                </script>
            </body>
            </html>
        )";
        
        res.set_content(html, "text/html");
    });
    
    // 9. Middleware: Logging simple
    svr.set_logger([](const Request& req, const Response& res) {
        cout << "[" << req.method << "] " << req.path 
             << " -> " << res.status << endl;
    });
    
    // 10. Manejo de errores personalizado
    svr.set_error_handler([](const Request& req, Response& res) {
        if (res.status == 404) {
            res.set_content("<h1>404 - Página no encontrada</h1>", "text/html");
        } else if (res.status == 500) {
            res.set_content("<h1>500 - Error interno del servidor</h1>", "text/html");
        }
    });
    
    // 11. Ruta para servir archivos estáticos (si tuvieras una carpeta 'public')
    // svr.set_mount_point("/static", "./public");
    
    cout << "==========================================" << endl;
    cout << "Microservicio de demostración" << endl;
    cout << "==========================================" << endl;
    cout << "Endpoints disponibles:" << endl;
    cout << "  GET  /" << endl;
    cout << "  GET  /saludo?nombre=Ana&edad=25" << endl;
    cout << "  GET  /usuarios" << endl;
    cout << "  GET  /usuarios/{id}" << endl;
    cout << "  POST /usuarios (JSON: {\"nombre\":\"...\",\"email\":\"...\"})" << endl;
    cout << "  PUT  /usuarios/{id}" << endl;
    cout << "  DELETE /usuarios/{id}" << endl;
    cout << "  GET  /formulario (para probar POST desde el navegador)" << endl;
    cout << "==========================================" << endl;
    cout << "Servidor iniciado en http://localhost:8080" << endl;
    cout << "Presiona Ctrl+C para detener" << endl;
    cout << "==========================================" << endl;
    
    svr.listen("localhost", 8080);
    
    return 0;
}