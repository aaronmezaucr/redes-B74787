# Programa de subtneteo

Este programa permite al usuario subtenear una red en varios conjuntos, de modo que a cada conjunto se le asigna una cantidad de direcciones IP para cumplir con la cantidad de hosts requeridos, siempre guardando una dirección para la red y una para el broadcast. Para esto el usuario puede decidir si quiere asignar las IPs e manera ascendente (IP más baja con el conjunto más grande) o descendente (IP más alta con el conjunto más grande).

## Instrucciones de compilación y ejecución

### Compilación

Para compilar el programa, se debe abrir la carpeta Tarea_Programada en terminal. Posteriromente se deben ejecutar los comandos:

    make clean
    make

### Ejecución

Para ejecutar el programa, se debe ejecutar con 2 parametros, siendo el primero el nombre del archivo con la red que se desea subnetear y el segundo el con el nombre del archivo con las solicitudes. En la carpeta test se brindan archivos de prueba.

El comando que se debe ejecutar para subneteo ascendente (IP más baja con el conjunto más grande) es:

    ./bin/subnetting tests/baseNetwork.txt tests/requests.txt ascending

Y para subneteo descendente (IP más alta con el conjunto más grande):

    ./bin/subnetting tests/baseNetwork.txt tests/requests.txt descending


### Ejemplo de la salida
- Archivo con red base: baseNetwork.txt
- Archivo con solicitudes: requests.txt

![alt text](images/image.png)