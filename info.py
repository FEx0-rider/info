import platform
import socket
import uuid
import psutil
import os
import subprocess
import requests
from datetime import datetime

# Funkce pro zjištění síťových informací
def get_network_info():
    hostname = socket.gethostname()
    try:
        ip_address = socket.gethostbyname(hostname)
    except socket.error:
        ip_address = "N/A"
    mac_address = ':'.join(['{:02x}'.format((uuid.getnode() >> elements) & 0xff) for elements in range(0, 8 * 6, 8)][::-1])
    
    # Získání veřejné IP adresy
    try:
        public_ip = requests.get("https://api.ipify.org").text
    except requests.RequestException:
        public_ip = "N/A"
    
    return hostname, ip_address, mac_address, public_ip

# Funkce pro získání informací o systému
def get_system_info():
    system_info = {
        "OS": platform.system(),
        "OS Version": platform.version(),
        "OS Release": platform.release(),
        "Node Name": platform.node(),
        "Machine": platform.machine(),
        "Processor": platform.processor(),
        "Architecture": platform.architecture()[0],
        "Boot Time": datetime.fromtimestamp(psutil.boot_time()).strftime("%Y-%m-%d %H:%M:%S"),
        "RAM Size (GB)": round(psutil.virtual_memory().total / (1024 ** 3), 2)
    }
    return system_info

# Funkce pro získání detailních informací na různých OS
def get_detailed_info():
    os_type = platform.system()
    details = {}
    try:
        if os_type == "Windows":
            details = get_windows_info()
        elif os_type == "Linux":
            details = get_linux_info()
        elif os_type == "Darwin":  # MacOS
            details = get_macos_info()
    except Exception as e:
        details = {"Error": str(e)}
    return details

# Windows specifické informace
def get_windows_info():
    try:
        import wmi
        w = wmi.WMI()
        bios = w.Win32_BIOS()[0]
        return {
            "BIOS Manufacturer": bios.Manufacturer,
            "BIOS Release Date": bios.ReleaseDate,
            "BIOS Serial Number": bios.SerialNumber
        }
    except ImportError:
        return {"BIOS Info": "wmi module not available"}

# Linux specifické informace
def get_linux_info():
    details = {}
    try:
        details["BIOS Info"] = subprocess.getoutput("sudo dmidecode -t bios")  # Vyžaduje práva root
    except Exception as e:
        details["BIOS Info"] = f"Nepodařilo se získat BIOS informace: {str(e)}"
    details["CPU Info"] = subprocess.getoutput("lscpu")
    details["Disk Info"] = subprocess.getoutput("lsblk -o NAME,SIZE,TYPE")
    return details

# MacOS specifické informace
def get_macos_info():
    details = {}
    details["BIOS Info"] = "BIOS informace nejsou na MacOS dostupné"
    details["CPU Info"] = subprocess.getoutput("sysctl -n machdep.cpu.brand_string")
    details["Disk Info"] = subprocess.getoutput("diskutil list")
    return details

# Uložení informací do souboru
def save_info_to_file(filename):
    hostname, ip_address, mac_address, public_ip = get_network_info()
    system_info = get_system_info()
    detailed_info = get_detailed_info()

    with open(filename, "w", encoding="utf-8") as file:
        file.write("=== System Information ===\n")
        for key, value in system_info.items():
            file.write(f"{key}: {value}\n")

        file.write("\n=== Network Information ===\n")
        file.write(f"Hostname: {hostname}\n")
        file.write(f"Local IP Address: {ip_address}\n")
        file.write(f"MAC Address: {mac_address}\n")
        file.write(f"Public IP Address: {public_ip}\n")

        file.write("\n=== Detailed Information ===\n")
        for key, value in detailed_info.items():
            file.write(f"{key}:\n{value}\n\n")

    print(f"Informace byly uloženy do souboru {filename}.")

# Spuštění
if __name__ == "__main__":
    FILENAME = "info_pc.txt"
    save_info_to_file(FILENAME)
