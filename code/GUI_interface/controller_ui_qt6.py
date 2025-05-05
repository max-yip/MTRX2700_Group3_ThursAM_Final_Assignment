import sys
import os
import random
import time

# PyQt6 import
from PyQt6.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton,
    QTextEdit, QSlider, QProgressBar, QLineEdit, QFrame, QSpacerItem, QSizePolicy,
    QLCDNumber
)
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QMovie, QPixmap, QPainter  # For GIFs

# dial class import
from dial import ColorfulDial

#serialise functions import
from serialise_handle import SerialReader


# Get the directory where the script is located
basedir = os.path.dirname(__file__)


class SpaceControlPanel(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Spaceship Control Panel")
        self.setGeometry(100, 100, 1100, 700)

        #relative path of the image
        self.bgimage_path = os.path.join(basedir, 'assets', 'space_bg2.jpg')
        self.bg = QPixmap(self.bgimage_path)
        self.initUI()
        self.start_serial_reading('/dev/tty.usbmodem1103')

    def initUI(self):
        main_layout = QHBoxLayout(self)

        # === Left Panel (Fuel/Data) ===
        left_frame = QFrame()
        left_frame.setStyleSheet("""
            QFrame {
                background-color: rgba(30, 30, 30, 180);
                border-radius: 15px;
                color: white;
            }
        """)
        left_layout = QVBoxLayout(left_frame)
        fuel_label = QLabel("Fuel Level")
        fuel_bar = QProgressBar()
        fuel_bar.setValue(100)
        left_layout.addWidget(fuel_label)
        left_layout.addWidget(fuel_bar)
        left_layout.addStretch()

        # === Center Panel (Map + Controls) ===
        center_frame = QFrame()
        center_frame.setStyleSheet("""
            QFrame {
                background-color: rgba(20, 20, 20, 160);
                border-radius: 20px;
                color: white;
            }
        """)
        center_layout = QVBoxLayout(center_frame)
        center_layout.addSpacerItem(QSpacerItem(20, 10, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))
        
        # Number label
        self.lidar_distance = QLCDNumber()
        self.lidar_distance.setDigitCount(5)  # Width
        self.lidar_distance.setSegmentStyle(QLCDNumber.SegmentStyle.Flat)
        center_layout.addWidget(self.lidar_distance)
        
        # === button ===
        btn1 = QPushButton("Calculate")
        btn1.clicked.connect(lambda _, x=0, y=0: self.cell_clicked(x, y))
        btn1.setMaximumWidth(100)
        center_layout.addWidget(btn1)
        

        # === servo speedometer ===
        speedometer_display = QHBoxLayout()
        self.dial1 = ColorfulDial(self)
        self.dial1.setRange(-2000, 2000)
        self.dial1.setMinimumSize(150, 150)
        self.dial1.setEnabled(False)
        speedometer_display.addWidget(self.dial1)
        
        self.dial2 = ColorfulDial(self)
        self.dial2.setRange(-2000, 2000)
        self.dial2.setMinimumSize(150, 150)
        self.dial2.setEnabled(False)
        speedometer_display.addWidget(self.dial2)
        
        self.dial3 = ColorfulDial(self)
        self.dial3.setRange(-2000, 2000)
        self.dial3.setMinimumSize(150, 150)
        self.dial3.setEnabled(False)
        speedometer_display.addWidget(self.dial3)
        
        center_layout.addLayout(speedometer_display)


        # === Servo sliders ===
        servo_controls = QVBoxLayout()
        self.slider1 = QSlider(Qt.Orientation.Horizontal)
        self.slider1.setMinimumHeight(20)
        self.slider1.setRange(1200, 1900)
        self.slider1.setValue(1551)
        self.slider1.setSingleStep(6)
        self.slider1.setPageStep(6)
        self.slider1.valueChanged.connect(self.send_servo_command)

        self.slider2 = QSlider(Qt.Orientation.Horizontal)
        self.slider2.setMinimumHeight(20)
        self.slider2.setRange(1200, 1900)
        self.slider2.setValue(1551)
        self.slider2.setSingleStep(6)
        self.slider2.setPageStep(6)
        self.slider2.valueChanged.connect(self.send_servo_command)

        label1 = QLabel("Pan")
        label1.setStyleSheet("background: none;")

        label2 = QLabel("Tilt")
        label2.setStyleSheet("background: none;")
        
        servo_controls.addWidget(label1)
        servo_controls.addWidget(self.slider1)
        servo_controls.addWidget(label2)
        servo_controls.addWidget(self.slider2)
       
        center_layout.addLayout(servo_controls)

        # === Right Panel (Chat + GIF) ===
        right_frame = QFrame()
        right_frame.setStyleSheet("""
            QFrame {
                background-color: rgba(40, 40, 40, 180);
                border-radius: 15px;
                color: white;
            }
        """)

        right_layout = QVBoxLayout(right_frame)
        right_layout.setSpacing(6)  # Reduced spacing between widgets
        right_layout.setContentsMargins(10, 10, 10, 10)

        # === Spacer to push everything to the bottom ===
        right_layout.addSpacerItem(QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))

        # === Chat Display ===
        self.chat_display = QTextEdit()
        self.chat_display.setReadOnly(True)
        self.chat_display.setMinimumHeight(100)
        self.chat_display.setMaximumHeight(200)
        self.chat_display.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.MinimumExpanding)
        self.chat_display.setStyleSheet("""
            background-color: rgba(0, 0, 0, 160);
            border-radius: 8px;
            color: white;
        """)
        right_layout.addWidget(self.chat_display)

        # === Chat Input ===
        self.chat_input = QLineEdit()
        self.chat_input.setPlaceholderText("Say something...")
        self.chat_input.returnPressed.connect(self.process_chat_input)
        self.chat_input.setStyleSheet("""
            background-color: rgba(20, 20, 20, 180);
            border-radius: 8px;
            color: white;
        """)
        right_layout.addWidget(self.chat_input)

        # === GIF ===
        gif_label = QLabel()

        #relative path of the image
        catgif_path = os.path.join(basedir, 'assets', 'cat_nobg.gif')

        movie = QMovie(catgif_path)
        gif_label.setMovie(movie)
        movie.start()
        gif_label.setFixedSize(200, 150)
        gif_label.setScaledContents(True)
        gif_label.setStyleSheet("background-color: rgba(0, 0, 0, 0);")
        right_layout.addWidget(gif_label)

        # Optional tiny bottom padding
        right_layout.addSpacerItem(QSpacerItem(20, 5, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Fixed))

        # === Main Layout Assembly ===
        main_layout.addWidget(left_frame, 1)
        main_layout.addWidget(center_frame, 3)
        main_layout.addWidget(right_frame, 1)

    def paintEvent(self, event):
        painter = QPainter(self)
        if hasattr(self, 'bg') and self.bg:
            painter.drawPixmap(self.rect(), self.bg)

    def cell_clicked(self, x, y):
        print(f"Button Clicked: Cell ({x}, {y})")

    def process_chat_input(self):
        user_input = self.chat_input.text().strip()
        if not user_input:
            return

        self.chat_display.append(f"You: {user_input}")
        self.chat_input.clear()

        # === CatGPT logic ===
        if user_input.lower() == "hint":
            self.chat_display.append("Cat: reveal hint")
        else:
            num_meows = random.randint(0, 4)
            response = "Meow" + "".join([" meow"] * num_meows) + "."
            self.chat_display.append(f"Cat: {response}")

    def start_serial_reading(self, port, baud_rate=115200, timeout=1):
        self.serial_reader = SerialReader(port, baud_rate, timeout)
        self.serial_reader.data_received.connect(self.handle_serial_data)
        # self.serial_reader.color_change.connect(self.change_button_color)
        self.serial_reader.start()

    def handle_serial_data(self, data):
        #print(f"Data read from serial port: {data}")``
        self.dial1.setValue(data[3])
        self.dial2.setValue(data[4])
        self.dial3.setValue(data[5])
        self.lidar_distance.display(data[6])
        # self.slider2.setValue(data[7])

    def send_servo_command(self):
        pwm1 = self.slider1.value()
        pwm2 = self.slider2.value()
        message = f"{pwm1},{pwm2}\n"
        print(message)
        self.serial_reader.serial_port.write(message.encode())

if __name__ == "__main__":
    app = QApplication(sys.argv)
    panel = SpaceControlPanel()
    panel.show()
    sys.exit(app.exec())
