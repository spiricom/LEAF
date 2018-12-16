/*
  ==============================================================================

    UIComponent.h
    Created: 5 Dec 2016 2:23:38pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#ifndef UICOMPONENT_H_INCLUDED
#define UICOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class UIComponent:
public Component,
public Slider::Listener,
public TextButton::Listener,
public ComboBox::Listener,
public TextEditor::Listener,
private Timer
{
public:
    UIComponent();

    ~UIComponent();

    void paint (Graphics& g) override;

    void resized() override;
    
    void sliderValueChanged(Slider* s) override;
    
    void buttonClicked (Button* b) override;
    
    void textEditorTextChanged (TextEditor&) override;
    
    void comboBoxChanged (ComboBox* cb) override;
    
    void timerCallback() override;
    
private:
    
    static const int cLeftOffset = 30;
    static const int cTopOffset = 30;
    
    static const int cXSpacing = 60;
    static const int cYSpacing = 5;
    
    static const int cSliderHeight = 200;
    static const int cSliderWidth = 20;
    
    static const int cLabelHeight = 20;
    static const int cLabelWidth  = cSliderWidth + cXSpacing;
    
    static const int cButtonHeight = 30;
    static const int cButtonWidth  = 60;
    
    static const int cBoxHeight = 20;
    static const int cBoxWidth  =  100;
    
    OwnedArray<Slider>      sliders;
    OwnedArray<Label>       sliderLabels;
    OwnedArray<TextButton>  buttons;
    OwnedArray<TextEditor>  textFields;
    OwnedArray<ComboBox>    comboBoxes;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UIComponent)
};


#endif  // UICOMPONENT_H_INCLUDED
