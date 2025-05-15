import random

def generate_chat_response(user_input: str) -> str:
    hints = [
        "Use the sliders to rotate the distance measurement device!\nStop when both the distance and the arrow are green meow!",
        "Once you are on the planet, try applying pressure on different area of the slider meow!"
    ]
    if user_input.lower() == "hint":
        return hints[random.randint(0,1)]
    else:
        return "Meow" + " meow" * random.randint(0, 4) + "."

