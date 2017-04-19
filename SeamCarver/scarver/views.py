from django.shortcuts import render
from django.http import HttpResponse
from .forms import ImageUploadForm, ImageResizeForm, MagnifyForm
import os
from django.core.files.images import get_image_dimensions
import numpy as np
from PIL import Image
from ctypes import *
import json
lib = cdll.LoadLibrary(os.path.dirname(os.path.realpath(__file__))+'/seamcarvinglib/shared_seamcarving.so')
lib.Rescale.argtypes = [c_char_p,c_double,c_double]
lib.Amplify.argtypes = [c_char_p,c_double]
lib.removeRetain.argtypes = [c_char_p]

# Create your views here.
def saveImage(f):
    app_path = os.path.dirname(os.path.realpath(__file__))
    save_path = app_path+os.sep+'static'+os.sep+'UploadedImages'+os.sep+f.name
    with open(save_path, 'wb+') as destination:
        for chunk in f.chunks():
            destination.write(chunk)
    return save_path

def index(request):
    return HttpResponse("The app is running successfully.")

def uploadImage(request):
    if request.method == 'POST':
        f = request.FILES.get('image',None)
        w,h = get_image_dimensions(f)
        print "=============="
        print "Width is: "+str(w)
        print "=============="
        print "Height is: "+str(h)
        print "=============="
        print f.size
        print "=============="
        print f.content_type
        print "=============="
        print f.content_type_extra
        print "=============="
        if f:
            save_path = saveImage(f)
            print save_path
            context = {
                'resizeform':ImageResizeForm(),
                'magnifyform': MagnifyForm(),
                'image_width':w,
                'image_height':h,
                'image_name':f.name,
            }
            return render(request,'scarver/displayImage.html',context)
        else:
            # TODO: write the error message of no file upload
            return render(request,'scarver/uploadImage.html',{'form':ImageUploadForm()})
    if request.method == 'GET':
        print "Request is get"
        form = ImageUploadForm()
        return render(request,'scarver/uploadImage.html',{'uploadform':form})

def resizeImage(request):
    if request.method == 'POST':
        print request.POST
    d_h = float(request.POST.get('desired_height_ratio',1.5))
    d_w = float(request.POST.get('desired_width_ratio',1.0))
    image_name = request.POST.get('image_name',None)
    image_path = os.path.dirname(os.path.realpath(__file__))+os.sep+'static'+os.sep+'UploadedImages'+os.sep+image_name
    lib.Rescale(image_path,d_h,d_w)
    pos = image_name.find('.')
    context = {
    'carved_image_name':image_name[:pos]+"_carved"+image_name[pos:],
    'image_name':image_name,
    }
    return render(request,'scarver/carvedImage.html',context)

def magnifyObject(request):
    if request.method == "POST":
        print request.POST
        d_m = float(request.POST.get('desired_magnification',1.25))
        image_name = request.POST.get('image_name',None)
        image_path = os.path.dirname(os.path.realpath(__file__))+os.sep+'static'\
        +os.sep+'UploadedImages'+os.sep+image_name
        lib.Amplify(image_path,d_m)
        pos = image_name.find('.')
        context = {
        'magnified_image_name':image_name[:pos]+"_carved"+image_name[pos:],
        'image_name':image_name,
        }
        return render(request,'scarver/magnifiedImage.html',context)

    return HttpResponse("You are not authorised for what you did just now.")

def objectRemovalSelection(request):
    if request.method == "POST":
        print request.POST
        image_name = request.POST.get('image_name',None)
        context = {
            'image_name':image_name,
        }
        return render(request,'scarver/selectImageArea.html',context)
    return HttpResponse("OK")

def objectRemSel(request):
    if request.method == "POST":
        print "Request method is post"
        # TODO: take the image name and modified image from the request post
        # and call the object removal subroutine from cpp shared library
        image_name = request.POST.get('image_name',None)
        image_path = os.path.dirname(os.path.realpath(__file__))+os.sep+'static'\
        +os.sep+'UploadedImages'+os.sep+image_name
        myarray = request.POST.get('retention_removal_array',None)
        json_array = json.loads(myarray)
        # print json_array
        data = np.asarray(list(json_array))
        shape = data.shape
        print shape
        for i in range(shape[0]):
            for j in range(shape[1]):
                data[i][j] = data[i][j]*127 + 127
        rescaled = data.astype(np.uint8)
        im = Image.fromarray(rescaled)
        save_path = os.path.dirname(os.path.realpath(__file__))+os.sep+'static'\
        +os.sep+'UploadedImages'
        pos = image_name.find('.')
        save_path = save_path+os.sep+image_name[:pos]+"_gray"+image_name[pos:]
        im.save(save_path)
        lib.removeRetain(image_path)

        context = {
        'modified_image_name':image_name[:pos]+"_modified"+image_name[pos:],
        'image_name':image_name,
        }
        return render(request,'scarver/modifiedImage.html',context)
    return HttpResponse("Something went wrong.")
