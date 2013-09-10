Strict

'version 1
' - first release working on iOS

#If TARGET = "ios"
	'ios
	Import "native/nativeui.ios.cpp"

	Const INPUT_TYPE_STRING:= 0
	Const INPUT_TYPE_INT:= 1
	Const INPUT_TYPE_FLOAT:= 2
	
	Extern
		Function InitNativeUI:Void()
		
		Function ShowPicker:Void(values:String[],value:String="")
		Function HasPickerFinished:Bool()
		Function GetPickerValue:String()
		
		Function ShowInput:Void(title:String, value:String = "", type:Int = INPUT_TYPE_STRING)
		Function ShowConfirm:Void(title:String)
		Function ShowMessage:Void(title:String)
		Function HasInputFinished:Bool()
		Function WasInputCancelled:Bool()
		Function GetInputValue:String()
	Public
#Else
	'unsupported target
	Const INPUT_TYPE_STRING:= 0
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

	Function ShowInput:Void(title:String, value:String = "", type:Int = INPUT_TYPE_STRING)
	End
	
	Function ShowConfirm:Void(title:String)
	End
	
	Function ShowMessage:Void(title:String)
	End
	
	Function HasInputFinished:Bool()
		Return True
	End
	
	Function WasInputCancelled:Bool()
		return False
	End
	
	Function GetInputValue:String()
		Return ""
	End
#EndIf