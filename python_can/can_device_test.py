from can.interfaces.slcan import slcanBus
from can import Message
import time
msg = Message(arbitration_id=0x666,dlc=8,data=[11,22,33,44,55,66,77,88],is_extended_id=False)

can_dev = slcanBus(channel="COM7",bitrate=500000)

while True:
    try:
        can_dev.send(msg=msg)
        rec_msg = can_dev.recv(0.01)
        if rec_msg:
            print(rec_msg)
    except KeyboardInterrupt:
        can_dev.shutdown()
        break