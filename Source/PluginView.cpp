/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "PluginView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
void PluginView::setProcesser(Fm_noise_reducerAudioProcessor& p)
{
	processer = &p;
}
//[/MiscUserDefs]

//==============================================================================
PluginView::PluginView()
{
	//[Constructor_pre] You can add your own custom stuff here..
	//[/Constructor_pre]

	NoiseThreshold.reset(new juce::Slider("NoiseThreshold"));
	addAndMakeVisible(NoiseThreshold.get());
	NoiseThreshold->setRange(-1, 1, 0);
	NoiseThreshold->setSliderStyle(juce::Slider::Rotary);
	NoiseThreshold->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	NoiseThreshold->addListener(this);

	NoiseThreshold->setBounds(248, 296, 112, 112);

	NoiseLearnButton.reset(new juce::TextButton("NoiseLearnButton"));
	addAndMakeVisible(NoiseLearnButton.get());
	NoiseLearnButton->setButtonText(TRANS("Capture"));
	NoiseLearnButton->addListener(this);
	NoiseLearnButton->setColour(juce::TextButton::buttonColourId, juce::Colours::coral);

	NoiseLearnButton->setBounds(152, 320, 87, 40);


	//[UserPreSize]
	//[/UserPreSize]

	setSize(600, 400);


	//[Constructor] You can add your own custom stuff here..
	//[/Constructor]
}

PluginView::~PluginView()
{
	//[Destructor_pre]. You can add your own custom destruction code here..
	//[/Destructor_pre]

	NoiseThreshold = nullptr;
	NoiseLearnButton = nullptr;


	//[Destructor]. You can add your own custom destruction code here..
	//[/Destructor]
}

//==============================================================================
void PluginView::paint(juce::Graphics& g)
{
	//[UserPrePaint] Add your own custom painting code here..
	if (processer->isUpdateParameter()) {
		NoiseThreshold.get()->setValue(processer->getNoiseThreshold());
	}
	//[/UserPrePaint]

	g.fillAll(juce::Colour(0xff323e44));

	{
		int x = 12, y = 12, width = 512, height = 256;
		//[UserPaintCustomArguments] Customize the painting arguments here..
		g.drawImage(processer->getSpectrumView(), x, y, width, height, 0, 0, width, height);
		//[/UserPaintCustomArguments]
	}

	{
		int x = 244, y = 276, width = 116, height = 30;
		juce::String text(TRANS("Reduction Level"));
		juce::Colour fillColour = juce::Colour(0xffd9d9d9);
		//[UserPaintCustomArguments] Customize the painting arguments here..
		//[/UserPaintCustomArguments]
		g.setColour(fillColour);
		g.setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
		g.drawText(text, x, y, width, height,
			juce::Justification::centred, true);
	}

	//[UserPaint] Add your own custom painting code here..
	//[/UserPaint]
}

void PluginView::resized()
{
	//[UserPreResize] Add your own custom resize code here..
	//[/UserPreResize]

	//[UserResized] Add your own custom resize handling here..
	//[/UserResized]
}

void PluginView::sliderValueChanged(juce::Slider* sliderThatWasMoved)
{
	//[UsersliderValueChanged_Pre]
	//[/UsersliderValueChanged_Pre]

	if (sliderThatWasMoved == NoiseThreshold.get())
	{
		//[UserSliderCode_NoiseThreshold] -- add your slider handling code here..
		processer->setNoiseThreshold(NoiseThreshold.get()->getValue());
		//[/UserSliderCode_NoiseThreshold]
	}

	//[UsersliderValueChanged_Post]
	//[/UsersliderValueChanged_Post]
}

void PluginView::buttonClicked(juce::Button* buttonThatWasClicked)
{
	//[UserbuttonClicked_Pre]
	//[/UserbuttonClicked_Pre]

	if (buttonThatWasClicked == NoiseLearnButton.get())
	{
		//[UserButtonCode_NoiseLearnButton] -- add your button handler code here..
		processer->startNoiseCapture();
		//[/UserButtonCode_NoiseLearnButton]
	}

	//[UserbuttonClicked_Post]
	//[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

	This is where the Projucer stores the metadata that describe this GUI layout, so
	make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PluginView" componentName=""
				 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
				 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
				 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44">
	<IMAGE pos="12 12 512 256" resource="" opacity="1.0" mode="0"/>
	<TEXT pos="244 276 116 30" fill="solid: ffd9d9d9" hasStroke="0" text="Reduction Level"
		  fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
		  italic="0" justification="36"/>
  </BACKGROUND>
  <SLIDER name="NoiseThreshold" id="462860c2b8949ddf" memberName="NoiseThreshold"
		  virtualName="" explicitFocusOrder="0" pos="248 296 112 112" min="-1.0"
		  max="1.0" int="0.0" style="Rotary" textBoxPos="TextBoxBelow"
		  textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
		  needsCallback="1"/>
  <TEXTBUTTON name="NoiseLearnButton" id="d235ee298607b140" memberName="NoiseLearnButton"
			  virtualName="" explicitFocusOrder="0" pos="152 320 87 40" bgColOff="ffff7f50"
			  buttonText="Capture" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

