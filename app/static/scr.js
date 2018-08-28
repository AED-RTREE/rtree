// 	****************************************
// 	********** Autor: Jose Chavez **********
// 	****************************************

// 	Set Canvas
// 	******************************************************
/*	En siguiente script contiene las funciones necesarias
	para visualizar los MBR (Minimum Bounding Box), asi
	como tambien la funcion RANGE y K-NEAREST del R-Tree.

	A continuacion se muestran algunos parametros relevantes:

	polygons 	: 	Todos los poligonos que el usuario a dibujado.
	points 		: 	Todos los puntos que el usuario a dibujado.

	npoint 		: 	El punto respectivo para la funcion K-Nearest.

	xcur, ycur	: 	Posicion actual del mouse utilizados en la
					funcion "setNewPosition".

	xmov, ymov	: 	Posicion del mouse en movimiento utilizado para
					delimitar el area de dibujo.
	
	plgon_toSend:   Poligono que se envia por el socket
	point_toSend: 	Punto que se envia por el socket
	
	rectSearch 	: 	Rectangulo de busqueda temporal
*/

var c = document.getElementById("myRec");
var csize = c.getBoundingClientRect();
var mydiv = document.getElementById("myDiv");
var ctx = c.getContext("2d");

ctx.lineWidth=1.5;

ctx.fillStyle = "rgba(170,170,187,0.5)";
ctx.font = "12px Comic Sans MS"

var plgon_toSend  = [];
var points   = [];
var npoint = [];

// *****************************************

// Parameters
// *****************************************

var polygons = [];

if (ob.plgons_arr != null){
	polygons = ob.plgons_arr;
}

var points = [];
if (ob.points_arr != null){
	points = ob.points_arr;
}

var MBRsToPlot=[0];
if (ob.MBRs_arr != null){
	MBRsToPlot = ob.MBRs_arr;
}




var xcur = 0;
var ycur = 0;
var xmov = - csize.left - 1;
var ymov = - csize.top - 1;

var plgon_toSend = [];
var point_toSend;
var rectSearch 	= [];


var data_to_send="";
var data_to_rcv="";
var data_rect="";
var data_knrt="";

var d1 = document.getElementById("div1");
var d2 = document.getElementById("div2");
var d3 = document.getElementById("div3");

//	var poln = 0;

// 	Parameters To Save
// 	******************************************************************

var Prms = {};
Prms.newfig_FLAG 	= false;

// 	Restore Values
// 	******************************************************************
Prms.down_FLAG 		= JSON.parse(ob.get_down_FLAG);
Prms.radio_FLAG 	= ob.get_radio_FLAG;
Prms.draw_FLAG 		= JSON.parse(ob.get_draw_FLAG);
Prms.search_FLAG 	= JSON.parse(ob.get_search_FLAG);
Prms.nearest_FLAG 	= JSON.parse(ob.get_nearest_FLAG);
Prms.k_FLAG 		= JSON.parse(ob.get_k_FLAG);
Prms.polText_FLAG 	= ob.get_polText_FLAG;
Prms.coorText_FLAG 	= ob.get_coorText_FLAG;
Prms.Nrst_pX 		= JSON.parse(ob.get_Nrst_pX);
Prms.Nrst_pY 		= JSON.parse(ob.get_Nrst_pY);

Prms.rSX1			= JSON.parse(ob.get_rSX1);
Prms.rSX2			= JSON.parse(ob.get_rSX2);
Prms.rSX3			= JSON.parse(ob.get_rSX3);
Prms.rSX4			= JSON.parse(ob.get_rSX4);

// To HTML
// ******************************************************************
document.getElementById("poltexid").innerHTML 	= Prms.polText_FLAG;
document.getElementById("mousepoint").innerHTML = Prms.coorText_FLAG;
document.getElementById("myk").value = Prms.k_FLAG;

var knearest_text = "Location : ("+Prms.Nrst_pX+","+Prms.Nrst_pY+") | k = "+Prms.k_FLAG;
document.getElementById("knearst-text").innerHTML = knearest_text;

var text_Rect = "Rect: [("+Prms.rSX1+","+Prms.rSX2+"),("+Prms.rSX3+","+Prms.rSX4+")]";
document.getElementById("rect-search").innerHTML = text_Rect;

// To Radio Buttons
// ******************************************************************
if (Prms.radio_FLAG=="polygon"){
	document.radForm.radioButtons[0].checked=true;
}
else if (Prms.radio_FLAG=="point"){
	document.radForm.radioButtons[1].checked=true;
}

// Display Block
// ******************************************************************
if (Prms.draw_FLAG) {
	d1.style.display = "block";
	d2.style.display = "none";
	d3.style.display = "none";	
}

if (Prms.search_FLAG) {
	d2.style.display = "block";
	d1.style.display = "none";
	d3.style.display = "none";	
}

if (Prms.nearest_FLAG) {
	d3.style.display = "block";	
	d1.style.display = "none";
	d2.style.display = "none";
}


// **********************************

// Restore Figures
// **********************************
draw();
if (Prms.rSX3 != -1 ){
	ctx.strokeStyle="#A5D700";
	ctx.beginPath();
	ctx.rect(Prms.rSX1,Prms.rSX2,Prms.rSX3-Prms.rSX1,Prms.rSX4-Prms.rSX2);
	ctx.stroke();
}

// **********************************



function setrSX_Points(trSX1, trSX2, trSX3, trSX4){
	Prms.rSX1 = trSX1;
	Prms.rSX2 = trSX2;
	Prms.rSX3 = trSX3;
	Prms.rSX4 = trSX4;	
}

function idOf(i) {
	return (i >= 26 ? idOf((i / 26 >> 0) - 1) : '') +  'ABCDEFGHIJKLMNOPQRSTUVWXYZ'[i % 26 >> 0];
}

function PlotMRBs(MBRs_data){
	var num_Levels = MBRs_data[0];
	var PaletColors = ob.get_RandomColors;

	if (num_Levels != 0){
		var iter_MBR = 0;
		iter_MBR += 1;
		for (var iLevel = 0; iLevel < num_Levels; iLevel++) {
			
			var num_MBRs_LUnit = MBRs_data[iter_MBR] ;
			ctx.strokeStyle = PaletColors[iLevel];
			iter_MBR += 2;
			var label = idOf(iLevel);
			ctx.fillStyle = PaletColors[iLevel];
			for (var iMBR_Level = 0; iMBR_Level < num_MBRs_LUnit; iMBR_Level++) {
				ctx.beginPath();
				ctx.rect(MBRs_data[iter_MBR],MBRs_data[iter_MBR+1],MBRs_data[iter_MBR+2]-MBRs_data[iter_MBR], MBRs_data[iter_MBR+3]-MBRs_data[iter_MBR+1]);
				ctx.stroke();
				ctx.fillText(label+(iMBR_Level+1).toString(),parseInt(MBRs_data[iter_MBR])+5,parseInt(MBRs_data[iter_MBR+1])+15); 
				iter_MBR += 5;
			}

			iter_MBR -= 1;
		}
	}
	ctx.fillStyle = "rgba(170,170,187,0.5)";
}


function PlotObjects(OBJs_data){
	var num_OBJs = OBJs_data[0];
	if (num_OBJs != 0){
		var iterSearch = 1;
		var num_Points;

		for (var iobj = 0; iobj < num_OBJs; iobj++) {
			
			ctx.beginPath();
			ctx.fillStyle = "rgba(199,255,0,0.5)";
			num_Points = OBJs_data[iterSearch]*2;
			if (num_Points == 2){
				ctx.strokeStyle="#A5D700";
				ctx.rect(OBJs_data[iterSearch+1]-1.5,OBJs_data[iterSearch+2]-1.5,3.0,3.0);
				if (Prms.nearest_FLAG == true){
					ctx.moveTo(OBJs_data[iterSearch+1],OBJs_data[iterSearch+2]);
					ctx.lineTo(Prms.Nrst_pX,Prms.Nrst_pY);					
				}

				ctx.stroke();
				iterSearch += 3;
			}
			else {
				ctx.strokeStyle="#141419";
				var centerX= parseInt(OBJs_data[iterSearch+1]);
				var centerY= parseInt(OBJs_data[iterSearch+2]);
				ctx.moveTo(OBJs_data[iterSearch+1],OBJs_data[iterSearch+2]);
				for (var iobj_cc = 2; iobj_cc < num_Points - 1; iobj_cc+=2) {
					centerX += parseInt(OBJs_data[iterSearch+1+iobj_cc]);
					centerY += parseInt(OBJs_data[iterSearch+2+iobj_cc]);
					ctx.lineTo(OBJs_data[iterSearch+1+iobj_cc],OBJs_data[iterSearch+2+iobj_cc]);
				}
				ctx.lineTo(OBJs_data[iterSearch+1],OBJs_data[iterSearch+2]);
				ctx.fill();
				if (Prms.nearest_FLAG == true){
					ctx.strokeStyle="#A5D700";
					ctx.moveTo(centerX/(num_Points/2),centerY/(num_Points/2));
					ctx.lineTo(Prms.Nrst_pX,Prms.Nrst_pY);					
				}

				ctx.stroke();
				iterSearch += num_Points + 1;			
			}
		}	
	}
	PlotMRBs(MBRsToPlot);
}

function RecvToPlot(DATA){
	switch (DATA[0]){
		case "MBRS":
			console.log("INSERT executed!");
			MBRsToPlot = DATA.slice(1,DATA.length-1);
			PlotMRBs(MBRsToPlot);
			break;
		case "OBJECTS":
			console.log("OBJECT executed!");
			var OBJsToPlot = DATA.slice(1,DATA.length-1);
			PlotObjects(OBJsToPlot);	
			break;
		case "NONE":
			console.log("Initialized!");
			break;
		default:
			console.log("Invalid Commnad!");
	}
}


RecvToPlot(array_recv);

function PostFn(postVal){
	var form = document.getElementById("post-id");
//	Dato para enviar por el Socket
	var hField_ToInsert = document.createElement("input");
	hField_ToInsert.setAttribute("type", "hidden");
	hField_ToInsert.setAttribute("name", 'fig');
	hField_ToInsert.setAttribute("value", postVal);

//	Guardar Poligonos
	var hField_Polygon = document.createElement("input");
	hField_Polygon.setAttribute("type", "hidden");	
	hField_Polygon.setAttribute("name", 'polygon');
	if (plgon_toSend.length == 0){
		var nullval = [[false]];
		hField_Polygon.setAttribute("value", nullval);
	}
	else{
		hField_Polygon.setAttribute("value", plgon_toSend);
	}

//	Guardar Puntos	
	var hField_Point = document.createElement("input");
	hField_Point.setAttribute("type", "hidden");	
	hField_Point.setAttribute("name", 'points');

	if (point_toSend === undefined){
		var nullval = [[false]];
		hField_Point.setAttribute("value", nullval);
	}
	else {
		hField_Point.setAttribute("value", point_toSend);
	}

//	Guardar variables generales
	var hField_MBRs = document.createElement("input");
	hField_MBRs.setAttribute("type", "hidden");
	hField_MBRs.setAttribute("name", "mbrsreq");
	hField_MBRs.setAttribute("value", MBRsToPlot);

	for (var key in Prms) {
		var hField_Parm = document.createElement("input");
		hField_Parm.setAttribute("type", "hidden");
		hField_Parm.setAttribute("name", key.toString());
		hField_Parm.setAttribute("value", Prms[key]);
		form.appendChild(hField_Parm);
	}


	form.appendChild(hField_ToInsert);
	form.appendChild(hField_Polygon);
	form.appendChild(hField_Point);
	form.appendChild(hField_MBRs);

	document.body.appendChild(form);
	form.submit();
}

function send_fig(fig_array){
	aux = "INSERT|";
	aux += fig_array.length/2 +"|";
	for (var ifig = 0; ifig< fig_array.length; ifig ++) {
		aux += fig_array[ifig] + "|";
	}
	aux += "END";
	PostFn(aux);
}

function send_rect(var1,var2,var3,var4){
	data_rect = "RANGE|2|"+var1+"|"+var2+"|"+var3+"|"+var4+"|END";
	PostFn(data_rect);
}

function send_knearest(pt1,pt2,kval){
	console.log("send_knearest");
	data_knrt='NEAREST|'+kval+'|1|'+pt1+'|'+pt2+'|END';
	PostFn(data_knrt);
}

function draw(){
	ctx.clearRect(0,0,854,480);
	ctx.fillStyle = "rgba(170,170,187,0.5)";

	if (polygons.length > 0){
		ctx.strokeStyle="#141419";
		for (var poli = 0 ; poli < polygons.length ; poli++) {
			var plot = polygons[poli];
			ctx.beginPath();
			if (plot.length == 2){
				ctx.rect(plot[0],plot[1],1.8,1.8);
				ctx.stroke();
				//ctx.arc(plot[0],plot[1], 50, 0, Math.PI * 2, true);
				//ctx.fill();
				//ctx.stroke();
			}
			else{
				ctx.moveTo(plot[0], plot[1]);
				for (var i = 2; i < plot.length - 1; i+=2) {
					ctx.lineTo(plot[i],plot[i+1]);
				}
				ctx.fill();
				ctx.stroke();
			}
		}
	}

	if (points.length > 0){
		ctx.strokeStyle="#141419";
		//ctx.fillStyle ="#141419";

		ctx.beginPath();
		for (var pi = 0; pi < points.length - 1; pi+=2) {
			//ctx.arc(points[0],points[1], 50, 0, Math.PI * 2, true);
			//ctx.fill();
			ctx.rect(points[pi]-1.5,points[pi+1]-1.5,3.0,3.0);
			ctx.stroke();
		}
	}

	if (Prms.nearest_FLAG == true && Prms.Nrst_pX >= 0 && Prms.Nrst_pY >= 0){
		ctx.strokeStyle="#A5D700";
		ctx.beginPath();
		ctx.rect(Prms.Nrst_pX-1,Prms.Nrst_pY-1,2,2);

		ctx.strokeStyle="#141419";
		ctx.rect(Prms.Nrst_pX-0.5,Prms.Nrst_pY-0.5,1,1);
		ctx.stroke();
		var knearest_text = "location : ("+Prms.Nrst_pX+","+Prms.Nrst_pY+") | k = "+Prms.k_FLAG;
		document.getElementById("knearst-text").innerHTML = knearest_text;
	}
	PlotRectSearch(rectSearch);
}

function PlotRectSearch(Rect_temp){
	if (Rect_temp[0] == 1 && Prms.search_FLAG == true && Rect_temp.length>0){
		ctx.strokeStyle="#A5D700";
		ctx.beginPath();
		setrSX_Points(Rect_temp[1],Rect_temp[2],Rect_temp[3],Rect_temp[4]);
		
		if (Rect_temp[1] > Rect_temp[3]){
			Prms.rSX3 = Rect_temp[1];
			Prms.rSX1 = Rect_temp[3];
		}
		if (Rect_temp[2] > Rect_temp[4]){
			Prms.rSX4 = Rect_temp[2];
			Prms.rSX2 = Rect_temp[4];
		}

		var text_Rect = "Rect: [("+Prms.rSX1+","+Prms.rSX2+"),("+Prms.rSX3+","+Prms.rSX4+")]";
		document.getElementById("rect-search").innerHTML = text_Rect;
		ctx.rect(Prms.rSX1,Prms.rSX2,Prms.rSX3-Prms.rSX1,Prms.rSX4-Prms.rSX2);
		ctx.stroke()
	}	
}
function ClearFn(){
	ctx.clearRect(0,0,854,480);
	var ClearDataToSend = "DELETE|0|END";
	PostFn(ClearDataToSend);
}

function isInt(num){
	if (isNaN(num)){
		return false;
	}
	var rval = parseFloat(num);
	return (rval | 0) === rval;
}

function fnNeasrest(){
	ClosePolygon();
	var temp = document.getElementById("myk").value;
	if (isInt(temp)){
		temp = parseInt(temp, 10);
		if (temp>=0){
			Prms.k_FLAG = temp;
			if (Prms.Nrst_pX != -1 && Prms.Nrst_pY != -1){
				send_knearest(Prms.Nrst_pX, Prms.Nrst_pY, Prms.k_FLAG);
			}	
		}
		else{
			Prms.k_FLAG = "invalid number!";
		}
	}
	else{
		Prms.k_FLAG = "we need a integer number! ";
	}
	var knearest_text = "location : ("+Prms.Nrst_pX+","+Prms.Nrst_pY+") | k = "+Prms.k_FLAG;
	document.getElementById("knearst-text").innerHTML = knearest_text;
	draw();
	PlotMRBs(MBRsToPlot);
}

function ClosePolygon(){
	if (polygons.length > 0){
		var last = polygons.length-1;
		var lastpol = polygons[last];
		if (lastpol.length > 2){
			if (lastpol[0]!= lastpol[lastpol.length-2] || lastpol[1]!=lastpol[lastpol.length-1]){
				lastpol = lastpol.concat([lastpol[0],lastpol[1]]);
				polygons[last] = lastpol;
				plgon_toSend = [];
			}
		}
		else{
			polygons[last] = [];
			plgon_toSend = [];
		}
	}
}

function radfn(myradio){
	Prms.radio_FLAG = myradio.value;
	if (Prms.radio_FLAG == "point"){
		ClosePolygon();
	}
}

function setNewPosition(event){
	ctx.clearRect(0,0,854,480);
	if (Prms.draw_FLAG == true){
		rectSearch[0] = 0;
		xcur = event.clientX - csize.left;
		ycur = event.clientY - csize.top;
		if (Prms.radio_FLAG == "point"){
			if (xcur < 854 && xcur >= 0 && ycur < 480 && ycur >= 0){
				points = points.concat([xcur, ycur]);
				Prms.newfig_FLAG = false;
				Prms.polText_FLAG = "[("+ xcur +"," + ycur + ")]";
				document.getElementById("poltexid").innerHTML = Prms.polText_FLAG;
				point_toSend = [xcur, ycur];
				send_fig([xcur, ycur]);
			}
		}

		else if (Prms.radio_FLAG == "polygon"){
			if (xcur < 854 && xcur >= 0 && ycur < 480 && ycur >= 0){
				if (plgon_toSend.length > 4) {
					if (Math.pow(plgon_toSend[0]-xcur,2)+Math.pow(plgon_toSend[1]-ycur,2)<100){
						xcur = plgon_toSend[0];
						ycur = plgon_toSend[1];
						Prms.newfig_FLAG = true;
					}
				}

				plgon_toSend = plgon_toSend.concat([xcur, ycur]);
				var lenpoly = polygons.length;
				polygons[lenpoly] = plgon_toSend;
				Prms.polText_FLAG = "[";
				for (var ipt = 0; ipt < plgon_toSend.length-1; ipt+=2) {
					Prms.polText_FLAG += "("+ plgon_toSend[ipt] +"," + plgon_toSend[ipt+1] + ")";			
				}
				Prms.polText_FLAG += "]";
				document.getElementById("poltexid").innerHTML = Prms.polText_FLAG
				//xini = xcur;
				//yini = ycur;

				if (Prms.newfig_FLAG == true){
					var polygon_to_send = plgon_toSend.slice(0, plgon_toSend.length - 2);
					Prms.newfig_FLAG = false;
					send_fig(polygon_to_send);
					plgon_toSend = [];

				}
			}
		}
	}
	draw();
	PlotMRBs(MBRsToPlot);
}

function fndiv1(){
	d1 = document.getElementById("div1");
	d2 = document.getElementById("div2");
	d3 = document.getElementById("div3");
	if (d1.style.display == "none"){
		d1.style.display = "block";
		d2.style.display = "none";
		d3.style.display = "none";
		Prms.draw_FLAG = true;
		Prms.search_FLAG = false;
		Prms.nearest_FLAG = false;
		setrSX_Points(-1,-1,-1,-1);
	}
	else {
		d1.style.display = "none";
		Prms.draw_FLAG = false;
	}
}

function fndiv2(){
	d1 = document.getElementById("div1");
	d2 = document.getElementById("div2");
	d3 = document.getElementById("div3");
	if (d2.style.display == "none"){
		d1.style.display = "none";
		d2.style.display = "block";
		d3.style.display = "none";
		
		Prms.search_FLAG = true;
		Prms.draw_FLAG = false;
		Prms.nearest_FLAG = false;
		ClosePolygon();
	}
	else {
		d2.style.display = "none";
		Prms.search_FLAG = false;
	}
}

function fndiv3(){
	d1 = document.getElementById("div1");
	d2 = document.getElementById("div2");
	d3 = document.getElementById("div3");
	if (d3.style.display == "none"){
		d1.style.display = "none";
		d2.style.display = "none";
		d3.style.display = "block";

		Prms.draw_FLAG = false;
		Prms.search_FLAG = false;
		Prms.nearest_FLAG = true;
		setrSX_Points(-1,-1,-1,-1);
	}
	else {
		d3.style.display = "none";
		Prms.nearest_FLAG = false;
	}
}

function MouseMoveFn(event){
	xmov = event.clientX - csize.left;
	ymov = event.clientY - csize.top;

	if (Prms.search_FLAG == true){
		if (xmov < 854 && xmov >= 0 && ymov < 480 && ymov >= 0){
			if (Prms.down_FLAG == true){
				rectSearch[3] = xmov;
				rectSearch[4] = ymov;
				draw();
				PlotMRBs(MBRsToPlot);
			}
		}
	}

	if (xmov < 854 && xmov >= 0 && ymov < 480 && ymov >= 0){
		Prms.coorText_FLAG = "mouse : (" + xmov + "," + ymov + ")";
		document.getElementById("mousepoint").innerHTML = Prms.coorText_FLAG;
	}
}

function MouseDownFn(event){
	xdown = event.clientX - csize.left;
	ydown = event.clientY - csize.top;

	Prms.down_FLAG = true;
	if (Prms.search_FLAG == true){
		if (xdown < 854 && xdown >= 0 && ydown < 480 && ydown >= 0){
			rectSearch[0] = 1;
			rectSearch[1] = xdown;
			rectSearch[2] = ydown;
			rectSearch[3] = xdown;
			rectSearch[4] = ydown;
		}
	}
	if (Prms.nearest_FLAG == true ){
		if (xdown < 854 && xdown >= 0 && ydown < 480 && ydown >= 0){
			Prms.Nrst_pX=xdown;
			Prms.Nrst_pY=ydown;
			fnNeasrest();
			send_knearest(Prms.Nrst_pX,Prms.Nrst_pY,Prms.k_FLAG);
		}
	}
	setNewPosition(event);
}

function MouseUpFn(event){
	Prms.down_FLAG = false;
	if (Prms.search_FLAG == true){
		xup = event.clientX - csize.left;
		yup = event.clientY - csize.top;
		if (xup < 854 && xup >= 0 && yup < 480 && yup >= 0){
			rectSearch[3] = xup;
			rectSearch[4] = yup;
			setrSX_Points(rectSearch[1],rectSearch[2],rectSearch[3],rectSearch[4]);
			if (rectSearch[1] > rectSearch[3]){
				Prms.rSX3 = rectSearch[1];
				Prms.rSX1 = rectSearch[3];
			}
			if (rectSearch[2] > rectSearch[4]){
				Prms.rSX4 = rectSearch[2];
				Prms.rSX2 = rectSearch[4];
			}
			send_rect(Prms.rSX1,Prms.rSX2,Prms.rSX3,Prms.rSX4);
		}
	}
}

document.addEventListener("mousedown", MouseDownFn);
document.addEventListener("mouseup", MouseUpFn);
document.addEventListener("mousemove", MouseMoveFn);