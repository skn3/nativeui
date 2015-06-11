
#define INPUT_MODE_TEXT 1
#define INPUT_MODE_CONFIRM 2
#define INPUT_MODE_MESSAGE 3

#define INPUT_TYPE_STRING 0
#define INPUT_TYPE_INT 1
#define INPUT_TYPE_FLOAT 2

// --- shared delegate ------------
@interface NativeUIDelegateNative : NSObject<UIPickerViewDelegate,UIAlertViewDelegate,UITextFieldDelegate> {
}

@property (nonatomic, assign) NSObject *pickerLock;
@property (nonatomic, assign) UIView *pickerContainerView;
@property (nonatomic, assign) UIPickerView *pickerView;
@property (nonatomic, assign) UIView *pickerAccessoryView;
@property (nonatomic, assign) UITextField *pickerTextField;
@property (nonatomic, retain) NSMutableArray *pickerValues;
@property (nonatomic) NSUInteger pickerValuesTotal;
@property (nonatomic) NSInteger pickerIndex;
@property (nonatomic, retain) NSString *pickerValue;
@property (nonatomic) bool pickerActive;
@property (nonatomic) bool pickerFinished;


@property (nonatomic, assign) NSObject *datePickerLock;
@property (nonatomic, assign) UIView *datePickerContainerView;
@property (nonatomic, assign) UIPickerView *datePickerView;
@property (nonatomic, assign) UIView *datePickerAccessoryView;
@property (nonatomic, assign) UITextField *datePickerTextField;
@property (nonatomic, assign) UIDatePicker *datePicker;
@property (nonatomic) bool datePickerActive;
@property (nonatomic) bool datePickerFinished;


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
- (NSInteger)getPickerIndex;
- (NSString *)getPickerValue;

//date picker view
- (void)datePickerDoneClicked:(id)sender;
- (void)showDatePicker:(NSString*)mode;
- (bool)hasDatePickerFinished;
- (NSString *)getDatePickerValue;

//alert view
- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex;
- (void)showInput:(NSString *)title value:(NSString *)value type:(int)type;
- (void)showConfirm:(NSString *)title;
- (void)showMessage:(NSString *)message title:(NSString *)title button:(NSString *)button;
- (bool)hasInputFinished;
- (bool)wasInputCancelled;
- (NSString *)getInputValue;

@end

//implement delegate
@implementation NativeUIDelegateNative
@synthesize pickerLock;
@synthesize pickerContainerView;
@synthesize pickerView;
@synthesize pickerAccessoryView;
@synthesize pickerTextField;
@synthesize pickerValues;
@synthesize pickerValuesTotal;
@synthesize pickerIndex;
@synthesize pickerValue;
@synthesize pickerActive;
@synthesize pickerFinished;

@synthesize datePickerLock;
@synthesize datePickerContainerView;
@synthesize datePickerView;
@synthesize datePickerAccessoryView;
@synthesize datePickerTextField;
@synthesize datePicker;
@synthesize datePickerActive;
@synthesize datePickerFinished;

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
		
		self.datePickerLock = [[NSObject alloc] init];
		@synchronized(datePickerLock) {
			self.datePickerActive = false;
			self.datePickerFinished = true;
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
		
		
		
		// [ date picker ]
		// --- create accessory view ---
		self.datePickerAccessoryView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, mainView.frame.size.width, 44)];

		//create toolbar
		UIToolbar *dateToolbar = [[UIToolbar alloc] initWithFrame: CGRectMake(0, 0, self.datePickerAccessoryView.frame.size.width, self.datePickerAccessoryView.frame.size.height)];
		dateToolbar.barStyle = UIBarStyleBlack;
		
		//add date toolbar items
		UIBarButtonItem *dateEmptySpaceItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:self action:nil];
		UIBarButtonItem *dateDoneItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(datePickerDoneClicked:)];
		NSArray *dateToolbarItems = [NSArray arrayWithObjects: dateEmptySpaceItem, dateDoneItem, nil];
		[dateToolbar setItems:dateToolbarItems animated:NO];
				
		//add toolbar to accessoryview
		[self.datePickerAccessoryView addSubview:dateToolbar];
		
		
		// --- create custom "keyboard" ---
		//create container
		self.datePickerContainerView = [[UIView alloc] initWithFrame: CGRectMake ( 0, 0, mainView.frame.size.width, 216)];
		
		//create datePicker
		datePicker = [[UIDatePicker alloc] initWithFrame: CGRectMake(0, 0, self.datePickerAccessoryView.frame.size.width, self.datePickerAccessoryView.frame.size.height)];
		[datePicker addTarget:self action:@selector(updateDateTextField:) forControlEvents:UIControlEventValueChanged];
		//add control to accessoryview
		[self.datePickerContainerView addSubview:datePicker];
		
		// --- create textfield to handle input ---
		//need to create a hidden textinput to deal with getting value for date picker
		self.datePickerTextField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0,100, 25)];
		self.datePickerTextField.hidden = YES;		
		[self.datePickerTextField setInputView:self.datePickerContainerView];
		[self.datePickerTextField setInputAccessoryView:self.datePickerAccessoryView];
		[mainView addSubview: self.datePickerTextField];
		
		//release waste
		[dateEmptySpaceItem release];
		[dateDoneItem release];
		[dateToolbar release];
		
		
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
	
	//do we need to close the date picker?
	@synchronized(datePickerLock) {
		if (self.datePickerActive) {
			self.datePickerActive = false;
			[[BBIosGame::IosGame()->GetUIAppDelegate() view] endEditing:NO];
		}
		
		//remove ui from monkey view
		[self.datePickerTextField removeFromSuperview];
		
		//release stuff
		[self.datePickerTextField release];
		[self.datePickerAccessoryView release];
		[self.datePickerView release];
		[self.datePickerContainerView release];
		
		self.datePickerTextField = nil;
		self.datePickerAccessoryView = nil;
		self.datePickerView = nil;
		self.datePickerContainerView = nil;
	}
	
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
	
	//release locks
	[self.pickerLock release];
	[self.datePickerLock release];
	[self.inputLock release];
	self.pickerLock = nil;
	self.datePickerLock = nil;
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
		self.pickerIndex = row;
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
	self.pickerFinished = false;
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
	self.pickerIndex = selectedRow;
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

- (NSInteger)getPickerIndex {
	// --- get picker value as monkey string ---
	@synchronized(pickerLock) {
		return self.pickerIndex;
	}
}

//date picker view protocol

-(void)updateDateTextField:(UIDatePicker *)sender
{
	NSDateFormatter *formatter;
	
	formatter = [[NSDateFormatter alloc] init];
	if(sender.datePickerMode == UIDatePickerModeDate)
	{
		[formatter setDateFormat:@"dd-MM-yyyy"];
	}
	else if(sender.datePickerMode == UIDatePickerModeTime)
	{
		[formatter setDateFormat:@"HH:mm"];
	}
	else if(sender.datePickerMode == UIDatePickerModeDateAndTime)
	{
		[formatter setDateFormat:@"dd-MM-yyyy HH:mm"];
	}
	
    self.datePickerTextField.text = [formatter stringFromDate:[sender date]];
    
    [formatter release];
}

- (void)datePickerDoneClicked:(id)sender {
	// --- done has been pressed ---
	//save date and close the date picker
	
	@synchronized(datePickerLock) {	
		//finish the date picker
		self.datePickerActive = false;
		self.datePickerFinished = true;
	}
	
	//close the date picker
	[[BBIosGame::IosGame()->GetUIAppDelegate() view] endEditing:NO];
}

- (void)showDatePicker:(NSString*)mode {
	self.datePickerFinished = false;
	// --- show the date picker ---
	@synchronized(datePickerLock) {
		//check if there is an existing session
		if (self.datePickerActive == true) {
			//close previous session
			self.datePickerActive = false;
			//self.datePickerValues = nil;
			
			//close the date picker
			[[BBIosGame::IosGame()->GetUIAppDelegate() view] endEditing:NO];
		}
		
		//start new date picker session
		self.datePickerActive = true;
		self.datePickerFinished = false;
	}
	
	//tell picker to reload
	[self.datePickerView reloadAllComponents];
	
	//start the date picker session
	[self.datePickerTextField becomeFirstResponder];
	
	//set Type
	if([mode isEqual:@"date"])
		{
			datePicker.datePickerMode = UIDatePickerModeDate;
		}
		else if([mode isEqual:@"time"])
		{
			datePicker.datePickerMode = UIDatePickerModeTime;
		}
		else if([mode isEqual:@"datetime"])
		{
			datePicker.datePickerMode = UIDatePickerModeDateAndTime;
		}


	[self updateDateTextField:datePicker];
}

- (bool)hasDatePickerFinished {
	// --- return true if the date picker has finished ---
	@synchronized(datePickerLock) {
		//only once
		if (self.datePickerFinished == true) {
			self.datePickerFinished = false;
			return true;
		}
	}
	
	//nope
	return false;
}

- (NSString *)getDatePickerValue {
	// --- get picker value as monkey string ---
	@synchronized(datePickerLock) {
		return self.datePickerTextField.text;
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

- (void)showMessage:(NSString *)message title:(NSString *)title button:(NSString *)button {
	self.inputFinished = false;
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
	self.inputView = [[UIAlertView alloc] initWithTitle:title message:message delegate:self cancelButtonTitle:button otherButtonTitles:nil];
	
	//show the alert
	dispatch_async(dispatch_get_main_queue(), ^{
		[self.inputView show];
	});
}

- (void)showConfirm:(NSString *)title {
	self.inputFinished = false;
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
	dispatch_async(dispatch_get_main_queue(), ^{
		[self.inputView show];
	});
}

- (void)showInput:(NSString *)title value:(NSString *)value type:(int)type {
	self.inputFinished = false;
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
			if ([[vComp objectAtIndex:0] intValue] >= 5) {
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
	dispatch_async(dispatch_get_main_queue(), ^{
		[self.inputView show];
	});
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



// --- native ui class ---
//header
class NativeUINative {
public:
	static NativeUIDelegateNative *delegate;
	
	static void InitNative();
	
	//picker
	static void ShowPickerNative(Array<String > values,String value);
	static bool HasPickerFinishedNative();
	static String GetPickerValueNative();
	static int GetPickerIndexNative();
	
	//date picker
	static void ShowDatePickerNative(String mode);
	static bool HasDatePickerFinishedNative();
	static String GetDatePickerValueNative();
	
	//alert
	static void ShowInputNative(String rawTitle, String rawValue, int type);
	static void ShowConfirmNative(String rawTitle);
	static void ShowMessageNative(String rawMessage,String rawTitle,String rawButton);
	static bool HasInputFinishedNative();
	static bool WasInputCancelledNative();
	static String GetInputValueNative();
};

//body
NativeUIDelegateNative *NativeUINative::delegate = nil;

void NativeUINative::InitNative() {
	// --- lets us init at start of app ---
	//make sure instance is created
	if (NativeUINative::delegate == nil) { NativeUINative::delegate = [[NativeUIDelegateNative alloc] init]; }
}

// --- picker view stuff ------------
void NativeUINative::ShowPickerNative(Array<String > values,String value) {
	// --- so here we have a invisible picker object ??? ---
	//make sure instance is created
	if (NativeUINative::delegate == nil) { NativeUINative::delegate = [[NativeUIDelegateNative alloc] init]; }
	
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
	[NativeUINative::delegate showPicker:values2 selectedRow:row];
	
	//release values2
	[values2 release];
}

bool NativeUINative::HasPickerFinishedNative() {
	// --- has picker finished ---
	//check that instance exists
	if (NativeUINative::delegate == nil) { return true; }
	return [NativeUINative::delegate hasPickerFinished];
}

int NativeUINative::GetPickerIndexNative() {
	// --- has picker finished ---
	//check that instance exists
	if (NativeUINative::delegate == nil) { return -1; }
	return int([NativeUINative::delegate getPickerIndex]);
}

String NativeUINative::GetPickerValueNative() {
	// --- has picker finished ---
	//check that instance exists
	if (NativeUINative::delegate == nil) { return String(@""); }
	return String([NativeUINative::delegate getPickerValue]);
}


// --- date picker view stuff ------------
void NativeUINative::ShowDatePickerNative(String mode) {
	// --- so here we have a invisible date picker object ??? ---
	//make sure instance is created
	if (NativeUINative::delegate == nil) { NativeUINative::delegate = [[NativeUIDelegateNative alloc] init]; }
	
	NSString *myMode = [mode.ToNSString() lowercaseString];
		
	//call instance method
	[NativeUINative::delegate showDatePicker:myMode];
}

bool NativeUINative::HasDatePickerFinishedNative() {
	// --- has date picker finished ---
	//check that instance exists
	if (NativeUINative::delegate == nil) { return true; }
	return [NativeUINative::delegate hasDatePickerFinished];
}

String NativeUINative::GetDatePickerValueNative() {
	// --- has date picker finished ---
	//check that instance exists
	if (NativeUINative::delegate == nil) { return String(@""); }
	return String([NativeUINative::delegate getDatePickerValue]);
}


// --- alert view stuff ------------
void NativeUINative::ShowInputNative(String rawTitle, String rawValue, int type) {
	// --- open an alert box ---
	//make sure instance is created
	if (NativeUINative::delegate == nil) { NativeUINative::delegate = [[NativeUIDelegateNative alloc] init]; }
	
	//convert values
	NSString *title = rawTitle.ToNSString();
	NSString *value = rawValue.ToNSString();
	
	//call to instance to do it
	[NativeUINative::delegate showInput:title value:value type:type];
}

void NativeUINative::ShowConfirmNative(String rawTitle) {
	// --- open an alert box ---
	//make sure instance is created
	if (NativeUINative::delegate == nil) { NativeUINative::delegate = [[NativeUIDelegateNative alloc] init]; }
	
	//convert values
	NSString *title = rawTitle.ToNSString();
	
	//call to instance to do it
	[NativeUINative::delegate showConfirm:title];
}

void NativeUINative::ShowMessageNative(String rawMessage,String rawTitle,String rawButton) {
	// --- open an alert box ---
	//make sure instance is created
	if (NativeUINative::delegate == nil) { NativeUINative::delegate = [[NativeUIDelegateNative alloc] init]; }
	
	//convert values
	NSString *message = rawMessage.ToNSString();
	NSString *title = rawTitle.ToNSString();
	NSString *button = rawButton.ToNSString();
	
	//call to instance to do it
	[NativeUINative::delegate showMessage:message title:title button:button];
}

bool NativeUINative::HasInputFinishedNative() {
	// --- returns true (once) if alert box has finished ---
	//check that instance exists
	if (NativeUINative::delegate == nil) { return true; }
	return [NativeUINative::delegate hasInputFinished];
}

bool NativeUINative::WasInputCancelledNative() {
	//check that instance exists
	if (NativeUINative::delegate == nil) { return false; }
	return [NativeUINative::delegate wasInputCancelled];
}

String NativeUINative::GetInputValueNative() {
	// --- get input value ---
	//check that instance exists
	if (NativeUINative::delegate == nil) { return String(@""); }
	return String([NativeUINative::delegate getInputValue]);
}
