Import("env")
from sys import platform

print("Replace MKSPIFFSTOOL with mklittlefs")
if platform == "linux" or platform == "linux2":
    env.Replace (MKSPIFFSTOOL = "mklittlefs/mklittlefs")
elif platform == "darwin":
    # OS X
    pass
elif platform == "win32":
    env.Replace (MKSPIFFSTOOL = "mklittlefs\mklittlefs.exe")
