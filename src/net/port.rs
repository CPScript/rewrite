use std::process::Command;
use std::io::{self, Write};

fn disable_all_interfaces() -> io::Result<()> {
    let output = Command::new("netsh")
        .arg("interface")
        .arg("show")
        .arg("interface")
        .output()?;

    let output_str = String::from_utf8_lossy(&output.stdout);
    for line in output_str.lines() {
        if line.contains("Enabled") {
            let parts: Vec<&str> = line.split_whitespace().collect();
            if let Some(interface_name) = parts.get(0) {
                let interface_name = *interface_name;
                if interface_name != "Loopback" {
                    println!("{}", interface_name);
                    let _ = Command::new("netsh")
                        .arg("interface")
                        .arg("set")
                        .arg("interface")
                        .arg(interface_name)
                        .arg("disable")
                        .output()?;
                }
            }
        }
    }
    Ok(())
}

fn enable_smtp_port(port: u16) -> io::Result<()> {
    // port 587 for SMTP
    println!("Allowing traffic on port {}", port);
    let rule_name = format!("Allow Port {}", port);

    // rule 
    let _ = Command::new("netsh")
        .arg("advfirewall")
        .arg("firewall")
        .arg("add")
        .arg("rule")
        .arg("name=")
        .arg(&rule_name)
        .arg("protocol=TCP")
        .arg(format!("localport={}", port))
        .arg("action=allow")
        .arg("dir=in")
        .arg("enable=yes")
        .output()?;

    Ok(())
}

fn block_all_except_port(port: u16) -> io::Result<()> {
    let _ = Command::new("netsh")
        .arg("advfirewall")
        .arg("firewall")
        .arg("set")
        .arg("rule")
        .arg("name=all")
        .arg("new")
        .arg("rule")
        .arg("action=block")
        .arg("protocol=TCP")
        .arg("dir=in")
        .arg("enable=yes")
        .output()?;

    enable_smtp_port(port)?;
  
    Ok(())
}

fn main() -> io::Result<()> {
    let email_port = 587; // you can choose either SMTP (25) or Submission (587)

    disable_all_interfaces()?;

    block_all_except_port(email_port)?;

    Ok(())
}

fn failsafe() {
    println!("An error occurred!.");

    let output = Command::new("failsafe.rs") 
        .output();

    match output {
        Ok(_) => println!("OK"),
        Err(e) => eprintln!("!", e),
    }

    exit(1);
}

    if let Err(e) = modify_registry() {
        eprintln!("Err", e);
        failsafe();
    }
