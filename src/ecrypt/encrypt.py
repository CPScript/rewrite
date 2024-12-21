import os
import zipfile
import pyzipper
from pathlib import Path

START_DIR = Path("C:/")

OUTPUT_ZIP_PATH = Path("C:")

PASSWORD = "put your password for the zip here"

def add_to_zip(zip_writer, file_path):
    try:
        if file_path.is_file():
            zip_writer.write(file_path, file_path.relative_to(START_DIR))
    except Exception as e:
        print(f"Error adding {file_path}: {e}")

def collect_files(directory):
    for root, dirs, files in os.walk(directory):
        for file in files:
            file_path = Path(root) / file
            add_to_zip(zip_writer, file_path)

def create_encrypted_zip():
    try:
        with pyzipper.AESZipFile(OUTPUT_ZIP_PATH, 'w', compression=zipfile.ZIP_DEFLATED) as zip_writer:
            zip_writer.setpassword(PASSWORD.encode())
            print(f"Collecting files from {START_DIR}...")
            collect_files(START_DIR)
            print(f"Backup completed and encrypted at {OUTPUT_ZIP_PATH}")
    except Exception as e:
        print(f"Error creating encrypted ZIP: {e}")

if __name__ == "__main__":
    create_encrypted_zip()
