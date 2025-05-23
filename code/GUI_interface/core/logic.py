import random

hints = [
        "Once you've landed on the planet, try applying pressure on different area of the slider meow!",
        "Watch out for asteroids meow!",
        "Use the sliders to rotate the distance measurement device meow!"
    ]

def generate_chat_response(user_input: str) -> str:
    if user_input.lower() == "hint":
        if hints:
            return hints.pop()
        else:
            return "No more hints available meow!"
    else:
        return "Meow" + " meow" * random.randint(0, 4) + "."

