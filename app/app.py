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
MBRs_saved = [0]
data = {}

def resetData():
	global data, plgons_arr, points_arr
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
	data['get_k_FLAG'] 	  	= "0"

	data['get_polText_FLAG']= "[()]"
	data['get_coorText_FLAG']	= "()"

	data['get_Nrst_pX']		= "-1";
	data['get_Nrst_pY']		= "-1";

	data['get_rSX1']		= "-1";
	data['get_rSX2']		= "-1";
	data['get_rSX3']		= "-1";
	data['get_rSX4']		= "-1";


@app.route('/')
@app.route('/index')
def hello():
	global plgons_arr, points_arr, data, MBRs_saved
	resetData()

	return render_template('index.html',result=data)

@app.route('/', methods=['POST'])
def CommFunc():
	global count, plgons_arr, points_arr, data

	tag_to_send	 = request.form['fig']

	reset_FLAG = str(tag_to_send).split('|')[0]
	
	if (reset_FLAG == "DELETE"):
		print('Client(Send) ==>', tag_to_send)
		s.send(str(tag_to_send)+"\0")
		bar = s.recv(1024)
		print('Client(Recv) <==', bar)
		

		bar_ob = str(bar).split('\x00')
		print('BAR_OB', bar_ob, ", len = ", len(bar_ob))

		if len(bar_ob) == 2:
			print("if == 2")
			init = bar_ob[0].split('|')	
			if init[0]=='SUCCESS':
				print("success!")
				del_str=s.recv(1024)
				del_ob = del_str.split('|')

				if (del_ob[0] == "DELETE"):
					resetData()
					print('DATA(Recv) <== RESET')
					return render_template('index.html', result=data)

		elif len(bar_ob) == 3:
			print("if == 3")
			init = bar_ob[0].split('|')
			if init[0] == 'SUCCESS':
				print('success!')
				del_str = bar_ob[1]
				del_ob  = del_str.split('|')
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

	if (count == 0):
		s.connect((shost, sport))

	print('Client(Send) ==>', tag_to_send)
	s.send(str(tag_to_send)+"\0")
	
	bar = s.recv(1024)
	print('Client(Recv) <==', bar)	
	bar_ob = str(bar).split('\x00')
	print('BAR_OB', bar_ob, ", len = ", len(bar_ob))
	if len(bar_ob) == 2:
		print("if == 2")
		init = bar_ob[0].split('|')	
		if init[0]=='SUCCESS':
			print("success!")
			mbrs=s.recv(1024)
			mbr_ob = mbrs.split('|')
			x00 = mbr_ob[-1]
			mbr_ob[-1] = x00[:-2]
			print('DATA(Recv) <==', mbr_ob)

	if len(bar_ob) == 3:
		print("if == 3")
		init = bar_ob[0].split('|')
		if init[0] == 'SUCCESS':
			print('success!')
			mbrs = bar_ob[1]
			mbr_ob = mbrs.split('|')
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
