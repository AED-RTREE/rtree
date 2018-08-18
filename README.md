# R-Tree
Implementación de R-Tree usando una interface web para la visualización de datos.

![alt text](https://raw.githubusercontent.com/AED-RTREE/rtree/master/image.png)

# Instrucciones:

El desarrollo de la siguiente aplicación fue realizada usando la distribución de Mac High Sierra 10.13

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
brew update
brew install build-essential libssl-dev
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
Instalar el framework de Python necesario para ejecutar la aplicacion web.

Instalar Flask:

```
sudo easy_install flask
```

Ejecutar el archivo **app.py** de la carpeta:

```
python app.py
```

Finalmente abrir un navegador y colocar la siguiente url: http://localhost:5000
