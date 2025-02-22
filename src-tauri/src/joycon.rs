use joycon_rs::prelude::*;
use tauri::Emitter;

#[tauri::command]
pub async fn read_joycon(app: tauri::AppHandle) {	
	let manager = JoyConManager::get_instance();
	
	let devices = {
		let lock = manager.lock();
		match lock {
			Ok(manager) => manager.new_devices(),
			Err(_) => return,
		}
	};
	
	devices.iter()
		.flat_map(|device| SimpleJoyConDriver::new(&device))
		.try_for_each::<_, JoyConResult<()>>(|driver| {
			let joycon = StandardFullMode::new(driver)?;
			let app_clone = app.clone();

			std::thread::spawn( move || {
				loop {
					let report = joycon.read_input_report();
					let report_str = format!("{:?}", report);
					app_clone.emit("joycon-update", &report_str).unwrap();
				}
			});
	
			Ok(())
		})
		.unwrap();
}
