#include <vw/Image.h>
#include <vw/FileIO.h>
#include <vw/Plate/PlateFile.h>

using namespace vw;
using namespace vw::platefile;

int main( int argc, char *argv[] ) {
  
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <platefile name> <image 1> ... [image N]\n";
    exit(0);
  }

  // Create the plate file
  PlateFile platefile(argv[1], "jpg");

  for (int n = 2; n < argc; ++n) {
    ImageView<PixelRGB<uint8> > image;
    read_image(image, argv[n]);

    // Chop up the image into small chunk
    const int block_size = 256;
    std::vector<BBox2i> bboxes = image_blocks( image, block_size, block_size);
    
    // Compute the dimensions (in blocks) of the image so that we can
    // reshape the vector of bboxes into a "matrix" of bboxes in the
    // code below.
    int block_cols = ceilf(float(image.cols()) / block_size);
    int block_rows = ceilf(float(image.rows()) / block_size);
    int nlevels = ceilf(log(std::max(block_rows, block_cols))/log(2));
    std::cout << "Rows: " << block_rows << " Cols: " << block_cols << "\n";
    std::cout << "Nlevels = " << nlevels << "\n";

    // And save each block to the PlateFile
    std::vector<BBox2i>::iterator block_iter = bboxes.begin();
    for (int j = 0; j < block_rows; ++j) {
      for (int i = 0; i < block_cols; ++i) {
        std::cout << "Adding block: [ " << i << " " << j << " " <<  nlevels << "] " << *block_iter << "\n";
        
        platefile.write(i, j, nlevels, crop(image, *block_iter));

        ++block_iter;
      }
      std::cout << "\n";
    }    

    platefile.save();

  }
}