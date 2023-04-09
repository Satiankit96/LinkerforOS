"""
Design a light switch using the Command Pattern.

Command pattern has 4 components:

Receiver - The Object that will receive and execute the command
Invoker - Which will send the command to the receiver
Command Object - Itself, which implements an execute, or action method, and contains all required information to execute it
Client - The application or component which is aware of the Receiver, Invoker and Commands

Notes:
- Invoker knows nothing about the implementation of Light or other command objects.
- Invoker is responsible for executing/sending commands to the Receiver.
"""
from abc import ABC
from enum import Enum

########### Receiver ##################
class LightState(Enum):
    ON  = 1
    OFF = 0

class Light:
    def __init__(self):
        self.state = LightState.OFF
        self.brightness = 5

########### Command Objects ##################
# Command Interface
class CommandInterface(ABC):
    def execute(self):
        pass

# Concrete Command classes
class TurnONLight(CommandInterface):
    def __init__(self, light):
        self.light = light
    def execute(self):
        print("Light is ON")
        self.light = LightState.ON

class IncreaseBrightness(CommandInterface):
    def __init__(self, light, increase):
        self.light = light
        self.increase = increase
    def execute(self):
        old_brightness = self.light.brightness
        self.light.brightness += self.increase
        print(f"Increased brightness from {old_brightness} to {self.light.brightness}")

class DecreaseBrightness(CommandInterface):
    def __init__(self, light, decrease):
        self.light = light
        self.decrease = decrease
    def execute(self):
        old_brightness = self.light.brightness
        self.light.brightness -= self.decrease
        print(f"Reduced brightness from {old_brightness} to {self.light.brightness}")

class TurnOFFLight(CommandInterface):
    def __init__(self, light):
        self.light = light
    def execute(self):
        print("Light is OFF")
        self.light = LightState.OFF

########### Invoker ##################
class SwitchBoard:
    def __init__(self):
        self.commands = []
    def add(self, command):
        self.commands.append(command)
    def execute(self):
        for cmd in self.commands:
            cmd.execute()
        self.commands.clear() # ignoring exception handling

########### Client code ###########
light = Light() # Receiver XXX: We can use Factory Pattern to create Light objects

# Command objects
turn_light_on         = TurnONLight(light)
increase_brightness   = IncreaseBrightness(light, increase=2)
decrease_brightness   = DecreaseBrightness(light, decrease=2)
turn_light_off        = TurnOFFLight(light)

# Delegate calls to Invoker.
switch_board = SwitchBoard() # Invoker
switch_board.add(turn_light_on)
switch_board.add(increase_brightness)
switch_board.add(decrease_brightness)
switch_board.add(decrease_brightness)
switch_board.add(turn_light_off)
switch_board.execute()



