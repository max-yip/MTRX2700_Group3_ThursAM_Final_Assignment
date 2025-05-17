from PyQt6.QtWidgets import QFrame, QVBoxLayout, QLabel, QProgressBar

def create_left_panel(fuel, parent):
    frame = QFrame()
    frame.setStyleSheet("""
        QFrame {
            background-color: rgba(30, 30, 30, 180);
            border-radius: 15px;
            color: white;
        }
    """)
    layout = QVBoxLayout(frame)
    label = QLabel("Fuel Level")
    bar = QProgressBar()
    bar.setMaximum(3)
    bar.setValue(fuel)
    layout.addWidget(label)
    layout.addWidget(bar)
    layout.addStretch()
    frame.setStyleSheet("QFrame { background-color: rgba(30,30,30,180); border-radius: 15px; color: white; }")
    return frame, bar