#include "ESPWeb.h"

class ESPApp : public ESPWebBase {
public:
  ESPApp() : ESPWebBase() {}
};

ESPApp* app = new ESPApp();

void setup() {
  Serial.begin(115200);
  Serial.println();
  app->setup();
}

void loop() {
  app->loop();
}
