from django.shortcuts import render
from django.http import HttpResponse
from .forms import ImageUploadForm,ImageResizeForm
import os
from django.core.files.images import get_image_dimensions

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
        return render(request,'scarver/uploadImage.html',{'form':form})

def resizeImage(request):
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
            d_w = request.POST.get('desired_width',-1)
            d_h = request.POST.get('desired_height',-1)
            print d_w
            print d_h
            # TODO: call the cpp subroutine here
            print "File saved successfully"
            return HttpResponse("Request is post")
        else:
            # TODO: write the error message of no file upload
            return render(request,'scarver/uploadImage.html',{'form':ImageUploadForm()})
    if request.method == 'GET':
        print "Request is get"
        form = ImageUploadForm()
        return render(request,'scarver/uploadImage.html',{'form':form})
