var img = new Image();
var origImg = new Image();
var pseudoImg = document.getElementById('segimgloc');
var origImgloc = document.getElementById('imagelocation')
var redbutton = document.getElementById('selectred');
var greenbutton = document.getElementById('selectgreen');
var hiddeninput = document.getElementById('jsonarray');
var brushsize = document.getElementById('brushsize');
var segmentationinfo = document.getElementById('segmentinfo');
var submitbutton = document.getElementById('submitimage');
var imageform = document.getElementById('imageform');
var selectsegment = document.getElementById('selectsegment');
var canvas = document.getElementById('canvas');
var canvasorig = document.getElementById('canvasorigimg');
var segmentinfoarray = JSON.parse(segmentationinfo.value)
var height = pseudoImg.height;
var width = pseudoImg.width;
var mousedragflag = 0;
var segmentcolorflag = false;
var redset = 0;

var markedSegments = new Set();

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

  for(var i = xleft; i < xmax; i++) {
    for(var j = ytop; j < ymax; j++) {
      myarray[j][i] = val;  //j denotes row index and i denotes column index
    }
  }
}


img.src = pseudoImg.src;
origImg.src = origImgloc.src;

canvas.width = width;
canvas.height = height;
canvasorig.width = width;
canvasorig.height = height;

var ctx = canvas.getContext('2d');
var ctxorig = canvasorig.getContext('2d');

img.onload = function() {
  ctx.drawImage(img, 0, 0);
  img.style.display = 'none';
};

origImg.onload = function() {
  ctxorig.drawImage(origImg, 0, 0);
  origImg.style.display = 'none';
}

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
  segmentcolorflag = selectsegment.checked;
  if(mousedragflag == 1 && segmentcolorflag == false) {
    var x = event.layerX;
    var y = event.layerY;
    var xleft,ytop;
    var bsize = parseInt(brushsize.value);
    if(isNaN(bsize)) {
      fillwidth = 9;
    }
    else {
      fillwidth = bsize;
    }
    fillheight = fillwidth;
    xleft = x-parseInt((fillwidth-1)/2);
    ytop = y-parseInt((fillheight-1)/2);
    xleft = Math.max(0,xleft);
    ytop = Math.max(0,ytop);

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
  }
}

function setColorSegment(event) {
  var segmentcolorflag = selectsegment.checked;
  if(segmentcolorflag == true) {
    var x = event.layerX;
    var y = event.layerY;

    var segclass = segmentinfoarray[y][x];
    if(markedSegments.has(segclass)) {
      //Copy original image data
      for(var i = 0; i < width; i++) {
        for(var j = 0; j < height; j++) {
          if(segmentinfoarray[j][i] == segclass) {
            var pixel = ctxorig.getImageData(i, j, 1, 1);
            ctx.putImageData(pixel,i,j);
            modifyarray(i,j,1,1,0);
          }
        }
      }
      markedSegments.delete(segclass);
      console.log("Orignal data copied to Segment "+segclass);
    }
    else {
      if(redset == 1) {
        ctx.fillStyle="#FF0000";
      }
      else if(redset == -1) {
        ctx.fillStyle="#00FF00";
      }
      else{
        return;
      }
      for(var i = 0; i < width; i++) {
        for(var j = 0; j < height; j++) {
          if(segmentinfoarray[j][i] == segclass) {
            modifyarray(i,j,1,1,-redset);
            ctx.fillRect(i,j,1,1);
          }
        }
      }
      markedSegments.add(segclass);
      console.log("Segment "+segclass+" filled with color");
    }
  }
}

function submitimage(event) {
  hiddeninput.value = JSON.stringify(myarray);
  imageform.submit();
}
// canvas.addEventListener('mousemove', pick);
canvas.addEventListener('mousedown',setMouseDrag);
canvas.addEventListener('mouseup',unsetMouseDrag);
canvas.addEventListener('mousemove',setColor);
canvas.addEventListener('mouseleave',unsetMouseDrag);
canvas.addEventListener('click',setColorSegment);
redbutton.addEventListener('click',setred);
greenbutton.addEventListener('click',setgreen);
submitbutton.addEventListener('click',submitimage);
