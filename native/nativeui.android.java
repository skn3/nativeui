
import android.app.AlertDialog;
import android.widget.EditText;
import android.widget.TextView.OnEditorActionListener;
import android.content.DialogInterface;

class NativeUINative {
	private static boolean active = false;
	private static boolean cancelled = false;
	private static boolean finished = false;
	private static AlertDialog activeDialog = null;
	private static String selectedValue;
	private static int selectedIndex = -1;

	//internal
	private static void CloseDialog() {
		//assume this has been called from the ui thread...
		//is there an active dialog?
		if (active) {
			//stop active
			active = false;
			cancelled = true;
			finished = true;

			//close existing dialog
			if (activeDialog != null) {
				activeDialog.dismiss();
				activeDialog = null;
			}
		}
	}

	//api
	public static void InitNative() {
	}

	public static void ShowPickerNative(final String[] values,final String value) {
		//make sure this is executing on the correct thread!
		BBAndroidGame.AndroidGame().GetActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				//close existing dialogs
				NativeUINative.CloseDialog();

				//create dialog builder
				AlertDialog.Builder builder = new AlertDialog.Builder(BBAndroidGame.AndroidGame().GetActivity());

				//add cancel event
				builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
					@Override
					public void onCancel(DialogInterface dialog) {
						//cancel active dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = true;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				//setup details
				//builder.setTitle(rawTitle);

				//save value passed in
				NativeUINative.selectedValue = new String(value);

				//add items
				//build item list and find out the selected value
				List<String> itemsArray = new ArrayList<String>();

				selectedIndex = -1;
				for (int index = 0; index < values.length; index++) {
					//add string item
					itemsArray.add(values[index]);

					//is this the selected item?
					if (selectedIndex == -1 && values[index] == value) {
						selectedIndex = index;
					}
				}

				final CharSequence[] items = itemsArray.toArray(new CharSequence[itemsArray.size()]);

				//add radio choice item to builder
				builder.setSingleChoiceItems(items, selectedIndex, new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int which) {
						//save selected value
						selectedIndex = which;
						if (which == -1) {
							selectedValue = new String("");
						} else {
							selectedValue = new String(values[which]);
						}
					}
				});

				//add buttons
				builder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
					//handle on click
					@Override
					public void onClick(DialogInterface dialog, int whichButton) {
						//finish dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = false;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int whichButton) {
						//cancel active dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = true;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				//show it
				NativeUINative.active = true;
				NativeUINative.cancelled = false;
				NativeUINative.finished = false;
				NativeUINative.activeDialog = builder.create();
				NativeUINative.activeDialog.show();
			}
		});
	}

	public static boolean HasPickerFinishedNative() {
		//only return once
		if (finished) {
			finished = false;
			return true;
		}

		//nope
		return false;
	}

	public static String GetPickerValueNative() {
		return selectedValue;
	}

	public static int GetPickerIndexNative() {
		return selectedIndex;
	}

	public static void ShowInputNative(final String title, final String value, final int type) {
		//make sure this is executing on the correct thread!
		BBAndroidGame.AndroidGame().GetActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				//close existing dialogs
				NativeUINative.CloseDialog();

				//create dialog builder
				AlertDialog.Builder builder = new AlertDialog.Builder(BBAndroidGame.AndroidGame().GetActivity());

				//add cancel event
				builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
					@Override
					public void onCancel(DialogInterface dialog) {
						NativeUINative.active = false;
						NativeUINative.cancelled = true;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				//save value passed in
				NativeUINative.selectedValue = new String(value);

				//setup details
				builder.setTitle(title);

				//add input
				final EditText input = new EditText(BBAndroidGame._androidGame._activity);
				input.setSelectAllOnFocus(true);
				input.setText(value);
				input.setSingleLine();
				if (type == 1){
					input.setInputType(InputType.TYPE_CLASS_NUMBER);
				} else if (type == 2){
					input.setInputType(InputType.TYPE_NUMBER_FLAG_DECIMAL);
				}
				builder.setView(input);

				//handle done pressed on keyboard
				input.setOnEditorActionListener(new OnEditorActionListener() {
					@Override
					public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
						if (actionId == EditorInfo.IME_ACTION_DONE) {
							//save entered value
							NativeUINative.selectedValue = new String(input.getText().toString());

							//dismiss the dialog
							NativeUINative.activeDialog.dismiss();

							//finish dialog
							NativeUINative.active = false;
							NativeUINative.cancelled = false;
							NativeUINative.finished = true;
							NativeUINative.activeDialog = null;
						}
						return false;
					}
				});

				//add buttons
				builder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
					//handle on click
					@Override
					public void onClick(DialogInterface dialog, int whichButton) {
						//save entered value
						NativeUINative.selectedValue = new String(input.getText().toString());

						//finish dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = false;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int whichButton) {
						//cancel dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = true;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				//show it
				NativeUINative.active = true;
				NativeUINative.cancelled = false;
				NativeUINative.activeDialog = builder.create();

				//add focus handling for input/keyboard
				input.setOnFocusChangeListener(new View.OnFocusChangeListener() {
					@Override
					public void onFocusChange(View v, boolean hasFocus) {
						if (hasFocus) {
							NativeUINative.activeDialog.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
						}
					}
				});

				NativeUINative.activeDialog.show();
			}
		});
	}

	public static void ShowConfirmNative(final String title) {
		//make sure this is executing on the correct thread!
		BBAndroidGame.AndroidGame().GetActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				//close existing dialogs
				NativeUINative.CloseDialog();

				//create dialog builder
				AlertDialog.Builder builder = new AlertDialog.Builder(BBAndroidGame.AndroidGame().GetActivity());

				//add cancel event
				builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
					@Override
					public void onCancel(DialogInterface dialog) {
						//cancel dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = true;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				//setup details
				builder.setTitle(title);
				builder.setIcon(android.R.drawable.ic_dialog_alert);

				//add buttons
				builder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int whichButton) {
						//finish dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = false;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int whichButton) {
						//cancel dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = true;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				//show it
				NativeUINative.active = true;
				NativeUINative.cancelled = false;
				NativeUINative.activeDialog = builder.create();
				NativeUINative.activeDialog.show();
			}
		});
	}

	public static void ShowMessageNative(final String message, final String title) {
		//make sure this is executing on the correct thread!
		BBAndroidGame.AndroidGame().GetActivity().runOnUiThread(new Runnable() {
			@Override
			public void run() {
				//close existing dialogs
				NativeUINative.CloseDialog();

				//create dialog builder
				AlertDialog.Builder builder = new AlertDialog.Builder(BBAndroidGame.AndroidGame().GetActivity());

				//add cancel event
				builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
					@Override
					public void onCancel(DialogInterface dialog) {
						//cancel dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = true;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				//setup details
				//builder.setCancelable(false); // This blocks the 'BACK' button
				if (title.length() > 0) {
					builder.setTitle(title);
				}
				builder.setMessage(message);

				//add buttons
				builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						//finish dialog
						NativeUINative.active = false;
						NativeUINative.cancelled = false;
						NativeUINative.finished = true;
						NativeUINative.activeDialog = null;
					}
				});

				//show it
				NativeUINative.active = true;
				NativeUINative.cancelled = false;
				NativeUINative.activeDialog = builder.create();
				NativeUINative.activeDialog.show();
			}
		});
	}

	public static boolean HasInputFinishedNative() {
		//only return once
		if (finished) {
			finished = false;
			return true;
		}

		//nope
		return false;
	}

	public static boolean WasInputCancelledNative() {
		return cancelled;
	}

	public static String GetInputValueNative() {
		return selectedValue;
	}
}
