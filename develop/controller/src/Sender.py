import bge
import serial
import ctypes
import PathLoad

msgTitle = "WarDragon"
msgSent = "Sent!"
msgError = "Failed to send message.\nPlease check UART connection."

def convert(num):
    dword=[]
    for i in range(4):
        dword.append(num >> (i*8) & 0xFF)
    return bytes(dword)

def tx_init(ser):
    ser.baudrate = 115200
    ser.port = 'COM4'
    ser.open()

def tx_send(ser, val):
    ser.write(convert(val))
    
def tx_send_node(ser, node):
    for val in node:
        tx_send(ser, val)

def main():

    cont = bge.logic.getCurrentController()
    own = cont.owner

    mouseOver = cont.sensors["mouseOver"]
    mouseDown = cont.sensors["mouseDown"]
    if (mouseOver.positive and mouseDown.positive):
        nodes = PathLoad.getNodes()
        try:        
            hybrid = serial.Serial()
            tx_init(hybrid)
            for node in nodes:
                tx_send_node(hybrid, node)
            ctypes.windll.user32.MessageBoxW(0, msgSent, msgTitle, 0)
            hybrid.close()
        except serial.serialutil.SerialException:
            ctypes.windll.user32.MessageBoxW(0, msgError, msgTitle, 0)
        except:
            print("Unexpected error...")
            raise

main()
