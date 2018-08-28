# ****************************************************************
# ********************** Autor: Jose Chavez **********************
# ****************************************************************

# Flask 		: 	framework de Python que permite la creacion de la
#					aplicacion web

import os
import socket
import sys
import ast
import random

from flask import Flask
from flask import request
from flask import render_template
from flask import json 

shost, sport = "localhost", 8080

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

app = Flask(__name__)
app.debug = True

random.seed(2)

# Parametros
# ****************************************************************

# count			: 	indica que el se realiza la conexion solo la
# 					primera vez que se abre la pagina

# plgons_arr	:   polygonos 
# points_arr	:	puntos

# MBRs_saved	: 	conjunto de MBRs
# data 			:	un diccionario en donde se almacenan todos los
#					parametros que son devueltos desde el servidor
#					al cliente

count = 0
plgons_arr = []
points_arr = [] 
MBRs_saved = [0]
data = {}

RandomColors = []
Hex = "0123456789ABCDEF"
for i in range(100):
	strColor = "rgba("
	for i in range(3):
		strColor += str(random.randint(0,255)) + ","
	strColor += "0.9)" 
	RandomColors.append(strColor)

def RecvData(tagToSend):
	global s, count

	print('Client(Send) ==>', tagToSend)
	
	s.send(str(tagToSend)+"\0")
	data_recv_str = s.recv(4096)
	print('Client(Recv) <==', data_recv_str)
	
	data_recv = str(data_recv_str).split('\x00')
	print('data_recv : ', data_recv, ", len = ", len(data_recv))

	if len(data_recv) == 2:
		init = data_recv[0].split('|')	
		if init[0]=='SUCCESS':
			print("success!")
			data_str = s.recv(4096)
			data_arr = data_str.split('|')
			print("data_arr :", data_arr)

	elif len(data_recv) == 3:
		init = data_recv[0].split('|')
		if init[0] == 'SUCCESS':
			print('success!')
			del_str = data_recv[1]
			data_arr  = del_str.split('|')
			print("data_arr :", data_arr)

	return data_arr

def resetData():
	global data, plgons_arr, points_arr, MBRs_saved
	plgons_arr = []
	points_arr = [] 
	MBRs_saved = [0]

	mbr = 'NONE|NONE|NONE'
	mbr_ob = str(mbr).split('|')

	data['data_ob'] 		= mbr_ob
	data['plgons_arr']	 	= plgons_arr
	data['points_arr']		= points_arr
	data['MBRs_arr']		= MBRs_saved

	data['get_down_FLAG']	= "false" 		 	
	data['get_radio_FLAG'] 	= "polygon"		
	data['get_draw_FLAG'] 	= "false"		
	data['get_search_FLAG'] = "false"	
	data['get_nearest_FLAG']= "false" 	
	data['get_k_FLAG'] 	  	= "1"

	data['get_polText_FLAG']= "[()]"
	data['get_coorText_FLAG']	= "()"

	data['get_Nrst_pX']		= "-1";
	data['get_Nrst_pY']		= "-1";

	data['get_rSX1']		= "-1";
	data['get_rSX2']		= "-1";
	data['get_rSX3']		= "-1";
	data['get_rSX4']		= "-1";

	data['get_RandomColors']= RandomColors;
@app.route('/')
@app.route('/index')
def hello():
	global plgons_arr, points_arr, data, MBRs_saved, s, count
	if (count == 0):
		s.connect((shost, sport))
	tag_to_send = "DELETE|0|END"
	del_ob = RecvData(tag_to_send)
	if (del_ob [0] == "DELETE"):
		resetData()
		print('DATA(Recv) <== RESET')
	
	count += 1
	return render_template('index.html',result=data)

@app.route('/', methods=['POST'])
def CommFunc():
	global count, plgons_arr, points_arr, data

	tag_to_send	 = request.form['fig']

	reset_FLAG = str(tag_to_send).split('|')[0]
	
	if (reset_FLAG == "DELETE"):
		del_ob = RecvData(tag_to_send)
		if (del_ob [0] == "DELETE"):
			resetData()
			print('DATA(Recv) <== RESET')
			return render_template('index.html', result=data)		
		
	tag_polygons = request.form['polygon']
	tag_points   = request.form['points']
	tag_MBRs	 = request.form['mbrsreq']

	tag_down_FLAG 		= request.form['down_FLAG']
	tag_radio_FLAG		= request.form['radio_FLAG']
	tag_draw_FLAG 		= request.form['draw_FLAG']
	tag_search_FLAG 	= request.form['search_FLAG']
	tag_nearest_FLAG 	= request.form['nearest_FLAG']
	tag_k_FLAG			= request.form['k_FLAG']

	tag_polText_FLAG	= request.form['polText_FLAG']
	tag_coor_FLAG		= request.form['coorText_FLAG']


	tag_Nrst_pX			= request.form['Nrst_pX']
	tag_Nrst_pY			= request.form['Nrst_pY']

	tag_rSX1			= request.form['rSX1']
	tag_rSX2			= request.form['rSX2']
	tag_rSX3			= request.form['rSX3']
	tag_rSX4			= request.form['rSX4']

	mbr_ob = RecvData(tag_to_send)
	x00 = mbr_ob[-1]
	mbr_ob[-1] = x00[:-2]
	print('DATA(Recv) <==', mbr_ob)

	if not (tag_polygons == 'false'): 
		plgons_unit = ast.literal_eval(tag_polygons)
		plgons_arr.append(plgons_unit)

	if not (tag_points == 'false'):
		points_unit = ast.literal_eval(tag_points)
		points_arr += points_unit

	data['data_ob'] 			= mbr_ob
	data['plgons_arr']	 		= plgons_arr
	data['points_arr']			= points_arr

	MBRs_saved 					= ast.literal_eval(tag_MBRs)
	
	data['MBRs_arr']			= MBRs_saved
	data['get_down_FLAG']		= tag_down_FLAG 		
	data['get_radio_FLAG'] 		= tag_radio_FLAG		
	data['get_draw_FLAG'] 		= tag_draw_FLAG 		
	data['get_search_FLAG'] 	= tag_search_FLAG 	
	data['get_nearest_FLAG']	= tag_nearest_FLAG 	
	data['get_k_FLAG'] 	  		= tag_k_FLAG
	data['get_polText_FLAG']	= tag_polText_FLAG
	data['get_coorText_FLAG']	= tag_coor_FLAG
	data['get_Nrst_pX']			= tag_Nrst_pX;
	data['get_Nrst_pY']			= tag_Nrst_pY;
	data['get_rSX1']			= tag_rSX1;
	data['get_rSX2']			= tag_rSX2;
	data['get_rSX3']			= tag_rSX3;
	data['get_rSX4']			= tag_rSX4;

	count += 1
	return render_template('index.html', result=data)

if __name__ == "__main__":
	port = int(os.environ.get('PORT', 5000))
	app.run(host='0.0.0.0', port=port, debug=True)
