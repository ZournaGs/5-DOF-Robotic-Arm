import serial
import time
import os

SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 9600
FILE_PATH = "code/src/stockfish_cli/answer.txt"

ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1)
time.sleep(2)

last_move = ""

try:
    while True:
        if os.path.exists(FILE_PATH):
            with open(FILE_PATH, "r") as file:
                move = file.read().strip()

            if move != "" and move != last_move:
                print(f"\n---------------\nSending to Arduino: {move}")
                ser.write((move + "\n").encode())
                last_move = move

        if ser.in_waiting > 0:
            arduino_msg = ser.readline().decode(errors="ignore").strip()
            if arduino_msg:
                print(f"Arduino: {arduino_msg}\n---------------\n")

        time.sleep(0.1)

except KeyboardInterrupt:
    print("Closing serial...")
    ser.close()