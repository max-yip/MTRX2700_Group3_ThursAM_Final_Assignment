from PyQt6.QtWidgets import QWidget, QHBoxLayout, QMessageBox
from PyQt6.QtGui import QPixmap, QPainter, QColor
import os
import struct
import serial

from ui.left_panel import create_left_panel
from ui.center_panel import create_center_panel
from ui.right_panel import create_right_panel
from core.serial_handler import SerialReader, pack_buffer, MessageType

basedir = os.path.dirname(__file__)
basedir = os.path.abspath(os.path.join(basedir, ".."))

class SpaceControlPanel(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Spaceship Control Panel")
        self.setGeometry(100, 100, 1100, 700)
        self.bg = QPixmap(os.path.join(basedir, 'assets', 'space_bg2.jpg'))
        self.fuel = 3  # Initialize fuel to 3

        self.planet_index = 0  # Start with the first LCD and dial
        self.planet_data_storage = {0:[0,1551,''], 1:[0,1551,''], 2:[0,1551,'']}  
        # Store data for each planet index:[lidar_distance, dial_value, dial color]
        
        self.planet_target = {0:[[350,450],[1545,1565]], 1:[[350, 450],[1545,1565]], 2:[[350,450],[1545,1565]]}
        # Store target ranges for each planet index:[[lidar_range],[slider_range]]

        self.initUI()
        self.start_serial_reading('/dev/tty.usbmodem1103')

    def initUI(self):
        layout = QHBoxLayout(self)

        self.left_panel, self.fuel_bar = create_left_panel(self.fuel, self)
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
        target_slider_value = sum(self.planet_target[curr_index][1])/2  # predefined target distance
        target_lidar_range = sum(self.planet_target[curr_index][0])/2  # predefined target lidar range

        # Update LCD with lidar distance
        self.lcds[curr_index].display(lidar_distance)

        # Change LCD color based on lidar distance
        if lidar_distance < target_lidar_range:
            green_to_red_ratio = max(0, min(1, (target_lidar_range - lidar_distance) / target_lidar_range))
            color = QColor(int(255 * (green_to_red_ratio)), int(255 * (1 - green_to_red_ratio)), 0)
            self.lcds[curr_index].setStyleSheet(f"color: rgb({color.red()}, {color.green()}, {color.blue()});")

        elif lidar_distance > target_lidar_range:
            green_to_red_ratio = max(0, min(1, (lidar_distance - target_lidar_range) / target_lidar_range))
            color = QColor(int(255 * green_to_red_ratio), int(255 * (1 - green_to_red_ratio)), 0)
            self.lcds[curr_index].setStyleSheet(f"color: rgb({color.red()}, {color.green()}, {color.blue()});")
        
        else:
            self.lcds[curr_index].setStyleSheet("color: rgb(0, 255, 0);")  # Green for correct distance

        # Update dial color and value
        sensitivity = 10.0  # Higher = more sensitive color change

        if slider_value < target_slider_value:
            self.dials[curr_index].setValue(slider_value)  # Move left
            green_to_red_ratio = (target_slider_value - slider_value) * sensitivity / target_slider_value
            green_to_red_ratio = max(0, min(1, green_to_red_ratio))
            color = QColor(int(255 * green_to_red_ratio), int(255 * (1 - green_to_red_ratio)), 0)
            self.dials[curr_index].setDialColor(color)

        elif slider_value > target_slider_value:
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
        # data = struct.pack('<HH', pwm1, pwm2)
        # buffer = pack_buffer(MessageType.SERVO_PWM.value, data)
        # # Use the serial port from SerialReader
        # if hasattr(self, 'serial_reader') and self.serial_reader.serial_port and self.serial_reader.serial_port.is_open:
        #     self.serial_reader.serial_port.write(buffer)
        #     print(f"Sent servo command: {pwm1}, {pwm2}")
        # else:
        #     print("Serial port not open!")

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
        
        check_lidar1 = self.planet_data_storage[0][0] > min(self.planet_target[0][0]) and self.planet_data_storage[0][0] < max(self.planet_target[0][0])
        check_pwm1 = self.planet_data_storage[0][1] > min(self.planet_target[0][1]) and self.planet_data_storage[0][1] < max(self.planet_target[0][1])

        check_lidar2 = self.planet_data_storage[1][0] > min(self.planet_target[1][0]) and self.planet_data_storage[1][0] < max(self.planet_target[1][0])
        check_pwm2 = self.planet_data_storage[1][1] > min(self.planet_target[1][1]) and self.planet_data_storage[1][1] < max(self.planet_target[1][1])
        
        check_lidar3 =  self.planet_data_storage[2][0] > min(self.planet_target[2][0]) and self.planet_data_storage[2][0] < max(self.planet_target[2][0])
        check_pwm3 = self.planet_data_storage[2][1] > min(self.planet_target[2][1]) and self.planet_data_storage[2][1] < max(self.planet_target[2][1])

        check_all_ldrs = check_lidar1 and check_lidar2 and check_lidar3
        check_all_pwms = check_pwm1 and check_pwm2 and check_pwm3

        if check_all_ldrs and check_all_pwms:
            # create a popup message saying that the coordinates are correct
            msg = QMessageBox()
            msg.setWindowTitle("Coordinates Check")
            msg.setText("Coordinates of the hidden planet has successfully been calculated! Travelling to the next planet...")
            msg.setIcon(QMessageBox.Icon.Information)
            msg.setStandardButtons(QMessageBox.StandardButton.Ok)
            msg.buttonClicked.connect(self.on_popup_closed)
            msg.exec()
        else:
            # create a popup message saying that the coordinates are incorrect
            if self.fuel > 0:
                self.fuel -= 1
                self.fuel_bar.setValue(self.fuel)
                msg = QMessageBox()
                msg.setWindowTitle("Coordinates Check")
                msg.setText("Calculations are incorrect! You went to the wrong planet! (-1/3 fuel)")
                msg.setIcon(QMessageBox.Icon.Warning)
                msg.setStandardButtons(QMessageBox.StandardButton.Ok)
                msg.buttonClicked.connect(self.on_popup_closed)
                msg.exec()

            if self.fuel == 0:
                msg = QMessageBox()
                msg.setWindowTitle("Coordinates Check")
                msg.setText("Calculations are incorrect! You have run out of fuel!")
                msg.setIcon(QMessageBox.Icon.Warning)
                msg.setStandardButtons(QMessageBox.StandardButton.Ok)
                msg.buttonClicked.connect(self.on_popup_closed)
                msg.exec()
                #close app
                self.close()