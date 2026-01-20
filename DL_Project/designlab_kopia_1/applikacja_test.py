import tkinter as tk
from tkinter import ttk
import serial
import serial.tools.list_ports
import threading
import re


class AtmegaApp:
    def __init__(self, root):
        self.root = root
        self.root.title("ATmega328PB – Stacja pogodowa")
        self.root.geometry("600x500")

        self.serial_port = None
        self.running = False

        self.build_ui()
        self.load_ports()

    # ---------------- Ogolne ----------------
    def build_ui(self):
        style = ttk.Style()
        style.configure("TLabel", font=("Segoe UI", 11))
        style.configure("Header.TLabel", font=("Segoe UI", 14, "bold"))

        main = ttk.Frame(self.root, padding=10)
        main.pack(fill="both", expand=True)

        # ---------------- PORT ----------------
        port_frame = ttk.LabelFrame(main, text="Połączenie", padding=10)
        port_frame.pack(fill="x", pady=5)

        ttk.Label(port_frame, text="Port:").pack(side="left")

        self.port_box = ttk.Combobox(port_frame, width=15)
        self.port_box.pack(side="left", padx=5)

        self.connect_btn = ttk.Button(port_frame, text="Połącz", command=self.connect)
        self.connect_btn.pack(side="left")

        self.status_label = ttk.Label(port_frame, text="Status: Rozłączono", foreground="red")
        self.status_label.pack(side="left", padx=15)

        # ---------------- DANE ----------------
        data_frame = ttk.LabelFrame(main, text="Odczytane dane", padding=10)
        data_frame.pack(fill="x", pady=5)

        self.temp_label = ttk.Label(data_frame, text="Temperatura: --- °C", style="Header.TLabel")
        self.temp_label.pack(anchor="w")

        self.hum_label = ttk.Label(data_frame, text="DS Temp: --- °C", style="Header.TLabel")
        self.hum_label.pack(anchor="w")

        self.press_label = ttk.Label(data_frame, text="Ciśnienie: --- hPa", style="Header.TLabel")
        self.press_label.pack(anchor="w")

        # ---------------- WYSYŁANIE ----------------
        send_frame = ttk.LabelFrame(main, text="Wyślij komendę", padding=10)
        send_frame.pack(fill="x", pady=5)

        self.send_entry = ttk.Entry(send_frame)
        self.send_entry.pack(side="left", fill="x", expand=True, padx=5)

        send_btn = ttk.Button(send_frame, text="Wyślij", command=self.send_data)
        send_btn.pack(side="left")

        # ---------------- LOGI ----------------
        log_frame = ttk.LabelFrame(main, text="Logi", padding=10)
        log_frame.pack(fill="both", expand=True, pady=5)

        self.log_text = tk.Text(log_frame, height=12, wrap="word", bg="#f7f7f7")
        self.log_text.pack(fill="both", expand=True)

        clear_btn = ttk.Button(log_frame, text="Wyczyść logi", command=self.clear_logs)
        clear_btn.pack(pady=5)

    # ---------------- PORTY ----------------
    def load_ports(self):
        ports = serial.tools.list_ports.comports()
        port_list = [p.device for p in ports] or ["Brak portów"]
        self.port_box["values"] = port_list
        self.port_box.current(0)

    # ---------------- POŁĄCZENIE ----------------
    def connect(self):
        port = self.port_box.get()

        try:
            self.serial_port = serial.Serial(port, 9600, timeout=1)
            self.running = True

            threading.Thread(target=self.read_serial, daemon=True).start()

            self.status_label.config(text="Status: Połączono", foreground="green")
            self.log(f"[INFO] Połączono z {port}")
        except Exception as e:
            self.log(f"[ERROR] Nie udało się połączyć: {e}")

    # ---------------- ODBIÓR ----------------
    def read_serial(self):
        while self.running:
            try:
                if self.serial_port.in_waiting:
                    line = self.serial_port.readline().decode(errors="ignore").strip()
                    if line:
                        self.log(f"[RX] {line}")
                        self.parse_data(line)
            except Exception as e:
                self.log(f"[ERROR] Błąd odczytu: {e}")
                self.running = False

    # ---------------- PARSOWANIE DANYCH ----------------
    def parse_data(self, line):
        nums = re.findall(r"\d+", line)

        # Temperatura z BMP
        if len(nums) >= 2:
            raw_temp = nums[1]  
            if len(raw_temp) > 1:
                temp = raw_temp[:-1] + "," + raw_temp[-1]  
            else:
                temp = raw_temp

            self.temp_label.config(text=f"Temperatura: {temp} °C")

        # Ciśnienie
        if len(nums) >= 3:
            self.press_label.config(text=f"Ciśnienie: {nums[2]} hPa")

        # Temperatura z DS
        if len(nums) >= 4:
            raw_ds = nums[3]
            if len(raw_ds) > 1:
                ds_temp = raw_ds[:-1] + "," + raw_ds[-1]
            else:
                ds_temp = raw_ds

            self.hum_label.config(text=f"DS Temp: {ds_temp} °C")


    # ---------------- WYSYŁANIE ----------------
    def send_data(self):
        if not self.serial_port:
            self.log("[WARN] Brak połączenia")
            return

        data = self.send_entry.get()
        if data:
            try:
                self.serial_port.write((data + "\n").encode())
                self.log(f"[TX] {data}")
            except Exception as e:
                self.log(f"[ERROR] Nie wysłano: {e}")
        else:
            self.log("[WARN] Nie wpisano żadnej komendy")

    # ---------------- LOGI ----------------
    def log(self, text):
        self.log_text.insert("end", text + "\n")
        self.log_text.see("end")

    def clear_logs(self):
        self.log_text.delete("1.0", "end")


if __name__ == "__main__":
    root = tk.Tk()
    app = AtmegaApp(root)
    root.mainloop()
