from PyQt6.QtWidgets import QFrame, QVBoxLayout, QTextEdit, QLineEdit, QLabel, QSizePolicy, QSpacerItem
from PyQt6.QtGui import QMovie
import os

def create_right_panel(input_callback):
    basedir = os.path.dirname(__file__)
    basedir = os.path.abspath(os.path.join(basedir, ".."))
    frame = QFrame()
    frame.setStyleSheet("""
        QFrame {
            background-color: rgba(40, 40, 40, 180);
            border-radius: 15px;
            color: white;
        }
    """)
    layout = QVBoxLayout(frame)
    layout.setSpacing(6)  # Reduced spacing between widgets
    layout.setContentsMargins(10, 10, 10, 10)
    
    layout.addSpacerItem(QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding))


    chat_display = QTextEdit()
    chat_display.setReadOnly(True)
    chat_display.setMaximumHeight(200)
    chat_display.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.MinimumExpanding)
    chat_display.setStyleSheet("""
        background-color: rgba(0, 0, 0, 160);
        border-radius: 8px;
        color: white;
    """)
    
    
    # === Chat Input ===
    chat_input = QLineEdit()
    chat_input.setPlaceholderText("Say something...")
    chat_input.returnPressed.connect(input_callback)
    chat_input.setStyleSheet("""
        background-color: rgba(20, 20, 20, 180);
        border-radius: 8px;
        color: white;
    """)
    
    gif_label = QLabel()
    movie = QMovie(os.path.join(basedir, 'assets', 'cat_nobg.gif'))
    gif_label.setMovie(movie)
    movie.start()
    gif_label.setFixedSize(200, 150)
    gif_label.setScaledContents(True)

    layout.addStretch()
    layout.addWidget(chat_display)
    layout.addWidget(chat_input)
    layout.addWidget(gif_label)
    frame.setStyleSheet("QFrame { background-color: rgba(40,40,40,180); border-radius: 15px; color: white; }")

    return frame, chat_display, chat_input
