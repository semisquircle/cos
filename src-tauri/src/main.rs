#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

mod serial;
mod joycon;

fn main() {
	tauri::Builder::default()
		.invoke_handler(tauri::generate_handler![
			serial::open_serial_port,
			serial::close_serial_port,
			joycon::read_joycon
		])
		.run(tauri::generate_context!())
		.expect("Error while running Tauri application");
}
