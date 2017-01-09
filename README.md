# Proto11

Proto11 is an proof-of-concept `protoc` backend (Protocol Buffers compiler) that
emits modern C++11 code. It aims to be compatible with all other Protocol
Buffers implementations.

## Warning to Readers

This code is nowhere near ready to use. Many features are not yet implemented.

The following are implemented:

- Compiling simple Protocol Buffers `.proto` files. No `service` support, just
  messages and enums.
- Encoding/decoding of message types.
- Basic compile-time message introspection.

If it isn't listed above, it probably isn't implemented (yet).