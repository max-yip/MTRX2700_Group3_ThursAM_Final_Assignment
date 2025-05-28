import sys

from PyQt6.QtWidgets import QApplication, QMainWindow, QHBoxLayout, QVBoxLayout, QWidget, QPushButton, QSlider, QDial
from PyQt6.QtCore import Qt, QThread, pyqtSignal

from widgets.dial import ColorfulDial

import struct
import serial
import time
from enum import Enum


SENTINEL_1 = 0xAA
SENTINEL_2 = 0x55


class MessageType(Enum):
    SENSOR_DATA = 0
    STRING_PACKET = 1
    SERVO_PWM = 2


def pack_buffer(message_type, data):
    data_length = len(data)
    header = struct.pack('<BBHH', SENTINEL_1, SENTINEL_2, message_type, data_length)
    return header + data


def receive_and_unpack_buffer(ser):
    # Wait for the sentinel bytes
    while True:
        byte = ser.read(1)
        if byte == bytes([SENTINEL_1]):
            byte = ser.read(1)
            if byte == bytes([SENTINEL_2]):
                break

    # Read the remaining header bytes
    header_length = 4
    header_buffer = ser.read(header_length)
    header_buffer = bytes([SENTINEL_1, SENTINEL_2]) + header_buffer

    if len(header_buffer) < 6:
        return None, None

    sentinel1, sentinel2, message_type_received, data_length = struct.unpack('<BBHH', header_buffer[:6])

    if sentinel1 != SENTINEL_1 or sentinel2 != SENTINEL_2:
        return None, None

    data_buffer = ser.read(data_length)
    return MessageType(message_type_received), data_buffer


class SerialReader(QThread):
    data_received = pyqtSignal(list)
    servo_data_received = pyqtSignal(tuple)
    string_packet_received = pyqtSignal(str)

    def __init__(self, port, baud_rate=9600, timeout=1):
        super().__init__()
        self.port = port
        self.baud_rate = baud_rate
        self.timeout = timeout
        self.serial_port = serial.Serial(port, baud_rate, timeout=timeout)

    def run(self):
        try:
            ser = serial.Serial(self.port, self.baud_rate, timeout=self.timeout)

            while True:
                message_type, data = receive_and_unpack_buffer(ser)
                if message_type == MessageType.SENSOR_DATA:
                    # 6 int32 + 1 uint32 = 7 values
                    sensor_data = struct.unpack('<iiiiiiI', data)
                    self.data_received.emit(list(sensor_data))

                elif message_type == MessageType.SERVO_PWM:
                    pwm_values = struct.unpack('<HH', data)
                    self.servo_data_received.emit(pwm_values)

                elif message_type == MessageType.STRING_PACKET:
                    # data is raw bytes of string packet
                    string_data = data.decode('utf-8', errors='ignore')
                    self.string_packet_received.emit(string_data)

        except Exception as e:
            print(f"Error reading from serial port: {e}")


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.central_widget = QWidget()
        self.layout = QVBoxLayout(self.central_widget)

        self.button = QPushButton('Change Values', self.central_widget)
        self.button.clicked.connect(self.update_values)
        self.button.setMinimumHeight(50)  # Set the button's minimum height
        self.layout.addWidget(self.button)

        slider_style = '''
        QSlider::groove:horizontal {
            border: 1px solid #999999;
            height: 40px;
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);
            margin: 2px 0;
        }
        QSlider::handle:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #4444FF, stop:1 #6666FF);
            border: 1px solid #7777FF;
            width: 20px;
            margin: -2px 0;
            border-radius: 3px;
        }
        '''

        self.slider1 = QSlider(Qt.Orientation.Horizontal, self.central_widget)
        self.slider1.setRange(0, 5000)
        self.slider1.setMinimumHeight(50)  # Set the slider's minimum height
        self.slider1.setStyleSheet(slider_style)  # Set the slider's style        
        self.layout.addWidget(self.slider1)

        self.slider2 = QSlider(Qt.Orientation.Horizontal, self.central_widget)
        self.slider2.setRange(0, 5000)
        self.slider2.setMinimumHeight(50)  # Set the slider's minimum height
        self.slider2.setStyleSheet(slider_style)  # Set the slider's style        
        self.layout.addWidget(self.slider2)

        self.dials_layout = QHBoxLayout()
        self.layout.addLayout(self.dials_layout)

        self.dial1 = ColorfulDial(self.central_widget)
        self.dial1.setRange(-2000, 2000)
        self.dials_layout.addWidget(self.dial1)

        self.dial2 = ColorfulDial(self.central_widget)
        self.dial2.setRange(-2000, 2000)
        self.dials_layout.addWidget(self.dial2)

        self.dial3 = ColorfulDial(self.central_widget)
        self.dial3.setRange(-2000, 2000)
        self.dials_layout.addWidget(self.dial3)

        self.setWindowTitle("Dials and Sliders")
        self.setGeometry(100, 100, 800, 600)  # Change the window size on startup

        self.setCentralWidget(self.central_widget)

        self.start_serial_reading('/dev/tty.usbmodem1103')

    def update_values(self):
        self.slider1.setValue(1000)
        self.slider2.setValue(4000)
        self.dial1.setValue(-1000)
        self.dial2.setValue(500)
        self.dial3.setValue(1500)

    def start_serial_reading(self, port, baud_rate=115200, timeout=1):
        self.serial_reader = SerialReader(port, baud_rate, timeout)
        self.serial_reader.data_received.connect(self.handle_serial_data)
        self.serial_reader.color_change.connect(self.change_button_color)
        self.serial_reader.start()

    def change_button_color(self, change_color):
        if change_color:
            self.button.setStyleSheet('QPushButton { background-color: green; }')
        else:
            self.button.setStyleSheet('QPushButton { background-color: red; }')

    def handle_serial_data(self, data):
        #print(f"Data read from serial port: {data}")``
        self.dial1.setValue(data[3])
        self.dial2.setValue(data[4])
        self.dial3.setValue(data[5])
        self.slider1.setValue(data[6])
        # self.slider2.setValue(data[7])


if __name__ == '__main__':
    app = QApplication(sys.argv)
    main_win = MainWindow()
    main_win.show()
    sys.exit(app.exec())
