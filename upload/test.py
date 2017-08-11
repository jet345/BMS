  
# capture

import numpy as np
import cv2
import sys
import os
import zipfile
import paramiko

# capture handler
cap = cv2.VideoCapture(0)

cap.set(3,320)
cap.set(4,320)

count = 0
file_count = 0;

# make dir
folder = sys.argv[1]
if not os.path.exists(folder):
    os.mkdir(folder)

# loop 
while(True):
    count = count + 1
    ## capture Frame 
    ret,frame = cap.read()

    ## show windows
    cv2.startWindowThread()
    cv2.namedWindow("frame")
    cv2.imshow('frame', frame)

    if count % 50 == 0:
	file_count+=1;
        cv2.imwrite(os.path.join(folder,folder+'{0}.jpg'.format(file_count)), frame)

    ## execute shotdown
    if count==2000:
        break

# release cam resource
cap.release()
# release windows 
cv2.destroyAllWindows()
# make zipfile
jpgzip= zipfile.ZipFile(folder+'.zip','w')

for folder, subfolders, files in os.walk(folder):
 
    for file in files:
        jpgzip.write(os.path.join(folder, file), os.path.relpath(os.path.join(folder,file), '.'), compress_type = zipfile.ZIP_DEFLATED)
 
jpgzip.close()

#send zip file to server
server = '35.185.138.8'
uname = 'jet34540'
keyname = 'server.pem'
destpath = '/home/jet34540/openface/'
k = paramiko.RSAKey.from_private_key_file(keyname)
t = paramiko.Transport((server,22))
t.connect(username = uname, pkey = k )
print "connected"
sftp = paramiko.SFTPClient.from_transport(t)
sftp.put(sys.argv[1]+'.zip',destpath+sys.argv[1]+'.zip')
print "successfully sent"
sftp.close()
t.close()


