# Compilador
CC = gcc

CFLAGS = -Wall -fPIC

LDFLAGS = -pthread

# Regla por defecto
all: libclaves.so libproxyclaves.so app-cliente-1 servidor-sock app-cliente-2 app-cliente-4 app-cliente-3
# Crear librería dinámica
libclaves.so: claves.o
	$(CC) -shared -o $@ $^

# Compilar objeto de la librería
claves.o: claves.c
	$(CC) $(CFLAGS) -c $< -o $@

lines.o: lines.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar objeto de la app
app-cliente-1.o: app-cliente-1.c
	$(CC) $(CFLAGS) -c $< -o $@

app-cliente-2.o: app-cliente-2.c
	$(CC) $(CFLAGS) -c $< -o $@

app-cliente-3.o: app-cliente-3.c
	$(CC) $(CFLAGS) -c $< -o $@

app-cliente-4.o: app-cliente-4.c
	$(CC) $(CFLAGS) -c $< -o $@

proxy-sock.o: proxy-sock.c
	$(CC) $(CFLAGS) -c $< -o $@

libproxyclaves.so: proxy-sock.o lines.o
	$(CC) -shared -o $@ $^ -lrt

# Crear aplicación
app-cliente-1: app-cliente-1.o libproxyclaves.so
	$(CC) -o $@ app-cliente-1.o -L. -lproxyclaves -Wl,-rpath,.

app-cliente-2: app-cliente-2.o libproxyclaves.so
	$(CC) -o $@ app-cliente-2.o -L. -lproxyclaves -Wl,-rpath,.

app-cliente-3: app-cliente-3.o libproxyclaves.so
	$(CC) -o $@ app-cliente-3.o -L. -lproxyclaves -Wl,-rpath,.

app-cliente-4: app-cliente-4.o libproxyclaves.so
	$(CC) -o $@ app-cliente-4.o -L. -lproxyclaves -Wl,-rpath,.

servidor-sock: servidor-sock.o lines.o libclaves.so
	$(CC) $(LDFLAGS) -o $@ servidor-sock.o lines.o -L. -lclaves -Wl,-rpath,.

servidor-sock.o: servidor-sock.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar
clean:
	rm -f *.o *.so app-cliente-1 servidor-sock app-cliente-4 app-cliente-3 app-cliente-2
