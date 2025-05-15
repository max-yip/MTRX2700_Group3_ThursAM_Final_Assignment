from PyQt6.QtWidgets import QFrame, QVBoxLayout, QHBoxLayout, QLCDNumber, QPushButton, QSlider, QLabel, QSpacerItem, QSizePolicy
from widgets.dial import ColorfulDial
from PyQt6.QtCore import Qt

def create_center_panel(send_servo_command, parent):
    frame = QFrame()
    frame.setStyleSheet("""
        QFrame {
            background-color: rgba(20, 20, 20, 160);
            border-radius: 20px;
            color: white;
        }
    """)
    layout = QVBoxLayout(frame)
    layout.addSpacerItem(QSpacerItem(20, 10, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))


    lcds = [QLCDNumber() for _ in range(3)]
    lcd_layout = QHBoxLayout()
    for lcd in lcds:
        lcd.setFixedSize(80, 50)
        lcd.setDigitCount(5)
        lcd_layout.addWidget(lcd)

    dials = [ColorfulDial() for _ in range(3)]
    dial_layout = QHBoxLayout()
    for dial in dials:
        dial.setRange(1200, 1900)
        dial.setMinimumSize(150, 150)
        dial.setEnabled(False)
        dial_layout.addWidget(dial)

    sliders = [QSlider(Qt.Orientation.Horizontal) for _ in range(2)]
    for slider in sliders:
        slider.setRange(1200, 1900)
        slider.setValue(1551)
        slider.setSingleStep(6)
        slider.valueChanged.connect(send_servo_command)

    buttons_layout = QHBoxLayout()
    # Add "Next Planets" button
    next_planets_button = QPushButton("Next Planet")
    next_planets_button.clicked.connect(parent.handle_next_planets)
    buttons_layout.addWidget(next_planets_button)
    # Add "Calculate Coordinates" button
    calculate_coordinates_button = QPushButton("Calculate Coordinates")
    calculate_coordinates_button.clicked.connect(parent.handle_calculate_coordinates)
    buttons_layout.addWidget(calculate_coordinates_button)

    layout.addLayout(lcd_layout)
    layout.addLayout(buttons_layout)
    layout.addLayout(dial_layout)

    layout.addWidget(QLabel("Pan"))
    layout.addWidget(sliders[0])
    layout.addWidget(QLabel("Tilt"))
    layout.addWidget(sliders[1])

    frame.setStyleSheet("QFrame { background-color: rgba(20,20,20,160); border-radius: 20px; color: white; }")
    return frame, dials, lcds, sliders
