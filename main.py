import tkinter as tk
import serial
import threading
import time
from serial.tools import list_ports


class CircularButton(tk.Canvas):
    def __init__(self, master, **kwargs):
        tk.Canvas.__init__(self, master, **kwargs)
        self.config(width=80, height=80, bd=0, highlightthickness=0)
        self.circle = self.create_oval(5, 5, 75, 75, outline='black', width=2)
        self.text = self.create_text(40, 40, text="", font=('Helvetica', 12, 'bold'))
        self.bind("<ButtonPress-1>", self.on_press)
        self.bind("<ButtonRelease-1>", self.on_release)

    def on_press(self, event):
        self.itemconfig(self.circle, outline='red')

    def on_release(self, event):
        self.itemconfig(self.circle, outline='black')


class SerialTerminalGUI:
    def __init__(self, master, baud_rate):
        self.master = master
        self.master.title("Serial Terminal GUI")

        self.baud_rate = baud_rate

        # Serial port initialization
        self.ser = None
        self.find_serial_port()

        # GUI components
        self.text_area_received = tk.Text(self.master, wrap=tk.WORD, width=40, height=10)
        self.text_area_received.pack(padx=10, pady=5)

        self.text_area_transmitted = tk.Text(self.master, wrap=tk.WORD, width=40, height=5)
        self.text_area_transmitted.pack(padx=10, pady=5)

        self.input_entry = tk.Entry(self.master, width=30)
        self.input_entry.pack(padx=10, pady=5)

        self.send_button = CircularButton(self.master)
        self.send_button.itemconfig(self.send_button.text, text="Send")
        self.send_button.bind("<Button-1>", self.send_command)
        self.send_button.pack(pady=5)

        # Buttons for specific commands
        self.create_command_button("Turn On Green LED", "turn_on green_led", "green")
        self.create_command_button("Turn Off Green LED", "turn_off green_led", "green")

        self.create_command_button("Turn On Red LED", "turn_on red_led", "red")
        self.create_command_button("Turn Off Red LED", "turn_off red_led", "red")

        # Start a thread for reading from the serial port
        self.read_thread = threading.Thread(target=self.read_from_serial_port, daemon=True)
        self.read_thread.start()

    def find_serial_port(self):
        available_ports = [port.device for port in list_ports.comports()]
        if available_ports:
            self.ser = serial.Serial(available_ports[0], self.baud_rate, timeout=5, bytesize=serial.EIGHTBITS,
                                     parity=serial.PARITY_EVEN, stopbits=serial.STOPBITS_ONE)
            self.ser.flushInput()
            self.ser.flushOutput()
        else:
            print("No available serial ports found.")
            self.master.destroy()

    def create_command_button(self, text, command, color):
        button = CircularButton(self.master)
        button.itemconfig(button.text, text=text)
        button.configure(bg=color)
        button.bind("<Button-1>", lambda event, c=command: self.send_specific_command(c))
        button.pack(pady=5, padx=5)

    def send_command(self, _):
        if self.ser:
            command = self.input_entry.get() + '\0'  # Append the null character
            self.ser.write(command.encode('utf-8'))
            self.ser.flush()  # Flush input and output
            time.sleep(0.1)  # Introduce a small delay
            print(f"Transmitted: {command}")
            self.text_area_transmitted.insert(tk.END, f"Transmitted: {command}")

            self.input_entry.delete(0, tk.END)

    def send_specific_command(self, command):
        if self.ser:
            command += '\0'  # Append the null character
            self.ser.write(command.encode('utf-8'))
            self.ser.flush()  # Flush input and output
            time.sleep(0.1)  # Introduce a small delay
            print(f"Transmitted: {command}")
            self.text_area_transmitted.insert(tk.END, f"\nTransmitted: {command}\n")

    def read_from_serial_port(self):
        while True:
            if self.ser and self.ser.in_waiting > 0:
                received_data = self.ser.readline().decode('utf-8').rstrip('\n')
                print(f"Received: {received_data}")
                self.text_area_received.insert(tk.END, f"\nReceived: {received_data}\n")
                self.text_area_received.yview(tk.END)
            time.sleep(0.1)  # Adjust the delay based on your requirements

    def close_serial_port(self):
        if self.ser:
            self.ser.close()
        self.master.destroy()


if __name__ == "__main__":
    baud_rate = 9600

    root = tk.Tk()
    app = SerialTerminalGUI(root, baud_rate)
    root.protocol("WM_DELETE_WINDOW", app.close_serial_port)
    root.geometry("600x400")  # Set the initial size of the window
    root.mainloop()



