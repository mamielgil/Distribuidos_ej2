# Compilador
CC = gcc

# Regla por defecto
all: libclaves.so libproxyclaves.so app-cliente app-cliente-2 app-cliente-3 app-cliente-4 app-cliente-5 app-cliente-6 servidor-mq

# Crear librería dinámica
libclaves.so: claves.o
	$(CC) -shared -o $@ $^

# Compilar objeto de la librería
claves.o: claves.c
	$(CC) -Wall -fPIC -c $< -o $@

# Crear aplicación
app-cliente: app-cliente.o
	$(CC) -o $@ $^ -L. -lclaves -Wl,-rpath,.

# Compilar objeto de la app
app-cliente.o: app-cliente.c
	$(CC) -Wall -c $< -o $@

proxy-mq.o: proxy-mq.c
	$(CC) -Wall -fPIC -c $< -o $@

libproxyclaves.so: proxy-mq.o
	$(CC) -shared -o $@ $^ -lrt

servidor-mq: servidor-mq.o
	$(CC) -o $@ $^ -L. -lclaves -Wl,-rpath,.

servidor-mq.o: servidor-mq.c
	$(CC) -Wall -c $< -o $@

app-cliente-2: app-cliente-2.o
	$(CC) -o $@ $^ -L. -lproxyclaves -Wl,-rpath,.

# Compilar objeto de la app
app-cliente-2.o: app-cliente-2.c
	$(CC) -Wall -c $< -o $@

app-cliente-3: app-cliente-3.o
	$(CC) -o $@ $^ -L. -lproxyclaves -Wl,-rpath,.

# Compilar objeto de la app
app-cliente-3.o: app-cliente-3.c
	$(CC) -Wall -c $< -o $@

app-cliente-4: app-cliente-4.o
	$(CC) -o $@ $^ -L. -lproxyclaves -Wl,-rpath,.

# Compilar objeto de la app
app-cliente-4.o: app-cliente-4.c
	$(CC) -Wall -c $< -o $@

app-cliente-5: app-cliente-5.o
	$(CC) -o $@ $^ -L. -lproxyclaves -Wl,-rpath,.

# Compilar objeto de la app
app-cliente-5.o: app-cliente-5.c
	$(CC) -Wall -c $< -o $@

app-cliente-6: app-cliente-6.o
	$(CC) -o $@ $^ -L. -lproxyclaves -Wl,-rpath,.

# Compilar objeto de la app
app-cliente-6.o: app-cliente-6.c
	$(CC) -Wall -c $< -o $@




# Limpiar
clean:
	rm -f *.o *.so app-cliente app-cliente-2 app-cliente-3 app-cliente-4 app-cliente-5 app-cliente-6 servidor-mq
