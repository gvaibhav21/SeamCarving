from django.conf.urls import url

from . import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^uploadImage/$',views.uploadImage, name='uploadImage'),
    url(r'^resizeImage/$',views.resizeImage, name='resizeImage'),
    url(r'^magnifyObject/$',views.magnifyObject, name='magnifyObject'),
    url(r'^objectRemovalSelection/$',views.objectRemovalSelection, name='objectRemovalSelection'),
    url(r'^objectRemSel/$',views.objectRemSel, name='objectRemSel'),    
]
