# Compilador
CC = gcc

CFLAGS = -Wall -fPIC

LDFLAGS = -pthread

# Regla por defecto
all: libclaves.so libproxyclaves.so app-cliente servidor-sock
# Crear librería dinámica
libclaves.so: claves.o
	$(CC) -shared -o $@ $^

# Compilar objeto de la librería
claves.o: claves.c
	$(CC) $(CFLAGS) -c $< -o $@

lines.o: lines.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar objeto de la app
app-cliente.o: app-cliente.c
	$(CC) $(CFLAGS) -c $< -o $@

proxy-sock.o: proxy-sock.c
	$(CC) $(CFLAGS) -c $< -o $@

libproxyclaves.so: proxy-sock.o lines.o
	$(CC) -shared -o $@ $^ -lrt

# Crear aplicación
app-cliente: app-cliente.o libproxyclaves.so
	$(CC) -o $@ app-cliente.o -L. -lproxyclaves -Wl,-rpath,.

servidor-sock: servidor-sock.o lines.o libclaves.so
	$(CC) $(LDFLAGS) -o $@ servidor-sock.o lines.o -L. -lclaves -Wl,-rpath,.

servidor-sock.o: servidor-sock.c
	$(CC) $(CFLAGS) -c $< -o $@




# Limpiar
clean:
	rm -f *.o *.so app-cliente servidor-sock
