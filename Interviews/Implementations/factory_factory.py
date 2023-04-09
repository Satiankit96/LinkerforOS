# https://medium.com/@hardikpatel_6314/design-patterns-in-python-factory-c728b88603eb

## Simple Factory
# Create different types of persons - HR, Engineer, Scientist
"""
# Client code
person_factory = PersonFactory()
hr = person_factory.create("HR")
engineer = person_factory.create("Engineer")
scientist = person_factory.create("Scientist")
"""
## Solution
from abc import ABC, abstractmethod
from enum import Enum

###################### Create different types of persons ######################
# Represent different persons using a Enum
class PersonTypes(Enum):
    HR        = 0
    ENGINEER  = 1
    SCIENTIST = 2

# Create an interface for all types of persons.
class Person(ABC):
    def __init__(self, name):
        self.name = name

    @abstractmethod
    def who_are_you(self):
        pass

# Define concrete classes for all types of persons.
class HR(Person):
    def who_are_you(self):
        return f"I am {self.name}, an HR"

class Engineer(Person):
    def who_are_you(self):
        return f"I am {self.name}, an Engineer"

class Scientist(Person):
    def who_are_you(self):
        return f"I am {self.name}, a Scientist"

########### Create Person Factory interace and concrete class to pass to client ##################
# Create an interface for Factory
class PersonFactoryInterface(ABC):

    @staticmethod
    @abstractmethod
    def create(person_type, name):
        pass

# Implement Person interface
class PersonFactory(PersonFactoryInterface):

    @staticmethod
    def create(person_type, name):
        assert person_type in PersonTypes
        if person_type is PersonTypes.HR:
            return HR(name)
        elif person_type is PersonTypes.ENGINEER:
            return Engineer(name)
        elif person_type is PersonTypes.SCIENTIST:
            return Scientist(name)

########### Client code ##################
hr           = PersonFactory.create(PersonTypes.HR, "Emma")
print(hr.who_are_you())
engineer     = PersonFactory.create(PersonTypes.ENGINEER, "Saim")
print(engineer.who_are_you())
scientist    = PersonFactory.create(PersonTypes.SCIENTIST, "Albus")
print(scientist.who_are_you())
