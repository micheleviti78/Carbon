#01. Hello World:

# The following python script will print the text "Hello World!" as output.
print("")
print("***************************")
print("#01. Hello World!")
print("***************************")
print("")

#*************************************************************************************
#02. Join two strings:
print("")
print("***************************")
print("#02. Join two strings")
print("***************************")
print("")

# Two words "Programming" and "Languages" are joined and "ProgrammingLanguages" is printed as output.

x = "Programming"
y = "Languages"
z = x + y
print(z)

#*************************************************************************************
#03. Format floating point in the string:
print("")
print("***************************")
print("#03. Format floating point in the string")
print("***************************")
print("")

# Use of String Formatting
x = 462.75897
print("{:5.2f}".format(x))

# Use of String Interpolation
y = 462.75897
print("%5.2f" % y)

#*************************************************************************************
#04. Raise a number to a power:
print("")
print("***************************")
print("#04. Raise a number to a power")
print("***************************")
print("")

import math
# Assign values to a and n
a = 4
n = 3

# Method 1
b = a ** n
print("%d to the power %d is %d" % (a,n,b))

# Method 2
b = pow(a,n)
print("%d to the power %d is %d" % (a,n,b))

# Method 3
b = math.pow(a,n)
print("%d to the power %d is %5.2f" % (a,n,b))

#*************************************************************************************
#05. Working with Boolean types:
print("")
print("***************************")
print("#05. Working with Boolean types")
print("***************************")
print("")

# Boolean value
x = True
print(x)

# Number to Boolean
x = 10
print(bool(x))

x = -5
print(bool(x))

x = 0
print(bool(x))

# Boolean from comparison operator
x = 6
y = 3
print(x < y)

#*************************************************************************************
#06. If else statement:
print("")
print("***************************")
print("#06. If else statement")
print("***************************")
print("")

# Assign a numeric value
x = 35

# Check the is more than 35 or not
if (x >= 35):
    print("You have passed")
else:
    print("You have not passed")

#*************************************************************************************    
#07. Using AND and OR operators:
print("")
print("***************************")
print("#07. Using AND and OR operators")
print("***************************")
print("")

# Take practical marks
x = 26
# Take theory marks
y = 46

# Check the passing condition using AND and OR operator
if (x >= 25 and y >= 45) or (x + y) >=70:
    print("You have passed")
else:
    print("You have failed")

#*************************************************************************************    
#08. Switch case statement:
print("")
print("***************************")
print("#08. Switch case statement")
print("***************************")
print("")

# Switcher for implementing switch case options
def employee_details(ID):
    switcher = {
        "5006": "Employee Name: John",
        "5008": "Employee Name: Ram",  
        "5010": "Employee Name: Mohamend",
    }
    '''The first argument will be returned if the match found and
        employee ID does not exist will be returned if no match found'''
    return switcher.get(ID, "employee ID does not exist")

# Take the employee ID
ID = '5008'
# Print the output
print(employee_details(ID))

#*************************************************************************************
#09. While Loop:
print("")
print("***************************")
print("#09. While Loop")
print("***************************")
print("")

# Initialize counter
counter = 1
# Iterate the loop 9 times
while counter < 10:
    # Print the counter value
    print ("%d" % counter)
    # Increment the counter
    counter = counter + 1
    
#*************************************************************************************
#10. Use of command-line argument:
print("")
print("***************************")
print("#10. Use of command-line argument")
print("***************************")
print("")

# Import sys module
import sys

# Total number of arguments
print('Total arguments:', len(sys.argv))

print("Argument values are:")
# Iterate command-line arguments using for loop
for i in sys.argv:
  print(i)
    
#*************************************************************************************
#11. Use of regex:
print("")
print("***************************")
print("#11. Use of regex")
print("***************************")
print("")

# Import re module
import re

# Take any string data
string = 'ASDFgh78'
# Define the searching pattern
pattern = '^[A-Z]'

# match the pattern with input value
found = re.match(pattern, string)

# Print message based on the return value
if found:
  print("The input value is started with the capital letter")
else:
  print("You have to type string start with the capital letter")

    
#*************************************************************************************
#12. Use of date format:

#from datetime import date

# Read the current date
#current_date = date.today()

# Print the formatted date
#print("Today is :%d-%d-%d" % (current_date.day,current_date.month,current_date.year))

# Set the custom date
#custom_date = date(2026, 12, 26)
#print("The date is:",custom_date)
 
#*************************************************************************************
#13. Add and remove the item from a list:
print("")
print("***************************")
print("#13. Add and remove the item from a list")
print("***************************")
print("")

# Declare a fruit list
fruits = ["Mango","Orange","Guava","Banana"]

# Insert an item in the 2nd position
fruits.insert(1, "Apple")

# Displaying list after inserting
print("The fruit list after insert:")
print(fruits)
 
# Remove an item
fruits.remove("Banana")

# Print the list after delete
print("The fruit list after delete:")
print(fruits)

#*************************************************************************************
#14. List comprehension:
print("")
print("***************************")
print("#14. List comprehension")
print("***************************")
print("")

# Create a list of characters using list comprehension
char_list = [ char for char in "Python" ]
print(char_list)

# Define a tuple of websites
websites = ("google.com","yahoo.com", "history.com", "quora.com")

# Create a list from tuple using list comprehension
site_list = [ site for site in websites ]
print(site_list)

#*************************************************************************************
#15. Slice array:
print("")
print("***************************")
print("#15.Slice array")
print("***************************")
print("")

import array as arr 

numbers = arr.array('i', [1, 2, 3, 4, 5])

copy = numbers[:3]

print(copy)

# Assign string value
#text = "Python Programming Language"

# Slice using one parameter
#sliceObj = slice(5)
#print(text[sliceObj])  

# Slice using two parameter
#sliceObj = slice(6,12)
#print(text[sliceObj])  

# Slice using three parameter
#sliceObj = slice(6,25,5)
#print(text[sliceObj])

#*************************************************************************************
#16. Add and search data in the dictionary:
print("")
print("***************************")
print("#16. Add and search data in the dictionary")
print("***************************")
print("")

# Define a dictionary
customers = {'05453':'Ram','04457':'Krishna',
'02834':'Vishnu','01655':'Shiva', '07559':'David'}

# Append a new data
customers['06934'] = 'Salomon'

print("The customer names are:")
# Print the values of the dictionary
for customer in customers:
    print(customers[customer])

# Take customer ID as input to search
name = '07559'

# Search the ID in the dictionary
for customer in customers:
    if customer == name:
        print(customers[customer])
        break

#*************************************************************************************
#17. Add and search data in the set:
print("")
print("***************************")
print("#17. Add and search data in the set")
print("***************************")
print("")

# Define the number set
numbers = {13, 10, 56, 18, 12, 44, 87}
 
# Add a new data
numbers.add(63)
# Print the set values
print(numbers)

message = "Number is not found"

# Take a number value for search
search_number = '56'
# Search the number in the set
for val in numbers:
    if val == search_number:
        message = "Number is found"
        break

print(message)

#*************************************************************************************
#18. Count items in the list:
print("")
print("***************************")
print("#18. Count items in the list")
print("***************************")
print("")

# Define the string
string = 'Python Go JavaScript HTML CSS MYSQL Python'
# Define the search string
search = 'Python'
# Store the count value
count = string.count(search)
# Print the formatted output
print("%s appears %d times" % (search, count))

#*************************************************************************************
#19. Define and call a function:
print("")
print("***************************")
print("#19. Define and call a function")
print("***************************")
print("")

# Define addition function
def addition(number1, number2):
    result = number1 + number2
    print("Addition result:",result)

# Define area function with return statement
def area(radius):
    result = 3.14 * radius * radius
    return result  

# Call addition function
addition(5, 3)
# Call area function
print("Area of the circle is",area(2))

#*************************************************************************************
#20. Use of throw and catch exception:
print("")
print("***************************")
print("#20. Use of throw and catch exception")
print("***************************")
print("")

# Try block
try:
    # Take a number
    number = int('3')
    if number % 2 == 0:
        print("Number is even")
    else:
        print("Number is odd")

# Exception block    
except (ValueError):
  # Print error message
  print("Enter a numeric value")
  
# Try block
try:
    # Take a number
    number = int('a')
    if number % 2 == 0:
        print("Number is even")
    else:
        print("Number is odd")

# Exception block    
except (ValueError):
  # Print error message
  print("Enter a numeric value")

#*************************************************************************************
#21. Define class and method:
print("")
print("***************************")
print("#21. Define class and method")
print("***************************")
print("")

# Define the class
class Employee:
    name = "Johnson"
    # Define the method
    def details(self):
        print("Post: Associate")
        print("Department: QC")
        print("Salary: $6000")

# Create the employee object    
emp = Employee()
# Print the class variable
print("Name:",emp.name)
# Call the class method
emp.details()

#*************************************************************************************
#22. Use of range function:
print("")
print("***************************")
print("#22. Use of range function")
print("***************************")
print("")

# range() with one parameter
for val in range(7):
    print(val, end='  ')
print('\n')

# range() with two parameter
for val in range(7,16):
    print(val, end='  ')
print('\n')

# range() with three parameter
for val in range(0,4,1):
    print(val, end='  ')

#*************************************************************************************
#23. Use of map function:
print("")
print("***************************")
print("#23. Use of map function")
print("***************************")
print("")

# Define the function to calculate power
def cal_power(n):    
    return x ** n

# Take the value of x
x = int('67')
# Define a tuple of numbers
numbers = [2, 6, 3]

# Calculate the x to the power n using map()
result = map(cal_power, numbers)
print(list(result))

#*************************************************************************************
#24. Use of filter function:
print("")
print("***************************")
print("#24. Use of filter function")
print("***************************")
print("")

# Define a list of participant
participant = ['Ram', 'John', 'David', 'Krishna', 'Prasad']
# Define the function to filters selected candidates
def SelectedPerson(participant):
    selected = ['John', 'Rahman', 'Ram']
    if(participant in selected):
        return True
selectedList = filter(SelectedPerson, participant)
print('The selected candidates are:')
for candidate in selectedList:
    print(candidate)

#*************************************************************************************
#25. Use of filter function:
print("")
print("***************************")
print("#25. Multithreading")
print("***************************")
print("")

import _thread
import time

def second_thread():
    index = 1
    while index < 100:
        print("Hello, I'm here in the second thread writting every second")
        index = index + 1
        
_thread.start_new_thread(second_thread, ())

time.sleep(3)

#*************************************************************************************
#26. Test from micropython embed port:
print("")
print("***************************")
print("#26. Test from micropython embed port")
print("***************************")
print("")

print('hello world!', list(x + 1 for x in range(10)))

for i in range(10):
    print('iter {:08}'.format(i))
try:
    1//0
except Exception as er:
    print('caught exception', repr(er))

#*************************************************************************************
#running garbage collector:
print("")
print("***************************")
print("running garbage collector")
print("***************************")
print("")

import gc
print('run GC collect')
gc.collect()
print('finish')
