#include "web_ui.h"
#include "artnet_handler.h"

constexpr uint8_t DISPLAY_SLIDERS = 32;

uint16_t WebUI::sliderIds[DISPLAY_SLIDERS] = {0};
uint16_t WebUI::statusId = 0;
uint8_t* WebUI::dmxValuesPtr = nullptr;
String WebUI::title = "";

void WebUI::init(const String& deviceName) {
    title = "DMX Controller: " + deviceName;
    ESPUI.begin(title.c_str());
    ESPUI.sliderContinuous = true;
    
    statusId = ESPUI.addControl(Label, "Status", "", ControlColor::Turquoise);
    ESPUI.switcher("Art-Net", &WebUI::pauseArtnetCallback, ControlColor::Carrot, true);

    String labelStyle = "background-color: unset; margin-bottom: unset; top: .15em;";
    uint16_t dmxChanelPanel = ESPUI.addControl(Label, "", "");
    ESPUI.setPanelStyle(dmxChanelPanel, "display: grid; grid-template-columns: 40px auto;");
    ESPUI.setElementStyle(dmxChanelPanel, labelStyle);
    
    // empty element for grid elements
    ESPUI.setElementStyle(ESPUI.addControl(Label, "", "", None, dmxChanelPanel), labelStyle);
    
    for (int i = 0; i < DISPLAY_SLIDERS; i++) {
        String label = String(i + 1);
        ESPUI.setElementStyle(ESPUI.addControl(Label, "", label, None, dmxChanelPanel), labelStyle);
        sliderIds[i] = ESPUI.addControl(Slider, "", "0", ControlColor::Turquoise, dmxChanelPanel, &WebUI::sliderCallback);
        ESPUI.addControl(ControlType::Min, "", String(0), ControlColor::None, sliderIds[i]);
        ESPUI.addControl(ControlType::Max, "", String(255), ControlColor::None, sliderIds[i]);
    }
}

void WebUI::updateChannelValue(int channel, uint8_t value) {
    if (channel < DISPLAY_SLIDERS) {
        ESPUI.updateSlider(sliderIds[channel], value);
    }
}

void WebUI::pauseArtnetCallback(Control* sender, int value) {
    ArtnetHandler::setPaused(value == S_INACTIVE);
}

void WebUI::updateStatus(const String& status) {
    ESPUI.updateLabel(statusId, status);
}

void WebUI::onSliderChange(uint8_t* dmxValues) {
    dmxValuesPtr = dmxValues;
}

void WebUI::sliderCallback(Control* sender, int type) {
    if (type == SL_VALUE && dmxValuesPtr != nullptr) {
        int channel = -1;
        for (int i = 0; i < DISPLAY_SLIDERS; i++) {
            if (sliderIds[i] == sender->id) {
                channel = i;
                break;
            }
        }
        
        if (channel != -1) {
            dmxValuesPtr[channel] = sender->value.toInt();
        }
    }
}