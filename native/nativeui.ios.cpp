#define INPUT_MODE_TEXT 1
#define INPUT_MODE_CONFIRM 2
#define INPUT_MODE_MESSAGE 3

#define INPUT_TYPE_STRING 0
#define INPUT_TYPE_INT 1
#define INPUT_TYPE_FLOAT 2

// --- shared delegate ------------
@interface FuelSaverUIDelegate : NSObject<UIPickerViewDelegate,UIAlertViewDelegate,UITextFieldDelegate> {
}

@property (nonatomic, assign) NSObject *pickerLock;
@property (nonatomic, assign) UIView *pickerContainerView;
@property (nonatomic, assign) UIPickerView *pickerView;
@property (nonatomic, assign) UIView *pickerAccessoryView;
@property (nonatomic, assign) UITextField *pickerTextField;
@property (nonatomic, retain) NSMutableArray *pickerValues;
@property (nonatomic) NSUInteger pickerValuesTotal;
@property (nonatomic, retain) NSString *pickerValue;
@property (nonatomic) bool pickerActive;
@property (nonatomic) bool pickerFinished;

@property (nonatomic, assign) NSObject *inputLock;
@property (nonatomic, assign) UIAlertView *inputView;
@property (nonatomic, assign) UITextField *inputTextField;
@property (nonatomic) int inputMode;
@property (nonatomic) bool inputActive;
@property (nonatomic) bool inputFinished;
@property (nonatomic) bool inputCancelled;
@property (nonatomic, copy) NSString *inputValue;

//constructor/destructor
- (id)init;
- (void)dealloc;

//picker view
- (void)pickerView:(UIPickerView *)pickerView didSelectRow: (NSInteger)row inComponent:(NSInteger)component;
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component;
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView;
- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component;
- (CGFloat)pickerView:(UIPickerView *)pickerView widthForComponent:(NSInteger)component;
- (void)pickerDoneClicked:(id)sender;
- (void)showPicker:(NSMutableArray *)values selectedRow:(NSInteger)selectedRow;
- (bool)hasPickerFinished;
- (NSString *)getPickerValue;

//alert view
- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex;
- (void)showInput:(NSString *)title value:(NSString *)value type:(int)type;
- (void)showConfirm:(NSString *)title;
- (void)showMessage:(NSString *)title;
- (bool)hasInputFinished;
- (bool)wasInputCancelled;
- (NSString *)getInputValue;

@end

//implement delegate
@implementation FuelSaverUIDelegate
@synthesize pickerLock;
@synthesize pickerContainerView;
@synthesize pickerView;
@synthesize pickerAccessoryView;
@synthesize pickerTextField;
@synthesize pickerValues;
@synthesize pickerValuesTotal;
@synthesize pickerValue;
@synthesize pickerActive;
@synthesize pickerFinished;

@synthesize inputLock = _inputLock;
@synthesize inputView = _inputView;
@synthesize inputTextField = _inputTextField;
@synthesize inputMode = _inputMode;
@synthesize inputActive = _inputActive;
@synthesize inputFinished = _inputFinished;
@synthesize inputCancelled = _inputCancelled;
@synthesize inputValue = _inputValue;

//constructor/destructor
- (id)init {
    self = [super init];
    if (self) {
		// --- need to setup stuff which should stay ---
		self.pickerLock = [[NSObject alloc] init];
		@synchronized(pickerLock) {
			self.pickerActive = false;
			self.pickerFinished = true;
		}
		
		//get main view so we can add stuff to it
		BBMonkeyAppDelegate *appDelegate = BBIosGame::IosGame()->GetUIAppDelegate();
		UIView *mainView = [appDelegate view];
		
		// [ picker ]
		// --- create accessory view ---
		self.pickerAccessoryView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, mainView.frame.size.width, 44)];

		//create toolbar
		UIToolbar *toolbar = [[UIToolbar alloc] initWithFrame: CGRectMake(0, 0, self.pickerAccessoryView.frame.size.width, self.pickerAccessoryView.frame.size.height)];
		toolbar.barStyle = UIBarStyleBlack;
		
		//add toolbar items
		UIBarButtonItem *emptySpaceItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
		UIBarButtonItem *doneItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(pickerDoneClicked:)];
		NSArray *toolbarItems = [NSArray arrayWithObjects: emptySpaceItem, doneItem, nil];
		[toolbar setItems:toolbarItems animated:NO];
				
		//add toolbar to accessoryview
		[self.pickerAccessoryView addSubview:toolbar];
		
		//release waste
		[emptySpaceItem release];
		[doneItem release];
		[toolbar release];
		
		// --- create custom "keyboard" ---
		//create container
		self.pickerContainerView = [[UIView alloc] initWithFrame: CGRectMake ( 0, 0, mainView.frame.size.width, 216)];
		
		//add picker to container
		self.pickerView = [[UIPickerView alloc] initWithFrame:CGRectMake(0, 0,self.pickerContainerView.frame.size.width, 216)];
		self.pickerView.delegate = self;
		self.pickerView.showsSelectionIndicator = YES;
		[self.pickerContainerView addSubview:self.pickerView];
		
		// --- create textfield to handle input ---
		//need to create a hidden textinput to deal with getting value for picker
		self.pickerTextField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0,100, 25)];
		self.pickerTextField.hidden = YES;
		[self.pickerTextField setInputView:self.pickerContainerView];
		[self.pickerTextField setInputAccessoryView:self.pickerAccessoryView];
		[mainView addSubview: self.pickerTextField];
		
		
		//[ input ]
		self.inputLock = [[NSObject alloc] init];
		@synchronized(self.inputLock) {
			self.inputActive = false;
			self.inputFinished = true;
			self.inputCancelled = false;
		}
    }
    return self;
}

-(void)dealloc {
	//do need to dismiss the inputview?
	@synchronized(self.inputLock) {
		if (self.inputActive) {
			self.inputActive = false;
			[self.inputView dismissWithClickedButtonIndex:self.inputView.cancelButtonIndex animated:YES];
			
			//release stuff
			[self.inputTextField release];
			[self.inputView release];
			
			self.inputTextField = nil;
			self.inputView = nil;
		}
	}
	
	//do we need to close the picker?
	@synchronized(pickerLock) {
		if (self.pickerActive) {
			self.pickerActive = false;
			[[BBIosGame::IosGame()->GetUIAppDelegate() view] endEditing:NO];
		}
		
		//remove ui from monkey view
		[self.pickerTextField removeFromSuperview];
		
		//release stuff
		[self.pickerTextField release];
		[self.pickerAccessoryView release];
		[self.pickerView release];
		[self.pickerContainerView release];
		
		self.pickerTextField = nil;
		self.pickerAccessoryView = nil;
		self.pickerView = nil;
		self.pickerContainerView = nil;
	}
	
	//release locks
	[self.pickerLock release];
	[self.inputLock release];
	self.pickerLock = nil;
	self.inputLock = nil;
	
	//call chain
	[super dealloc];
}

//picker view protocol
- (void)pickerView:(UIPickerView *)pickerView didSelectRow: (NSInteger)row inComponent:(NSInteger)component {
    // --- Handle the selection ---
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
	// --- tell the picker how many rows are available for a given component ---
	return self.pickerValuesTotal;
}

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
	// --- tell the picker how many components it will have ---
	return 1;
}

- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
	// --- tell the picker the title for a given component ---
    return [self.pickerValues objectAtIndex:row];
}

- (CGFloat)pickerView:(UIPickerView *)pickerView widthForComponent:(NSInteger)component {
	//  --- tell the picker the width of each row for a given component ---
	int sectionWidth = 300;
	
	return sectionWidth;
}

- (void)pickerDoneClicked:(id)sender {
	// --- done has been pressed ---
	//save the value and close the picker
	NSInteger row = [self.pickerView selectedRowInComponent:0];
	
	@synchronized(pickerLock) {
		if (row == -1) {
			//no selection
			self.pickerValue = @"";
		} else {
			//has selection
			self.pickerValue = [self.pickerValues objectAtIndex:row];
		}
		
		//finish the picker
		self.pickerActive = false;
		self.pickerFinished = true;
		
		//release handle on values
		//this will release old 
		self.pickerValues = nil;
	}
	
	//close the picker
	[[BBIosGame::IosGame()->GetUIAppDelegate() view] endEditing:NO];
}

- (void)showPicker:(NSMutableArray *)values selectedRow:(NSInteger)selectedRow {
	// --- show the picker ---
	@synchronized(pickerLock) {
		//check if there is an existing session
		if (self.pickerActive == true) {
			//close previous session
			self.pickerActive = false;
			self.pickerValues = nil;
			
			//close the picker
			[[BBIosGame::IosGame()->GetUIAppDelegate() view] endEditing:NO];
		}
		
		//start new picker session
		self.pickerActive = true;
		self.pickerFinished = false;
	}
	
	//save values array
	//this will retain it
	self.pickerValues = values;
	self.pickerValuesTotal = [values count];
	
	//tell picker to reload
	[self.pickerView reloadAllComponents];
	
	//select speciffic item in picker (if specified)
	if (selectedRow > -1) {
		[self.pickerView selectRow:selectedRow inComponent:0 animated:NO];
	}
	
	//start the picker session
	[self.pickerTextField becomeFirstResponder];
}

- (bool)hasPickerFinished {
	// --- return true if the picker has finished ---
	@synchronized(pickerLock) {
		//only once
		if (self.pickerFinished == true) {
			self.pickerFinished = false;
			return true;
		}
	}
	
	//nope
	return false;
}

- (NSString *)getPickerValue {
	// --- get picker value as monkey string ---
	@synchronized(pickerLock) {
		return self.pickerValue;
	}
}



//alert view protocol
-(BOOL)textFieldShouldReturn:(UITextField *)textField{
	// --- return was pressed ---
	//dismiss the inputview
	[self.inputView dismissWithClickedButtonIndex:self.inputView.firstOtherButtonIndex animated:YES];
	return YES;
}
	
- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
	// --- the alertview has been dismissed ---
	@synchronized(self.inputLock) {
		
		//do mode speciffic stuff
		switch(self.inputMode) {
			case INPUT_MODE_TEXT:
				if (buttonIndex != self.inputView.firstOtherButtonIndex) {
					//cancel pressed
					self.inputCancelled = true;
				} else {
					//ok pressed
					self.inputCancelled = false;
					
					//extract value from it
					self.inputValue = self.inputTextField.text;
				}
				
				//free up the pointers
				//this also releases it
				self.inputTextField.delegate = nil;
				self.inputTextField = nil;
				
				break;
				
			case INPUT_MODE_CONFIRM:
				//yes or no
				if (buttonIndex != self.inputView.firstOtherButtonIndex) {
					//cancel pressed
					self.inputCancelled = true;
				} else {
					//ok pressed
					self.inputCancelled = false;
				}
				
				break;
				
			case INPUT_MODE_MESSAGE:
				//just a message
				self.inputCancelled = false;
				
				break;
				
		}
		
		//do generic stuff
		//end it
		self.inputFinished = true;
		self.inputActive = false;
		
		//free up the pointers
		//also releases it
		self.inputView.delegate = nil;
		self.inputView = nil;
	}
}

- (void)showMessage:(NSString *)title {
	// --- show teh alert view ---
	@synchronized(self.inputLock) {
		//stop old
		if (self.inputActive) {
			[self.inputView dismissWithClickedButtonIndex:[self.inputView cancelButtonIndex] animated:NO];
			[self.inputView release];
		}
		
		//start new
		self.inputMode = INPUT_MODE_MESSAGE;
		self.inputActive = true;
		self.inputFinished = false;
		self.inputCancelled = false;
	}
	
	//create new alert view
	self.inputView = [[UIAlertView alloc] initWithTitle:title message:@"" delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
	
	//show the alert
	[self.inputView show];
}

- (void)showConfirm:(NSString *)title {
	// --- show teh alert view ---
	@synchronized(self.inputLock) {
		//stop old
		if (self.inputActive) {
			[self.inputView dismissWithClickedButtonIndex:[self.inputView cancelButtonIndex] animated:NO];
			[self.inputView release];
		}
		
		//start new
		self.inputMode = INPUT_MODE_CONFIRM;
		self.inputActive = true;
		self.inputFinished = false;
		self.inputCancelled = false;
	}
	
	//create new alert view
	self.inputView = [[UIAlertView alloc] initWithTitle:title message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
	
	//show the alert
	[self.inputView show];
}

- (void)showInput:(NSString *)title value:(NSString *)value type:(int)type {
	// --- show teh alert view ---
	@synchronized(self.inputLock) {
		//stop old
		if (self.inputActive) {
			[self.inputView dismissWithClickedButtonIndex:[self.inputView cancelButtonIndex] animated:NO];
			[self.inputView release];
		}
		
		//start new
		self.inputMode = INPUT_MODE_TEXT;
		self.inputActive = true;
		self.inputFinished = false;
		self.inputCancelled = false;
		self.inputValue = value;
	}
	
	//create new alert view
	//check if this is running ios5 >= so we can use built in stuff
	NSArray *vComp = [[UIDevice currentDevice].systemVersion componentsSeparatedByString:@"."];
	if ([[vComp objectAtIndex:0] intValue] >= 5) {
		//ios5 (or greater)
		self.inputView = [[UIAlertView alloc] initWithTitle:title message:nil delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
		
		//setup style of alert view
		self.inputView.alertViewStyle = UIAlertViewStylePlainTextInput;
		
		//get textfield
		self.inputTextField = [self.inputView textFieldAtIndex:0];
		[self.inputTextField retain];
		self.inputTextField.delegate = self;
		
		//change value
		self.inputTextField.text = value;
				
	} else {
		//lower than ios5
		self.inputView = [[UIAlertView alloc] initWithTitle:title message:@"\n\n" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
		
		//create text field
		self.inputTextField = [[UITextField alloc] initWithFrame:CGRectMake(16,53,252,25)];
		if (type == INPUT_TYPE_STRING) {
			[self.inputTextField setKeyboardType:UIKeyboardTypeDefault];
		} else {
			[self.inputTextField setKeyboardType:UIKeyboardTypeNumberPad];
		}
		self.inputTextField.text = value;
		self.inputTextField.delegate = self;
		self.inputTextField.borderStyle = UITextBorderStyleRoundedRect;
		self.inputTextField.keyboardAppearance = UIKeyboardAppearanceAlert;
		[self.inputView addSubview:self.inputTextField];
	}

	//set keyboard type
	switch(type) {
		case INPUT_TYPE_STRING:
			//string
			[self.inputTextField setKeyboardType:UIKeyboardTypeDefault];
			break;
		case INPUT_TYPE_INT:
			//int
			[self.inputTextField setKeyboardType:UIKeyboardTypeNumberPad];
			break;
		case INPUT_TYPE_FLOAT:
			//float
			if ([[vComp objectAtIndex:0] intValue] >= 4 && [[vComp objectAtIndex:1] intValue] >= 1) {
				//new method
				[self.inputTextField setKeyboardType:UIKeyboardTypeDecimalPad];
			} else {
				//old method
				[self.inputTextField setKeyboardType:UIKeyboardTypeNumbersAndPunctuation];
			}
			break;
	}

	//select the textfield
	[self.inputTextField becomeFirstResponder];
	
	//show the alert
	[self.inputView show];
}

- (bool)hasInputFinished {
	// --- has input finished ---
	@synchronized(self.inputLock) {				
		//only returns true once per session
		if (self.inputFinished == true) {
			self.inputFinished = false;
			return true;
		}
	}
	
	//nope
	return false;
}

- (bool)wasInputCancelled {
	// --- was input cancelled ? ---
	@synchronized(self.inputLock) {
		return self.inputCancelled;
	}
}

- (NSString *)getInputValue {
	// --- get value ---
	@synchronized(self.inputLock) {
		switch(self.inputMode) {
			case INPUT_MODE_TEXT:
				//there is text to return
				return self.inputValue;
				
			default:
				//default to nothing
				return @"";
		}
	}
}
@end



// --- shared globals ---------
FuelSaverUIDelegate *fuelSaverUIDelegate;

void InitNativeUI() {
	// --- lets us init at start of app ---
	//make sure instance is created
	if (fuelSaverUIDelegate == nil) { fuelSaverUIDelegate = [[FuelSaverUIDelegate alloc] init]; }
}



// --- picker view glue ------------
void ShowPicker(Array<String > values,String value) {
	// --- so here we have a invisible picker object ??? ---
	//make sure instance is created
	if (fuelSaverUIDelegate == nil) { fuelSaverUIDelegate = [[FuelSaverUIDelegate alloc] init]; }
	
	//convert monkey array
	NSInteger total = values.Length();
	NSMutableArray *values2 = [NSMutableArray arrayWithCapacity:total];
	[values2 retain];
	
	//copy items in from monkey array
	NSInteger index;
	NSInteger row = -1;
	for (index = 0; index < total; index++) {
		//add to values
		[values2 addObject:values[index].ToNSString()];
		
		//check to see if this is selected item
		if (row == -1 && values[index] == value) {
			row = index;
		}
	}
	
	//call instance method
	[fuelSaverUIDelegate showPicker:values2 selectedRow:row];
	
	//release values2
	[values2 release];
}

bool HasPickerFinished() {
	// --- has picker finished ---
	//check that instance exists
	if (fuelSaverUIDelegate == nil) { return true; }
	return [fuelSaverUIDelegate hasPickerFinished];
}

String GetPickerValue() {
	// --- has picker finished ---
	//check that instance exists
	if (fuelSaverUIDelegate == nil) { return String(@""); }
	return String([fuelSaverUIDelegate getPickerValue]);
}



// --- alert view stuff ------------
void ShowInput(String rawTitle, String rawValue, int type) {
	// --- open an alert box ---
	//make sure instance is created
	if (fuelSaverUIDelegate == nil) { fuelSaverUIDelegate = [[FuelSaverUIDelegate alloc] init]; }
	
	//convert values
	NSString *title = rawTitle.ToNSString();
	NSString *value = rawValue.ToNSString();
	
	//call to instance to do it
	[fuelSaverUIDelegate showInput:title value:value type:type];
}

void ShowConfirm(String rawTitle) {
	// --- open an alert box ---
	//make sure instance is created
	if (fuelSaverUIDelegate == nil) { fuelSaverUIDelegate = [[FuelSaverUIDelegate alloc] init]; }
	
	//convert values
	NSString *title = rawTitle.ToNSString();
	
	//call to instance to do it
	[fuelSaverUIDelegate showConfirm:title];
}

void ShowMessage(String rawMessage,String rawTitle=NULL) {
	// --- open an alert box ---
	//make sure instance is created
	if (fuelSaverUIDelegate == nil) { fuelSaverUIDelegate = [[FuelSaverUIDelegate alloc] init]; }
	
	//convert values
	NSString *message = rawMessage.ToNSString();
	
	//call to instance to do it
	[fuelSaverUIDelegate showMessage:message];
}

bool HasInputFinished() {
	// --- returns true (once) if alert box has finished ---
	//check that instance exists
	if (fuelSaverUIDelegate == nil) { return true; }
	return [fuelSaverUIDelegate hasInputFinished];
}

bool WasInputCancelled() {
	//check that instance exists
	if (fuelSaverUIDelegate == nil) { return false; }
	return [fuelSaverUIDelegate wasInputCancelled];
}

String GetInputValue() {
	// --- get input value ---
	//check that instance exists
	if (fuelSaverUIDelegate == nil) { return String(@""); }
	return String([fuelSaverUIDelegate getInputValue]);
}

