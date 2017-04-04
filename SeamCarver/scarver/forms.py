from django import forms

class ImageUploadForm(forms.Form):
    image = forms.ImageField()

class ImageResizeForm(forms.Form):
    desired_width_ratio = forms.FloatField()    #in pixels
    desired_height_ratio = forms.FloatField()   #in pixels

class MagnifyForm(forms.Form):
    desired_magnification = forms.FloatField()
