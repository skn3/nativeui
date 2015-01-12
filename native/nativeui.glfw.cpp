
//header
class NativeUINative {
public:
	static void InitNative();
	static void ShowPickerNative(Array<String > values,String value);
	static bool HasPickerFinishedNative();
	static String GetPickerValueNative();
	static int GetPickerIndexNative();
	static void ShowInputNative(String rawTitle, String rawValue, int type);
	static void ShowConfirmNative(String rawTitle);
	static void ShowMessageNative(String rawMessage,String rawTitle);
	static bool HasInputFinishedNative();
	static bool WasInputCancelledNative();
	static String GetInputValueNative();
};

//body
void NativeUINative::InitNative() {
}

void NativeUINative::ShowPickerNative(Array<String > values,String value) {
}

bool NativeUINative::HasPickerFinishedNative() {
	return true;
}

String NativeUINative::GetPickerValueNative() {
	return String("");
}

int NativeUINative::GetPickerIndexNative() {
	return -1;
}

void NativeUINative::ShowInputNative(String rawTitle, String rawValue, int type) {
}

void NativeUINative::ShowConfirmNative(String rawTitle) {
}

void NativeUINative::ShowMessageNative(String rawMessage,String rawTitle) {
	// --- open an alert box ---
	//show the message
	MessageBoxW(HWND_DESKTOP,(LPWSTR)(const wchar_t*)rawMessage.ToCString<wchar_t>(),(LPWSTR)(const wchar_t*)rawTitle.ToCString<wchar_t>(),MB_OK);
}

bool NativeUINative::HasInputFinishedNative() {
	return true;
}

bool NativeUINative::WasInputCancelledNative() {
	return true;
}

String NativeUINative::GetInputValueNative() {
	return String("");
}
