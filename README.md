# R-Tree
Implementación de R-Tree usando una interface web para la visualización de datos.

![alt text](https://raw.githubusercontent.com/AED-RTREE/rtree/master/image.png)

# Instrucciones:

El desarrollo de la siguiente aplicación fue realizada usando la distribución de Linux: **Ubuntu 18.04 y 16.04**,
esto es importante porque la libería de socket utilizada **socket.h** se encuentra disponible solo en distribuciones Linux.


Clonar el repositorio:

```
git clone https://github.com/AED-RTREE/rtree.git
```

Moverse a la carpeta descargada:

```
cd ($DOWNLOAD-PATH)/rtree
```

Abrir 2 terminales: Primero ejecutar el **Server** y luego, en el otro, ejecutar el **App** (La ejecución de ambas aplicaciones está explicada líneas abajo).



## R-Tree: Server

Moverse a la carpeta **server**:

```
cd ($DOWNLOAD-PATH)/rtree/server
```

Instalación de dependencias:

```
sudo apt-get update
sudo apt-get install build-essential libssl-dev
```

Compilar:

```
make build
```

Ejecutar:

```
./out/server
```

## R-Tree: App

Moverse a la carpeta **app**:

```
cd ($DOWNLOAD-PATH)/rtree/app
```

Instalar Python (2.7.12):

```
sudo apt-get install python2.7 
```

Instalar Pip:

```
sudo apt-get install python-pip
```

Instalar Flask:

```
pip install flask
```

Ejecutar el archivo **app.py** de la carpeta:

```
python app.py
```

Finalmente abrir un navegador y colocar la siguiente url: http://localhost:5000
