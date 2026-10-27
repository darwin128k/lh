# lh documentation

Every guide in this project, in one list. The [README](../README.md) is the
project overview: what the library is, how to build it and what it contains.
This page is the map of the detailed guides.

## Integration guides

How the library is added to a host project, per target. Start with
[usage.md](usage.md) for the model and the option matrix.

| Guide | Target | What it covers |
|---|---|---|
| [usage.md](usage.md) | All three | The model, adding lh with CMake, the option matrix, common mistakes |
| [usage-embedded.md](usage-embedded.md) | Microcontroller, no OS (STM32F407 + ILI9341) | Build, runtime, display, present sink, text, UI tree |
| [usage-desktop.md](usage-desktop.md) | Desktop window (Win32 GDI, X11) | Build, window and loop, canvas, UI tree, input, diagnostics |
| [usage-web.md](usage-web.md) | Browser | The web host, hooks, scaling, frames without a server, limits |

## Subsystem guides

One guide per subsystem that has behaviour worth stating in prose.

| Guide | Subsystem | What it covers |
|---|---|---|
| [ui.md](ui.md) | UI model (`lh/ui/`) | The entity tree, canvas and view, input, sinks, controllers, recipes per target |
| [module-loader.md](module-loader.md) | Modules (`lh/os/module.h`) | The loader tree, loading and unloading, shared images, errors, writing a module |
| [concurrency.md](concurrency.md) | OS threads (`lh/os/thread.h`) | Threads, mutexes, conditions, monotonic time, backend notes |
| [net-sockets.md](net-sockets.md) | OS sockets (`lh/os/net/socket.h`) | Non-blocking servers, handle inheritance, pending I/O and backend guarantees |
| [http.md](http.md) | HTTP/1 head parser (`lh/http.h`) | Parsing a message head, view lifetime, framing, syntax, limits |
| [renderer3d.md](renderer3d.md) | Software 3D renderer (`lh/ui/renderer3d.h`) | Camera setup, drawing, limits, relation to the GPU sink |

## Project files

| File | What it covers |
|---|---|
| [../README.md](../README.md) | Overview, features, build options, performance, safety conventions |
| [../CHANGELOG.md](../CHANGELOG.md) | Release history, following [Keep a Changelog](https://keepachangelog.com/en/1.1.0/) |
| [../LICENSE.md](../LICENSE.md) | MIT license and third-party notices |
| [../scripts/README.md](../scripts/README.md) | Build scripts, fonts, images, the CSS-to-declaration converter |
| [../tools/README.md](../tools/README.md) | Code generation tools and data tables |

## API reference

The generated reference is built from the sources themselves:

```sh
cmake --build build --target lh_docs
```

The HTML output is written to `docs/html/index.html`. It covers every public
header under `include/lh/` and every implementation file under `src/lh/`.

- [Background worker and SPSC queue](worker-queue.md) - bounded jobs and fixed-record buffering.
