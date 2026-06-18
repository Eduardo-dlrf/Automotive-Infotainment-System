# RTOS-Based Distributed Automotive Infotainment System
![Platform](https://img.shields.io/badge/Platform-STM32F103-blue?style=flat-square&logo=stmicroelectronics)
![Kernel](https://img.shields.io/badge/Kernel-FreeRTOS-green?style=flat-square)
![Languages](https://img.shields.io/badge/Languages-C%20%2F%20Python-yellow?style=flat-square)
![Architecture](https://img.shields.io/badge/Architecture-Client--Server%20(UART)-orange?style=flat-square)

An embedded, distributed client-server infotainment prototype designed to mirror the multi-threaded operation of modern automotive vehicle consoles. This project was developed during my **4th semester of Mechatronics Engineering**, focusing on software concurrency, deterministic scheduling, and systems integration over fundamental hardware components.

## 📊 Project Metadata
* **Academic Timeline:** Sophomore Year / 4th Semester Project (June 2022)
* **Engineering Discipline:** Mechatronics Engineering (Embedded Software Systems Focus)
* **System Difficulty Level:** Intermediate-Advanced (High-abstraction software architecture mapped over basic hardware constraints)

---

## 🛠️ System Architecture & Hardware Topology

The platform breaks away from basic linear looping execution, adopting a distributed hardware topology linked via full-duplex asynchronous serial communication (UART @ 9600 bps):

* **Client Node (STM32 MCU):** Acts as the local edge-processor for physical I/O telemetry. It processes matrix keypad inputs via low-level register configuration and drives an alphanumeric 16x2 LCD via a synchronous 4-bit parallel data bus.
* **Server Node (Raspberry Pi 3 SBC):** Handles the high-level multimedia subsystem. It hosts an `.ogg` multimedia audio library on flash memory, managed dynamically by a Python control daemon.
* **Discrete Analog Audio Path:** Embedded signal amplification path utilizing a low-noise discrete bipolar junction transistor (BJT 2N2222A) configuration. Integrated with a heavy-duty electrolytic capacitor network (2200 µF) designed for transient power smoothing and acoustic DC-blocking, driving a high-efficiency hardware speaker.
* **Enclosure:** Custom robust mechanical protective case modeled entirely in SolidWorks and engineered with optimized airflow pathways for active fan cooling mounts.

---

## 💻 Firmware Design (FreeRTOS Multi-Threading & Concurrency)

The core value of this project lies in its **software architecture**. To guarantee standard automotive deterministic execution and eliminate processor-blocking states during asynchronous packet transmission, the micro-controller's firmware implements a **FreeRTOS (CMSIS-RTOS v1)** scheduling kernel. 

The execution pipeline is decoupled across **4 distinct tasks (Threads)**, structurally isolated through **2 thread-safe message queues (Queues)** to prevent race conditions:

### Task Execution Architecture:
1. **`dTask1` (Priority: Above Normal):** Actively scans rows and lines on the 4x4 keypad matrix using sequential line driving and debouncing software delays. Enqueues the raw keystroke matrices into `TaskQueue`.
2. **`dTask2` (Priority: Normal):** Listens synchronously to `TaskQueue`. Converts internal hardware key-index codes into formatted ASCII control characters and dispatches them instantly through the UART1 peripheral data register.
3. **`dTask3` (Priority: Below Normal):** Monitors the hardware USART1 RX buffer for telemetry stream bytes echoed back from the server engine. Pipes verified tracking bytes down to `TaskQueueuwu`.
4. **`dTask4` (Priority: Low):** Evaluates packet inputs incoming from `TaskQueueuwu`. Clears and overwrites character structures onto the LCD via high-speed, 4-bit timing command functions.

---

## 🐍 Media Server Logic (Python Subsystem)

The Linux environment processes multimedia application streams simultaneously through a continuous Python event loop located in `Server/audio.py`:
* **Audio Subsystem Controls:** Leverages `pygame.mixer` APIs to execute dynamic tracking controls. Maps numerical inputs (`0-9`) directly to individual audio indices, maps `A`/`B` inputs to strict playback toggle blocks (Pause/Unpause), maps `C`/`D` keys to skip index functions, and loops math assignments across volume increments (`+= 0.1`) corresponding to `#` and `*` inputs.
* **Full-Stack Web Integration:** Integrates an asynchronous REST architecture micro-service to trigger tracking commands and outputs a polished, responsive web-based visual dashboard showing active project status metadata alongside current track profiles.

---

## 🔍 Engineering Retrospective & Future Work (Post-Years Evaluation)

Looking back at this platform from a post-graduate perspective, the architecture stands as a solid foundation for concurrent systems, but it reveals clear constraints when compared against production-grade automotive standards. To preserve the historical integrity of this sophomore-year milestone, **the codebase remains completely unaltered**, but the following architectural areas of opportunity have been identified for future industrial iterations:

### 1. CPU Efficiency: Polling vs. Interrupt-Driven I/O
* **As-Is:** `dTask1` actively scans the 4x4 matrix keypad via software polling loops under an *Above Normal* scheduling priority. Without precise blocking states, this risks CPU starvation for lower-priority tasks (like LCD rendering in `dTask4`).
* **Production Approach:** Transition the matrix keypad to an **External Interrupt (EXTI)** configuration on the STM32. The micro-controller should remain in a low-power state or yield the processor entirely until a hardware edge-trigger (button press) wakes the context, maximizing CPU efficiency.

### 2. Data Integrity: Raw Serial Data vs. Packet Framing & CRCs
* **As-Is:** The UART interface transmits raw, single-byte ASCII characters directly over the wire. This leaves the distributed system highly vulnerable to electromagnetic interference (EMI) generated by the adjacent discrete analog audio amplifier loop.
* **Production Approach:** Implement a robust serialization protocol featuring **Packet Framing** (Start/End of frame delimiters, Payload Length, Message ID) paired with a **Cyclic Redundancy Check (CRC-8 or Checksum)**. This ensures that any corrupted telemetry packet over the bus is instantly dropped and re-requested.

### 3. Network Scalability: Point-to-Point UART vs. CAN/LIN Bus
* **As-Is:** The system relies on a rigid, point-to-point full-duplex USART configuration between two isolated processing units.
* **Production Approach:** Upgrade the physical and data link layers to standard automotive multiplexed buses like **CAN (Controller Area Network)** for high-priority chassis control data and **LIN (Local Interconnect Network)** for comfort/dashboard telemetry, enabling seamless modular scaling.

### 4. Software Decoupling & Configuration Management
* **As-Is:** The server script (`audio.py`) features hardcoded absolute OS directory paths and concrete hardware serial device bindings (`/dev/ttyS0`), and contains a legacy multi-byte string condition (`'-4'`) that remains unreachable due to the single-byte streaming nature of the UART reader loop.
* **Production Approach:** Refactor application configurations into external environment structures (`.env` or JSON files) or launch arguments (`argparse`). Decouple the low-level hardware abstraction layer (HAL) from the core Python media player module to allow platform-agnostic deployments.
