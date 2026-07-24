##This is a simple sender + receiver application##

To build from top directory:

## Build

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

> **Note:** `CMakeLists.txt` builds libzmq from source with draft APIs enabled. A clean rebuild (wiping `build/`) will take longer on the first run.

## To open a new GitHub Codespace

1. Sign in to GitHub and open the repository.
2. From the menu (upper left), select **Codespaces** → **New codespace**.
3. Choose the `mdepiro/kwx-sw-assessment` repository; leave the remaining options at their defaults.
4. In the Codespaces terminal, follow the [Build](#build) steps above.

## Assessment Instructions:
## Use any tools, but explain your work and solution process.
## Ask questions! This is a collaboration session and there may be new 
1) Build the code. Run the sender service and the receiver service.
2) Filtering:
    2.1 In the filter.hpp is a class called MovingAverage which has a method update which returns \
        the 10-sample moving average for the given parameter. Implement this method for pose.position.z \
        in the `receiver.cpp` service.
    2.2 Build and run to see how the filter affects the z position
3) Cleanup the Receiver:
    3.1 Look at the while loop in `sender.cpp` it is clean and easy to read what steps it is doing.
    3.2 create a method `get_msg(){}` to contain the receiving message code and streamline the loop.
    HINT: in `sender.cpp` the `send_msg` method takes arguments in and returns nothing. `get_msg` will
    return a type `drone::Payload` 
