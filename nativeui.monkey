Strict

'This module was backed by CopperCircle, you should thank him with praise and gifts a plenty!

'version 3
' - added code to force all android with ui are run on ui thread.
' - added solution to repo
' - implemented lee's changes to android
' - added GetPickerIndex to find out the array index of teh selected item
' - added android native code for handling one picker/input/message at a time
' - tweaked lees text input code to clean things up
' - added picker routine
' - added confirm ok/cancel routine
' - tweaked existing message box routine
' - added code to handle "finished/done" being pressed when input box is showing
'version 2
' - refactored all native files to use different structure
' - added support for android ShowMessage
'version 1
' - first release working on iOS

#If TARGET = "ios" Or TARGET = "glfw" Or TARGET = "android"
	'ios
	Import "native/nativeui.${TARGET}.${LANG}"

	Const INPUT_TYPE_STRING:= 0
	Const INPUT_TYPE_INT:= 1
	Const INPUT_TYPE_FLOAT:= 2
	
	#If TARGET = "android"
		Extern
			Function InitNativeUI:Void() = "NativeUINative.InitNative"
			
			Function ShowPicker:Void(values:String[], value:String = "") = "NativeUINative.ShowPickerNative"
			Function HasPickerFinished:Bool() = "NativeUINative.HasPickerFinishedNative"
			Function GetPickerValue:String() = "NativeUINative.GetPickerValueNative"
			Function GetPickerIndex:Int() = "NativeUINative.GetPickerIndexNative"
				
			Function ShowDatePicker:Void(type:String) = "NativeUINative.ShowDatePickerNative"
			Function HasDatePickerFinished:Bool() = "NativeUINative.HasDatePickerFinishedNative"
			Function GetDatePickerValue:String() = "NativeUINative.GetDatePickerValueNative"
			
			Function ShowInput:Void(title:String, value:String = "", type:Int = INPUT_TYPE_STRING) = "NativeUINative.ShowInputNative"
			Function ShowConfirm:Void(title:String) = "NativeUINative.ShowConfirmNative"
			Function ShowMessage:Void(message:String, title:String = "", button:String = "OK") = "NativeUINative.ShowMessageNative"
			Function HasInputFinished:Bool() = "NativeUINative.HasInputFinishedNative"
			Function WasInputCancelled:Bool() = "NativeUINative.WasInputCancelledNative"
			Function GetInputValue:String() = "NativeUINative.GetInputValueNative"
		Public
	#Else
		Extern
			Function InitNativeUI:Void() = "NativeUINative::InitNative"
				
			Function ShowPicker:Void(values:String[], value:String = "") = "NativeUINative::ShowPickerNative"
			Function HasPickerFinished:Bool() = "NativeUINative::HasPickerFinishedNative"
			Function GetPickerValue:String() = "NativeUINative::GetPickerValueNative"
			Function GetPickerIndex:Int() = "NativeUINative::GetPickerIndexNative"
			
			Function ShowDatePicker:Void(type:String) = "NativeUINative::ShowDatePickerNative"
			Function HasDatePickerFinished:Bool() = "NativeUINative::HasDatePickerFinishedNative"
			Function GetDatePickerValue:String() = "NativeUINative::GetDatePickerValueNative"
			
			Function ShowInput:Void(title:String, value:String = "", type:Int = INPUT_TYPE_STRING) = "NativeUINative::ShowInputNative"
			Function ShowConfirm:Void(title:String) = "NativeUINative::ShowConfirmNative"
			Function ShowMessage:Void(message:String, title:String = "", button:String = "OK") = "NativeUINative::ShowMessageNative"
			Function HasInputFinished:Bool() = "NativeUINative::HasInputFinishedNative"
			Function WasInputCancelled:Bool() = "NativeUINative::WasInputCancelledNative"
			Function GetInputValue:String() = "NativeUINative::GetInputValueNative"
		Public
	#EndIf
#Else
	'unsupported target
	Const INPUT_TYPE_STRING := 0
	Const INPUT_TYPE_INT:= 1
	Const INPUT_TYPE_FLOAT:= 2
	
	Function InitNativeUI:Void()
	End
	
	Function ShowPicker:Void(values:String[], value:String = "")
	End
		
	Function HasPickerFinished:Bool()
		Return True
	End
	
	Function GetPickerValue:String()
		Return ""
	End
	
	Function GetPickerIndex:Int()
		Return -1
	End
	
	Function ShowDatePicker:Void(type:String)
	End

	Function HasDatePickerFinished:Bool()
		Return True
	End

	Function GetDatePickerValue:String()
		Return ""
	End

	Function ShowInput:Void(title:String, value:String = "", type:Int = INPUT_TYPE_STRING)
	End
	
	Function ShowConfirm:Void(title:String)
	End
	
	Function ShowMessage:Void(message:String, title:String = "", button:String = "OK")
	End
	
	Function HasInputFinished:Bool()
		Return True
	End
	
	Function WasInputCancelled:Bool()
		Return False
	End
	
	Function GetInputValue:String()
		Return ""
	End
#EndIf