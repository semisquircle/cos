use std::time::Duration;
use std::sync::Arc;
use std::sync::atomic::{AtomicBool, Ordering};

use btleplug::api::{Central, CharPropFlags, Manager as _, Peripheral as _, ScanFilter};
use btleplug::platform::{Manager, Peripheral};

use tokio::sync::Mutex;
use tauri::{State, Emitter};


struct BleState {
	discovered_devices: Arc<Mutex<Vec<Peripheral>>>,
	connected_device: Arc<Mutex<Option<Peripheral>>>,
	keep_reading: Arc<AtomicBool>
}


#[tauri::command]
async fn scan_ble_devices(state: State<'_, BleState>) -> Result<Vec<String>, String> {
	let mut device_names = Vec::new();
	let manager = Manager::new().await.map_err(|e| e.to_string())?;
	let adapter_list = manager.adapters().await.map_err(|e| e.to_string())?;

	if adapter_list.is_empty() {
		return Err("No Bluetooth adapters found".to_string());
	}

	let mut discovered_devices = state.discovered_devices.lock().await;
	discovered_devices.clear();

	for adapter in adapter_list.iter() {
		adapter.start_scan(ScanFilter::default()).await.map_err(|e| e.to_string())?;
		tokio::time::sleep(Duration::from_secs(3)).await;
		let peripherals = adapter.peripherals().await.map_err(|e| e.to_string())?;

		for peripheral in peripherals.iter() {
			if let Some(properties) = peripheral.properties().await.map_err(|e| e.to_string())? {
				if let Some(local_name) = properties.local_name {
					device_names.push(local_name.clone());
					discovered_devices.push(peripheral.clone());
				}
			}
		}
	}
	Ok(device_names)
}


#[tauri::command]
async fn connect_ble_device(device_name: String, state: State<'_, BleState>) -> Result<String, String> {
	let discovered_devices = state.discovered_devices.lock().await;
	let mut connected_device = state.connected_device.lock().await;

	for peripheral in discovered_devices.iter() {
		if let Ok(Some(properties)) = peripheral.properties().await {
			if let Some(local_name) = properties.local_name {
				if local_name == device_name {
					state.keep_reading.store(true, Ordering::SeqCst);
					peripheral.connect().await.map_err(|e| e.to_string())?;
					*connected_device = Some(peripheral.clone());
					return Ok(format!("Connected to {}", device_name));
				}
			}
		}
	}
	Err("Device not found in scanned list".to_string())
}


#[tauri::command]
async fn read_ble_data(state: State<'_, BleState>, app: tauri::AppHandle) -> Result<(), String> {
	let mut connected_device = state.connected_device.lock().await;
	if let Some(peripheral) = connected_device.as_mut() {
		let characteristics = peripheral.characteristics();
		let mut char_to_read = None;

		for characteristic in characteristics.iter() {
			if characteristic.properties.contains(CharPropFlags::READ) {
				char_to_read = Some(characteristic.clone());
				break;
			}
		}

		if let Some(characteristic) = char_to_read {
			while state.keep_reading.load(Ordering::SeqCst) {
				let data = peripheral.read(&characteristic).await.map_err(|e| e.to_string())?;
				app.emit("arduino-update", String::from_utf8_lossy(&data).to_string())
					.map_err(|e| format!("Failed to emit event: {}", e))?;
			}
			Ok(())
		} else {
			return Err("No readable characteristic found".to_string());
		}
	} else {
		Err("No device connected".to_string())
	}
}


#[tauri::command]
async fn disconnect_ble_device(state: State<'_, BleState>) -> Result<String, String> {
	state.keep_reading.store(false, Ordering::SeqCst);
	let mut connected_device = state.connected_device.lock().await;
	if let Some(peripheral) = connected_device.take() {
		peripheral.disconnect().await.map_err(|e| e.to_string())?;
		Ok("Disconnected successfully".to_string())
	} else {
		Ok("No device connected".to_string())
	}
}


fn main() {
	tauri::Builder::default()
		.manage(BleState {
			discovered_devices: Arc::new(Mutex::new(Vec::new())),
			connected_device: Arc::new(Mutex::new(None)),
			keep_reading: Arc::new(AtomicBool::new(false)),
		})
		.invoke_handler(tauri::generate_handler![
			scan_ble_devices,
			connect_ble_device,
			read_ble_data,
			disconnect_ble_device
		])
		.run(tauri::generate_context!())
		.expect("error while running Tauri application");
}
