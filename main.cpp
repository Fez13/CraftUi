#include "src/core/app.h"
#include <cstdlib>
#include <iostream>

int main() {

  

  cui::App *app;
  cui::exit_state state = cui::CUI_EXIT_STATE_NULL;
  while (state != cui::CUI_EXIT_CLOSE) {
    app = new cui::App();
    app->initialize();
    state = app->execute();
    app->clean();
  }
  
  
}
