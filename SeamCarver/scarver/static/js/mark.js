var img = new Image();
var pseudoImg = document.getElementById('imagelocation');
var redbutton = document.getElementById('selectred');
var greenbutton = document.getElementById('selectgreen');
var hiddeninput = document.getElementById('jsonarray');
var height = pseudoImg.height;
var width = pseudoImg.width;
var mousedragflag = 0;
var redset = 0;
console.log("Height"+height);
console.log("Width"+width);

// Initialize the object selection/removal array
var myarray = [];
for(var i = 0; i < height; i++){
  var row = [];
  for(var j = 0; j < width; j++) {
    row.push(0);
  }
  myarray.push(row);
}
console.log(myarray);
hiddeninput.value = JSON.stringify(myarray);

function modifyarray(xleft,ytop,fillwidth,fillheight,val) {
  var xmax = Math.min(width,xleft+fillwidth);
  var ymax = Math.min(height,ytop+fillheight);

  console.log("xleft "+xleft);
  console.log("ytop "+ytop);
  for(var i = xleft; i < xmax; i++) {
    for(var j = ytop; j < ymax; j++) {
      myarray[j][i] = val;  //j denotes row index and i denotes column index
    }
  }
  hiddeninput.value = JSON.stringify(myarray);
}


img.src = pseudoImg.src;
var canvas = document.getElementById('canvas');
canvas.width = width;
canvas.height = height;
var ctx = canvas.getContext('2d');

img.onload = function() {
  ctx.drawImage(img, 0, 0);
  img.style.display = 'none';
};
var color = document.getElementById('color');

function pick(event) {
  var x = event.layerX;
  var y = event.layerY;
  var pixel = ctx.getImageData(x, y, 1, 1);
  var data = pixel.data;
  var rgba = 'rgba(' + data[0] + ', ' + data[1] +
             ', ' + data[2] + ', ' + (data[3] / 255) + ')';
  color.style.background =  rgba;
  color.textContent = rgba;
}

function setMouseDrag(event) {
  mousedragflag = 1;
}
function unsetMouseDrag(event) {
  mousedragflag = 0;
}
function setred(event) {
  redset = 1;
}
function setgreen(event) {
  redset = -1;
}
function setColor(event) {
  if(mousedragflag == 1) {
    var x = event.layerX;
    var y = event.layerY;
    var xleft,ytop;
    fillwidth = 9;
    fillheight = 9;
    xleft = x-parseInt((fillwidth-1)/2);
    ytop = y-parseInt((fillheight-1)/2);
    if(x == 0) {
      xleft = x;
    }
    if(y == 0) {
      ytop = y;
    }

    if(redset == 1) {
      ctx.fillStyle="#FF0000";
      modifyarray(xleft,ytop,fillwidth,fillheight,-1);
    }
    else if(redset == -1) {
      ctx.fillStyle="#00FF00";
      modifyarray(xleft,ytop,fillwidth,fillheight,1);
    }
    else{
      return;
    }
    ctx.fillRect(xleft,ytop,fillwidth,fillheight);
    console.log("MouseDown Detected");
    var imgData = ctx.getImageData(0,0,width,height);
    console.log(imgData);
  }
}
// canvas.addEventListener('mousemove', pick);
canvas.addEventListener('mousedown',setMouseDrag);
canvas.addEventListener('mouseup',unsetMouseDrag);
canvas.addEventListener('mousemove',setColor);

redbutton.addEventListener('click',setred);
greenbutton.addEventListener('click',setgreen);
