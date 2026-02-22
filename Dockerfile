# 1. Usa una imagen base con GCC 13 listo
FROM gcc:13

# 2. Define la carpeta de trabajo dentro del contenedor
WORKDIR /app

# 3. Copia todo el contenido de src/ al contenedor
COPY src/ .

# 4. Compila el C++ dentro del contenedor
#    httplib.h está en el mismo directorio que main.cpp
RUN g++ -std=c++17 main.cpp -o server -pthread

# 5. Declara y expone el puerto 8080
ENV PORT=8080
EXPOSE 8080

# 6. Ejecuta el programa compilado cuando el contenedor inicial
CMD ["./server"]
