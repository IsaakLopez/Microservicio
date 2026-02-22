# Microservicio en C++ con cpp-httplib

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Render](https://img.shields.io/badge/Deployed%20on-Render-46E3B7.svg)](https://render.com)

Un microservicio RESTful desarrollado en C++ utilizando la biblioteca cpp-httplib. Implementa operaciones CRUD básicas para gestión de usuarios con almacenamiento en memoria.

## Características

- **API RESTful** con endpoints para operaciones CRUD
- **Almacenamiento en memoria** (vector de objetos)
- **Formato JSON** para intercambio de datos
- **Logging** de peticiones en consola
- **Manejo de errores** personalizado (404, 500)
- **Interfaz web** simple para pruebas
- **Despliegue en contenedor Docker**

## 🛠️ Tecnologías utilizadas

- **C++17** - Estándar del lenguaje
- **cpp-httplib** - Biblioteca HTTP header-only
- **Docker** - Contenedorización
- **TDM-GCC** - Compilador (desarrollo local)
- **GCC:13** - Compilador

## 🚀 Endpoints disponibles

| Método | Ruta | Descripción |
|--------|------|-------------|
| GET | `/` | Página de bienvenida |
| GET | `/saludo?nombre=Ana&edad=25` | Saludo personalizado con parámetros |
| GET | `/usuarios` | Lista todos los usuarios |
| GET | `/usuarios/{id}` | Obtiene un usuario por ID |
| POST | `/usuarios` | Crea un nuevo usuario |
| PUT | `/usuarios/{id}` | Actualiza un usuario existente |
| DELETE | `/usuarios/{id}` | Elimina un usuario |
| GET | `/formulario` | Formulario HTML para pruebas |
 
## 🔧 Instalación y ejecución local

### Prerrequisitos

- **TDM-GCC 10.3.0** o superior (en Windows)
- **Git** (opcional)
- **Docker** (opcional, para contenedor)

### Compilación manual

```bash
# Clonar el repositorio
git clone https://github.com/IsaakLopez/Microservicio.git
cd microservicio-cpp

# Compilar
g++ -std=c++17 -D_WIN32_WINNT=0x0A00 src/main.cpp -o server.exe -lws2_32

# Ejecutar
./server.exe
```

### Usando Docker
```bash

# Construir la imagen
docker build -t microservicio-cpp .

# Ejecutar el contenedor
docker run -p 8080:8080 microservicio-cpp
