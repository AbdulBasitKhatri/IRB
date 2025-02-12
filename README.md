# IRB

This project is a **Universal IR Blaster** built using **NodeMCU (ESP8266)** and the **IRremote library**. It enables you to **capture**, **save**, and **blast** infrared (IR) codes with ease through a **web-based interface**. The system integrates both an **IR receiver** and **transmitter**, allowing users to manage and control IR-based devices remotely.

## Features

- **IR Receiver & Transmitter**: Capture IR signals and replay them whenever you want.
- **Web UI**: Easy-to-use interface for downloading, storing, and blasting IR codes.
- **JSON-based Code Storage**: IR codes are stored in JSON format, enabling users to save and load codes for later use.
- **Customizable**: Can be adapted to work with a wide range of IR protocols such as NEC, Samsung, Sony, Panasonic, and more.
- **Wi-Fi Support**: Wi-Fi functionality powered by **ESP8266** to host a local server for web interaction.

## Requirements

- **Hardware**:
  - NodeMCU ESP8266
  - IR Receiver (e.g., TSOP38238)
  - IR LED (for transmitting signals)
  
- **Software**:
  - **Arduino IDE**
  - **IRremote library** for IR control
  - **ESP8266 Board Support** in Arduino IDE
  - **ArduinoJson library** for managing JSON data

## Setup & Installation

### 1. Hardware Setup
- Connect the **IR Receiver** to the NodeMCU:
  - VCC → 3V3
  - GND → GND
  - OUT → D5 (GPIO pin)
  
- Connect the **IR LED** for transmitting:
  - Anode (+) → D6 (GPIO pin)
  - Cathode (-) → GND

### 2. Arduino IDE Setup
- Install the **ESP8266 Board Support** in the **Arduino IDE** via the Board Manager.
- Install the **IRremote** and **ArduinoJson** libraries in Arduino IDE:
  - Go to **Sketch** → **Include Library** → **Manage Libraries**.
  - Search for and install **IRremote** and **ArduinoJson**.

### 3. Upload the Code
- Select the correct board (**NodeMCU (ESP8266)**) and port in the Arduino IDE.
- Upload the provided sketch to the NodeMCU.

### 4. Accessing the Web UI
- After uploading, the NodeMCU will create a Wi-Fi access point.
- Connect your device to the **IRB** Wi-Fi network (password: **IRHACKED**).
- Open a web browser and navigate to `192.168.4.1` to access the **web interface**.

## Usage

### Capturing IR Codes
- Point your IR remote at the receiver and press a button.
- The captured IR code will be automatically saved and shown in the web UI as a JSON file.

### Storing IR Codes
- IR codes can be saved in **JSON format**. You can download the captured IR code file for later use.

### Blasting IR Codes
- Load the saved JSON IR code(s) to the web UI.
- Select the code you want to blast and click the **Blast** button to transmit the IR signal to your device.

### Stopping Blasting
- Click the **Stop** button on the web UI to stop blasting the IR code.

## Code Breakdown

### Key Functions:
- **`storeCode(IRData* aIRReceivedData)`**: Captures and stores the received IR code into a structured format (IR data).
- **`sendCode()`**: Sends the stored IR code using the IR LED, supporting different protocols like NEC, Samsung, Sony, etc.
- **`handleRoot()`**: Serves the main HTML page for the web interface.
- **`handleBlast()`**: Toggles blasting on or off based on the user request.
- **`handleLoadCode()`**: Loads the IR code from the provided JSON data and prepares it for blasting.
- **`fromIR()` and `toIR()`**: Functions that convert between IR code data and JSON format for easy storage and retrieval.

### IR Protocols Supported:
- **Raw**, **NEC**, **SAMSUNG**, **SONY**, **PANASONIC**, **DENON**, **SHARP**, **LG**, **JVC**, **RC5**, **RC6**, **ONKYO**, **APPLE**, **BOSEWAVE**, **LEGO_PF**.

## Troubleshooting

- **Web UI Not Accessible**: Ensure that your device is connected to the **IRB** Wi-Fi network and try accessing `192.168.4.1`.
- **IR Code Not Captured**: Ensure the IR receiver is properly connected and within range of your remote control.
- **IR Blasting Not Working**: Check the wiring of the IR LED and ensure it is connected to the correct GPIO pin.

## Contributing

Feel free to fork the project and open issues or pull requests. Contributions are always welcome!

## License

This project is licensed under the MIT License.
