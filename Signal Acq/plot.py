import matplotlib.pyplot as plt
from tkinter import filedialog, Tk

# Hide the root window
root = Tk()
root.withdraw()

# Prompt user to select a file
file_path = filedialog.askopenfilename(
    title="Select Samples File",
    filetypes=(("Text files", "*.txt"), ("All files", "*.*"))
)

if not file_path:  # If user cancels the dialog
    print("No file selected.")
    exit()

# Read samples from the selected file
try:
    with open(file_path, 'r') as f:
        samples = [float(line.strip()) for line in f]
except FileNotFoundError:
    print(f"Error: File '{file_path}' not found.")
    exit()
except ValueError:
    print(f"Error: Invalid data in '{file_path}'. All lines must be numbers.")
    exit()

# Create time axis (assuming 125 MS/s sampling rate with decimation 8)
sample_rate = 125e6 / 8  # 15.625 MS/s
time = [i / sample_rate for i in range(len(samples))]

# Plot
plt.plot(time, samples)
plt.xlabel('Time (s)')
plt.ylabel('Voltage (V)')
plt.title('Red Pitaya Signal')
plt.grid(True)
plt.show()

# Destroy the root window after use
root.destroy()