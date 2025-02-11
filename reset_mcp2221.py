#!/usr/bin/env python3
import hid

mcp2221 = hid.device()
mcp2221.open( 0x4d8, 0xdd )

reset = b"\x70\xAB\xCD\xEF"

buff64 = b"\0" + reset + b"\0" * (64-len(reset))

mcp2221.write(buff64)

mcp2221.close()
