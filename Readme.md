# KWX Software Assessment

A simple ZeroMQ **RADIO/DISH** sender and receiver that exchange drone state and pose over UDP multicast.

## Build

From the repository root:

```bash
cmake -S . -B build
cmake --build build
```

> **Note:** `CMakeLists.txt` builds libzmq from source with draft APIs enabled. A clean rebuild (wiping `build/`) will take longer on the first run.

## Run with GitHub Codespaces

1. Sign in to GitHub and open the repository.
2. From the menu (upper left), select **Codespaces** → **New codespace**.
3. Choose the `mdepiro/kwx-sw-assessment` repository; leave the remaining options at their defaults.
4. In the Codespaces terminal, follow the [Build](#build) steps above.
