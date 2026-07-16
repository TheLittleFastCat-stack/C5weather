from ba63usb import BA63USB

devices = BA63USB.get()
display = BA63USB(devices[0]["path"])

display.clear()
display.set_cursor(1, 1)
display.print("Hello")