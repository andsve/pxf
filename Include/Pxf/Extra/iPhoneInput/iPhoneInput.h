#ifndef __PXF_EXTRA_IPHONEINPUT_H__
#define __PXF_EXTRA_IPHONEINPUT_H__

#import <UIKit/UIKit.h>

struct InputDragData
{
    float start[2];
    float end[2];
};

struct InputTapData
{
    float pos[2];
};

@interface InputHandler : NSObject
{
    char m_InputResponseText[2048];
    int m_InputResponseButton;
    bool m_HasResponded;
}

// Input text
- (void) requireTextInput:(const char*)popupTitle message:(const char*)msg textField:(const char*)field;
- (void) clearResponse;
- (bool) hasRespondedToInput;
- (void) getInputResponseText:(char *)outText;
- (int) getInputResponseButton;

@end

#endif // __PXF_EXTRA_IPHONEINPUT_H__
