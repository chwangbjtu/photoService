import StringIO
from PIL import Image
from tornado import log
import traceback
from common.conf import Conf
class deal_image(object):
    def __init__(self,input,width,hight):
        self.__inImage = input
        self.__outImage = None
        self.__tmpImage = None
        self.__width = width
        self.__hight = hight
    def image_to_string(self,formats):
        try:
            output = StringIO.StringIO()
            self.__tmpImage.save(output,format=formats)
            self.__outImage = output.getvalue()
            output.close()
        except Exception,e:
            log.app_log.info(traceback.format_exc())
    def deal(self):
        try:
            image = Image.open(StringIO.StringIO(self.__inImage))
            w,h = image.size
            routh_w = self.__width/w
            routh_h = self.__hight/h
            if routh_w == routh_h:
                self.__tmpImage = image.resize((self.__width,self.__hight),Image.ANTIALIAS)
            elif routh_w > routh_h:
                cut_h = h*routh_w
                self.__tmpImage = image.resize((self.__width,h*routh_w),Image.ANTIALIAS)
                box = (0,(cut_h-self.__hight)/2,self.__width,(cut_h+self.__hight)/2)
                self.__tmpImage = self.__tmpImage.crop(box)
            else:
                cut_w = w*routh_h
                self.__tmpImage = image.resize((cut_w,self.__hight),Image.ANTIALIAS)
                box = ((cut_w-self.__width)/2,0,(cut_w+self.__width)/2,self.__hight)
                self.__tmpImage = self.__tmpImage.crop(box)
            self.image_to_string(image.format)
        except Exception,e:
            log.app_log.info(traceback.format_exc())

    def get_image(self):
        return self.__outImage


if __name__=="__main__":
        finename = 'Chrysanthemum.png'
        file_object = open(finename,'rb')
        data = file_object.read()
        file_object.close()

        myImage = deal_image(data,1024,10)
        myImage.deal()
        result = myImage.get_image()
        f = open('testpng.png','wb')
        f.write(result)
        f.close()
