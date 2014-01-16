ISS PROJECT FIT VUTBR 2013/2014

* written in C99
* Should compile on modern *NIX (Linux,FreeBSD tested)
* uses mmap() to map bmp file into memory as well as mapping copies
* contains libbmp which is extremely stripped implementation of BMP format
  (only useful in this project)
* struct bmp contains pointers to mmapped memory (file + bmp attributes)
* Rounding used on several spots to acquire result conforming to reference
  solution

* for inspiration only, clearly thinking people will use Matlab.
