
import android.app.AlertDialog;

class NativeUINative {
	public static void InitNative() {
	}
	
	public static void ShowPickerNative(String[] values,String value) {
	}
	
	public static boolean HasPickerFinishedNative() {
		return true;
	}
	
	public static String GetPickerValueNative() {
		return new String("");
	}
	
	public static void ShowInputNative(String rawTitle, String rawValue, int type) {
	}
	
	public static void ShowConfirmNative(String rawTitle) {
	}
	
	public static void ShowMessageNative(String rawMessage,String rawTitle) {
		// --- open an alert box ---

		//show the message
		AlertDialog popup = new AlertDialog.Builder(BBAndroidGame.AndroidGame().GetActivity()).create();  
		popup.setCancelable(false); // This blocks the 'BACK' button  
		popup.setMessage(rawMessage);  
		popup.setButton("OK", new DialogInterface.OnClickListener() {  
		    @Override  
		    public void onClick(DialogInterface dialog, int which) {  
		        dialog.dismiss();                      
		    }  
		});  
		popup.show();
	}
	
	public static boolean HasInputFinishedNative() {
		return true;
	}
	
	public static boolean WasInputCancelledNative() {
		return true;
	}
	
	public static String GetInputValueNative() {
		return new String("");
	}
}
