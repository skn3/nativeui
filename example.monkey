Strict

Import mojo
Import skn3.nativeui

Function Main:Int()
	New MyApp
	Return 0
End

Class MyApp Extends App
	Field lastId:String
	Field buttons:= New List<Button>
	Field waiting:= False
	
	Method OnCreate:Int()
		'startup native ui
		InitNativeUI()
		
		'setup buttons
		Local cursorX:= 5
		Local cursorY:= 5
		Local width:= DeviceWidth() -10
		Local height:= 40
		Local spacing:= 8
		
		buttons.AddLast(New Button("text", "Click to show Text Input", cursorX, cursorY, width, height))
		cursorY += height + spacing
		
		buttons.AddLast(New Button("number", "Click to show Number Input", cursorX, cursorY, width, height))
		cursorY += height + spacing
		
		buttons.AddLast(New Button("message", "Click to show message", cursorX, cursorY, width, height))
		cursorY += height + spacing
		
		buttons.AddLast(New Button("confirm", "Click to show confirm", cursorX, cursorY, width, height))
		cursorY += height + spacing
		
		buttons.AddLast(New Button("picker", "Click to show Picker", cursorX, cursorY, width, height))
		cursorY += height + spacing
		
		'set monkey update rate
		SetUpdateRate(30)
		
		Return 0
	End
	
	Method OnUpdate:Int()
		'check for waiting for input
		If waiting = False
			'we are waiting for user to open some input
			If MouseHit(MOUSE_LEFT)
				For Local button:= Eachin buttons
					If MouseX() >= button.x And MouseY() >= button.y And MouseX() < button.x + button.width And MouseY() < button.y + button.height
						Select button.id
							Case "text"
								Print "opening text input"
								waiting = True
								lastId = button.id
								ShowInput("Enter Some Text", "", INPUT_TYPE_STRING)
							Case "number"
								Print "opening number input"
								waiting = True
								lastId = button.id
								ShowInput("Enter a Number", "", INPUT_TYPE_INT)
							Case "message"
								Print "opening message"
								waiting = True
								lastId = button.id
								ShowMessage("Hello World!")
							Case "confirm"
								Print "opening confirm"
								waiting = True
								lastId = button.id
								ShowConfirm("Are You Sure?")
							Case "picker"
								Print "opening picker"
								waiting = True
								lastId = button.id
								ShowPicker(["one", "two", "three", "four"], "")
						End
					EndIf
				Next
			EndIf
		Else
			'we are waiting for input to return
			Select lastId
				Case "confirm"
					If HasInputFinished()
						waiting = False
						If WasInputCancelled()
							Print "NO!"
						Else
							Print "YES!"
						Endif
					Endif
				Case "message"
					If HasInputFinished()
						waiting = False
						Print "Hello World!"
					Endif
				Case "picker"
					If HasPickerFinished()
						waiting = False
						If WasInputCancelled()
							Print "picker was cancelled"
						Else
							Print "picker value = "+GetPickerValue()
						Endif
					Endif
					
				Default 
					If HasInputFinished()
						waiting = False
						If WasInputCancelled()
							Print "input was cancelled"
						Else
							Print "input value = "+GetInputValue()
						Endif
					Endif
			End
		EndIf
		Return 0
	End
	
	Method OnRender:Int()		
		For Local button:= EachIn buttons
			SetColor(128, 128, 128)
			DrawRect(button.x, button.y, button.width, button.height)
			SetColor(255, 255, 255)
			DrawText(button.text, Int(button.x + button.width / 2 - TextWidth(button.text) / 2), Int(button.y + button.height / 2 - FontHeight() / 2))
		Next
		Return 0
	End
End

Class Button
	Field id:String
	Field text:String
	Field x:Int
	Field y:Int
	Field width:Int
	Field height:Int
	
	Method New(id:String, text:String, x:Int, y:Int, width:Int, height:Int)
		Self.id = id
		Self.text = text
		Self.x = x
		Self.y = y
		Self.width = width
		Self.height = height
	End
End