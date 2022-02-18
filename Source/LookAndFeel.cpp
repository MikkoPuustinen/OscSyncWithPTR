#include "LookAndFeel.h"

using namespace juce;


OscSyncLookAndFeel::OscSyncLookAndFeel()
{
    setColour(juce::Slider::trackColourId, juce::Colour(230, 57, 70));
    setColour(juce::Slider::backgroundColourId, juce::Colour(245, 239, 237));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(106, 112, 110));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(15, 10, 10));
    setColour(juce::Slider::thumbColourId, juce::Colour(51, 51, 51));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0.0f, 0.0f, 0.0f, 0.0f));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(51, 51, 51));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(245, 239, 237));
    setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(245, 239, 237));
    setColour(juce::Label::textColourId, juce::Colour(51, 51, 51));
    setColour(juce::TextButton::buttonColourId, juce::Colour(51, 51, 51));
    setColour(juce::ToggleButton::textColourId, juce::Colour(51, 51, 51));
    setColour(juce::ToggleButton::tickColourId, juce::Colour(51, 51, 51));
    setColour(juce::ToggleButton::tickDisabledColourId, juce::Colour(51, 51, 51));
    setColour(juce::TextButton::buttonOnColourId, juce::Colour(230, 57, 70));
    setColour(juce::ComboBox::textColourId, juce::Colour(51, 51, 51));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(245, 239, 237));
    setColour(juce::ComboBox::focusedOutlineColourId, juce::Colour(245, 239, 237));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(245, 239, 237));
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(245, 239, 237));
    setColour(juce::PopupMenu::textColourId, juce::Colour(51, 51, 51));

}

OscSyncLookAndFeel& OscSyncLookAndFeel::getInstance()
{
    static OscSyncLookAndFeel instance;
    return instance;
}

void OscSyncLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(Slider::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(box.findColour(Slider::backgroundColourId));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    Rectangle<int> arrowZone(width - 30, 0, 20, height);
    Path path;
    path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
    path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

    g.setColour(box.findColour(ComboBox::textColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath(path, PathStrokeType(2.0f));
}

void OscSyncLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    FillType fillType;
    auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(Slider::rotarySliderFillColourId);

    auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin(8.0f, radius * 0.5f);
    auto arcRadiusB = radius - lineW * 0.05f;
    auto arcRadius = radius - lineW * 0.5f;
    Point<float> knobPoint(bounds.getCentreX(), bounds.getCentreY());

    Rectangle<float> minR(2, 10);
    Rectangle<float> maxR(2, 10);
    const float offset = radius + 5;

    Point<float> minPoint(bounds.getCentreX() + offset * std::cos(rotaryStartAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + offset * std::sin(rotaryStartAngle - MathConstants<float>::halfPi));
    Point<float> maxPoint(bounds.getCentreX() + offset * std::cos(rotaryEndAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + offset * std::sin(rotaryEndAngle - MathConstants<float>::halfPi));

    AffineTransform t1 = AffineTransform::rotation(rotaryStartAngle, minPoint.getX(), minPoint.getY());
    AffineTransform t2 = AffineTransform::rotation(rotaryEndAngle, maxPoint.getX(), maxPoint.getY());

    Path min, max;
    fillType.setColour(slider.findColour(Slider::thumbColourId));
    min.addRectangle(minR.withCentre(minPoint));
    max.addRectangle(maxR.withCentre(maxPoint));
    g.fillPath(min, t1);
    g.fillPath(max, t2);

    g.setColour(outline);
    g.fillEllipse(Rectangle<float>(bounds.getWidth() + 8, bounds.getWidth() + 8).withCentre(knobPoint));
    g.setColour(fill);


    g.fillEllipse(Rectangle<float>(bounds.getWidth(), bounds.getWidth()).withCentre(knobPoint));

    auto thumbWidth = lineW * 2.0f;
    Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));

    g.setColour(slider.findColour(Slider::thumbColourId));
    Rectangle<float> thumbRect(thumbWidth * 0.5f, thumbWidth);
    AffineTransform t = AffineTransform::rotation(toAngle, thumbPoint.getX(), thumbPoint.getY());
    Path p;
    p.addRectangle(thumbRect.withCentre(thumbPoint));
    p.closeSubPath();

    fillType.setColour(fill);
    g.fillPath(p, t);


}

void OscSyncLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos,
    const Slider::SliderStyle style, Slider& slider)
{
    auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
    auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

    auto trackWidth = jmin(4.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

    Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
        slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

    Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
        slider.isHorizontal() ? startPoint.y : (float)y);

    Path backgroundTrack;
    backgroundTrack.startNewSubPath(startPoint);
    backgroundTrack.lineTo(endPoint);
    g.setColour(slider.findColour(Slider::backgroundColourId));
    g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

    Path valueTrack;
    Point<float> minPoint, maxPoint, thumbPoint;

    if (isTwoVal || isThreeVal)
    {
        minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                        slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

        if (isThreeVal)
            thumbPoint = { slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
                            slider.isHorizontal() ? (float)height * 0.5f : sliderPos };

        maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                        slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
    }
    else
    {
        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

        minPoint = startPoint;
        maxPoint = { kx, ky };
    }

    auto thumbWidth = getSliderThumbRadius(slider);

    valueTrack.startNewSubPath(minPoint);
    valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
    g.setColour(slider.findColour(Slider::trackColourId));
    g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

    g.setColour(slider.findColour(Slider::thumbColourId));
    g.fillRoundedRectangle(Rectangle<float>(static_cast<float> (thumbWidth * 4), static_cast<float> (thumbWidth * 0.5f)).withCentre(maxPoint), 2);
}

Label* OscSyncLookAndFeel::createSliderTextBox(Slider& slider)
{
    auto* l = LookAndFeel_V4::createSliderTextBox(slider);
    l->setColour(TextEditor::focusedOutlineColourId, slider.findColour(Slider::textBoxOutlineColourId));
    l->setColour(TextEditor::highlightedTextColourId, slider.findColour(Slider::textBoxTextColourId));
    l->setColour(TextEditor::highlightColourId, slider.findColour(Slider::backgroundColourId));
    l->setColour(TextEditor::textColourId, slider.findColour(Slider::textBoxTextColourId));
    l->setColour(Label::textWhenEditingColourId, slider.findColour(Slider::textBoxTextColourId));
    if (slider.getName() == "resonance" || slider.getName() == "filterFreq")
    {
        l->setFont(18.0f);
    }
    else {
        l->setFont(18.0f);
    }

    return l;
}

void OscSyncLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
    auto bColor = shouldDrawButtonAsHighlighted ? button.findColour(TextButton::buttonOnColourId) : button.findColour(TextButton::buttonColourId);
    DBG(button.getButtonText());
    g.setColour(bColor);
    if (button.getButtonText() == "what")
    {
        Path path;
        path.startNewSubPath((float)bounds.getCentreX() + bounds.getWidth() / 6, (float)bounds.getY());
        path.lineTo((float)bounds.getCentreX() - bounds.getWidth() / 6, (float)bounds.getCentreY());
        path.lineTo((float)bounds.getCentreX() + bounds.getWidth() / 6, (float)bounds.getBottom());

        // g.setColour(box.findColour(ComboBox::textColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
        g.strokePath(path, PathStrokeType(4.0f));
    }
    else
    {
        Point<float> centre(bounds.getWidth() * 0.5f, bounds.getHeight() * 0.5f);
        Rectangle<float> h(centre.getX(), centre.getY(), bounds.getWidth() * 0.5f, 5.0f);
        g.fillRoundedRectangle(h.withCentre(centre), 2.0f);
        Rectangle<float> v(centre.getX(), centre.getY(), 5.0f, bounds.getWidth() * 0.5f);
        g.fillRoundedRectangle(v.withCentre(centre), 2.0f);
    }
}


