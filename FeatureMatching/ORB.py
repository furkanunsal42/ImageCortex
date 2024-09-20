import cv2

image = cv2.imread("Images/hatay.tif")
print(image.shape)

subimage0 = cv2.getRectSubPix(image, (1024, 1024), (15000, 15000))
subimage1 = cv2.getRectSubPix(image, (1024, 1024), (15400, 15200))

orb = cv2.ORB_create()

kp0, des0 = orb.detectAndCompute(subimage0, None)
subimage0_orb = cv2.drawKeypoints(subimage0, kp0, None, color=(0,255,0), flags=0)

kp1, des1 = orb.detectAndCompute(subimage1, None)
subimage1_orb = cv2.drawKeypoints(subimage1, kp1, None, color=(0,255,0), flags=0)

cv2.imshow("hatay1", subimage0_orb)
cv2.imshow("hatay2", subimage1_orb)
cv2.waitKey()
