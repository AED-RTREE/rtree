// **********************
// * Autor: José Chavez *
// **********************

// Set Canvas
// *****************************************

var c = document.getElementById("myRec");
var csize = c.getBoundingClientRect();
var mydiv = document.getElementById("myDiv");
var ctx = c.getContext("2d");

ctx.lineWidth=1.5;

ctx.fillStyle = "rgba(170,170,187,0.5)";
ctx.font = "10px Arial";

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

var polygon = [];
var point_toSend;
var npoint = [];
var recsrc = [];
var old_recsrc;
var r1;
var r2;
var r3;
var r4;

var data_to_send="";
var data_to_rcv="";
var data_rect="";
var data_knrt="";
var xmov = - csize.left - 1;
var ymov = - csize.top - 1;

var xini = 0;
var yini = 0;
var xcur = 0;
var ycur = 0;

var d1 = document.getElementById("div1");
var d2 = document.getElementById("div2");
var d3 = document.getElementById("div3");

//var poln = 0;

// Parameters To Save
// *****************************************

var Prms = {};
Prms.newfig_FLAG = false;

// Restore Values
// ******************************************************************
Prms.down_FLAG 		= JSON.parse(ob.get_down_FLAG);
Prms.radio_FLAG 	= ob.get_radio_FLAG;
Prms.draw_FLAG 		= JSON.parse(ob.get_draw_FLAG);
Prms.search_FLAG 	= JSON.parse(ob.get_search_FLAG);
Prms.nearest_FLAG 	= JSON.parse(ob.get_nearest_FLAG);
Prms.k_FLAG 		= JSON.parse(ob.get_k_FLAG);
Prms.polText_FLAG 	= ob.get_polText_FLAG;
Prms.coor_FLAG 		= ob.get_coor_FLAG;


document.getElementById("poltexid").innerHTML 	= Prms.polText_FLAG;
document.getElementById("mousepoint").innerHTML = Prms.coor_FLAG;

if (Prms.radio_FLAG=="polygon"){
	document.radForm.radioButtons[0].checked=true;
}
else if (Prms.radio_FLAG=="point"){
	document.radForm.radioButtons[1].checked=true;
}

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
// **********************************


function PlotMRBs(MBRs_data){
	var num_MBRs = MBRs_data[0];
	var iter = 2;
	var num_Points;
	ctx.strokeStyle="blue";
	for (var iMBR = 0; iMBR < num_MBRs; iMBR++) {
		num_Points = MBRs_data[iter];
		ctx.beginPath();
		console.log([MBRs_data[iter],MBRs_data[iter+1],MBRs_data[iter+2], MBRs_data[iter+3]]);
		ctx.rect(MBRs_data[iter],MBRs_data[iter+1],MBRs_data[iter+2]-MBRs_data[iter], MBRs_data[iter+3]-MBRs_data[iter+1]);
		ctx.stroke();
		iter += 5;
	}

}
function RecvToPlot(DATA){
	console.log(DATA[0]);
	switch (DATA[0]){
		
		case "MBR":
			console.log("MBR Executed!");
			PlotMRBs(DATA.slice(1,DATA.length-1));
			break;
		default:
			console.log("Invalid Commnad!");
	}
}

RecvToPlot(array_recv);


function post(fn,value){
	var form = document.getElementById("post-id");

	var hf_toSend = document.createElement("input");
	hf_toSend.setAttribute("type", "hidden");
	data = fn + value;
	hf_toSend.setAttribute("name", 'fig');
	hf_toSend.setAttribute("value", data);

	var hf2 = document.createElement("input");
	hf2.setAttribute("type", "hidden");	
	hf2.setAttribute("name", 'polygon');
	if (polygon.length == 0){
		var nullval = [[false]];
		hf2.setAttribute("value", nullval);
	}
	else{
		hf2.setAttribute("value", polygon);
	}
	
	var hf3 = document.createElement("input");
	hf3.setAttribute("type", "hidden");	
	hf3.setAttribute("name", 'points');

	if (point_toSend === undefined){
		var nullval = [[false]];
		hf3.setAttribute("value", nullval);
	}
	else {
		hf3.setAttribute("value", point_toSend);
	}
	
	for (var key in Prms) {
		var hf_parms = document.createElement("input");
		hf_parms.setAttribute("type", "hidden");
		console.log(key);
		hf_parms.setAttribute("name", key.toString());
		hf_parms.setAttribute("value", Prms[key]);
		form.appendChild(hf_parms);
	}


	form.appendChild(hf_toSend);
	form.appendChild(hf2);
	form.appendChild(hf3);
	

	document.body.appendChild(form);
	form.submit();
}

function send_fig(fig_array){
	aux = "|";
	aux += fig_array.length/2 +"|";
	for (var ifig = 0; ifig< fig_array.length; ifig ++) {
		aux += fig_array[ifig] + "|";
	}
	aux += "END";
	post('INSERT',aux);
}

function send_rect(var1,var2,var3,var4){
	data_rect = "RANGE|"+var1+"|"+var2+"|"+var3+"|"+var4+"|END";
}

function send_knearest(pt1,pt2,kval){
	data_knrt='NEAREST|'+kval+'|'+pt1+'|'+pt2+'|END';
}

function draw(){
	ctx.clearRect(0,0,854,480);
	if (polygons.length > 0){
		ctx.strokeStyle="red";
		for (var poli = 0 ; poli < polygons.length ; poli++) {
			var plot = polygons[poli];
			ctx.beginPath();
			if (plot.length == 2){
				ctx.rect(plot[0],plot[1],1.3,1.3);
				ctx.stroke();
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
		ctx.strokeStyle="red";
		ctx.beginPath();
		for (var pi = 0; pi < points.length - 1; pi+=2) {
			ctx.rect(points[pi],points[pi+1],1.3,1.3);
			ctx.stroke();
		}
	}

	if (Prms.nearest_FLAG == true && npoint.length > 0){
		ctx.strokeStyle="black";
		ctx.beginPath();
		ctx.rect(npoint[0],npoint[1],1.3,1.3);
		ctx.stroke();
		if (Prms.down_FLAG == true){
			if(isInt(Prms.k_FLAG)){
				if(Prms.k_FLAG>=0){
					send_knearest(npoint[0],npoint[1],Prms.k_FLAG);
				}
			}
		}
		var knearest_text = "location : ("+npoint[0]+","+npoint[1]+") | k = "+Prms.k_FLAG;
		document.getElementById("knearst-text").innerHTML = knearest_text;
	}

	if (recsrc[0] == 1 && Prms.search_FLAG == true && recsrc.length>0){
		ctx.strokeStyle="#01DF74";
		ctx.beginPath();
		r1 = recsrc[1];
		r2 = recsrc[2];
		r3 = recsrc[3];
		r4 = recsrc[4];
		if (recsrc[1] > recsrc[3]){
			r3 = recsrc[1];
			r1 = recsrc[3];
		}
		if (recsrc[2] > recsrc[4]){
			r4 = recsrc[2];
			r2 = recsrc[4];
		}

		if (Prms.down_FLAG == true){
			send_rect(r1,r2,r3,r4);
		}

		var text_Rect = "rect: [("+r1+","+r2+"),("+r3+","+r4+")]";
		document.getElementById("rect-search").innerHTML = text_Rect;
		ctx.rect(r1,r2,r3-r1,r4-r2);
		ctx.stroke()
	}

}

function clearfn(){
	polygons = [0];
	polygon = [];
	points = [];
	Prms.newfig_FLAG = true;
	//poln = 0;
	npoint = [];
	rectscr = [];
	rectscr[0] = 0;
	send_clear_data = "DELETE|0|END";
	ctx.clearRect(0,0,854,480);
}

function isInt(num){
	if (isNaN(num)){
		return false;
	}
	var rval = parseFloat(num);
	return (rval | 0) === rval;
}

function fnSearch(myk){
	closepolygon();
	var temp = document.getElementById("myk").value;
	if (isInt(temp)){
		if (temp>=0){
			Prms.k_FLAG = temp;
			send_knearest(npoint[0], npoint[1], Prms.k_FLAG);
		}
		else{
			Prms.k_FLAG = "invalid number!";
		}
	}
	else{
		Prms.k_FLAG = "we need a integer number! ";
	}
	draw();
}

function closepolygon(){
	if (polygons.length > 0){
		var last = polygons.length-1;
		var lastpol = polygons[last];
		if (lastpol.length > 2){
			if (lastpol[0]!= lastpol[lastpol.length-2] || lastpol[1]!=lastpol[lastpol.length-1]){
				lastpol = lastpol.concat([lastpol[0],lastpol[1]]);
				polygons[last] = lastpol;
				polygon = [];
				//poln = poln + 1;
			}
		}
		else{
			polygons[last] = [];
			polygon = [];
		}
	}
}

function radfn(myradio){
	Prms.radio_FLAG = myradio.value;
	if (Prms.radio_FLAG == "point"){
		closepolygon();
	}
}

function plotfig(event){
	ctx.clearRect(0,0,854,480);
	if (Prms.draw_FLAG == true){
		recsrc[0] = 0;
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
				if (polygon.length > 4) {
					if (Math.pow(polygon[0]-xcur,2)+Math.pow(polygon[1]-ycur,2)<100){
						xcur = polygon[0];
						ycur = polygon[1];
						Prms.newfig_FLAG = true;
					}
				}

				polygon = polygon.concat([xcur, ycur]);
				var lenpoly = polygons.length;
				polygons[lenpoly] = polygon;
				Prms.polText_FLAG = "[";
				for (var ipt = 0; ipt < polygon.length-1; ipt+=2) {
					Prms.polText_FLAG += "("+ polygon[ipt] +"," + polygon[ipt+1] + ")";
				}
				Prms.polText_FLAG += "]";
				document.getElementById("poltexid").innerHTML = Prms.polText_FLAG
				xini = xcur;
				yini = ycur;

				if (Prms.newfig_FLAG == true){
					var polygon_to_send = polygon.slice(0, polygon.length - 2);
					Prms.newfig_FLAG = false;
					send_fig(polygon_to_send);
					polygon = [];

				}
			}
		}
	}
	draw();
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
		closepolygon();
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
	}
	else {
		d3.style.display = "none";
	}
}

function mousemovefn(event){
	xmov = event.clientX - csize.left;
	ymov = event.clientY - csize.top;

	if (Prms.search_FLAG == true){
		if (xmov < 854 && xmov >= 0 && ymov < 480 && ymov >= 0){
			if (Prms.down_FLAG == true){
				recsrc[3] = xmov;
				recsrc[4] = ymov;
				draw();
			}
		}
	}

	if (xmov < 854 && xmov >= 0 && ymov < 480 && ymov >= 0){
		Prms.coor_FLAG = "mouse : (" + xmov + "," + ymov + ")";
		document.getElementById("mousepoint").innerHTML = Prms.coor_FLAG;
		//if (xmov < 430 && ymov < 240) {
		//	Prms.coor_FLAG = "mouse : (" + xmov + "," + ymov + ")";
		//	document.getElementById("mousepoint").innerHTML = Prms.coor_FLAG;
		//}
		//else if (xmov < 430 && ymov > 240) {
		//	Prms.coor_FLAG = "mouse : (" + xmov + "," + ymov + ")";
		//	document.getElementById("mousepoint").innerHTML = Prms.coor_FLAG;
		//}
		//else if (xmov > 430 && ymov < 240) {
		//	Prms.coor_FLAG = "mouse : (" + xmov + "," + ymov + ")";
		//	document.getElementById("mousepoint").innerHTML = Prms.coor_FLAG;
		//}
		//else {
		//	Prms.coor_FLAG = "mouse : (" + xmov + "," + ymov + ")";
		//	document.getElementById("mousepoint").innerHTML = Prms.coor_FLAG;
		//}
	}
	//draw();
}

function mousedownfn(event){
	xdown = event.clientX - csize.left;
	ydown = event.clientY - csize.top;

	Prms.down_FLAG = true;
	if (Prms.search_FLAG == true){
		if (xdown < 854 && xdown >= 0 && ydown < 480 && ydown >= 0){
			recsrc[0] = 1;
			recsrc[1] = xdown;
			recsrc[2] = ydown;
			recsrc[3] = xdown;
			recsrc[4] = ydown;
		}
	}
	if (Prms.nearest_FLAG == true ){
		if (xdown < 854 && xdown >= 0 && ydown < 480 && ydown >= 0){
			npoint[0]=xdown;
			npoint[1]=ydown;

		}
	}
	plotfig(event);
}

function mouseupfn(event){
	Prms.down_FLAG = false;
	if (Prms.search_FLAG == true){
		xup = event.clientX - csize.left;
		yup = event.clientY - csize.top;
		if (xup < 854 && xup >= 0 && yup < 480 && yup >= 0){
			recsrc[3] = xup;
			recsrc[4] = yup;
		}
	}
}

document.addEventListener("mousedown", mousedownfn);
document.addEventListener("mouseup", mouseupfn);
document.addEventListener("mousemove", mousemovefn);
