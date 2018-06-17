#include <iostream>
#include <comfybar.h>

using namespace comfybar;

int main(int argc, char** argv) {
  Bar bar(argc, argv);
  bar.create();

  if(bar.isCreated()) {
    bar.show();
  }
  return 0;
}