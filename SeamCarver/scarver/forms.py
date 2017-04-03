from django import forms

class ImageUploadForm(forms.Form):
    image = forms.ImageField()

class ImageResizeForm(forms.Form):
    desired_width = forms.IntegerField()    #in pixels
    desired_height = forms.IntegerField()   #in pixels
