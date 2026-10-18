# War Gods PC assets

Research uses the supplied June 1997 WARGODS.EXE and WARGODS.DAT. Files are
read directly through the shared named package API. No upstream code is copied.

## Resource container and numbers

All 219 `.W` files have a WIFF header. The big-endian size at +4 describes the
historical expanded resource, not its physical packed length. Iterate from +8
using four-character chunk tags and big-endian payload sizes, rounded to four
bytes. All 2,904 chunks in the supplied archive validate this way.

DOT4 holds a little-endian resource ID, u16 height, u16 width and indexed pixels.
434 chunks use standard RNC method 1 compression; some small textures are raw.
The independent bounded decoder validates both packed and expanded CRC16s.
The method is canonical Huffman coding of literal lengths, backward distances
and match lengths, with interleaved little-endian control words and raw bytes.
A final partial control word can extend one byte beyond the packed stream;
its unused byte is zero, and an empty final literal consumes no bytes.
Format background: https://psx-spx.consoledev.net/dev/ps1/cdr/cdromfileformats/compression/

COL4 holds a resource ID, a palette entry count and little-endian RGB555 colors.
Fighter palettes contain multiple 256-color lighting ramps.

Resource coordinates retain TMS320C3x single-precision encoding. The PC converter
at VA 0x427940 converts the exponent byte, sign and complemented negative
mantissa to IEEE float. The asset loader calls it at 0x429634/0x429643/0x429652.
The implementation preserves the retail conversion's negative-value rounding.

OBJ4 contains an ID, command word, flags, a last-vertex index, XYZ coordinate
triples, a last-face index and 36-byte faces. The PC reader at 0x429460 adds one
to both last indices. Face indices address coordinate words (divide by three).
Geometry, animation and scene interpretation are documented as they are verified.

`make wargods-test` checks all WIFF files and every compressed texture against
its stored CRCs and dimensions. The original game files are required.
