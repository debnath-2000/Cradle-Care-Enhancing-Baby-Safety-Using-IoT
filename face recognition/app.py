from flask import Flask, render_template, Response
import cv2
import numpy as np
import os 
import requests
import winsound
import time

app = Flask(__name__)

recognizer = cv2.face.LBPHFaceRecognizer_create()
recognizer.read('trainer/trainer.yml')
cascadePath = "haarcascade_frontalface_default.xml"
faceCascade = cv2.CascadeClassifier(cascadePath)

font = cv2.FONT_HERSHEY_SIMPLEX
names = ['None', 'known']

cam = cv2.VideoCapture(0)
cam.set(3, 640)  # set video width
cam.set(4, 480)  # set video height

minW = 0.1 * cam.get(3)
minH = 0.1 * cam.get(4)

TELEGRAM_BOT_TOKEN = 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
TELEGRAM_CHAT_ID = 'xxxxxxxxx'
def send_telegram_message(message, image_path):
    url = f'https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}/sendPhoto'
    files = {'photo': open(image_path, 'rb')}
    data = {'chat_id': TELEGRAM_CHAT_ID, 'caption': message}
    requests.post(url, files=files, data=data)

def gen_frames():
    while True:
        ret, img = cam.read()

        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        faces = faceCascade.detectMultiScale(
            gray,
            scaleFactor=1.2,
            minNeighbors=5,
            minSize=(int(minW), int(minH)),
        )

        intruder_detected = False

        for (x, y, w, h) in faces:
            cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)

            id, confidence = recognizer.predict(gray[y:y + h, x:x + w])

            if confidence < 100:
                id = names[id]
                confidence = "  {0}%".format(round(100 - confidence))
            else:
                id = "unknown"
                confidence = "  {0}%".format(round(100 - confidence))
                intruder_detected = True  # Mark intruder as detected
                timestamp = int(time.time())
                image_path = f'unknown_faces/unknown_{timestamp}.jpg'
                cv2.imwrite(image_path, img)
                send_telegram_message('Unknown person detected!', image_path)

            cv2.putText(img, str(id), (x + 5, y - 5), font, 1, (255, 255, 255), 2)
            cv2.putText(img, str(confidence), (x + 5, y + h - 5), font, 1, (255, 255, 0), 1)

        ret, jpeg = cv2.imencode('.jpg', img)
        frame = jpeg.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')

        if intruder_detected:
            play_alert_sound()  # Play alert sound if intruder detected

def play_alert_sound():
    winsound.Beep(440, 1000)  # Beep at 440 Hz for 1000 milliseconds (1 second)

@app.route('/')
def index():
    return render_template('index.html')  # Add your HTML template if needed

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == "__main__":
    app.run(host='0.0.0.0', debug=True)
