from PyQt6.QtWidgets import QDial
from PyQt6.QtCore import Qt, QPointF, QRectF
from PyQt6.QtGui import QPainter, QColor, QPen, QBrush, QConicalGradient

class ColorfulDial(QDial):
    def __init__(self, parent=None):
        super().__init__(parent)

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)

        # Draw the dial groove
        groove_size = min(self.width(), self.height()) - 20  # leave margins
        x = (self.width() - groove_size) / 2
        y = (self.height() - groove_size) / 2
        groove_rect = QRectF(x, y, groove_size, groove_size)

        groove_color = QColor(200, 200, 200)
        painter.setPen(QPen(groove_color, 4))
        painter.setBrush(QBrush(Qt.BrushStyle.NoBrush))
        painter.drawEllipse(groove_rect)

        # Draw the dial handle as an arrow
        value_angle = (self.value() - self.minimum()) / (self.maximum() - self.minimum()) * 270 - 135
        gradient = QConicalGradient(QPointF(self.rect().center()), value_angle - 90)
        gradient.setColorAt(0, QColor(0, 255, 0))
        gradient.setColorAt(1, QColor(0, 0, 255))
        painter.setPen(Qt.PenStyle.NoPen)
        painter.setBrush(QBrush(gradient))

        handle_radius = (groove_rect.width() - 10) / 2
        arrow_size = handle_radius / 2
        painter.translate(self.rect().center())
        painter.rotate(value_angle)

        arrow_points = [
            QPointF(-arrow_size / 2, -handle_radius + arrow_size),
            QPointF(arrow_size / 2, -handle_radius + arrow_size),
            QPointF(0, -handle_radius),
        ]
        painter.drawPolygon(*arrow_points)

        painter.end()