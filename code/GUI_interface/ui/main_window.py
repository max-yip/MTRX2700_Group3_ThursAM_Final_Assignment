from PyQt6.QtWidgets import QWidget, QHBoxLayout, QMessageBox
from PyQt6.QtGui import QPixmap, QPainter, QColor
import os

from ui.left_panel import create_left_panel
from ui.center_panel import create_center_panel
from ui.right_panel import create_right_panel
from core.serial_handler import SerialReader

basedir = os.path.dirname(__file__)
basedir = os.path.abspath(os.path.join(basedir, ".."))

class SpaceControlPanel(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Spaceship Control Panel")
        self.setGeometry(100, 100, 1100, 700)
        self.bg = QPixmap(os.path.join(basedir, 'assets', 'space_bg2.jpg'))

        self.planet_index = 0  # Start with the first LCD and dial
        self.planet_data_storage = {0:[0,1551,''], 1:[0,1551,''], 2:[0,1551,'']}  
        # Store data for each planet index:[lidar_distance, dial_value, dial color]
        self.initUI()
        self.start_serial_reading('/dev/tty.usbmodem1103')

    def initUI(self):
        layout = QHBoxLayout(self)

        self.left_panel, self.fuel_bar = create_left_panel()
        self.center_panel, self.dials, self.lcds, self.sliders = create_center_panel(self.send_servo_command, self)
        self.right_panel, self.chat_display, self.chat_input = create_right_panel(self.process_chat_input)

        layout.addWidget(self.left_panel, 1)
        layout.addWidget(self.center_panel, 3)
        layout.addWidget(self.right_panel, 1)

    def paintEvent(self, event):
        painter = QPainter(self)
        if self.bg:
            painter.drawPixmap(self.rect(), self.bg)

    def start_serial_reading(self, port, baud_rate=115200, timeout=1):
        self.serial_reader = SerialReader(port, baud_rate, timeout)
        self.serial_reader.data_received.connect(self.handle_serial_data)
        self.serial_reader.start()

    def handle_serial_data(self, data):
        curr_index = self.planet_index
        lidar_distance = data[6]  # lidar distance (mm)
        slider_value = self.sliders[0].value()  # slider[0] value
        target_slider_range = [1545, 1565]  # predefined target distance
        target_slider_value = 1551  # predefined slider value
        target_lidar_range = [400, 410]  # predefined target lidar range

        # Update LCD with lidar distance
        self.lcds[curr_index].display(lidar_distance)

        # Change LCD color based on lidar distance
        if lidar_distance < min(target_lidar_range):
            green_to_red_ratio = max(0, min(1, (min(target_lidar_range) - lidar_distance) / min(target_lidar_range)))
            color = QColor(int(255 * (green_to_red_ratio)), int(255 * (1 - green_to_red_ratio)), 0)
            self.lcds[curr_index].setStyleSheet(f"color: rgb({color.red()}, {color.green()}, {color.blue()});")

        elif lidar_distance > max(target_lidar_range):
            green_to_red_ratio = max(0, min(1, (lidar_distance - max(target_lidar_range)) / max(target_lidar_range)))
            color = QColor(int(255 * green_to_red_ratio), int(255 * (1 - green_to_red_ratio)), 0)
            self.lcds[curr_index].setStyleSheet(f"color: rgb({color.red()}, {color.green()}, {color.blue()});")
        
        else:
            self.lcds[curr_index].setStyleSheet("color: rgb(0, 255, 0);")  # Green for correct distance

        # Update dial color and value
        sensitivity = 10.0  # Higher = more sensitive color change

        if slider_value < min(target_slider_range):
            self.dials[curr_index].setValue(slider_value)  # Move left
            green_to_red_ratio = (target_slider_value - slider_value) * sensitivity / target_slider_value
            green_to_red_ratio = max(0, min(1, green_to_red_ratio))
            color = QColor(int(255 * green_to_red_ratio), int(255 * (1 - green_to_red_ratio)), 0)
            self.dials[curr_index].setDialColor(color)

        elif slider_value > max(target_slider_range):
            self.dials[curr_index].setValue(slider_value)  # Move right
            green_to_red_ratio = (slider_value - target_slider_value) * sensitivity / target_slider_value
            green_to_red_ratio = max(0, min(1, green_to_red_ratio))
            color = QColor(int(255 * green_to_red_ratio), int(255 * (1 - green_to_red_ratio)), 0)
            self.dials[curr_index].setDialColor(color)

        else:
            self.dials[curr_index].setValue(slider_value)  # Centered
            self.dials[curr_index].setDialColor(QColor(0, 255, 0))  # Green for correct distance

    def send_servo_command(self):
        pwm1, pwm2 = self.sliders[1].value(), self.sliders[0].value()
        msg = f"{pwm1},{pwm2}\n"
        # print(msg)
        self.serial_reader.serial_port.write(msg.encode())

    def process_chat_input(self):
        from core.logic import generate_chat_response
        user_input = self.chat_input.text().strip()
        if not user_input:
            return
        self.chat_display.append(f"You: {user_input}")
        self.chat_input.clear()
        response = generate_chat_response(user_input)
        self.chat_display.append(f"Cat: {response}")
    

    def start_timer_on_stm(self):
        msg = "start timer\n"
        print(msg)
        self.serial_reader.serial_port.write(msg.encode())  # Send a simple command

    def show_startup_popup(self):
        msg = QMessageBox()
        msg.setWindowTitle("Welcome")
        msg.setText("Summary of the game and what you need to do")
        msg.setIcon(QMessageBox.Icon.Information)
        msg.setStandardButtons(QMessageBox.StandardButton.Ok)
        msg.buttonClicked.connect(self.on_popup_closed)
        msg.exec()

    def on_popup_closed(self):
        self.start_timer_on_stm()

    def handle_next_planets(self):
        # 1. Save current values (from shared display) into planet_data_storage
        current_index = self.planet_index
        curr_lidar_distance = self.lcds[current_index].intValue()  # Get the current lidar distance from the main LCD
        curr_dial_value = self.dials[current_index].value()  # Get the current dial value
        curr_dial_color = self.dials[current_index].dial_color.name()  # Get the current dial color as a string

        # Store the current state in planet_data_storage
        self.planet_data_storage[current_index] = [curr_lidar_distance, curr_dial_value, curr_dial_color]
        print(f"Saved data for planet {current_index}: {self.planet_data_storage[current_index]}")

        # 2. Reset sliders
        self.sliders[0].setValue(1551)
        self.sliders[1].setValue(1551)

        # 3. Update the GUI to reflect the new planet
        self.update_gui_for_planet()

    def update_gui_for_planet(self):
        """Update the main LCD and dial to reflect the current planet."""
        current_index = self.planet_index
        # print(f"Updating GUI for planet {current_index}")

        # Retrieve the stored state from planet_data_storage
        lidar_distance, dial_value, dial_color = self.planet_data_storage[current_index]
        # print(f"Loaded data for planet {current_index}: {self.planet_data_storage[current_index]}")
        # Update the main LCD and dial with the stored state
        self.lcds[current_index].display(lidar_distance)
        self.dials[current_index].setValue(dial_value)
        self.dials[current_index].setDialColor(QColor(dial_color))
        # print(f"Updated main LCD and dial for planet {current_index}")

        # Move to the next planet
        self.planet_index += 1
        if self.planet_index >= len(self.lcds):
            self.planet_index = 0  # Wrap around to the first planet

        # print(f"Moving to planet {self.planet_index}")
    
    def handle_calculate_coordinates(self):
        # 1. Save current values (from shared display) into planet_data_storage
        current_index = self.planet_index
        curr_lidar_distance = self.lcds[current_index].intValue()  # Get the current lidar distance from the main LCD
        curr_dial_value = self.dials[current_index].value()  # Get the current dial value
        curr_dial_color = self.dials[current_index].dial_color.name()  # Get the current dial color as a string

        # Store the current state in planet_data_storage
        self.planet_data_storage[current_index] = [curr_lidar_distance, curr_dial_value, curr_dial_color]
        # print(f"Saved data for planet {current_index}: {self.planet_data_storage[current_index]}")
        
        print(self.planet_data_storage)