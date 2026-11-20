# 🐾 PawOS

### A modern systems-engineering experiment for the Commodore 64.

<p align="center">
  <img src="assets/source.png" alt="PawOS Puppy graphical Easter egg" width="700">
</p>

<p align="center">
  <strong>Reimagining what an operating-system-style environment can look like on a 1982-era computer.</strong>
</p>

---

## What is PawOS?

**PawOS** is an experimental operating-system-style environment being developed for the Commodore 64 using C and cc65.

Built around the limitations and architecture of the 6510 processor and Commodore 64 hardware, PawOS explores how modern software engineering concepts—including modular architecture, memory management, hardware interaction, disk-based resources, and extensible software—can be applied to a machine with only 64 KB of address space.

PawOS isn't an attempt to pretend the Commodore 64 is a modern computer.

It's an exploration of just how far thoughtful engineering can take one.

## ✨ Features & Subsystems

PawOS is evolving into a collection of interconnected subsystems designed to bring an OS-like experience to the Commodore 64.

| Subsystem | Description |
|---|---|
| 🖥️ Shell | Command-driven environment for interacting with PawOS. |
| 🧠 Memory | Memory inspection and system resource management tools. |
| 💾 Disk | Commodore disk operations, directory access, file handling and disk commands. |
| 🎵 SID | Sound interaction with the Commodore 64's SID chip. |
| 🎹 Synth | Experimental sound and synthesis functionality. |
| ☎️ Phone | An experimental phone-oriented subsystem. |
| 🔢 DTMF | Dual-tone multifrequency generation and functionality. |
| 📝 Editor | Integrated text editing functionality. |
| 🐾 Puppy Easter Egg | A graphical bitmap experiment using the VIC-II. |

<sub>Subsystems are actively evolving. Features, integration and architecture may change as development progresses.</sub>

---

## ⚙️ Built for the Commodore 64

PawOS is developed with the realities of the Commodore 64 hardware in mind.

- **CPU:** MOS 6510
- **Platform:** Commodore 64
- **Architecture:** 8-bit
- **Address space:** 64 KB
- **Graphics:** VIC-II
- **Audio:** SID
- **Language:** C with cc65
- **Testing:** VICE emulator
- **Development environment:** Linux / WSL

Working within a constrained environment means memory, hardware mapping, executable size, and resource placement are not abstract concerns. They are fundamental parts of the design.

---

## 🧩 Architecture & Experiments

PawOS is designed as an evolving collection of software subsystems rather than a single monolithic application.

Development currently explores:

- Modular C architecture.
- Subsystem interfaces and separation of responsibilities.
- Memory layout and resource constraints.
- VIC-II bitmap graphics and memory banking.
- SID audio experimentation.
- Disk operations and file management.
- Runtime resource loading.
- Externalizing static assets from the main executable.
- Exploring disk-backed program loading.

### Disk-backed architecture

One of the primary architectural goals is to reduce the footprint of the resident system by moving resources and potentially programs into external disk files.

The project is exploring a model in which:

- Core functionality remains resident.
- Graphics and other resources are loaded from disk when needed.
- Nonessential programs can eventually be loaded on demand.
- Memory can be reclaimed when components are no longer needed.
- The disk becomes part of the system's software architecture, rather than simply persistent storage.

This architecture is a work in progress.

---

## 🐾 VIC-II Graphics Experiment

PawOS includes a bitmap-based graphical Easter egg that experiments with the Commodore 64's VIC-II graphics hardware.

The experiment involves:

- Bitmap graphics.
- Screen matrix manipulation.
- Color RAM.
- VIC-II memory configuration.
- CPU memory banking.
- Video state restoration.
- External bitmap and color-map assets.

The graphics assets are being transitioned from embedded C arrays toward disk-based loading to reduce the resident executable's footprint.

---

## 🛠️ Building PawOS

PawOS is built using cc65.

### Requirements

- [cc65](https://cc65.github.io/)
- GNU Make
- VICE (recommended for testing)

### Build

Clone the repository:

```bash
git clone https://github.com/Barkly-Labs/pawos-c64.git
cd pawos-c64
```

Build the project:

```bash
make
```

The build system generates the PawOS executable in the project's build directory.

<sub>Build requirements and commands may evolve as the project develops.</sub>

---

## 🧪 Development philosophy

PawOS is an experimental systems project.

Development focuses on exploring ideas, testing implementations against actual hardware constraints, and evolving the architecture incrementally.

The project values:

- **Resource awareness** — working within the constraints of the target hardware.
- **Modularity** — separating subsystems and responsibilities.
- **Experimentation** — exploring unconventional approaches to retro computing.
- **Verification** — validating behavior instead of assuming it works.
- **Incremental engineering** — making changes that can be tested and understood.
- **Curiosity** — learning by building.

---

## 🚧 Project status

PawOS is actively under development.

The current focus includes subsystem development, disk functionality, memory optimization, graphics experiments, and exploring a disk-backed modular architecture.

This is an experimental project, not a finished production operating system.

---

## 🐶 Why?

Because limitations can be an invitation to engineer.

The Commodore 64 is a machine with an extraordinarily constrained environment by modern standards. Exploring operating-system-style architecture on it is an opportunity to understand software, hardware, memory, and engineering tradeoffs at a much deeper level.

PawOS is an attempt to bring modern development ideas into a classic computing environment—and to discover what is still possible.

---

<p align="center">
  <strong>🐾 PawOS — People are people, not numbers. Even in the world of computing.</strong>
</p>
