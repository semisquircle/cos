use once_cell::sync::Lazy;
use serialport::{SerialPort, self};
use std::io::{BufRead, BufReader};
use std::sync::{Arc, Mutex, atomic::{AtomicBool, Ordering}};
use tauri::Emitter;
use tokio::task;

static SERIAL_PORT: Lazy<Mutex<Option<Box<dyn SerialPort>>>> = Lazy::new(|| Mutex::new(None));
static LOOP_RUNNING: Lazy<Arc<AtomicBool>> = Lazy::new(|| Arc::new(AtomicBool::new(false)));

#[tauri::command(rename_all = "snake_case")]
pub async fn open_serial_port(app: tauri::AppHandle, port_name: String, baud_rate: u32) -> Result<(), String> {
	task::spawn_blocking(move || {
		let port = serialport::new(port_name, baud_rate)
			.timeout(std::time::Duration::from_millis(1000))
			.open()
			.map_err(|e| e.to_string())?;

		let mut serial_lock = SERIAL_PORT.lock().unwrap();
		*serial_lock = Some(port);

		drop(serial_lock);

		let mut serial_lock = SERIAL_PORT.lock().unwrap();
		let port = serial_lock.as_mut().ok_or("Failed to open serial port")?;

		let mut reader = BufReader::new(port);
		let mut my_str = String::new();

		LOOP_RUNNING.store(true, Ordering::SeqCst);

		while LOOP_RUNNING.load(Ordering::SeqCst) {
			my_str.clear();
			match reader.read_line(&mut my_str) {
				Ok(bytes_read) if bytes_read > 0 => {
					app.emit("arduino-update", &my_str).unwrap();
				}
				Ok(_) => {}
				Err(e) => {
					eprintln!("Error reading from serial port: {}", e);
					break;
				}
			}
		}

		LOOP_RUNNING.store(false, Ordering::SeqCst);
		Ok(())
	})
	.await
	.map_err(|e| e.to_string())?
}

#[tauri::command]
pub async fn close_serial_port() -> Result<(), String> {
	task::spawn_blocking(move || {
		LOOP_RUNNING.store(false, Ordering::SeqCst);

		let mut serial_lock = SERIAL_PORT.lock().unwrap();
		*serial_lock = None;
		Ok(())
	})
	.await
	.map_err(|e| e.to_string())?
}