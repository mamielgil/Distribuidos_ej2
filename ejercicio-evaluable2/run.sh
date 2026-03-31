#! /bin/bash

if [ -d "clientes" ]; then
    rm -rf clientes/*
fi

make
env IP_TUPLAS=localhost PORT_TUPLAS=3000 ./app-cliente-1 > test_funcionalidad.txt
