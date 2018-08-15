# R-Tree
Implementación de R-Tree usando una interface web para la visualización de datos.

![](https://raw.githubusercontent.com/AED-RTREE/rtree/master/image.png)

# Instrucciones:

Clonar el repositorio:

```
git clone https://github.com/AED-RTREE/rtree.git
```

Moverse a la carpeta descargada:

```
cd ($DOWNLOAD-PATH)/rtree
```

Abrir 2 terminales: Primero ejecutar el **Server** y luego, en el otro, ejecutar el **App**.

Finalmente abrir un navegador y colocar la siguiente url: http://localhost:5000

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
sudo apt-get instal python2.7 
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
