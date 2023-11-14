// Be careful and always backup your old Config/Firmware, you can brick your board!!!!
// First byte must be equal or greater than the current version of your board

#pragma pack(1)
uint8_t g911xFW[] = {
        0x42, 
        0xD0, 0x02, 0x00, 0x05, 0x05, 0x75, 0x01, 0x01, 0x0F, 0x24, 
        0x0F, 0x64, 0x3C, 0x03, 0x05, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 
        0x00, 0x16, 0x19, 0x1C, 0x14, 0x8C, 0x0E, 0x0E, 0x24, 0x00, 0x31, 
        0x0D, 0x00, 0x00, 0x00, 0x83, 0x33, 0x1D, 0x00, 0x41, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x08, 0x0A, 0x00, 0x2B, 0x1C, 0x3C, 0x94, 0xD5, 
        0x03, 0x08, 0x00, 0x00, 0x04, 0x93, 0x1E, 0x00, 0x82, 0x23, 0x00, 
        0x74, 0x29, 0x00, 0x69, 0x2F, 0x00, 0x5F, 0x37, 0x00, 0x5F, 0x20, 
        0x40, 0x60, 0x00, 0xF0, 0x40, 0x30, 0x55, 0x50, 0x27, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x19, 0x00, 0x00, 
        0x50, 0x50, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 
        0x14, 0x16, 0x18, 0x1A, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1D, 
        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x24, 0x26, 0x28, 0x29, 0x2A, 0x1C, 
        0x18, 0x16, 0x14, 0x13, 0x12, 0x10, 0x0F, 0x0C, 0x0A, 0x08, 0x06, 
        0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x00};

uint8_t g911xLenovoTab[] = {0x81,
        0x80, 0x7, 0xB0, 0x4,
        0xA, 0xD, 0x20, 0x1, 0x8, 0x28, 0x5, 0x50, 0x32, 0x3, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x18, 0x1A, 0x1D, 0x14, 0x95, 0x35, 0xFF, 0x46, 0x48, 0x31, 0xD, 0x0, 0x0, 0x0, 0x80, 0x3, 0x1D,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1D, 0x59, 0x94, 0xC5, 0x2, 0x8, 0x0, 0x0,
        0x4, 0xFF, 0xFF, 0xFF, 0xC3, 0x28, 0x0, 0xA0, 0x33, 0x0, 0xFF, 0xFF, 0x0, 0x75, 0x50, 0x0, 0x75,
        0x0, 0x0, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x7F, 0xC0, 0x0, 0x0, 0x0, 0x1,
        0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14,
        0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
        0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1B,
        0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,
        0x88, 0x00};

uint8_t g911xOrig[] ={ // first byte was 0x55 on my no-name-board
        0x81, 0x0, 0x4, 0x58, 0x2, 0xA, 0xC, 0x20, 0x1, 0x8, 0x28, 0x5, 0x50, 0x3C, 0x3, 0x5,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x89, 0x2A, 0xB, 0x2D, 0x2B,
        0xF, 0xA, 0x0, 0x0, 0x1, 0xA9, 0x3, 0x2D, 0x0, 0x1, 0x0, 0x0, 0x0, 0x3, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x21, 0x59, 0x94, 0xC5, 0x2, 0x7, 0x0, 0x0, 0x4, 0x93, 0x24, 0x0, 0x7D,
        0x2C, 0x0, 0x6B, 0x36, 0x0, 0x5D, 0x42, 0x0, 0x53, 0x50, 0x0, 0x53, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x2, 0x4, 0x6, 0x8, 0xA, 0xC, 0xE, 0x10, 0x12, 0x14, 0x16, 0xFF, 0xFF, 0xFF, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2,
        0x4, 0x6, 0x8, 0xA, 0xF, 0x10, 0x12, 0x16, 0x18, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22,
        0x24, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xD6, 0x0
        };
#pragma pack()