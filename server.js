var http = require('http');
var ref = require('ref');
var ffi = require('ffi');
var ArrayType = require('ref-array');
var IntArray = ArrayType(int);
var a = new IntArray(10); // creates an integer array of size 10

var int = ref.types.int

var libprime = ffi.Library('../cpp/lib4ffi/build/Release/prime', {
  'getPrimes': [ int, [ int, IntArray] ]
})
var server = http.createServer(function(request, response){
    var path = url.parse(request.url).pathname;

    if(path == '<write relative path here>'){

    }
  }).listen(8001);

  console.log("Server Initialized");
