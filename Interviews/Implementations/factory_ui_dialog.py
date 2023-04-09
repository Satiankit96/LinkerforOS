"""
From https://refactoring.guru/oo_design-patterns/factory-method

Create a Dialog class having methods createButton and render.
The Dialog class should behave depending on the sys.platform (e.g., linux, win64)
"""
from abc import ABC, abstractmethod
from enum import Enum

import sys

class DialogInterface(ABC):
    @abstractmethod
    def createButton(self):
        pass

    @abstractmethod
    def render(self):
        pass

class MacDialog(DialogInterface):

    platform_name = "mac"

    def createButton(self):
        print(f"This creates Button on {self.platform_name}")

    def render(self):
        print(f"Render button on {self.platform_name}")

class LinuxDialog(DialogInterface):

    platform_name = "linux"

    def createButton(self):
        print(f"This creates Button on {self.platform_name}")

    def render(self):
        print(f"Render button on {self.platform_name}")


class DialogTypes(Enum):
    DARWIN  = 0 # Mac
    LINUX   = 1

class DialogFactoryInterface(ABC):
    @staticmethod
    @abstractmethod
    def create(type):
        pass

class DialogFactory(DialogFactoryInterface):
    @staticmethod
    def create(type):
        assert type in DialogTypes
        if type is DialogTypes.DARWIN:
            return MacDialog()
        if type is DialogTypes.LINUX:
            return LinuxDialog()

########### Client code ##################
if sys.platform == "linux":
    dialog = DialogFactory.create(DialogTypes.LINUX)
elif sys.platform == "darwin":
    dialog = DialogFactory.create(DialogTypes.DARWIN)
else:
    print("Unknown platform")
    sys.exit(1)

dialog.createButton()
dialog.render()
