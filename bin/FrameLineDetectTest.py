# -*- coding: utf-8 -*-
"""
Created on Wed May 10 11:19:46 2017

@author: jtcstc
"""
import os

root_path = os.getcwd()
exePath = root_path  + "/"+ "./x86/Test.exe"

# -1: all, 0：营业执照， 1：外国人工作许可证， 2：就业证, 3:户口本， 4：身份证
doc_type = -1

img_dir_list = ["../sample/bussinesscard", "../sample/外国人工作许可证", "../sample/就业证", "../sample/户口本""]
img_res_dir_list = ["../result/bussinesscard", "../result/外国人工作许可证", "../result/就业证", "../result/户口本"]



def image_rectifying(img_dir, img_res_dir):
    for dir, dirs, files in os.walk(img_dir):
        for file in files:
        
            fileInfo = os.path.splitext(file)
            filename = fileInfo[0]
            fileExt = fileInfo[1]

            #print (filename)
            #print (fileExt)

            img_path = root_path + "/" + dir + "/" + file
            img_res_path = root_path + "/" + img_res_dir + "/" + filename + "_res" + fileExt
            
            #print (img_res_path)
            cmdInfo = "%s --doc_type=%d --input_img=%s --output_img=%s" % (exePath, doc_type, img_path, img_res_path)
            

            print (cmdInfo)
            os.system(cmdInfo)
            
def image_rectifying_allclass(img_dir_list, img_res_dir_list):
    for doc_type in range(len(img_dir_list)):
        image_rectifying(img_dir_list[doc_type], img_res_dir_list[doc_type])
        
        
if __name__ == "__main__":
   
    if doc_type == -1: # create all class
        image_rectifying_allclass(img_dir_list, img_res_dir_list)
    else: # create single class
        image_rectifying(img_dir_list[doc_type], img_res_dir_list[doc_type]) 
    
    

    