#!/usr/bin/env python3
"""A Modbus server LH did not write: pymodbus, as the reference the LH link is held to.

Serves unit 1 with 1000 holding and 1000 input registers, holding ``a`` starting at
``a + 1000`` and input ``a`` at ``a ^ 0x00FF``. Registers past 999 do not exist, so
pymodbus refuses them with "illegal data address" by its own rules, not by ours.

    reference.py rtu /dev/pts/N BAUD PARITY STOP
    reference.py tcp PORT
"""

import logging
import sys

from pymodbus import FramerType
from pymodbus.datastore import ModbusDeviceContext, ModbusSequentialDataBlock, ModbusServerContext
from pymodbus.server import StartSerialServer, StartTcpServer

SIZE = 1000


def context() -> ModbusServerContext:
    # pymodbus adds one to every address a device context is asked for (the
    # "register 1 is address 0" convention), so the blocks start at 1 to make
    # protocol address 0 the first value.
    device = ModbusDeviceContext(
        hr=ModbusSequentialDataBlock(1, [a + 1000 for a in range(SIZE)]),
        ir=ModbusSequentialDataBlock(1, [a ^ 0x00FF for a in range(SIZE)]),
    )
    return ModbusServerContext(devices={1: device}, single=False)


def main() -> None:
    logging.basicConfig(level=logging.WARNING)
    if sys.argv[1] == "rtu":
        parity = {"N": "N", "E": "E", "O": "O"}[sys.argv[4]]
        StartSerialServer(
            context=context(),
            framer=FramerType.RTU,
            port=sys.argv[2],
            baudrate=int(sys.argv[3]),
            parity=parity,
            stopbits=int(sys.argv[5]),
            bytesize=8,
        )
    else:
        StartTcpServer(context=context(), framer=FramerType.SOCKET, address=("127.0.0.1", int(sys.argv[2])))


if __name__ == "__main__":
    main()
