import threading
import subprocess
import sys
import os


BASE_DIR = os.path.dirname(os.path.abspath(__file__))


def run_script(script_name):
    script_path = os.path.join(BASE_DIR, script_name)

    try:
        subprocess.run([sys.executable, script_path])
    except KeyboardInterrupt:
        pass


def main():
    engine_thread = threading.Thread(target=run_script, args=("engine.py",))
    serial_thread = threading.Thread(target=run_script, args=("serial_monitor.py",))

    engine_thread.start()
    serial_thread.start()

    try:
        engine_thread.join()
        serial_thread.join()

    except KeyboardInterrupt:
        pass

    print("Program closed.")


if __name__ == "__main__":
    main()