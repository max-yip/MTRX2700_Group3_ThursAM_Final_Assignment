import sys
import os
import random
from PyQt5.QtWidgets import (
    QApplication, QWidget, QGridLayout, QVBoxLayout, QHBoxLayout, QLabel, QPushButton,
    QTextEdit, QSlider, QProgressBar, QLineEdit, QFrame, QSpacerItem, QSizePolicy
)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QMovie, QPalette, QPixmap, QBrush, QPainter  # For GIFs


# Get the directory where the script is located
basedir = os.path.dirname(__file__)


class SpaceControlPanel(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Spaceship Control Panel")
        self.setGeometry(100, 100, 1100, 700)
        #relative path of the image
        bgimage_path = os.path.join(basedir, 'assets', 'space_bg2.jpg')

        self.bg = QPixmap(bgimage_path)

        self.initUI()

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
        fuel_bar.setValue(75)
        data_label = QLabel("Data Buffer")
        data_bar = QProgressBar()
        data_bar.setValue(45)
        left_layout.addWidget(fuel_label)
        left_layout.addWidget(fuel_bar)
        left_layout.addWidget(data_label)
        left_layout.addWidget(data_bar)
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
        center_layout.addSpacerItem(QSpacerItem(20, 10, QSizePolicy.Minimum, QSizePolicy.Expanding))
        # map_layout = QGridLayout()
        # self.map_buttons = []
        # for i in range(8):
        #     row = []
        #     for j in range(8):
        #         btn = QPushButton("")
        #         btn.setFixedSize(40, 40)
        #         btn.setStyleSheet("background-color: #222; border: 1px solid #444;")
        #         btn.clicked.connect(lambda _, x=i, y=j: self.cell_clicked(x, y))
        #         map_layout.addWidget(btn, i, j)
        #         row.append(btn)
        #     self.map_buttons.append(row)
        # center_layout.addLayout(map_layout)

        bottom_controls = QHBoxLayout()
        slider1 = QSlider(Qt.Horizontal)
        slider2 = QSlider(Qt.Horizontal)
        btn1 = QPushButton("Engage")
        btn2 = QPushButton("Scan")
        bottom_controls.addWidget(QLabel("Power"))
        bottom_controls.addWidget(slider1)
        bottom_controls.addWidget(QLabel("Shields"))
        bottom_controls.addWidget(slider2)
        bottom_controls.addWidget(btn1)
        bottom_controls.addWidget(btn2)
        center_layout.addLayout(bottom_controls)

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
        right_layout.addSpacerItem(QSpacerItem(20, 20, QSizePolicy.Minimum, QSizePolicy.Expanding))

        # === Chat Display ===
        self.chat_display = QTextEdit()
        self.chat_display.setReadOnly(True)
        self.chat_display.setMinimumHeight(100)
        self.chat_display.setMaximumHeight(200)
        self.chat_display.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.MinimumExpanding)
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
        right_layout.addSpacerItem(QSpacerItem(20, 5, QSizePolicy.Minimum, QSizePolicy.Fixed))

        # === Main Layout Assembly ===
        main_layout.addWidget(left_frame, 1)
        main_layout.addWidget(center_frame, 3)
        main_layout.addWidget(right_frame, 1)

    def paintEvent(self, event):
        painter = QPainter(self)
        scaled_bg = self.bg.scaled(self.size(), Qt.KeepAspectRatioByExpanding, Qt.SmoothTransformation)
        painter.drawPixmap(0, 0, scaled_bg)

    def cell_clicked(self, x, y):
        print(f"Map Clicked: Cell ({x}, {y})")

    def process_chat_input(self):
        user_input = self.chat_input.text().strip()
        if not user_input:
            return

        self.chat_display.append(f"You: {user_input}")
        self.chat_input.clear()

        # === CatGPT logic ===
        # if user_input.lower() == "exit":
        #     self.chat_display.append("Cat: Bye Meow!")
        # el
        if user_input.lower() == "hint":
            self.chat_display.append("Cat: reveal hint")
        else:
            num_meows = random.randint(0, 4)
            response = "Meow" + "".join([" meow"] * num_meows) + "."
            self.chat_display.append(f"Cat: {response}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    panel = SpaceControlPanel()
    panel.show()
    sys.exit(app.exec_())
