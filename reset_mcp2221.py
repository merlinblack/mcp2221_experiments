#!/usr/bin/env python3

# hidapi (not hid)
import hid

mcp2221 = hid.device()
mcp2221.open(0x4D8, 0xDD)

reset = b"\x70\xab\xcd\xef"

buff64 = b"\0" + reset + b"\0" * (64 - len(reset))

mcp2221.write(buff64)

mcp2221.close()
