import serial
import time
import sys
import signal

def signal_handler(sig, frame):
    print("\nStopping audio playback...")
    sys.exit(0)

def send_audio_data():
    # Set up signal handler for Ctrl+C
    signal.signal(signal.SIGINT, signal_handler)
    
    ser = serial.Serial(
        port='/dev/ttyO4',      # UART4 on BeagleBone
        baudrate=115200,        # Match this with Tiva's baudrate
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS)
    
    SAMPLE_RATE = 8000000  # 8kHz
    SAMPLE_PERIOD = 1.0/SAMPLE_RATE
    
    try:
        while True:
            with open('dk_music.txt', 'r') as file:
                print("Starting audio playback")
                next_time = time.perf_counter()
                
                for line in file:
                    # Wait until exactly the next sample time
                    current_time = time.perf_counter()
                    if next_time > current_time:
                        time.sleep(next_time - current_time)
                    
                    value = int(line.strip())
                    ser.write(bytes([value]))
                    next_time += SAMPLE_PERIOD
                    
                print("Reached end of file - restarting")
                #time.sleep(0.1)
                
    except KeyboardInterrupt:
        print("\nStopping audio playback")
        ser.close()
                
    except Exception as e:
        print(f"Error: {e}")
    finally:
        ser.close()

if __name__ == "__main__":
    try:
        send_audio_data()
    except KeyboardInterrupt:
        print("\nExiting program")
        sys.exit(0)