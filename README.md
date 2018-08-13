# R-Tree
Implementación de R-Tree usando una interface web para la visualización de datos.

Clonar el repositorio:

```
git clone https://gitlab.com/AED-RTREE/rtree.git
```

Moverse a la carpeta descargada:

```
cd ($DOWNLOAD-PATH)/rtree
```

Abrir 2 terminales: Primero ejecutar el **Servidor** y en el otro ejecutar el **App**.

# R-Tree: Server

Moverse a la carpeta **server**:

```
cd ($DOWNLOAD-PATH)/rtree/server
```

Instalación de dependencias:

```
sudo apt-get update
sudo apt-get install build-essential libssl-dev
```

Compilar y ejecutar:

```
make exec
```

# R-Tree: App

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

Ejecutar el archivo **app.py** de la carpeta raíz:

```
python app.py
```
