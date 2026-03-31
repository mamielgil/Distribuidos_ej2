#! /bin/bash
make


# Borramos todas las claves generadas para ejecutar los tests desde 0
if [ -d "clientes" ]; then
    rm -rf clientes/*
fi

# Exportamos las variables de entorno requeridas
export PORT_TUPLAS=3000
export IP_TUPLAS="localhost"

./app-cliente-2 > test_escritor.txt &

./app-cliente-4 > test_destructor.txt &

./app-cliente-3 > test_lector.txt &

make clean
