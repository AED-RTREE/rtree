# **********************
# * Autor: Jose Chavez *
# **********************


import os
import socket
import sys
import ast

from flask import Flask
from flask import request
from flask import render_template
from flask import json 

shost, sport = "localhost", 8080

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

app = Flask(__name__)
app.debug = True

count = 0
plgons_arr = []
points_arr = [] 
data = {}
@app.route('/')
@app.route('/index')
def hello():
	global plgons_arr, points_arr, data
	
	mbr = 'NONE|NONE|NONE'
	mbr_ob = str(mbr).split('|')
	#data = {'data_ob': mbr_ob}

	data['data_ob'] 		= mbr_ob
	data['plgons_arr']	 	= plgons_arr
	data['points_arr']		= points_arr
	data['get_down_FLAG']	= "false" 		 	
	data['get_radio_FLAG'] 	= "polygon"		
	data['get_draw_FLAG'] 	= "false"		
	data['get_search_FLAG'] = "false"	
	data['get_nearest_FLAG']= "false" 	
	data['get_k_FLAG'] 	  	= "0"

	data['get_polText_FLAG']= "[()]"
	data['get_coor_FLAG']	= "()"

	return render_template('index.html',result=data)

@app.route('/insert', methods=['POST'])
def clear():
	global count, plgons_arr, points_arr, data

	tag_to_send	 = request.form['fig']
	tag_polygons = request.form['polygon']
	tag_points   = request.form['points']

	tag_down_FLAG 		= request.form['down_FLAG']
	tag_radio_FLAG		= request.form['radio_FLAG']
	tag_draw_FLAG 		= request.form['draw_FLAG']
	tag_search_FLAG 	= request.form['search_FLAG']
	tag_nearest_FLAG 	= request.form['nearest_FLAG']
	tag_k_FLAG			= request.form['k_FLAG']

	tag_polText_FLAG	= request.form['polText_FLAG']
	tag_coor_FLAG		= request.form['coor_FLAG']

	if (count == 0):
		s.connect((shost, sport))

	print('Client(Send) ==>', tag_to_send)
	s.send(str(tag_to_send))
	
	mbr = s.recv(1024)
	print('Client(Recv) <==', mbr)	

	# TEST
	# ====================================================
	# Comentar la linea de abajo para probar con el R-Tree
	# mbr_ob = "MBR|2|2|120|360|520|450|2|360|120|450|420|END".split('|')

	# Quitar el comentario de abajo para probar con el R-Tree
	mbr_ob = str(mbr).split('|')
	
	if not (tag_polygons == 'false'): 
		plgons_unit = ast.literal_eval(tag_polygons)
		plgons_arr.append(plgons_unit)

	if not (tag_points == 'false'):
		#points_arr = [] 
		points_unit = ast.literal_eval(tag_points)
		points_arr += points_unit

	#print("tag_parms:", tag_parms)
	#parms_dic = ast.literal_eval(tag_parms)
	#data = {}
	data['data_ob'] 		= mbr_ob
	data['plgons_arr']	 	= plgons_arr
	data['points_arr']		= points_arr
	data['get_down_FLAG']	= tag_down_FLAG 		
	data['get_radio_FLAG'] 	= tag_radio_FLAG		
	data['get_draw_FLAG'] 	= tag_draw_FLAG 		
	data['get_search_FLAG'] = tag_search_FLAG 	
	data['get_nearest_FLAG']= tag_nearest_FLAG 	
	data['get_k_FLAG'] 	  	= tag_k_FLAG
	data['get_polText_FLAG']= tag_polText_FLAG
	data['get_coor_FLAG']	= tag_coor_FLAG

	#data = {'data_ob': mbr_ob, 'plgons_arr':plgons_arr, 'points_arr': points_arr}#, 'parms_dic': parms_dic}
	count += 1

	return render_template('index.html', result=data)

if __name__ == "__main__":
	port = int(os.environ.get('PORT', 5000))
	app.run(host='0.0.0.0', port=port, debug=True)
