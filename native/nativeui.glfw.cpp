void InitNativeUI() {
}

void ShowPicker(Array<String > values,String value) {
}

bool HasPickerFinished() {
	return true;
}

String GetPickerValue() {
	return String("");
}

void ShowInput(String rawTitle, String rawValue, int type) {
}

void ShowConfirm(String rawTitle) {
}

void ShowMessage(String rawMessage,String rawTitle=NULL) {
	// --- open an alert box ---
	//show the message
	MessageBoxW(HWND_DESKTOP,(LPWSTR)(const wchar_t*)rawMessage.ToCString<wchar_t>(),(LPWSTR)(const wchar_t*)rawTitle.ToCString<wchar_t>(),MB_OK);
}

bool HasInputFinished() {
	return true;
}

bool WasInputCancelled() {
	return true;
}

String GetInputValue() {
	return String("");
}