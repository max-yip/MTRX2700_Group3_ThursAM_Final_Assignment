from PyQt6.QtWidgets import QApplication
import sys
from ui.main_window import SpaceControlPanel

if __name__ == "__main__":
    app = QApplication(sys.argv)
    panel = SpaceControlPanel()
    panel.show()
    panel.show_startup_popup()
    sys.exit(app.exec())
