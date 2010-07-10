#import <Pxf/Extra/iPhoneInput/iPhoneInput.h>
#import <Pxf/Graphics/OpenGL/EAGLView11.h>

using namespace Pxf;

@implementation InputHandler

- (void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	//printf("User Pressed Button %d\n", buttonIndex + 1);
	//printf("Text Field 1: %s\n", [[[alertView textFieldAtIndex:0] text] cStringUsingEncoding:1]);
    sprintf(m_InputResponseText, "%s", [[[alertView textFieldAtIndex:0] text] cStringUsingEncoding:1]);
    m_InputResponseButton = buttonIndex + 1;
    m_HasResponded = true;
	[alertView release];
}


- (void) requireTextInput:(const char*)popupTitle message:(const char*)msg textField:(const char*)field
{
    printf("WAS I CALLED, LOL?\n");
    NSString *t_title = [NSString stringWithCString:popupTitle length:strlen(popupTitle)];
    NSString *t_msg = [NSString stringWithCString:msg length:strlen(msg)];
    NSString *t_field = [NSString stringWithCString:field length:strlen(field)];
    
	UIAlertView *alert = [[UIAlertView alloc] 
						  initWithTitle: t_title
						  message:t_msg
						  delegate:self
						  cancelButtonTitle:@"Cancel"
						  otherButtonTitles:@"OK", nil];
	[alert addTextFieldWithValue:@"" label:t_field];
	
	// Name field
	UITextField *tf = [alert textFieldAtIndex:0];
	tf.clearButtonMode = UITextFieldViewModeWhileEditing;
	tf.keyboardType = UIKeyboardTypeAlphabet;
	tf.keyboardAppearance = UIKeyboardAppearanceAlert;
	tf.autocapitalizationType = UITextAutocapitalizationTypeWords;
	tf.autocorrectionType = UITextAutocorrectionTypeNo;
	
	//[alert addSubView:tf];

	[alert show];
}

- (void) clearResponse
{
    m_HasResponded = false;
}

- (bool) hasRespondedToInput
{
    return m_HasResponded;
}

- (void) getInputResponseText:(char *)outText
{
    printf("Function is called! Value: %s\n", m_InputResponseText);
    sprintf(outText, "%s", m_InputResponseText);
}

- (int) getInputResponseButton
{
    return m_InputResponseButton;
}


-(void) dealloc
{
	/*[m_Window release];
	*/
	[super dealloc];
}
@end


