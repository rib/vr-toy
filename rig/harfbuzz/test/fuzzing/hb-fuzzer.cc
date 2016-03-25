#include <stddef.h>
#include "src/hb.h"
#include "src/hb-ot.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  const char text[] = "ABCDEXYZ123@_%&)*$!";

  hb_blob_t *blob = hb_blob_create((const char *)data, size,
                                   HB_MEMORY_MODE_READONLY, NULL, NULL);
  hb_face_t *face = hb_face_create(blob, 0);
  hb_font_t *font = hb_font_create(face);
  hb_ot_font_set_funcs(font);
  hb_font_set_scale(font, 12, 12);

  hb_buffer_t *buffer = hb_buffer_create();
  hb_buffer_add_utf8(buffer, text, -1, 0, -1);
  hb_buffer_guess_segment_properties(buffer);

  hb_shape(font, buffer, NULL, 0);

  hb_buffer_destroy(buffer);
  hb_font_destroy(font);
  hb_face_destroy(face);
  hb_blob_destroy(blob);
  return 0;
}

#ifdef MAIN
#include <iostream>
#include <iterator>
#include <fstream>
#include <assert.h>

std::string FileToString(const std::string &Path) {
  std::ifstream T(Path.c_str());
  return std::string((std::istreambuf_iterator<char>(T)),
                     std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    std::string s = FileToString(argv[i]);
    std::cout << argv[i] << std::endl;
    LLVMFuzzerTestOneInput((const unsigned char*)s.data(), s.size());
  }
}
#endif
